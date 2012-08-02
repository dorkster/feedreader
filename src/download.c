#include <curl/curl.h>
#include <stdlib.h>

#include "download.h"

size_t writecurlfile(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

void download (int id, char *uri) {
    CURL *curl_handle;
    
    FILE *outfile;
    char filename[BUFSIZ];
    
    system("mkdir -p /tmp/feedreader/");
    sprintf(filename,"/tmp/feedreader/%d",id);
    
    curl_global_init(CURL_GLOBAL_ALL);
    outfile = fopen(filename, "w+");
    curl_handle = curl_easy_init();

    if(uri != NULL) curl_easy_setopt(curl_handle, CURLOPT_URL, uri);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, outfile);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writecurlfile);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, NETTIMEOUT);

    curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);
    
    curl_global_cleanup();
    
    fclose(outfile);
}
