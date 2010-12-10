#include <signal.h>
#include <sys/time.h>
#include "signal.h"

static struct sigaction termination_action;
static struct sigaction update_action;
static struct itimerval timer_new, timer_old;

void set_termination_signal_handler(void (*handler)(int)){

	termination_action.sa_handler = handler;
	
	sigemptyset(&termination_action.sa_mask);
	sigaddset(&termination_action.sa_mask, SIGINT);
	sigaddset(&termination_action.sa_mask, SIGTERM);
	sigaddset(&termination_action.sa_mask, SIGHUP);
	termination_action.sa_flags = 0;

	sigaction(SIGHUP, &termination_action, NULL);
	sigaction(SIGTERM, &termination_action, NULL);
	sigaction(SIGINT, &termination_action, NULL);
}

void set_update_check_handler(void (*handler)(int), unsigned int interval){

	timer_new.it_interval.tv_usec = 0;
	timer_new.it_interval.tv_sec = (long int) interval;
	timer_new.it_value.tv_usec = 0;
	timer_new.it_value.tv_sec = (long int) interval;
	setitimer(ITIMER_REAL, &timer_new, &timer_old);

	update_action.sa_handler = handler;
	sigemptyset(&update_action.sa_mask);
	sigaddset(&update_action.sa_mask, SIGALRM);
	update_action.sa_flags = 0;

	sigaction(SIGALRM, &update_action, NULL);

}
