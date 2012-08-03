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
