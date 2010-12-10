#ifndef _PACMAN_H_
#define _PACMAN_H_

#define PACMAN_PATH "/usr/bin/pacman"
#define XPACMANTFY "xpacmantfy"
#define PACMAN_PARAMS " -Syu --noprogressbar --print --print-format '%n %v'"
#define PACMAN_TERMINAL_CMD "xterm -hold -e sudo pacman -Syu"
#define PACMAN_UPGRADE_IDENTIFIER ":: Starting full system upgrade..."
#define PACMAN_NOTHING_TO_DO " there is nothing to do"


typedef struct pacmanPackage {

	char* name;
	char* version;

	struct pacmanPackage* next;

} PacmanPackage;

typedef struct pacmanUpdate {

	int num_of_packages;
	int has_packages;

	PacmanPackage* last_package;
	PacmanPackage* packages;

} PacmanUpdate;

extern int is_pacman_accessible(void);
extern PacmanUpdate* check_pacman_updates(void);
extern void free_pacman_updates(PacmanUpdate* update);

#endif
