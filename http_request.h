#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

enum http_request_method {
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

// on success, the value of the content-length
// header is returned. keep in mind, this does
// not necessarily matches with the actual
// length of the content.
// -1 is returned if it's a partial read,
// meaning, the tag was found but couldn't be
// fully read.
// -2 is returned if no content length header was found.
int http_request_content_length(char *request);
// get a pointer to the beginning of the body (past headers)
// if the request is partial, NULL is returned.
char *http_request_body(char *request);
enum http_request_method http_request_get_method(char *request);
// 1 if the request is not complete.
// 0 if the request is complete and the
// body of the request matches the length
// sent in the content-length header.
// if no content-length was sent, checks 
// if the request ends with the two final 
// crlf characters.
int http_request_is_partial(char *request);
// check if the request matches a particular uri.
// 1 if the uri matches.
// 0 if the uri does not match.
// example: 
// int user_id = 0;
// http_request_matches_path(request, "/users/%d/update", &user_id);
// given a uri "/users/42/update", user_id will be 42.
// supported formats: %s for string, %d for integers.
int http_request_matches_path_va(char *request, char *format, va_list params);
int http_request_matches_path(char *src, char *fmt, ...);

#endif //HTTP_REQUEST_H
