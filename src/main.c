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

#include <stdlib.h>

#include "gui.h"
#include "util.h"

int main( int argc, char* argv[] ) {
    gtk_init( &argc, &argv );
    
    set_browser("xdg-open");
    create_dirs();
    create_tray_icon();
    create_primary_menu();

    gtk_main();

    // Remove temporary files on exit
    clean_up();
    
    return 0;
}
