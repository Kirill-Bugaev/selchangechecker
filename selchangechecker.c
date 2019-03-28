/* 
 * check SELECTION changes each <pause arg=500> (in milliseconds)	* 
 * and run <callback> if change occurs								*
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <inttypes.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Box.h>
#include <X11/Xlib.h>

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void
/* Define to 1 if the `setpgrp' function takes no argument. */
#define SETPGRP_VOID 1

typedef struct {
  String  selection_name;
  String  callback_path;
  String  buttonup_option;
  int     pause;
  char*   value;
  int     length;
  Atom    selection;
  int     buttonup;
} OptionsRec;

static XrmOptionDescRec optionDesc[] = {
  {"-selection", "selection", XrmoptionSepArg, NULL},
  {"-s",         "selection", XrmoptionSepArg, NULL},
  {"-callback",  "callback",  XrmoptionSepArg, NULL},
  {"-c",    	 "callback",  XrmoptionSepArg, NULL},
  {"-pause",     "pause",     XrmoptionSepArg, NULL},
  {"-p",         "pause",     XrmoptionSepArg, NULL},
  {"-buttonup",  "buttonup",  XrmoptionNoArg,  "on"},
};

XtAppContext context;
OptionsRec options;
Widget box;
Display* dpy;
Atom selection;
int buffer;

#define Offset(field) XtOffsetOf(OptionsRec, field)
static XtResource resources[] = {
  {"selection", "Selection", XtRString, sizeof(String),
    Offset(selection_name), XtRString, "CLIPBOARD"},
  {"callback", "Callback", XtRString, sizeof(String),
    Offset(callback_path), XtRString, "notify-send --icon=info \"selection changed\""},
  {"pause", "Pause", XtRInt, sizeof(int),
    Offset(pause), XtRImmediate, (XtPointer)500},
  {"buttonup", "ButtonUp", XtRString, sizeof(String),
    Offset(buttonup_option), XtRString, "off"},
};
#undef Offset

int Syntax(char *call);
static void TrapSignals();
static void CloseStdFds();
void timeout(XtPointer p, XtIntervalId* i);

int main(int argc, char *argv[])
{
	Widget top;

	top = XtVaAppInitialize(&context, "SelChangeChecker",
		optionDesc, XtNumber(optionDesc), &argc, argv, NULL,
		XtNoverrideRedirect, True,
		XtNgeometry, "-10-10",
		NULL);

	if (argc != 1) Syntax(argv[0]);
	
	XtGetApplicationResources(top, (XtPointer)&options,
		resources, XtNumber(resources),
		NULL, ZERO );

	if (strcmp(options.buttonup_option, "on") == 0)
		options.buttonup = 1;
	else
		options.buttonup = 0;

	options.value = NULL;
	options.length = 0;

	box = XtCreateManagedWidget("box", boxWidgetClass, top, NULL, 0);
	dpy = XtDisplay(top);
  
	selection = XInternAtom(dpy, options.selection_name, 0);
	options.selection = selection;
	buffer = 0;

	XtAppAddTimeOut(context, options.pause, timeout, 0);

	XtRealizeWidget(top);
	XUnmapWindow(XtDisplay(top), XtWindow(top));

	XtAppMainLoop(context);
}	

int Syntax(char *call)
{
	fprintf (stderr, "usage: %s [-selection <name>] [-callback <path>]"
		   	" [-pause <milliseconds>] [-buttonup]\n" , call);
	exit(1);
}

static void SelectionReceived(Widget w, XtPointer client_data, Atom *selection,
                              Atom *type, XtPointer value,
                              unsigned long *received_length, int *format);

// Called each <pause arg=500> milliseconds
void timeout(XtPointer p, XtIntervalId* i)
{
    int get_value = 1;
    if (options.buttonup) {
      int screen_num = DefaultScreen(dpy);
      int root_x, root_y, win_x, win_y;
      unsigned int mask;
      Window root_wnd, child_wnd;
      XQueryPointer(dpy, RootWindow(dpy,screen_num), &root_wnd, &child_wnd,
        &root_x, &root_y, &win_x, &win_y, &mask);
      if (mask & (ShiftMask | Button1Mask))
        get_value = 0;
    }
    if (get_value)
		XtGetSelectionValue(box, selection, XInternAtom(dpy, "UTF8_STRING", False),
			SelectionReceived, NULL,
			CurrentTime);

	XtAppAddTimeOut(context, options.pause, timeout, 0);
}

static int ValueDiffers(char *value, int length);
static void ChangeValue(char *value, int length);

// Called when the requested selection value is availiable
static void SelectionReceived(Widget w, XtPointer client_data, Atom *selection,
                              Atom *type, XtPointer value,
                              unsigned long *received_length, int *format)
{
  int length = *received_length;
  
  if (*type != 0 && *type != XT_CONVERT_FAIL)
    if (length > 0 && ValueDiffers(value, length)) {
      ChangeValue((char*)value, length);
	  system(options.callback_path);
    }
  XtFree(value);
}

// Returns true if value (or length) is different
// than current ones.
static int ValueDiffers(char *value, int length)
{
  return (!options.value ||
    length != options.length ||
    memcmp(options.value, value, options.length));
}

// Update the current value
static void ChangeValue(char *value, int length)
{
  if (options.value)
    XtFree(options.value);

  options.length = length;
  options.value = XtMalloc(options.length);
  if (!options.value)
    printf("WARNING: Unable to allocate memory to store the new value\n");
  else {
    memcpy(options.value, value, options.length);
  }
}
