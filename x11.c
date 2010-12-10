#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xft/Xft.h>
#include "x11.h"
#include "x11_util.h"
#include "pacman.h"

static int open_display(PacmanWindow* pacman_window);
static void create_window(PacmanWindow* pacman_window);
static int create_GC(PacmanWindow* pacman_window);
static int init_font_and_colors(PacmanWindow* pacman_window);
static int init_hints(PacmanWindow* pacman_window);
static void draw_title(PacmanWindow* pacman_window);
static void draw_packages(PacmanWindow* pacman_window, PacmanUpdate* update);
static void draw_package_count(PacmanWindow* pacman_window, PacmanUpdate* update);


int init_pacman_window(PacmanWindow* pacman_window){

	if(!open_display(pacman_window))
		return 0;
	
	create_window(pacman_window);
	if(!create_GC(pacman_window))
		return 0;

	if(!init_font_and_colors(pacman_window))
		return 0;
	
	if(!init_hints(pacman_window))
		return 0;

	pacman_window->is_visible = 0;
	XSelectInput(pacman_window->display, pacman_window->window, 
			ExposureMask|ButtonPressMask|StructureNotifyMask);

	return 1;
}

void free_pacman_window(PacmanWindow* pacman_window){

	if(pacman_window->class_hints != NULL){
		XFree(pacman_window->class_hints);
	}
	if(pacman_window->size_hints != NULL){
		XFree(pacman_window->size_hints);
	}
	if(pacman_window->wm_hints != NULL){
		XFree(pacman_window->wm_hints);
	}
	if(pacman_window->xft_draw != NULL){
		XftDrawDestroy(pacman_window->xft_draw);
	}
	if(pacman_window->context != NULL){
		XFreeGC(pacman_window->display, pacman_window->context);
	}
	if(pacman_window->title_font != NULL){
		XftFontClose(pacman_window->display, pacman_window->title_font);
	}
	if(pacman_window->text_font != NULL){
		XftFontClose(pacman_window->display, pacman_window->text_font);
	}

	XUnmapWindow(pacman_window->display, pacman_window->window);
	XCloseDisplay(pacman_window->display);
}

void show_pacman_window(PacmanWindow* pacman_window, PacmanUpdate* update){

	XMapWindow(pacman_window->display, pacman_window->window);
	XClearWindow(pacman_window->display, pacman_window->window);
	pacman_window->is_visible = 1;

	draw_title(pacman_window);	
	draw_packages(pacman_window, update);	
	draw_package_count(pacman_window, update);
	
	XFlush(pacman_window->display);
}

void hide_pacman_window(PacmanWindow* pacman_window){

	XUnmapWindow(pacman_window->display, pacman_window->window);
	XFlush(pacman_window->display);

	pacman_window->is_visible = 0;
}

void open_installation_terminal(){
	system(PACMAN_TERMINAL_CMD);
}

static int open_display(PacmanWindow* pacman_window) {

	Display* display = NULL;
	if((display = XOpenDisplay(getenv("DISPLAY"))) == NULL){
		fprintf(stderr, "%s: Couldn't open display!\n", XPACMANTFY);
		return 0;
	}
	
	pacman_window->display = display;
	return 1;
}

static void create_window(PacmanWindow* pacman_window){

	memset(&pacman_window->win_attributes, 0, sizeof(XSetWindowAttributes));
	pacman_window->win_attributes.override_redirect = True;

	pacman_window->screen = XDefaultScreen(pacman_window->display);
	pacman_window->colormap = XDefaultColormap(pacman_window->display, pacman_window->screen);
	pacman_window->visual = DefaultVisual(pacman_window->display, pacman_window->screen);	
	pacman_window->screen = XDefaultScreen(pacman_window->display);
	pacman_window->window = XCreateWindow(pacman_window->display, 
			RootWindow(pacman_window->display, pacman_window->screen), PACMAN_WINDOW_X, PACMAN_WINDOW_Y, 
			PACMAN_WINDOW_DEFAULT_WIDTH, PACMAN_WINDOW_DEFAULT_HEIGHT,PACMAN_WINDOW_BORDER_WIDTH, 
			CopyFromParent, InputOutput, CopyFromParent, CWOverrideRedirect, &pacman_window->win_attributes);	
}

static int create_GC(PacmanWindow* pacman_window){

	GC context = NULL;
	pacman_window->gc_values.line_width = 2;
	if((context = XCreateGC(pacman_window->display, pacman_window->window, 
					GCLineWidth, &pacman_window->gc_values)) == NULL){
		fprintf(stderr, "%s: Couldn't create GC!\n", XPACMANTFY);
		return 0;
	}

	pacman_window->context = context;
	XSetFillStyle(pacman_window->display, 
			pacman_window->context, FillSolid);
	
	return 1;
}

static int init_font_and_colors(PacmanWindow* pacman_window){

	if((pacman_window->text_font = XftFontOpenName(pacman_window->display, pacman_window->screen, 
				PACMAN_WINDOW_TEXTFONT)) == NULL){
		fprintf(stderr, "%s: Couldn't open font %s\n", XPACMANTFY, PACMAN_WINDOW_TEXTFONT);
		return 0;
	}
	if((pacman_window->title_font = XftFontOpenName(pacman_window->display, pacman_window->screen,
				PACMAN_WINDOW_TITLEFONT)) == NULL){
		fprintf(stderr, "%s: Couldn't open font %s\n", XPACMANTFY, PACMAN_WINDOW_TITLEFONT);
		return 0;
	}

	if((XParseColor(pacman_window->display, pacman_window->colormap, 
				PACMAN_WINDOW_BGCOLOR, &pacman_window->background_color)) == 0){
		fprintf(stderr, "%s: Couldn't parse color %s\n", XPACMANTFY, PACMAN_WINDOW_BGCOLOR);
		return 0;
	}	
	if((XParseColor(pacman_window->display, pacman_window->colormap, 
				PACMAN_WINDOW_FGCOLOR, &pacman_window->foreground_color)) == 0){
		fprintf(stderr, "%s: Couldn't parse color %s\n", XPACMANTFY, PACMAN_WINDOW_FGCOLOR);
		return 0;
	}
	if((XParseColor(pacman_window->display, pacman_window->colormap, 
				PACMAN_WINDOW_BORDERCOLOR, &pacman_window->border_color)) == 0){
		fprintf(stderr, "%s: Couldn't parse color %s\n", XPACMANTFY, PACMAN_WINDOW_BORDERCOLOR);
		return 0;
	}

	if((XAllocColor(pacman_window->display, pacman_window->colormap, 
				&pacman_window->background_color)) == 0){
		fprintf(stderr, "%s: Couldn't allocate color %s", XPACMANTFY, PACMAN_WINDOW_BGCOLOR);
		return 0;
	}
	if((XAllocColor(pacman_window->display, pacman_window->colormap, 
				&pacman_window->foreground_color)) == 0){
		fprintf(stderr, "%s: Couldn't allocate color %s", XPACMANTFY, PACMAN_WINDOW_FGCOLOR);
		return 0;
	}
	if((XAllocColor(pacman_window->display, pacman_window->colormap, 
				&pacman_window->border_color)) == 0){
		fprintf(stderr, "%s: Couldn't allocate color %s", XPACMANTFY, PACMAN_WINDOW_BORDERCOLOR);
		return 0;
	}

	pacman_window->font_color.pixel = pacman_window->foreground_color.pixel;
	pacman_window->font_color.color.red = pacman_window->foreground_color.red;
	pacman_window->font_color.color.green = pacman_window->foreground_color.green;
	pacman_window->font_color.color.blue = pacman_window->foreground_color.blue;

	pacman_window->xft_draw = XftDrawCreate(pacman_window->display, pacman_window->window, 
			pacman_window->visual, pacman_window->colormap);

	XSetForeground(pacman_window->display, pacman_window->context, pacman_window->foreground_color.pixel);
	XSetWindowBackground(pacman_window->display, pacman_window->window, pacman_window->background_color.pixel);	
	XSetWindowBorder(pacman_window->display, pacman_window->window, pacman_window->border_color.pixel);

	return 1;
}

static int init_hints(PacmanWindow* pacman_window){

	pacman_window->class_hints = XAllocClassHint();
	pacman_window->size_hints = XAllocSizeHints();	
	pacman_window->wm_hints = XAllocWMHints();

	if(pacman_window->class_hints == NULL ||
			pacman_window->size_hints == NULL ||
			pacman_window->wm_hints == NULL){
		
		fprintf(stderr, "%s: Couldn't allocate hints!\n", XPACMANTFY);
		return 0;
	}

	pacman_window->size_hints->flags = PPosition|PSize|USPosition|USSize;
	pacman_window->size_hints->x = 0;	
	pacman_window->size_hints->y = 0;
	pacman_window->size_hints->width = PACMAN_WINDOW_DEFAULT_WIDTH;
	pacman_window->size_hints->height = PACMAN_WINDOW_DEFAULT_HEIGHT;

	pacman_window->wm_hints->flags = StateHint;
	pacman_window->wm_hints->initial_state = NormalState;
	XSetWMHints(pacman_window->display, pacman_window->window, pacman_window->wm_hints);
	XSetWMProperties(pacman_window->display, pacman_window->window, NULL, NULL, NULL, 0, 
			pacman_window->size_hints, pacman_window->wm_hints, pacman_window->class_hints);

	pacman_window->delete_atom = XInternAtom(pacman_window->display, "WM_DELETE_WINDOW", False);	
	XSetWMProtocols(pacman_window->display, pacman_window->window, &pacman_window->delete_atom, 1);

	pacman_window->desktop_atom = XInternAtom(pacman_window->display, "_NET_WM_DESKTOP", False);
	pacman_window->desktops_property = 0xffffffff;	
	XChangeProperty(pacman_window->display, pacman_window->window, pacman_window->desktop_atom,
			XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&pacman_window->desktops_property, 1);

	 pacman_window->state_atom = XInternAtom(pacman_window->display, "_NET_WM_STATE", False);
     pacman_window->state_atoms[0] = XInternAtom(pacman_window->display, "_NET_WM_STATE_STICKY", False);
     pacman_window->state_atoms[1] = XInternAtom(pacman_window->display, "_NET_WM_STATE_SKIP_TASKBAR", False);
     pacman_window->state_atoms[2] = XInternAtom(pacman_window->display, "_NET_WM_STATE_ABOVE", False);
     XChangeProperty(pacman_window->display, pacman_window->window, pacman_window->state_atom, XA_ATOM, 32, 
			 PropModeReplace,(unsigned char *)pacman_window->state_atoms, 3);

	return 1;
}

static void draw_title(PacmanWindow* pacman_window){
	
	TextPosition title_position = get_title_position(pacman_window);
	XftDrawStringUtf8(pacman_window->xft_draw, &pacman_window->font_color, 
			pacman_window->title_font, title_position.pos_x, title_position.pos_y, 
			(const XftChar8 *)TITLE_TEXT, strlen(TITLE_TEXT));

	XDrawLine(pacman_window->display, pacman_window->window, pacman_window->context, 
			20, 22, PACMAN_WINDOW_DEFAULT_WIDTH - 20, 22);
}

static void draw_packages(PacmanWindow* pacman_window, PacmanUpdate* update){

	char text_buffer[256] = {0};
	TextPosition text_position = get_package_text_position();
	PacmanPackage* each = update->packages;

	while(each != NULL){

		if(text_position.pos_y >= PACMAN_WINDOW_DEFAULT_HEIGHT)
			break;

		format_package_text(text_buffer, sizeof(text_buffer), each);
		XftDrawStringUtf8(pacman_window->xft_draw, &pacman_window->font_color, 
			pacman_window->text_font, text_position.pos_x, text_position.pos_y, 
			(const XftChar8 *)text_buffer, strlen(text_buffer));
		
		get_package_next_position(pacman_window, &text_position);	
		each = each->next;
	}
}

static void draw_package_count(PacmanWindow* pacman_window, PacmanUpdate* update){

	TextPosition count_position = get_count_position(pacman_window);	
	char text_buffer[16] = {0};

	format_count_text(text_buffer, sizeof(text_buffer), update);	
	XftDrawStringUtf8(pacman_window->xft_draw, &pacman_window->font_color, 
			pacman_window->text_font, count_position.pos_x, count_position.pos_y, 
			(const XftChar8 *)text_buffer, strlen(text_buffer));
}
