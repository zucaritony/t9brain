#include <string.h>
#include "letters.c"
#include "t9char.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

typedef struct {
	XColor *bg, *fg;
	Display *display;
	Window win;
	XFontStruct *font_info;
	GC gc;
	unsigned int xl, xr, yl, box_w, box_h, ty;
} Winpass_type;

bool frocmp(string sub, string full);
string strlower(string str);
XKeyEvent createKeyEvent(Display *display, Window &win, Window &winRoot, bool press, int keycode, int modifiers);
void sendword(Display *display, const char *input);
void update_panel(Winpass_type *info, const char *str);

extern queue<unsigned short> presslog;
extern pthread_mutex_t q_mu;

int timer() {
	timeval tim;
	int err = gettimeofday(&tim, NULL);
	
	if(err != 0)
		cout << "TIME ERROR\n";
	
	return tim.tv_usec;
}




//int main(int argc, char** argv) {
void *braint9(void *args) {
	//this is the function included in letters.c ...I didn't want to scroll through 25 lines of drivel all the time
	int t1, t2, t3, finalt;
	long d1, d2, d3, d4;

	Winpass_type *win_essen = (Winpass_type *)args;
	Display *display = win_essen->display;
	char* letters[10];
	make_letters(letters);
	
	cout << "BRAIN JUST STARTED\n";
	
	//len is the length of the current string being scrutinized, or the number
	//of times that a number has been pressed to find the right word
	//used in backspace implementation
	unsigned int len = 0;
	
	//nletts is the number of letters in the letters[i] referenced string (ie #1's "abc" has 3 letters, nletts=3)
	//nump is "number pressed" and is the numerical representation of the numerical character grabbed by 'grab'
	//char nump,nletts;
	unsigned short nletts, nump;
	nump = T9_ERR;
	
	//grab is the buffer for the character grab function. this is where the captured key value goes
	//char grab;
	
	//reading from this file for all words... this is on an ubuntu 9.04 system
	//if you don't have this file, go find a dictionary file online that fits the following format:
	//alphabetical (or ASCIIbetical) order, and it can order them like A-Z-a-z or like the regular Aa-Zz
	ifstream dict;
	//dict.open("/usr/share/dict/american-english");
	
	//must use a multiset because Compare orders them by string length, and the set
	//interprets this as only having 1 word at every length value (1 word with length 3, 1 of length 4, etc)
	//multiset lets you have as many of any length you want
	vector<string> possible, poss_temp, curr_dict, dict_temp;
	vector<string>::iterator dict_iter, poss_iter;
	vector<bool> dict_list, poss_list;

	//temporary location for the current dictionary word
	string dword;
	
	//while(dict >> dword)
	//	curr_dict.insert(dword);
	
	//nwords is just a word counter on the phrase string
	unsigned int nwords = 0;
	
	//phrase is the combination of all words type out so far
	string phrase = "";
	
	//while(!kbhit());
//	grab = getchar();
	//while(!isdigit(grab)) { cout << "push a digit\n"; while(!kbhit()); grab = getchar(); }
	
	cout << "TRYING TO GRAB A CHARACTER\n";
	while(!nump) {
		pthread_mutex_lock(&q_mu);
		if(presslog.size() > 0) {
			nump = presslog.front();
			presslog.pop();
		} else
			cout << "NOTHING FOUND\n";
		pthread_mutex_unlock(&q_mu);
	}
	
	cout << nump << "-- got here\n";
		
	cout << "before inf while\n";

	vector<string>::iterator curr;
	bool setswitch = false;
	bool haschar = false;
	string input, wordloc;

	while(1) {	
		cout << "***time  " << t1 << " - " << t2 << "  = " << t1-t2 << "\n";
	
		cout << "T9 BRAIN GRABBED " << nump << "\n";

		switch(nump) {
			case T9_NEXT:
				//they pushed the NEXT button, which means we either increment the current word or add a space
				if(len>0) {
					curr++;
					if(setswitch && curr == possible.end())
						curr = possible.begin();
					else if(!setswitch && curr == curr_dict.end())
						curr = curr_dict.begin();
				} else {
				input = " ";
					sendword(display, input.c_str());
				}
				break;
				
			case T9_PREV:
				//cout << "pushed left\n";
				if(len>0) {
					if(setswitch && curr == possible.begin())
						curr = possible.end();
					else if(!setswitch && curr == curr_dict.begin()) {
						curr = curr_dict.end();
					}
					//we have to go backwards whether it's the end of the set or not, because the end iterator
					//points to one element past the last one so we have to go back to the last one
					curr--;							
				} else {
					input = " ";
					sendword(display, input.c_str());
				}
				break;
		
			case T9_SEND:
				cout << "SHOULD SEND, LEN= " << len << "\n";
				if(len>0) {
					cout << "SENDING WORD  " << (*curr) << "\n";
					input = (*curr);
					input += " ";
					sendword(display, input.c_str());
					
					haschar = false;
					
					nwords++;
					len = 0;
					dict.close();
				
					possible.clear();
					curr_dict.clear();
				}
				break;
				
			case T9_BACK:	
				haschar = false;
				if(len>0) {
					len = 0;
				

				
					dict.close();
					possible.clear();
					curr_dict.clear();
				} else {
					input = "\b";
					sendword(display, input.c_str());
					cout << "DONE BACKSPACING\n";
				}
				break;				
		
			default:
				t1 = timer();
				
				nletts = strlen(letters[nump]);
				unsigned int count = 0;
				int i,j;
				
				//here is where we set up the 'possible' set
				if(len==0) {
					cout << "INITIALIZING " << letters[nump] << "\n";
					string dummy;
					for(int i=0; i<nletts; i++) {
						//only way to get C++ to actually add a character as a string...
						dummy = "";
						dummy += letters[nump][i];
				
						possible.push_back(dummy);
						poss_list.push_back(false);
					//	curr_dict.insert(dummy);			
		
						wordloc = "/usr/bin/words/";
					//	wordloc = "/home/halcyon/moko_halc/t9/words/";
						wordloc += letters[nump];

					}		
						
					dict.open(wordloc.c_str());
					cout << dict.fail() << " --failed?-- " << wordloc << " --- OPENING WORDS\n";
				} else {
					cout << "updating possible list " << possible.size() << "\n";
					poss_temp.clear();
					possible.swap(poss_temp);
					for(curr = poss_temp.begin(); curr != poss_temp.end(); curr++) {
						poss_list.push_back(false);
						for(i=0; i<nletts; i++) {
							possible.push_back((*curr)+letters[nump][i]);
						}
					}
				}
				
				t2 = timer();
				d1 = t2 - t1;
				cout << "***************time after initialization " << t2 << " - " << t1 << " = " << d1 << "\n";

				
				cout << "updating curr_dict\n";
				
				len++;
				
				t1 = timer();
				curr_dict.clear();
//				cout << "111\n";
				while(dict >> dword) {
					//cout << dword.length() << "  DWORD LENGTH\n";
					if(dword.length() == len)
						curr_dict.push_back(dword);
					else if(curr_dict.size() == 0)
						curr_dict.push_back(dword);
					else
						break;
				}
				
				t2 = timer();
				d2 = t2 - t1;
				cout << "*************time after curr_dict clear and reload " << t2 << " - " << t1 << " = " <<  d2 << "\n";

				//i and j are the current locations in the 'possible' and 'curr_dict' sets for reference with their
				//removal-marking vector counterparts, 'poss_list' and 'dict_list' respectively
//				cout << curr_dict.size() << " CURR DICT SIZE   222\n";
				bool isin;
				
				t1 = timer();				
				dict_iter = curr_dict.begin();
				while(dict_iter != curr_dict.end()) {
					isin = false;
					for(j=0, poss_iter = possible.begin(); poss_iter != possible.end(); poss_iter++, j++) {
						//if we find a use for the possible word prefix and the dictionary word, we keep track of it
						if(frocmp((*poss_iter), (*dict_iter))) {
							isin = true;
							poss_list[j] = true;
						}
					}
					
					if(!isin)
						dict_iter = curr_dict.erase(dict_iter);
					else
						dict_iter++;
				}
				
				t2 = timer();
				d3 = t2 - t1;
				cout << "*****************time after updating curr_dict with poss " << t2 << " - " << t1 << " = " << d3 << "\n";

	//		poss_iter = possible.begin();
	//			while(poss_iter != possible.end()) {
					
		//		}
				

				
				if(curr_dict.size() > 0) {
					curr = curr_dict.begin();
					setswitch = false;
					haschar = true;
				} else if(possible.size() > 0) {
					curr = possible.begin();
					haschar = setswitch = true;
				} else {
					cout << "neither list has any elements\n";
					haschar = false;
				}
				

				
				break;
		}
		
		t1 = timer();
//		cout << "hahahaha\n";
		if(haschar) {
			cout << (*curr) << "\n";
			update_panel(win_essen, (*curr).c_str());
		} else {
			cout << "???\n";
			update_panel(win_essen, "???");
		}
		
		t2 = timer();
		d4 = t2 - t1;
		cout << "*****************time waiting for output " << t2 << " - " << t1 << " = " << d4 << "\n";
		
		cout << "WAITING FOR INPUT\n";
		nump = T9_ERR;
		while(nump == T9_ERR) {
			pthread_mutex_lock(&q_mu);
			if(presslog.size() > 0) {
				nump = presslog.front();
				presslog.pop();
			}
			pthread_mutex_unlock(&q_mu);
		}
	}
	
//	printf("%d\n", nump);
	
	pthread_exit(NULL);
}

void update_panel(Winpass_type *info, const char *str) {
	cout << "IN UPDATE\n";
	unsigned int tx, ty, tw, xl, xr, box_w, box_h, yl;
	Window win = info->win;
	GC gc = info->gc;
	
	tw = XTextWidth(info->font_info, str, strlen(str));
	cout << "AFTER 1ST DEREF\n";
	ty = info->ty;
	
	yl = info->yl;
	xl = info->xl;
	xr = info->xr;
	box_w = info->box_w;
	box_h = info->box_h;
	
	tx = (xl + (((xr - xl) - tw) / 2));
	
	XSetForeground(info->display, gc, info->bg->pixel);
	XFillRectangle(info->display, info->win, info->gc, xl+1, yl, box_w, box_h);
	XFlush(info->display);
	XSync(info->display, False);
	XSetForeground(info->display, gc, info->fg->pixel);	
	
	cout << "BEFORE DRAW\n";
	if(strlen(str) > 0) {
		XDrawString(info->display, win, gc, tx, ty, str, strlen(str));
		XFlush(info->display);
		XSync(info->display, False);
	}
}

XKeyEvent createKeyEvent(Display *display, Window &win,
                           Window &winRoot, bool press,
                           int keycode, int modifiers)
{
   XKeyEvent event;

   event.display     = display;
   event.window      = win;
   event.root        = winRoot;
   event.subwindow   = None;
   event.time        = CurrentTime;
   event.x           = 1;
   event.y           = 1;
   event.x_root      = 1;
   event.y_root      = 1;
   event.same_screen = True;
   event.keycode     = XKeysymToKeycode(display, keycode);
   event.state       = modifiers;

   if(press)
      event.type = KeyPress;
   else
      event.type = KeyRelease;

   return event;
}

void sendword(Display *display, const char *input) {
// Send a fake key press event to the window.
	cout << "IN SENDWORD WITH " << input << "\n";

	XKeyEvent event;
   	Window winFocus;
   	Window winRoot = XDefaultRootWindow(display);
	int revert, len;
	XGetInputFocus(display, &winFocus, &revert);
	
	len = strlen(input);

	
	cout << "SENDWORD BEFORE LOOP\n";
	if(!strcmp(input, "\b")) {
		cout << "BACKSPACING NOW\n";
		event = createKeyEvent(display, winFocus, winRoot, true, XK_BackSpace, 0);//XStringToKeysym(input[i]), 0);
		XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);

		event = createKeyEvent(display, winFocus, winRoot, false, XK_BackSpace, 0);//, XStringToKeysym(input[i]), 0);
		XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
		
		return;
	}
	
	for(int i=0; i<len; i++) {
		if(isupper(input[i])) {
			event = createKeyEvent(display, winFocus, winRoot, true, XK_Shift_L, 0);//XStringToKeysym(input[i]), 0);
			XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
		}
		
		event = createKeyEvent(display, winFocus, winRoot, true, (int)input[i], 0);//XStringToKeysym(input[i]), 0);
		XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);

		event = createKeyEvent(display, winFocus, winRoot, false, (int)input[i], 0);//, XStringToKeysym(input[i]), 0);
		XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);

		if(isupper(input[i])) {
			event = createKeyEvent(display, winFocus, winRoot, false, XK_Shift_L, 0);//XStringToKeysym(input[i]), 0);
			XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
		}
	}
}

string strlower(string str) {
	string into = "";
	unsigned int len = str.length();
	for(int i=0; i<len; i++)
		into += tolower(str[i]);
		
	return into;
}

bool frocmp(string sub, string full) {
	//cout << "inside frocmp\n";
	unsigned int lensub, lenfull;
	lensub = sub.length();
	lenfull = full.length();
	full = strlower(full);
	
//	cout << sub << "  " << full << "\n";
	
	if(lensub == lenfull) {
		return sub == full;
	} else {
		for(int i=0; i<lensub; i++) {
			if(sub[i] != full[i])
				return false;
		}
		return true;
	}
}
