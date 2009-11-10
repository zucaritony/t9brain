#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>

int main() {
	Display *dpy = XOpenDisplay(NULL);
	assert(dpy);

	int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
	int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

	Window w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 2001, 100, 0, blackColor, blackColor);

	XSelectInput(dpy, w, StructureNotifyMask);
	XMapWindow(dpy, w);

	return 0;
}
