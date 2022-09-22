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
#include <string.h>

#include "download.h"
#include "util.h"

DownloadData download_data = {};

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct DownloadData *data = (struct DownloadData *)userp;

    data->memory = realloc(data->memory, data->size + realsize + 1);
    if(!data->memory) return 0;

    memcpy(&(data->memory[data->size]), contents, realsize);
    data->size += realsize;
    data->memory[data->size] = 0;

    return realsize;
}

void download (int id, char *uri) {
    download_clear_data();

    CURL *curl_handle;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    if(uri) curl_easy_setopt(curl_handle, CURLOPT_URL, uri);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&download_data);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, NETTIMEOUT);

    curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);

    curl_global_cleanup();
}

void download_clear_data() {
    if (download_data.memory) {
        free(download_data.memory);
        download_data.memory = NULL;
        download_data.size = 0;
    }
}
