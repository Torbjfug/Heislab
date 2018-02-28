#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void timer_set(int length_1, int length_2){
	
	time_t current_time;
    timer_start = time(NULL);
    if(length_1 < length_2){
    	timer_length_1 = length_1;
    	timer_length_2 = length_2;
    }else {
    	timer_length_2 = length_1;
    	timer_length_1 = length_2;
    }
	
}

int timer_get(void){
	time_t current_time ;
	current_time = time(NULL);
	if (current_time - timer_start >= timer_length_2){
		return 2;

	}
	else if (current_time - timer_start >= timer_length_1)
		return 1;
	else{
		return 0;
	}

}

void timer_reset(){
	timer_start = 0;
	timer_length_1 = 0;
	timer_length_2 = 0;
}