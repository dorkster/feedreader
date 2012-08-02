#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include <glib.h>

GtkWidget *main_menu;
GtkWidget *alt_menu;
GtkStatusIcon *status_icon;

gboolean create_primary_menu();
void destroy (GtkWidget *widget, gpointer data);
void primary_menu (GtkStatusIcon *status_icon, gpointer user_data);
void alternate_menu (GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer user_data);

#endif
