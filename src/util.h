#ifndef UTIL_H
#define UTIL_H

#include <glib.h>

#define BROWSER "xdg-open"
#define UPDATE_INTERVAL 600 // in seconds

char *homedir;

void open_link(gpointer data);
void clean_up();

#endif