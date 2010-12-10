#ifndef _SIGNAL_H_
#define _SIGNAL_H_

extern void set_termination_signal_handler(void (*handler)(int));
extern void set_update_check_handler(void (*handler)(int), unsigned int interval);

#endif
