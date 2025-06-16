#include <stdlib.h> // atoi
#include <string.h> // strlen
#include <stdarg.h> // va args
#include <ctype.h> // tolower, isspace
#include "http_request.h"

int http_request_content_length(char *src)
{
    char *content_length_key = "content-length:";
    int partial = -1;
    int not_found = -2;
    if (!src)
        return not_found;
    while (*src) {
        char *key = content_length_key;
        // find the key.
        while (*src && tolower(*src++) == *key && key++);
        // read something but not entirely.
        // ie, "content-le", partial match.
        if (!*src && key != content_length_key)
            return partial;
        // null only if the entire key was found.
        if (*key)
            continue;
        // skip white space after ":".
        while (*src && isspace(*src) && src++);
        char *value = src;
        // skip past the number.
        while (*src && !isspace(*src) && src++);
        // make sure the entire value is read.
        // if it doesn't end on a new line, it means
        // the entire request hasn't been sent.
        if (!isspace(*src))
            return partial;
        return atoi(value);
    }
    return not_found;
}

char *http_request_body(char *src)
{
    if (!src)
        return 0;
    int crlf = '\r' + '\n';
    char *body = src;
    // skip first line (ie, GET /path HTTP...)
    while (!isspace(*body) && body++);
    while (isspace(*body) && body++);
    // minimum size is 4 bytes (two crlf)
    if (body - src < 4)
        return 0;
    // find last two crlf characters.
    while (*body &&
           !((*(body - 3) + *(body - 2)) == crlf &&
             (*(body - 1) + *(body - 0)) == crlf) &&
           body++);
    if (!*body)
        return 0;
    // skip last new line. start at the body.
    body++;
    return body;
}

enum http_request_method http_request_get_method(char *src)
{
    if (!src)
        return METHOD_UNKNOWN;
    if (strncmp(src, "GET ", sizeof("GET")) == 0)
        return METHOD_GET;
    if (strncmp(src, "POST ", sizeof("POST")) == 0)
        return METHOD_POST;
    if (strncmp(src, "PATCH ", sizeof("PATCH")) == 0)
        return METHOD_PATCH;
    if (strncmp(src, "PUT ", sizeof("PUT")) == 0)
        return METHOD_PUT;
    if (strncmp(src, "DELETE ", sizeof("DELETE")) == 0)
        return METHOD_DELETE;
    if (strncmp(src, "HEAD ", sizeof("HEAD")) == 0)
        return METHOD_HEAD;
    if (strncmp(src, "CONNECT ", sizeof("CONNECT")) == 0)
        return METHOD_CONNECT;
    if (strncmp(src, "OPTIONS ", sizeof("OPTIONS")) == 0)
        return METHOD_OPTIONS;
    if (strncmp(src, "TRACE ", sizeof("TRACE")) == 0)
        return METHOD_TRACE;
    return METHOD_UNKNOWN;
}

int http_request_is_partial(char *src)
{
    if (!src)
        return 1;
    int crlf = '\r' + '\n';
    char *body = http_request_body(src);
    if (!body)
        return 1;
    int content_len = http_request_content_length(src);
    // no content length, then the entire request was read.
    if (content_len == -2)
        return 0;
    if (content_len == -1)
        return 1;
    return strlen(body) < content_len;
}

int http_request_matches_path_va(char *request, char *format, va_list va)
{
    if (!request)
        return 0;

    if (!format)
        return 0;

    // find path
    while (!isspace(*request++));

    // try matching the route until the first space.
    // example: GET /some/url HTTP/1.1\r\n
    while (!isspace(*request)) {
        if (format[0] == '%' && format[1] == 's') {
            format++;
            format++;

            char *dest = va_arg(va, char *);
            char *tail = dest;

            char terminator = format[0];

            while (request[0] != terminator)
                *tail++ = *request++;

            int nothing_was_copied = dest == tail;

            if (nothing_was_copied)
                return 0;

            *tail = 0;
        } else if (format[0] == '%' && format[1] == 'd') {
            format++;
            format++;

            int *dest = va_arg(va, int *);

            int number_was_found = sscanf(request, "%d", dest);

            if (!number_was_found)
                return 0;

            while (isdigit(request[0]))
                request++;
        }

        if (request[0] != format[0])
            return 0;

        request++;
        format++;
    }

    return 1;
}

int http_request_matches_path(char *src, char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int matches = http_request_matches_path_va(src, fmt, va);
    va_end(va);
    return matches;
}
