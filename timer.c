#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void timer_set(int length){

	time_t current_time;
	timer_start = time(NULL);
	timer_length=length;
}

int timer_get(void){
	time_t current_time;
	current_time = time(NULL);

	return((current_time-timer_start)>timer_length);
}

void timer_reset(){
	timer_start = 0;
	timer_length = 0;
}