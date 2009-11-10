/*
* Window_With_Name: routine to locate a window with a given name on a display.
* If no window with the given name is found, 0 is returned.
* If more than one window has the given name, the first
* one found will be returned. Only top and its subwindows
* are looked at. Normally, top should be the RootWindow.
*/
Window
Window_With_Name(Display *display, Window top, const char *name)
{
Window *children, dummy;
unsigned int nchildren;
unsigned int i;
Window w=0;
char *window_name;

if (XFetchName(display, top, &window_name) && !strcmp(window_name, name))
return(top);

if (!XQueryTree(display, top, &dummy, &dummy, &children, &nchildren))
return(0);

for (i=0; i<nchildren; i++) {
w = Window_With_Name(display, children[i], name);
if (w)
break;
}
if (children) XFree ((char *)children);
return(w);
}

int
main(int argc, char *argv[])
{
display = XOpenDisplay(0);
if (!display)
exit(EXIT_FAILURE);

Window window_root = DefaultRootWindow(display);
if (!window_root)
exit(EXIT_FAILURE);

Window window_found = Window_With_Name(display, window_root, “put here the name of the window”);

if (window_found)
{
XSetInputFocus(display, window_found, RevertToNone, CurrentTime);

// Send keys to window_found
