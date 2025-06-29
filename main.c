#include <assert.h>
#include <stdio.h>
#include "http_request.c"

int main(int argc, char **argv)
{
    char *request =
        "GET /users/42/edit/and_return HTTP/1.1\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
        "Content-Length: 8\r\n"
        "\r\n"
        "12345678"
        ;
    char user_action[32];
    int user_id = 0;
    printf("method = %d, is partial: %s\n",
           http_request_get_method(request),
           http_request_is_partial(request) ? "true" : "false");
    printf("matches '%s' route: %s\n", "/users/42/edit/and_return",
           http_request_matches_path(request, "/users/%d/%s/and_return", &user_id, user_action) ? "true" : "false");
    printf("user id: %d, user action: '%s'\n", user_id, user_action);
    printf("body: '%s'\n", http_request_body(request));
    
    // tests
    request = 
        "GET /my/complex/path.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
        "\r\n"
        ;
    assert(http_request_get_method(request) == METHOD_GET);
    assert(http_request_content_length(request) == -2);
    assert(!http_request_is_partial(request));
    assert(http_request_matches_path(request, "/my/complex/path.html"));
    
    // partial
    request = 
        "POST /my/path.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
        "" // partial
        ;
    assert(http_request_get_method(request) == METHOD_POST);
    assert(http_request_is_partial(request));
    assert(http_request_matches_path(request, "/my/path.html"));
    
    // partial body
    request = 
        "POST /my/path.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
        "Content-Length: 8\r\n"
        "\r\n"
        "1234567" // partial
        ;
    assert(http_request_content_length(request) == 8);
    assert(http_request_is_partial(request));
    
    // partial content-length
    request = 
        "POST /my/path.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
        "Content-Length: 8" // partial
        ;
    assert(http_request_content_length(request) == -1);

    printf("all done\n");
    
    return 0;
}