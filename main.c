#include <assert.h>
#include <stdio.h>
#include "http_request.h"

int main(int argc, char **argv)
{
    char *request =
        "GET /hello.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
        "Content-Length: 8\r\n"
        "\r\n"
        "12345678"
        ;
    printf("method = %d, is partial: %s\n",
           http_request_get_method(request),
           http_request_is_partial(request) ? "true" : "false");
    printf("matches '%s' route: %s\n", "/hello.html",
           http_request_matches_path(request, "/hello.html") ? "true" : "false");
    printf("body: '%s'\n", http_request_body(request));
    
    return 0;
}