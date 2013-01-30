#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

void sendKey(Display *dpy, Window root, int keycode) {
  XKeyEvent ke;
  ke.display = dpy;
  ke.root = root;
  ke.send_event = True;
  ke.subwindow = None;
  ke.time = CurrentTime;
  ke.x = 1;
  ke.y = 1;
  ke.x_root = 1;
  ke.y_root = 1;
  ke.same_screen = True;
  ke.type = KeyPress;
  ke.state = 0;
  ke.keycode = keycode;
  Window w; int x;
  XGetInputFocus(dpy, &w, &x);
  ke.window = w;
  XSendEvent(dpy, w, True, KeyPressMask, (XEvent*)&ke);
  XSync(dpy, False);
}

/* can't use AnyModifier, it's too often unavailable */

void grabKey(Display *dpy, Window root, int keycode) {
  XGrabKey(dpy, keycode, 0, root, False, GrabModeAsync,
           GrabModeAsync);
  XGrabKey(dpy, keycode, ControlMask, root, False, GrabModeAsync,
           GrabModeAsync);
  XGrabKey(dpy, keycode, ShiftMask, root, False, GrabModeAsync,
           GrabModeAsync);
  XGrabKey(dpy, keycode, ControlMask | ShiftMask, root, False, GrabModeAsync,
           GrabModeAsync);
}

void unGrabKey(Display *dpy, Window root, int keycode) {
  XUngrabKey(dpy, keycode, 0, root);
  XUngrabKey(dpy, keycode, ControlMask, root);
  XUngrabKey(dpy, keycode, ShiftMask, root);
  XUngrabKey(dpy, keycode, ControlMask | ShiftMask, root);
}

int main(int argc, char **argv) {
  if(argc < 2) {
    printf("usage: tr-trigger <triggers-per-TR>\n");
    printf("Captures all 5s and emits a 9 for each TR\n");
    exit(1);
  }

  int tr_count = atoi(argv[1]);

  Display*    dpy     = XOpenDisplay(0);
  Window      root    = DefaultRootWindow(dpy);
  XEvent      ev;

  int keycode0 = XKeysymToKeycode(dpy,XK_0);
  int keycode5 = XKeysymToKeycode(dpy,XK_5);
  int keycode9 = XKeysymToKeycode(dpy,XK_9);

  grabKey(dpy, root, keycode0);
  grabKey(dpy, root, keycode5);
  XSelectInput(dpy, root, KeyPressMask );
  
  int count = 0;
  while(1) {
    XNextEvent(dpy, &ev);
    if(ev.type == KeyPress) {
      XKeyEvent *ke = (XKeyEvent*)&ev;
      if(ke->keycode == keycode0)
        break;
      else if(ke->keycode == keycode5) {
        if(count == tr_count - 1) {
          sendKey(dpy, root, keycode9);
          count = 0;
        } else count++;
      } else {
        printf("unexpected keycode %d\n", ke->keycode);
        exit(1);
      }
    }
  }

  unGrabKey(dpy, root, keycode0);
  unGrabKey(dpy, root, keycode5);
  XCloseDisplay(dpy);

  return 0;
}
