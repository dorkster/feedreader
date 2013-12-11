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

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileparser.h"

FileParser* fileOpen(char *filename) {
    FileParser* f = malloc(sizeof(FileParser));
    if (f != NULL) {
        f->file = fopen(filename,"r");
        if (f->file != NULL) {
            f->line = NULL;
            f->key = NULL;
            f->val = NULL;
            f->val_next = NULL;
            f->val_cursor = 0;
            f->first_tok = TRUE;
        } else {
            free(f);
            f = NULL;
        }
    }

    return f;
}

char* fileNext(FileParser* f) {
    if (f == NULL) return NULL;

    f->key = NULL;
    f->val = NULL;
    if (f->val_next != NULL) {
        free(f->val_next);
        f->val_next = NULL;
    }
    f->val_cursor = 0;
    f->first_tok = TRUE;

    if (f->file == NULL) return NULL;


    char *tmp = fgets(f->buffer,BUFSIZ,f->file);
    if (tmp != NULL) {
        if (f->line != NULL) free(f->line);
        f->line = g_strdup(tmp);
    }
    else return NULL;

    if (f->line != NULL && f->line[0] != '#') {
        f->key = strtok(f->line,"=");
        f->val = strtok(NULL,"\n");
    }

    return f->line;
}

char* fileGetKey(FileParser* f) {
    if (f == NULL) return "";

    if (f->key) return f->key;
    else return "";
}

char* fileGetVal(FileParser* f) {
    if (f == NULL) return "";

    if (f->val) return f->val;
    else return "";
}

char* fileGetValNext(FileParser* f) {
    if (f == NULL) return "";

    if (f->val_next != NULL) {
        free(f->val_next);
        f->val_next = NULL;
    }
    int i;
    int len = strlen(f->val);
    int new_cursor = 0;
    for (i=f->val_cursor; i<=len; i++) {
        if (f->val[i] != ',' && f->val[i] != '\n') {
            f->val_cursor++;
            new_cursor++;
            f->val_next = realloc(f->val_next, f->val_cursor);
            f->val_next[new_cursor-1] = f->val[i];
        } else {
            f->val_cursor++;
            new_cursor++;
            f->val_next = realloc(f->val_next, f->val_cursor);
            f->val_next[new_cursor-1] = '\0';
            break;
        }
    }
    if (f->val_next) return f->val_next;
    else return "";
}

char* fileGetLine(FileParser* f) {
    if (f == NULL) return "";

    if (f->line) return f->line;
    else return "";
}

char* fileEatNextVal(FileParser* f) {
    if (f == NULL) return "";

    if (f->val && f->first_tok) {
        f->first_tok = FALSE;
        return strtok(f->val,",");
    }
    else if (f->val) return strtok(NULL,",");
    else return "";
}

void fileClose(FileParser *f) {
    if (f == NULL) return;

    if (f->val_next != NULL)
        free(f->val_next);
    if (f->line != NULL)
        free(f->line);
    if (f->file != NULL)
        fclose(f->file);

    free(f);
}

