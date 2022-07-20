HTTP REQUEST
  simple lib to know:
  - the method of a request (GET, POST, etc.)
  - if a path matches (ie, is uri /my/path.html?)
  - if the request is complete or partial.
  - start of the body.

NOTES
  - no memory is allocated.
  - NULL can be safely used for all parameters.
  - strings MUST be NULL terminated.

INSTALLATION
  copy http_request.h/c in your project.

HOW TO USE IT
  check main.c for code usage examples.
