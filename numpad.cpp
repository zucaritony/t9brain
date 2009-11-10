#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>		/* getenv(), etc. */
#include <unistd.h>		/* sleep(), etc.  */
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <queue>
#include <pthread.h>
#include "t9char.h"
#include "t9_op.cpp"

#define MIN_WIDTH		480
#define MIN_HEIGHT		213
#define MAX_WIDTH		480
#define MAX_HEIGHT		320

pthread_mutex_t q_mu;
//pthread_cond_t q_cond;
queue<unsigned short> presslog;
Atom wmDeleteMessage, mtp_im_invoker_command, mb_im_invoker_command;

int set_window_properties(Display *dpy, Window toplevel);
bool inarea(int x, int y, int xmin, int ymin, int xmax, int ymax);


using namespace std;

int main(int argc, char** argv) {
	string input2;

//	pthread_cond_init(&1_cond, NULL);
	
	if(argc>1) {
		input2 = argv[1];
	} else {
		input2 = "test";
	}
	
	char *input = (char*)malloc(input2.length());
	strcpy(input, input2.c_str());
	
//	cout << input << "   " << input2 << "\n";

	Display* display;		/* pointer to X Display structure.					 */
	int screen_num;		/* number of screen to place the window on.	*/
	Window win, winRoot;			/* pointer to the newly created window.			*/
	unsigned int display_width, display_height;	/* height and width of the X display.				*/
	unsigned int width, height;	/* height and width for the new window.			*/
	unsigned int win_x, win_y;	/* location of the window's top-left corner. */
	unsigned int win_border_width; /* width of window's border.								*/
	char *display_name = XDisplayName(NULL);//getenv("DISPLAY");	/* address of the X display.			*/
	//char *display_name = ":0"; //getenv("DISPLAY");

	display = XOpenDisplay(display_name);
	assert(display);
	screen_num = DefaultScreen(display);
	
	unsigned int blackcolor = BlackPixel(display, screen_num);
	unsigned int whitecolor = WhitePixel(display, screen_num);

	/* get the geometry of the default screen for our display. */

	display_width = DisplayWidth(display, screen_num);
	display_height = DisplayHeight(display, screen_num);

	/* make the new window occupy 1/9 of the screen's size. */
	width = 480; //display_width;
	height = 270; //(display_height / 3);

	/* the window should be placed at the top-left corner of the screen. */
	win_x = 0;
	win_y = display_height - height;

	/* the window's border shall be 0 pixels wide. */
	win_border_width = 0;
	
	//win = XCreateSimpleWindow(display, RootWindow(display, screen_num), win_x, win_y, width, height, 0, blackcolor, whitecolor);
	win = XCreateWindow(display, DefaultRootWindow(display), win_x, win_y, width, height, 0, CopyFromParent, CopyFromParent, CopyFromParent, 0, NULL);
	
	set_window_properties(display, win);
	
	XMoveWindow(display, win, win_x, win_y);
	XResizeWindow(display, win, width, height);	
	
	XSelectInput(display, win, SubstructureNotifyMask | StructureNotifyMask | ExposureMask | EnterWindowMask | LeaveWindowMask | ButtonPressMask);

	XMapWindow(display, win);
	XSync(display, False);
	
	printf("%d ... %d\n", win_x, win_y);
	printf("%d ... %d\n", width, height);
	printf("%d ... %d\n", display_width, display_height);

	Colormap screen_colormap = DefaultColormap(display, DefaultScreen(display));
	
	//orange:  #f87c00 > r248 g124 b000
	//grey:    #303030 > r48 g48 b48  
	XColor grey, orange;
	
	grey.red = grey.blue = grey.green = 12288;
	
	orange.red = 65535;
	orange.green = 32767;
	orange.blue = 0;
	
	XAllocColor(display, screen_colormap, &grey);
	XAllocColor(display, screen_colormap, &orange);
	
	/* this variable will contain the handle to the returned graphics context. */
	GC gc;
	XGCValues values;
	//values.foreground = blackcolor;
	//values.background = grey.pixel;

	unsigned long valuemask = 0; //GCForeground | GCBackground;

	gc = XCreateGC(display, win, valuemask, &values);
//	gc = create_gc(display, win, 0);
	if (gc < 0) {
	    fprintf(stderr, "XCreateGC: \n");
	}
	
	XSetForeground(display, gc, grey.pixel);
//	XSetBackground(display, gc, orange.pixel);
	
	XSetLineAttributes(display, gc, 1, LineSolid, CapButt, JoinBevel);
	XSetFillStyle(display, gc, FillSolid);
	
	//gc = create_gc(display, win, 0);
	
	XSync(display, False);
	XFlush(display);
	
	//XDrawRectangle(display, win, gc, 0, 0, height, width);
//	XFillRectangle(display, win, gc, 60, 150, 50, 60);
	XFillRectangle(display, win, gc, 0, 0, width, height);

	//sendword(display, input);

	XFlush(display);
	XSync(display, False);
	
	int numcount;
	XFontStruct *font_infos;
	//char **list = XListFonts(display, "*", 2000000, &numcount);
	
//	for(int i=0; i<numcount; i++) {
//		printf("%s\n", list[i]);
//	}
	
	//now we draw the numpad stuff
//	unsigned short top_bar = 27;
	unsigned short buttonh = height / 3;
	
	//drawing the numpad on the leftmost, largest square possible
	unsigned short buttonw = height / 3;
	
	//this is where the wordbar begins
	unsigned short barw = 3 * buttonw;
	
	XSetForeground(display, gc, orange.pixel);
	
	//horizontal lines...
	XDrawLine(display, win, gc, 0, buttonw, barw, buttonw);
	XDrawLine(display, win, gc, 0, (2*buttonw), barw, (2*buttonw));
	
	XDrawLine(display, win, gc, barw, height/4, width, height/4);
	XDrawLine(display, win, gc, barw, height/2, width, height/2);
	XDrawLine(display, win, gc, barw, height*.75, width, height*.75);
	
	//vertical lines...
	XDrawLine(display, win, gc, buttonw, 0, buttonw, height);
	XDrawLine(display, win, gc, (2*buttonw), 0, (2*buttonw), height);
	XDrawLine(display, win, gc, barw, 0, barw, height);
	//XDrawLine(display, win, gc, 0, top_bar+(2*buttonh), width, top_bar+(2*buttonh));
	
	XFontStruct* font_info;

	/* try to load the given font. */
	string font_name = "-misc-fixed-bold-r-normal--18-120-100-100-c-90-iso8859-16";
//	string font_name = "-misc-fixed-bold-r-normal--13-120-75-75-c-70-iso8859-14";
	font_info = XLoadQueryFont(display, font_name.c_str());
	if (!font_info)
	    fprintf(stderr, "XLoadQueryFont: failed loading font '%s'\n", font_name.c_str());
	else
		XSetFont(display, gc, font_info->fid);
	
	//now we start to draw the letters on the numpad, aligning horiz and vert
	string curr[9] = {"punc", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};
	unsigned int sw, sx, sy;
	unsigned int sh = 1.5*(font_info->ascent + font_info->descent);
	
	//how far we are through the letter groups
	unsigned short k = 0;
	
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			string str = curr[k];
			sw = XTextWidth(font_info, str.c_str(), str.length());
			sx = ((buttonw - sw) / 2) + (j * buttonw);
			sy = ((buttonw - sh) / 2) + (i * buttonw);
			XDrawString(display, win, gc, sx, sy, str.c_str(), str.length());
			k++;
		}
	}

	string str;
	
	Winpass_type *win_info = (Winpass_type *) malloc(1*sizeof(Winpass_type));
	win_info->bg = &grey;
	win_info->fg = &orange;
	cout << "GOT PAST COLORS\n";
	
	win_info->display = display;
	win_info->font_info = font_info;
	win_info->win = win;
	win_info->gc = gc;
	win_info->ty = (((height / 4) - sh ) / 2);
	win_info->xl = barw;
	win_info->xr = width;
	win_info->yl = 0;
	win_info->box_w = width-barw;
	win_info->box_h = height/4;
	
	str = "NEXT";
	sw = XTextWidth(font_info, "NEXT", 4);
	sx = (((width - barw) - sw) / 2) + barw;
	sy = (((height / 4) - sh ) / 2) + (height / 4);
	XDrawString(display, win, gc, sx, sy, str.c_str(), 4);

	sy += (height/4);
	str = "PREV";
	sw = XTextWidth(font_info, "PREV", 4);
	sx = (((width - barw) - sw) / 2) + barw;
	XDrawString(display, win, gc, sx, sy, str.c_str(), 4);
	
	sy += (height/4);
	str = "BACKSPACE";
	sw = XTextWidth(font_info, "BACKSPACE", 9);
	sx = (((width - barw) - sw) / 2) + barw;
	XDrawString(display, win, gc, sx, sy, str.c_str(), 9);
	
	XFlush(display);
	XSync(display, False);
	
	cout << "I'm HEREREERE\n";
	int err;
		
	unsigned short padpress;
	err = pthread_mutex_init(&q_mu, NULL);
	cout << "MUTEX INIT ERR:  " << err << "\n";
	
	err = pthread_mutex_lock(&q_mu);
	cout << "MUTEX LOCK ERR:  " << err << "\n";
	
	pthread_t t9thread;
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	cout << "ATTR INIT ERR:  " << err << "\n";

	err = pthread_create(&t9thread, NULL, braint9, (void *)win_info);
	if(err != 0)
		cout << "ERROR  " <<  err << "\n";
	else
		cout << "EVERYTHING AOK\n";
	
	
	XEvent e;
	bool run = true;
	while(run) {
		int bx, by;
		XNextEvent(display, &e);
		cout << e.type << " looking for...." << ButtonPress << "\n";
		switch(e.type) {
			case ButtonPress:
				bx = e.xbutton.x;
				by = e.xbutton.y;
				printf("X: %d   Y: %d\n", bx, by);
				
				//if it's in the numpad area...
				if(bx <= barw) {
					if(inarea(bx, by, 0, 0, buttonw, buttonw)) {
						cout << "PUNC\n";
						padpress = T9_PUNC;
					} else if(inarea(bx, by, buttonw, 0, 2*buttonw, buttonw)) {
						cout << "ABC\n";
						padpress = T9_ABC;
					} else if(inarea(bx, by, 2*buttonw, 0, barw, buttonw)) {
						cout << "DEF\n";
						padpress = T9_DEF;
					} else if(inarea(bx, by, 0, buttonw, buttonw, 2*buttonw)) {
						cout << "GHI\n";
						padpress = T9_GHI;
					} else if(inarea(bx, by, buttonw, buttonw, 2*buttonw, 2*buttonw)) {
						cout << "JKL\n";
						padpress = T9_JKL;
					} else if(inarea(bx, by, 2*buttonw, buttonw, barw, 2*buttonw)) {
						cout << "MNO\n";
						padpress = T9_MNO;
					} else if(inarea(bx, by, 0, 2*buttonw, buttonw, height)) {
						cout << "PQRS\n";
						padpress = T9_PQRS;
					} else if(inarea(bx, by, buttonw, 2*buttonw, 2*buttonw, height)) {
						cout << "TUV\n";
						padpress = T9_TUV;
					} else if(inarea(bx, by, 2*buttonw, 2*buttonw, barw, height)) {
						cout << "WXYZ\n";
						padpress = T9_WXYZ;
					}
				} else {
					//now we know it was on the control pad (right side)
					if(by >= 0 && by < height/4) {
						cout << "WORD AREA\n";
						padpress = T9_SEND;
					} else if(by >= height/4 && by < height/2) {
						cout << "NEXT WORD\n";
						padpress = T9_NEXT;
					} else if(by >= height/4 && by < height*.75) {
						cout << "PREV WORD\n";
						padpress = T9_PREV;
					} else if(by < height) {
						cout << "BACKSPACE\n";
						padpress = T9_BACK;
					}
				}
				XFlush(display);
				
				pthread_mutex_trylock(&q_mu);
				cout << "SENDING  " << padpress << "\n";
				presslog.push(padpress);
				pthread_mutex_unlock(&q_mu);
				
				break;
			case ClientMessage:
				if (e.xclient.data.l[0] == wmDeleteMessage) {
					run = 0;
					break;
				}
		}
	}


//	sleep(4);
	pthread_mutex_destroy(&q_mu);
	pthread_attr_destroy(&attr);

	XCloseDisplay(display);

	return 0;
}

bool inarea(int x, int y, int xmin, int ymin, int xmax, int ymax) {
	if(x >= xmin && x <= xmax && y >= ymin && y<= ymax)
		return true;
	else
		return false;
}

//written by
int set_window_properties(Display *dpy, Window toplevel){
	XWMHints *wm_hints;
	Atom net_wm_state_skip_taskbar, net_wm_state_skip_pager, net_wm_state,
		net_wm_window_type, net_wm_window_type_toolbar,
		net_wm_allowed_actions, net_wm_action_resize;
	Atom actions[1];
	XSizeHints	size_hints;

	size_hints.flags = PMinSize | PMaxSize ;
	size_hints.min_width = size_hints.min_height = MIN_WIDTH;
	size_hints.max_width = size_hints.max_height = MAX_WIDTH;

	XSetWMNormalHints(dpy, toplevel, &size_hints);

	XStoreName(dpy, toplevel, "Keyboard");

	wm_hints = XAllocWMHints();

	if (wm_hints) {
		wm_hints->input = False;
		wm_hints->flags = InputHint;
		XSetWMHints(dpy, toplevel, wm_hints);
		XFree(wm_hints);
	}

	wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	mtp_im_invoker_command = XInternAtom(dpy, "_MTP_IM_INVOKER_COMMAND", False);
	mb_im_invoker_command = XInternAtom(dpy, "_MB_IM_INVOKER_COMMAND", False);
	XSetWMProtocols(dpy, toplevel, &wmDeleteMessage, 1);

	net_wm_state_skip_pager = XInternAtom(dpy, "_NET_WM_STATE_SKIP_PAGER", False);
	net_wm_state_skip_taskbar = XInternAtom(dpy, "_NET_WM_STATE_SKIP_TASKBAR", False);
	net_wm_state = XInternAtom (dpy, "_NET_WM_STATE", False);
	net_wm_window_type = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE" , False);
	net_wm_window_type_toolbar = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_TOOLBAR", False);

	net_wm_allowed_actions = XInternAtom(dpy, "_NET_WM_ALLOWED_ACTIONS", False);
	net_wm_action_resize = XInternAtom(dpy, "_NET_WM_ACTION_RESIZE", False);

	actions[0] = net_wm_action_resize;

	XChangeProperty (dpy, toplevel, net_wm_state, XA_ATOM, 32, PropModeAppend,
			(unsigned char *)&net_wm_state_skip_taskbar, 1);
	XChangeProperty (dpy, toplevel, net_wm_state, XA_ATOM, 32, PropModeAppend,
			(unsigned char *)&net_wm_state_skip_pager, 1);
	XChangeProperty(dpy, toplevel, net_wm_window_type, XA_ATOM, 32,
			PropModeReplace, (unsigned char *) &net_wm_window_type_toolbar, 1);
	XChangeProperty(dpy, toplevel, net_wm_allowed_actions, XA_ATOM, 32,
			PropModeReplace, (unsigned char *) &actions, 1);

	return 0;
}
/*
#include <gtk/gtk.h>

int main( int	 argc,
					char *argv[] )
{
		GtkWidget *window;
		
		gtk_init (&argc, &argv);
		
		window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_widget_show	(window);
		
		gtk_main ();
		
		return 0;
}
*/
