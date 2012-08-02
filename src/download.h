#ifndef FETCH_H
#define FETCH_H

#include <stdio.h>
#define NETTIMEOUT 20 // in seconds

size_t writecurlfile(void *ptr, size_t size, size_t nmemb, FILE *stream);
void download (int id, char *uri);

#endif
