#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "pacman.h"

static PacmanUpdate* init_pacman_update(void);
static int read_updates_from_pipe(PacmanUpdate* pacman_update);
static void get_pacman_command(char* command_buf, size_t buf_size);
static int add_package(PacmanUpdate* pacman_update, char* buffer);
static PacmanPackage* del_package(PacmanPackage* package);
static PacmanPackage* new_package(char* buffer);
static int has_updates_available(char* buffer, size_t buf_size);

int is_pacman_accessible(void){

	int is_it = 0;
	if(!access(PACMAN_PATH, F_OK)){
		if(!access(PACMAN_PATH, X_OK))
			is_it = 1;
	}
	return is_it;
}

PacmanUpdate* check_pacman_updates(void){

	PacmanUpdate* pacman_update = init_pacman_update();
	if(pacman_update == NULL){
		fprintf(stderr, "%s: Couldn't check pacman updates!\n%s\n", XPACMANTFY, strerror(errno));
		return NULL;
	}

	read_updates_from_pipe(pacman_update);	
	return pacman_update;
}

void free_pacman_updates(PacmanUpdate* update){
	
	if(update != NULL){
		PacmanPackage* each = update->packages;
		while(each != NULL){
			each = del_package(each);
		}
		free(update);
	}
	update = NULL;
}

static PacmanPackage* del_package(PacmanPackage* package){

	PacmanPackage* next_package = NULL;
	if(package != NULL){
		if(package->name)
			free(package->name);
		if(package->version)
			free(package->version);

		next_package = package->next;
		free(package);
	}
	return next_package;
}

static PacmanUpdate* init_pacman_update(void){

	PacmanUpdate* pacman_update = (PacmanUpdate *)malloc(sizeof(struct pacmanUpdate));
	if(pacman_update != NULL){
		memset(pacman_update, 0, sizeof(struct pacmanUpdate));
		pacman_update->packages = NULL;
		pacman_update->last_package = NULL;
		pacman_update->has_packages = 0;
		pacman_update->num_of_packages = 0;
	}
	return pacman_update;
}

static int read_updates_from_pipe(PacmanUpdate* pacman_update){

	char buffer[256] = {0};
	char command[256] = {0};
	
	get_pacman_command(command, sizeof(command));
	FILE* pacman_pipe = popen(command, "r");
	if(!pacman_pipe){
		fprintf(stderr, "%s: Couldn't open pipe for reading. %s", XPACMANTFY, 
				strerror(errno));
		return 0;
	}

	while(fgets(buffer, sizeof(buffer), pacman_pipe) != 0){

		if(strncmp(buffer, PACMAN_UPGRADE_IDENTIFIER, strlen(PACMAN_UPGRADE_IDENTIFIER)) == 0){
			if(fgets(buffer, sizeof(buffer), pacman_pipe) != 0){
				if(has_updates_available(buffer, sizeof(buffer))){
					pacman_update->has_packages = 1;
					if(!add_package(pacman_update, buffer))
						fprintf(stderr, "%s: Error while adding package to list!\n", XPACMANTFY);
					continue;
				}
			}
		}

		if(pacman_update->has_packages){
			if(!add_package(pacman_update, buffer)){
				fprintf(stderr, "%s: Error while adding package to list!\n", XPACMANTFY);
			}	
		}
	}	

	if(pclose(pacman_pipe)){
		fprintf(stderr, "%s: pclose failed!\n%s", XPACMANTFY, strerror(errno));
		return 0;
	}
	return 1;
}

static void get_pacman_command(char* command_buf, size_t buf_size) {
	bzero(command_buf, buf_size);	
	sprintf(command_buf, "sudo %s%s", PACMAN_PATH, PACMAN_PARAMS);
}

static int add_package(PacmanUpdate* pacman_update, char* buffer){

	PacmanPackage* package = new_package(buffer);
	if(package == NULL)
		return 0;

	if(pacman_update->packages == NULL){
		pacman_update->packages = package;
		pacman_update->last_package = package;
		pacman_update->num_of_packages++;
		return 1;
	}

	pacman_update->last_package->next = package;
	pacman_update->last_package = package;
	pacman_update->num_of_packages++;
	return 1;
}

static PacmanPackage* new_package(char* buffer){

	PacmanPackage* package = (PacmanPackage *)malloc(sizeof(struct pacmanPackage));
	if(package == NULL){
		fprintf(stderr, "%s: couldn't allocate memory!\n", XPACMANTFY);
		return NULL;
	}
	memset(package, 0, sizeof(struct pacmanPackage));

	buffer[strlen(buffer)-1] = '\0';
	char* token = strtok(buffer, " ");
	if(token != NULL){
		package->name = strdup(token);
	}
	if((token = strtok(NULL, "")) != NULL){
		package->version = strdup(token);
	}

	package->next = NULL;
	return package;
}

static int has_updates_available(char* buffer, size_t buf_size){

	int has_updates = 0;
	if(strncmp(buffer, PACMAN_NOTHING_TO_DO, 
			strlen(PACMAN_NOTHING_TO_DO)) != 0){
		has_updates = 1;	
	}
	return has_updates;
}
