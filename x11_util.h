#ifndef _X11_UTIL_H_
#define _X11_UTIL_H_

#include "x11.h"
#include "pacman.h"

typedef struct textPosition {

	int pos_x;
	int pos_y;

} TextPosition;

TextPosition get_title_position(PacmanWindow* pacman_window);
TextPosition get_package_text_position(void);
TextPosition get_count_position(PacmanWindow* pacman_window);

void get_package_next_position(PacmanWindow* pacman_window, 
		TextPosition* text_position);

void format_package_text(char* buffer, size_t buf_size, 
		PacmanPackage* package);

void format_count_text(char* buffer, size_t size, PacmanUpdate* update);

#endif
