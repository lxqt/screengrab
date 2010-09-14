#ifndef NETWM_H
#define NETWM_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>

namespace netwm
{

struct net_wm_state
{
    unsigned int modal          : 1;
    unsigned int sticky         : 1;
    unsigned int maximized_vert : 1;
    unsigned int maximized_horz : 1;
    unsigned int shaded         : 1;
    unsigned int skip_taskbar   : 1;
    unsigned int skip_pager     : 1;
    unsigned int hidden         : 1;
    unsigned int fullscreen     : 1;
    unsigned int above          : 1;
    unsigned int below          : 1;
};

struct net_wm_window_type
{
    unsigned int desktop : 1;
    unsigned int dock    : 1;
    unsigned int toolbar : 1;
    unsigned int menu    : 1;
    unsigned int utility : 1;
    unsigned int splash  : 1;
    unsigned int dialog  : 1;
    unsigned int normal  : 1;
};

/* functions */

void init();

void* property(Window win, Atom prop, Atom type, int *nitems = 0);

void climsg(Window win, long type, long l0, long l1 = 0, long l2 = 0, long l3 = 0, long l4 = 0);

void getNetwmState(Window win, net_wm_state *nws);
void getNetwmWindowType(Window win, net_wm_window_type *nwwt);

/* Atoms */

extern Atom UTF8_STRING;
extern Atom XROOTPMAP_ID;

extern Atom WM_STATE;
extern Atom WM_CLASS;
extern Atom WM_DELETE_WINDOW;
extern Atom WM_PROTOCOLS;

extern Atom NET_WORKAREA;
extern Atom NET_CLIENT_LIST;
extern Atom NET_CLIENT_LIST_STACKING;
extern Atom NET_NUMBER_OF_DESKTOPS;
extern Atom NET_CURRENT_DESKTOP;
extern Atom NET_DESKTOP_NAMES;
extern Atom NET_ACTIVE_WINDOW;
extern Atom NET_SUPPORTED;
extern Atom NET_WM_DESKTOP;
extern Atom NET_WM_STATE;
extern Atom NET_WM_STATE_SKIP_TASKBAR;
extern Atom NET_WM_STATE_SKIP_PAGER;
extern Atom NET_WM_STATE_STICKY;
extern Atom NET_WM_STATE_HIDDEN;
extern Atom NET_WM_STATE_SHADED;
extern Atom NET_WM_WINDOW_TYPE;
extern Atom NET_WM_WINDOW_TYPE_DESKTOP;
extern Atom NET_WM_WINDOW_TYPE_DOCK;
extern Atom NET_WM_WINDOW_TYPE_TOOLBAR;
extern Atom NET_WM_WINDOW_TYPE_MENU;
extern Atom NET_WM_WINDOW_TYPE_UTILITY;
extern Atom NET_WM_WINDOW_TYPE_SPLASH;
extern Atom NET_WM_WINDOW_TYPE_DIALOG;
extern Atom NET_WM_WINDOW_TYPE_NORMAL;
extern Atom NET_WM_NAME;
extern Atom NET_WM_VISIBLE_NAME;
extern Atom NET_WM_STRUT;
extern Atom NET_WM_STRUT_PARTIAL;
extern Atom NET_WM_ICON;
extern Atom NET_WM_PID;
extern Atom NET_SHOWING_DESKTOP;
extern Atom NET_DESKTOP_GEOMETRY;
extern Atom NET_DESKTOP_VIEWPORT;

}

#endif
