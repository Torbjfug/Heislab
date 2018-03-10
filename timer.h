#ifndef __HEISLAB_TIMER_H__
#define __HEISLAB_TIMER_H__

#include <time.h>

static int timer_length;
static time_t timer_start;


/**
	start the timers
	@param length Length of timer in seconds
*/
void timer_set(int length_1);

/**
	check if timer is finished
	@return 1 if timer is finished and 0 otherwise
*/
int timer_get(void);

/**
	Stops timer and reset timer_active_flag
*/
void timer_reset(void);


#endif