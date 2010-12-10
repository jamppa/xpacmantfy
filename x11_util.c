#include "x11_util.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>

static int get_title_text_width(PacmanWindow* pacman_window, const char* text);
static int get_title_position_x(int title_width);
static int get_font_height(XftFont* font);

TextPosition get_title_position(PacmanWindow* pacman_window){

	TextPosition title_position;

	int title_width = get_title_text_width(pacman_window, TITLE_TEXT);
	title_position.pos_x = get_title_position_x(title_width);
	title_position.pos_y = 18;

	return title_position;
}

TextPosition get_package_text_position(void){

	TextPosition text_position;

	text_position.pos_x = 20;
	text_position.pos_y = 35;

	return text_position;
}

TextPosition get_count_position(PacmanWindow* pacman_window){

	TextPosition count_position;
	XWindowAttributes win_attributes;
	XGetWindowAttributes(pacman_window->display, pacman_window->window, &win_attributes);

	count_position.pos_x = win_attributes.width - 35;
	count_position.pos_y = win_attributes.height - 10;

	return count_position;
}

void get_package_next_position(PacmanWindow* pacman_window, TextPosition* text_position){

	int font_height = get_font_height(pacman_window->text_font);
	text_position->pos_y += (font_height + 1);
}

void format_package_text(char* text_buffer, size_t buf_size, PacmanPackage* package){

	bzero(text_buffer, buf_size);
	sprintf(text_buffer, "- %s %s", package->name, package->version);
}

void format_count_text(char* buffer, size_t size, PacmanUpdate* update){

	bzero(buffer, size);
	snprintf(buffer, size, "(%i)", update->num_of_packages);
}

static int get_title_text_width(PacmanWindow* pacman_window, const char* text){
	XGlyphInfo gi;
	XftTextExtentsUtf8(pacman_window->display, pacman_window->title_font, 
			(const XftChar8 *)text, strlen(text), &gi);
	return gi.xOff;	
}

static int get_title_position_x(int title_width){
	if(title_width < PACMAN_WINDOW_DEFAULT_WIDTH){
		return (PACMAN_WINDOW_DEFAULT_WIDTH - title_width) / 2;
	}
	return 0;
}

static int get_font_height(XftFont* font){
	return (font->ascent + font->descent);
}
