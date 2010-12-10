#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "pacman.h"
#include "x11.h"
#include "event.h"
#include "signal.h"

PacmanUpdate* pacman_update = NULL;
PacmanWindow pacman_window;
int running = 0;
int check_updates = 1;

static void termination_signal_handler(int signo);
static void update_check_handler(int signo);
static void free_resources(void);

int main(int argc, char** argv){

	if(!is_pacman_accessible()){
		fprintf(stderr, "%s: Couldn't find %s\n", 
				strerror(errno), PACMAN_PATH);
		exit(EXIT_FAILURE);
	}

	if(!init_pacman_window(&pacman_window)){
		fprintf(stderr, "%s: Couldn't init window: %s\n", 
				XPACMANTFY, strerror(errno));
		free_pacman_window(&pacman_window);
		exit(EXIT_FAILURE);
	}	

	XEvent event;
	running = 1;

	set_termination_signal_handler(termination_signal_handler);
	set_update_check_handler(update_check_handler, 60 * 30);
	while(running){
	
		while(!pacman_window.is_visible && running){
		
			if(check_updates){

				free_pacman_updates(pacman_update);
				pacman_update = check_pacman_updates();
				check_updates = 0;

				if(pacman_update != NULL && pacman_update->has_packages){
					show_pacman_window(&pacman_window, pacman_update);	
					break;
				}
			}
			sleep(1);
		}

		if(!running)
			continue;

		XNextEvent(pacman_window.display, &event);
		handle_event(&pacman_window, &event);
	}
	
	free_resources();
	exit(EXIT_SUCCESS);
}

static void termination_signal_handler(int signo){
	fprintf(stdout, "%s: terminating...\n", XPACMANTFY);
	running = 0;
}

static void update_check_handler(int signo){
	check_updates = 1;
}

static void free_resources(void){
	fprintf(stdout, "%s: freeing resources...\n", XPACMANTFY);
	free_pacman_window(&pacman_window);
	free_pacman_updates(pacman_update);
}





