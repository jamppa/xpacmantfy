#include "pacman.h"
#include "event.h"

static void handle_button_press(int button, PacmanWindow* window);

void handle_event(PacmanWindow* pacman_window, XEvent* event){

	switch(event->type){

		case Expose:
			// redraw window
		break;

		case ButtonPress:
			handle_button_press(event->xbutton.button, pacman_window);
		break;

		default:
		break;
	}
}

static void handle_button_press(int button, PacmanWindow* pacman_window){

	switch(button){
		case Button1:
			hide_pacman_window(pacman_window);
		break;

		case Button2:
			hide_pacman_window(pacman_window);
			open_installation_terminal();
		break;
	
		case Button3:
			hide_pacman_window(pacman_window);
			open_installation_terminal();
		break;

		default:
			break;
	}
}
