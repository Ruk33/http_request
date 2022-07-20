#include <stdio.h>
#include <stdlib.h> // atoi
#include <string.h> // strlen
#include <ctype.h> // tolower, isspace

// -2 = no content length found
// -1 = partial found
int content_length(char *src)
{
    char *content_length_key = "content-length:";
    if (!src)
        return -2;
    while (*src) {
        char *key = content_length_key;
        while (*src && tolower(*src++) == *key && key++);
        // read something but not entirely.
        // ie, "content-le", partial match.
        if (!*src && key != content_length_key)
            return -1;
        // null only if the entire key was found.
        if (*key)
            continue;
        // skip white space after ":".
        while (*src && isspace(*src) && src++);
        char *value = src;
        // skip past the number.
        while (*src && !isspace(*src) && src++);
        // make sure the entire number is read.
        // if it doesn't end on a new line, it means
        // the entire request hasn't been sent.
        if (!isspace(*src))
            return -1;
        return atoi(value);
    }
    return -2;
}

enum method {
    METHOD_UNKNOWN,
    METHOD_GET,
    METHOD_HEAD,
    METHOD_POST,
    METHOD_PUT,
    METHOD_DELETE,
    METHOD_CONNECT,
    METHOD_OPTIONS,
    METHOD_TRACE,
    METHOD_PATCH,
};

enum method get_method(char *src)
{
    if (!src)
        return METHOD_UNKNOWN;
    if (strcmp(src, "GET ") == 0)
        return METHOD_GET;
    if (strcmp(src, "HEAD ") == 0)
        return METHOD_HEAD;
    if (strcmp(src, "POST ") == 0)
        return METHOD_POST;
    if (strcmp(src, "PUT ") == 0)
        return METHOD_PUT;
    if (strcmp(src, "DELETE ") == 0)
        return METHOD_DELETE;
    if (strcmp(src, "CONNECT ") == 0)
        return METHOD_CONNECT;
    if (strcmp(src, "OPTIONS ") == 0)
        return METHOD_OPTIONS;
    if (strcmp(src, "TRACE ") == 0)
        return METHOD_TRACE;
    if (strcmp(src, "PATCH ") == 0)
        return METHOD_PATCH;
    return METHOD_UNKNOWN;
}

int is_partial(char *src)
{
    if (!src)
        return 1;
    int crlf = '\r' + '\n';
    char *body = src;
    // skip first line
    while (!isspace(*body) && body++);
    while (isspace(*body) && body++);
    // minimum size is 4 bytes (two crlf)
    if (body - src < 4)
        return 1;
    // find last two crlf characters.
    while (*body &&
           !((*(body - 3) + *(body - 2)) == crlf &&
             (*(body - 1) + *(body - 0)) == crlf) &&
           body++);
    if (!*body)
        return 1;
    // skip last new line. start at the body.
    body++;
    int body_len = content_length(src);
    // no content length, then the entire request was read.
    if (body_len == -2)
        return 0;
    if (body_len == -1)
        return 1;
    return strlen(body) < body_len;
}

int is_path(char *src, char *path)
{
    // skip method
    while (!isspace(*src++));
    // the start of the path + length should
    // end up in a space/newline if it matches.
    // example: GET /my_path HTTP/1.1
    size_t len = strlen(path);
    if (!isspace(*(src + len)))
        return 0;
    return strncmp(src, path, len) == 0;
}

int main(int argc, char **argv)
{
    char *request =
        "GET /hello.htm HTTP/1.1\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
        "Content-Length: 8\r\n"
        "\r\n"
        "12345678"
        ;
    printf("method = %d, is partial: %s (%d)\n",
           get_method(request),
           is_partial(request) ? "true" : "false",
           is_partial(request));
}