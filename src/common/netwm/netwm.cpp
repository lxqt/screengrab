#include <QX11Info>

#include <climits>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <X11/Xutil.h>

#include "netwm.h"

namespace netwm
{

Atom UTF8_STRING = 0;
Atom XROOTPMAP_ID = 0;

Atom WM_STATE = 0;
Atom WM_CLASS = 0;
Atom WM_DELETE_WINDOW = 0;
Atom WM_PROTOCOLS = 0;

Atom NET_WORKAREA = 0;
Atom NET_CLIENT_LIST = 0;
Atom NET_CLIENT_LIST_STACKING = 0;
Atom NET_NUMBER_OF_DESKTOPS = 0;
Atom NET_CURRENT_DESKTOP = 0;
Atom NET_DESKTOP_NAMES = 0;
Atom NET_ACTIVE_WINDOW = 0;
Atom NET_SUPPORTED = 0;
Atom NET_WM_DESKTOP = 0;
Atom NET_WM_STATE = 0;
Atom NET_WM_STATE_SKIP_TASKBAR = 0;
Atom NET_WM_STATE_SKIP_PAGER = 0;
Atom NET_WM_STATE_STICKY = 0;
Atom NET_WM_STATE_HIDDEN = 0;
Atom NET_WM_STATE_SHADED = 0;
Atom NET_WM_WINDOW_TYPE = 0;
Atom NET_WM_WINDOW_TYPE_DESKTOP = 0;
Atom NET_WM_WINDOW_TYPE_DOCK = 0;
Atom NET_WM_WINDOW_TYPE_TOOLBAR = 0;
Atom NET_WM_WINDOW_TYPE_MENU = 0;
Atom NET_WM_WINDOW_TYPE_UTILITY = 0;
Atom NET_WM_WINDOW_TYPE_SPLASH = 0;
Atom NET_WM_WINDOW_TYPE_DIALOG = 0;
Atom NET_WM_WINDOW_TYPE_NORMAL = 0;
Atom NET_WM_NAME = 0;
Atom NET_WM_VISIBLE_NAME = 0;
Atom NET_WM_STRUT = 0;
Atom NET_WM_STRUT_PARTIAL = 0;
Atom NET_WM_ICON = 0;
Atom NET_WM_PID = 0;
Atom NET_SHOWING_DESKTOP = 0;
Atom NET_DESKTOP_GEOMETRY = 0;
Atom NET_DESKTOP_VIEWPORT = 0;

/******************************/

void init()
{
    Display *dpy = QX11Info::display();

    UTF8_STRING                = XInternAtom(dpy, "UTF8_STRING", False);
    XROOTPMAP_ID               = XInternAtom(dpy, "_XROOTPMAP_ID", False);
    WM_STATE                   = XInternAtom(dpy, "WM_STATE", False);
    WM_CLASS                   = XInternAtom(dpy, "WM_CLASS", False);
    WM_DELETE_WINDOW           = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    WM_PROTOCOLS               = XInternAtom(dpy, "WM_PROTOCOLS", False);
    NET_WORKAREA               = XInternAtom(dpy, "_NET_WORKAREA", False);
    NET_CLIENT_LIST            = XInternAtom(dpy, "_NET_CLIENT_LIST", False);
    NET_CLIENT_LIST_STACKING   = XInternAtom(dpy, "_NET_CLIENT_LIST_STACKING", False);
    NET_NUMBER_OF_DESKTOPS     = XInternAtom(dpy, "_NET_NUMBER_OF_DESKTOPS", False);
    NET_CURRENT_DESKTOP        = XInternAtom(dpy, "_NET_CURRENT_DESKTOP", False);
    NET_DESKTOP_NAMES          = XInternAtom(dpy, "_NET_DESKTOP_NAMES", False);
    NET_ACTIVE_WINDOW          = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
    NET_SUPPORTED              = XInternAtom(dpy, "_NET_SUPPORTED", False);
    NET_WM_DESKTOP             = XInternAtom(dpy, "_NET_WM_DESKTOP", False);
    NET_WM_STATE               = XInternAtom(dpy, "_NET_WM_STATE", False);
    NET_WM_STATE_SKIP_TASKBAR  = XInternAtom(dpy, "_NET_WM_STATE_SKIP_TASKBAR", False);
    NET_WM_STATE_SKIP_PAGER    = XInternAtom(dpy, "_NET_WM_STATE_SKIP_PAGER", False);
    NET_WM_STATE_STICKY        = XInternAtom(dpy, "_NET_WM_STATE_STICKY", False);
    NET_WM_STATE_HIDDEN        = XInternAtom(dpy, "_NET_WM_STATE_HIDDEN", False);
    NET_WM_STATE_SHADED        = XInternAtom(dpy, "_NET_WM_STATE_SHADED", False);
    NET_WM_WINDOW_TYPE         = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);

    NET_WM_WINDOW_TYPE_DESKTOP = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
    NET_WM_WINDOW_TYPE_DOCK    = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);
    NET_WM_WINDOW_TYPE_TOOLBAR = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_TOOLBAR", False);
    NET_WM_WINDOW_TYPE_MENU    = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_MENU", False);
    NET_WM_WINDOW_TYPE_UTILITY = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_UTILITY", False);
    NET_WM_WINDOW_TYPE_SPLASH  = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_SPLASH", False);
    NET_WM_WINDOW_TYPE_DIALOG  = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    NET_WM_WINDOW_TYPE_NORMAL  = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_NORMAL", False);
    NET_WM_DESKTOP             = XInternAtom(dpy, "_NET_WM_DESKTOP", False);
    NET_WM_NAME                = XInternAtom(dpy, "_NET_WM_NAME", False);
    NET_WM_VISIBLE_NAME        = XInternAtom(dpy, "_NET_WM_VISIBLE_NAME", False);
    NET_WM_STRUT               = XInternAtom(dpy, "_NET_WM_STRUT", False);
    NET_WM_STRUT_PARTIAL       = XInternAtom(dpy, "_NET_WM_STRUT_PARTIAL", False);
    NET_WM_ICON                = XInternAtom(dpy, "_NET_WM_ICON", False);
    NET_WM_PID                 = XInternAtom(dpy, "_NET_WM_PID", False);
    NET_SHOWING_DESKTOP        = XInternAtom(dpy, "_NET_SHOWING_DESKTOP", False);
    NET_DESKTOP_GEOMETRY       = XInternAtom(dpy, "_NET_DESKTOP_GEOMETRY", False);
    NET_DESKTOP_VIEWPORT       = XInternAtom(dpy, "_NET_DESKTOP_VIEWPORT", False);
}

void* property(Window win, Atom prop, Atom type, int *nitems)
{
    Atom type_ret;
    int format_ret;
    unsigned long items_ret;
    unsigned long after_ret;
    unsigned char *prop_data = 0;

    if(XGetWindowProperty(QX11Info::display(), win, prop, 0, 0x7fffffff, False,
                            type, &type_ret, &format_ret, &items_ret,
                            &after_ret, &prop_data) != Success)
        return 0;

    if(nitems)
        *nitems = items_ret;

    return prop_data;
}

void climsg(Window win, long type, long l0, long l1, long l2, long l3, long l4)
{
    XClientMessageEvent xev;

    xev.type = ClientMessage;
    xev.window = win;
    xev.message_type = type;
    xev.format = 32;
    xev.data.l[0] = l0;
    xev.data.l[1] = l1;
    xev.data.l[2] = l2;
    xev.data.l[3] = l3;
    xev.data.l[4] = l4;

    XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), False,
          (SubstructureNotifyMask | SubstructureRedirectMask),
          (XEvent *)&xev);
}

void getNetwmState(Window win, net_wm_state *nws)
{
    Atom *state;
    int num3;

    bzero(nws, sizeof(nws));

    if(!(state = (Atom *)property(win, NET_WM_STATE, XA_ATOM, &num3)))
        return;

    while(--num3 >= 0)
    {
        if(state[num3] == NET_WM_STATE_SKIP_PAGER)
            nws->skip_pager = 1;
        else if(state[num3] == NET_WM_STATE_SKIP_TASKBAR)
            nws->skip_taskbar = 1;
        else if(state[num3] == NET_WM_STATE_STICKY)
            nws->sticky = 1;
        else if(state[num3] == NET_WM_STATE_HIDDEN)
            nws->hidden = 1;
        else if(state[num3] == NET_WM_STATE_SHADED)
            nws->shaded = 1;
    }

    XFree(state);
}

void getNetwmWindowType(Window win, net_wm_window_type *nwwt)
{
    Atom *state;
    int num3;

    bzero(nwwt, sizeof(nwwt));

    if (!(state = (Atom *)property(win, NET_WM_WINDOW_TYPE, XA_ATOM, &num3)))
        return;

    while(--num3 >= 0)
    {
        if(state[num3] == NET_WM_WINDOW_TYPE_DESKTOP)
            nwwt->desktop = 1;
        else if(state[num3] == NET_WM_WINDOW_TYPE_DOCK)
            nwwt->dock = 1;
        else if(state[num3] == NET_WM_WINDOW_TYPE_TOOLBAR)
            nwwt->toolbar = 1;
        else if(state[num3] == NET_WM_WINDOW_TYPE_MENU)
            nwwt->menu = 1;
        else if(state[num3] == NET_WM_WINDOW_TYPE_UTILITY)
            nwwt->utility = 1;
        else if(state[num3] == NET_WM_WINDOW_TYPE_SPLASH)
            nwwt->splash = 1;
        else if(state[num3] == NET_WM_WINDOW_TYPE_DIALOG)
            nwwt->dialog = 1;
        else if(state[num3] == NET_WM_WINDOW_TYPE_NORMAL)
            nwwt->normal = 1;
    }

    XFree(state);
}

}
