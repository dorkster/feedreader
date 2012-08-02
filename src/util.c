#include <gtk/gtk.h>
#include <stdlib.h>

#include "util.h"

void open_link(gpointer data) {
    gchar *link = gtk_widget_get_tooltip_text(data);
    if(link != NULL) {
        gchar *command = g_strconcat(BROWSER," \"",link,"\" &",NULL);
        g_print("%s\n",command);
        system(command);
        g_free(command);
    }
}

void clean_up() {
    system("rm -rf /tmp/feedreader/");
}

