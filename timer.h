#ifndef __HEISLAB_TIMER_H__
#define __HEISLAB_TIMER_H__

#include <time.h>

static int timer_length_1;
static int timer_length_2;
static time_t timer_start;

/**
*/
void timer_init();

/**
	start two timers
	@param length1 Length of first timer in seconds
	@param length_2 Length of second timer in seconds
*/
void timer_set(int length_1, int length_2);



/**
	check if timer is finished
	@return 1 if timer_1 is finished, 2 if both timers are finished and 0 otherwise
*/
int timer_get(void);

/**
	Stops timer and reset timer_active_flag
*/	
void timer_reset(void);


#endif