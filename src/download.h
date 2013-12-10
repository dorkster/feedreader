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

#ifndef FETCH_H
#define FETCH_H

#include <stdio.h>
#define NETTIMEOUT 20 // in seconds

typedef struct DownloadData {
    char* memory;
    size_t size;
}DownloadData;

DownloadData download_data;
void download (int id, char *uri);
void download_clear_data();

#endif
