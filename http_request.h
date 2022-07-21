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

// on success, the length of the body
// (NOT the entire request, meaning
// headers + body) is returned.
// -1 is returned if it's a partial read,
// meaning, the tag was found but couldn't be
// fully read.
// -2 is returned if no content lenght header was found.
int http_request_content_length(char *request);
// get a pointer to the beginning of the body (past headers)
// if the request is partial, NULL is returned.
char *http_request_body(char *request);
enum http_request_method http_request_get_method(char *request);
int http_request_is_partial(char *request);
int http_request_matches_path(char *request, char *path);

#endif //HTTP_REQUEST_H
