#ifndef _X11_H_
#define _X11_H_

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include "pacman.h"

#define PACMAN_WINDOW_X 0
#define PACMAN_WINDOW_Y 0
#define PACMAN_WINDOW_BORDER_WIDTH 2
#define PACMAN_WINDOW_TEXTFONT "Trebuchet MS:size=8"
#define PACMAN_WINDOW_TITLEFONT "Trebuchet MS:size=12"
#define PACMAN_WINDOW_BGCOLOR "black"
#define PACMAN_WINDOW_FGCOLOR "white"
#define PACMAN_WINDOW_BORDERCOLOR "white"
#define PACMAN_WINDOW_DEFAULT_HEIGHT 180
#define PACMAN_WINDOW_DEFAULT_WIDTH 250

#define TITLE_TEXT "New updates"

	typedef struct pacmanWindow {

		int screen; 
		int desktops_property;
		int is_visible;

		Display* display;
		Window window;
		Colormap colormap;
		GC context;
		XGCValues gc_values;
		XSetWindowAttributes win_attributes;

		XColor background_color;
		XColor foreground_color;
		XColor border_color;

		Visual* visual;
		XftDraw* xft_draw;
		XftColor font_color; 
		XftFont* title_font;
		XftFont* text_font;

		XClassHint* class_hints;
		XSizeHints* size_hints;
		XWMHints* wm_hints;

		Atom delete_atom;
		Atom desktop_atom;
		Atom state_atom;
		Atom state_atoms[3];

	} PacmanWindow;	


extern int init_pacman_window(PacmanWindow* pacman_window);
extern void show_pacman_window(PacmanWindow* window, PacmanUpdate* update);
extern void hide_pacman_window(PacmanWindow* window);
extern void free_pacman_window(PacmanWindow* pacman_window);

extern void open_installation_terminal(void);

#endif 
