/*
    feedreader
    Copyright (C) 2013 Justin Jacobs

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

#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <glib.h>
#include <stdio.h>

typedef struct FileParser{
    FILE *file;
    char buffer[BUFSIZ];
    char *line;
    char *key;
    char *val;
    char *val_next;
    int val_cursor;
    gboolean first_tok;
}FileParser;

FileParser* fileOpen(char *filename);
char* fileNext(FileParser* f);
char* fileGetKey(FileParser *f);
char* fileGetVal(FileParser *f);
char* fileGetValNext(FileParser* f);
char* fileGetLine(FileParser *f);
char* fileEatNextVal(FileParser* f);
void fileClose(FileParser *f);

#endif
