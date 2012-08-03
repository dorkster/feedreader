/*
    feedreader
    Copyright (C) 2011-2012  Justin Jacobs

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
