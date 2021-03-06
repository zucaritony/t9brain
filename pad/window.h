/*
 *  Copyright (C) 2008, 2009 Charles Clement <caratorn _at_ gmail.com>
 *
 *  This file is part of qwo.
 *
 *  qwo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301  USA
 *
 */

#ifndef WINDOW_H
#define WINDOW_H 1

//#include <config.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <X11/extensions/shape.h>
#include <X11/extensions/XTest.h>

//#include <Imlib2.h>

#define DEFAULT_WIDTH   300
#define DEFAULT_HEIGHT  DEFAULT_WIDTH



#define INCREMENT		60

#define MAX_REGIONS 9
#define MAX_POINTS  9

#define DEFAULT_DELTA                   (DEFAULT_WIDTH >> 4)

#define DEFAULT_GRID_COLOR				"orange"

#define MAX_CONFIG_PATH 256

#define DATA_PATH		DATADIR "/" PACKAGE_NAME "/"

#define MAX_IMAGE_NAME 10
#define MAX_IMAGE_PATH 50

#define MAX_IMAGES		3

#define IMAGE_SUFFIXE ".png"

#define FILL_REGION4(a, b, c, d)        {a, b, c, d, a, a, a, a, a}
#define FILL_REGION5(a, b, c, d, e)     {a, b, c, d, e, a, a, a, a}
#define FILL_REGION8(a, b, c, d, e, f, g, h)     {a, b, c, d, e, f, g, h, a}

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define DIRECTION(a, b)  \
		(abs(b - a) == 1) ? ( (b - a) == 1) : (b - a) < 0

#define FG_COLOR		0
#define BG_COLOR		1
#define GRID_COLOR		2

extern XColor color_scheme[3];
extern unsigned int defined_colors;

extern Pixmap char_pixmaps[3];

extern Atom wmDeleteMessage, mtp_im_invoker_command, mb_im_invoker_command;

typedef enum {
	KeyboardNone = 0,
	KeyboardShow,
	KeyboardHide,
	KeyboardToggle
} KeyboardOperation;


void update_display(Display *dpy, Window toplevel, int shift, int help);
Window resize_window(Display *dpy, Window win, int number);
int init_window(Display *dpy, Window win, char *user_geometry);
void close_window(Display *dpy, Window win);
XColor convert_color(Display *dpy, const char *color);

#endif /* WINDOW_H */
