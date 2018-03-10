#include "elevator.h"
#include "control.h"
#include "timer.h"

#include <stdio.h>
#include <time.h>

typedef enum tag_elevator_states
{
    INITIAL_ST, STOP_ST, IDLE_ST, EMERGENCY_ST, UP_ST, DOWN_ST 
}elevator_states_t;


int main() {
    elevator_states_t state;
    state = INITIAL_ST;

    while(1) {
        switch (state) {
            case INITIAL_ST:
                printf("Initial state\n");
                if (!control_init()) {
                    printf("Unable to initialize elevator hardware!\n");
                    return 1;
                }
                timer_reset();
                elevator_set_motor_direction(DIRN_UP);
		        control_set_previous_direction(DIRN_UP);
                while (elevator_get_floor_sensor_signal() == -1)  { //Do nothing until elevator is at floor

		        }
                elevator_set_floor_indicator(elevator_get_floor_sensor_signal());
                control_set_previous_floor(elevator_get_floor_sensor_signal());
                state = STOP_ST;
                break;

            case STOP_ST:
                {
                
                elevator_set_motor_direction(DIRN_STOP);
                printf("Stop state\n");
                elevator_set_door_open_lamp(1);
                timer_set(0.1,3);

                while (timer_get() != TIMER_2_FINISHED) { //Wating for three seconds
		          if(timer_get() == TIMER_1_FINISHED){ //Initial delay to avoid multiple orders
                        control_update_control_matrix();
                  }
		          if(elevator_get_stop_signal()){
    		          state = EMERGENCY_ST;
                      goto emergency;
    		       }
            
                }

                state = IDLE_ST;
                }break;

            case IDLE_ST: {

                printf("Idle state\n");

                elevator_set_door_open_lamp(0);
                elev_motor_direction_t direction;
                direction = DIRN_IDLE;

                while (direction == DIRN_IDLE) { //No existin orders 
                    direction = control_direction(control_get_previous_direction());
		            control_update_control_matrix();
                    
                    if(elevator_get_stop_signal()){
                        state = EMERGENCY_ST;
                        goto emergency;
                   }

                }

                control_clear_floor_order(direction, control_get_previous_floor());
                if (direction == DIRN_STOP){
                  state = STOP_ST;
                }
                else if (direction == DIRN_UP)
                    state = UP_ST;
                else if (direction == DIRN_DOWN)
                    state = DOWN_ST;

            }break;

            case UP_ST: {
                control_set_previous_direction(DIRN_UP);

                printf("Up state\n");

                elevator_set_motor_direction(DIRN_UP);
                int floor, prev_floor;
                prev_floor = control_get_previous_floor();


                do {//Go up until at next floor then check if stop signal
                    control_update_control_matrix();
                    floor = elevator_get_floor_sensor_signal();
                    if(elevator_get_stop_signal()){
                        state = EMERGENCY_ST;
                        goto emergency;
                    }
                    if (control_get_emergency_flag() && floor == prev_floor){
                    //If elevator has stoped between flors break to check for stop signal
                        break;
                    }
                } while (floor < 0 || floor == prev_floor);
                elevator_set_floor_indicator(floor);
                control_set_previous_floor(floor);
                elev_motor_direction_t direction;
                
                if(control_get_emergency_flag()){
                    control_set_emergency_flag(0);
                }

		
                direction = DIRN_UP;
                direction = control_direction(direction);
		
                if (direction == DIRN_STOP){
                    control_clear_floor_order(DIRN_UP, floor);
                    state = STOP_ST;
                }
		    
                else{
                    state = UP_ST;
                }

                elevator_set_floor_indicator(floor);

            }break;

            case DOWN_ST: {
                control_set_previous_direction(DIRN_DOWN);
                printf("Down state\n");

                elevator_set_motor_direction(DIRN_DOWN);
                int floor, prev_floor;
                prev_floor = control_get_previous_floor();
                do {//Go down until at next floor then check if stop signal
                    control_update_control_matrix();
                    floor = elevator_get_floor_sensor_signal();
		    
    		    if(elevator_get_stop_signal()){
                    state = EMERGENCY_ST;
                    goto emergency;
                }
    		    if(control_get_emergency_flag() && floor == prev_floor){
                    //If elevator has stoped between flors break to check for stop signal
    			break;
                }

                }
                while (floor<0 || floor == prev_floor);
                elevator_set_floor_indicator(floor);
                control_set_previous_floor(floor);
		
		        if(control_get_emergency_flag()){
                    control_set_emergency_flag(0);
                }


                       
                elev_motor_direction_t direction;
                direction = DIRN_DOWN;
                direction = control_direction(direction);

                if (direction == DIRN_STOP{
                    state = STOP_ST;
                    control_clear_floor_order(DIRN_DOWN, floor);
		        }
                else{
                    state = DOWN_ST;
                }

            } break;

            case EMERGENCY_ST: {

    		emergency:
                elevator_set_motor_direction(DIRN_STOP);
                printf("Emergency state\n");
                
                elevator_set_stop_lamp(1);
                control_clear_control_matrix();
		
                int at_floor = (elevator_get_floor_sensor_signal() != -1); //Check if at or between floors
                if (at_floor) {
                    elevator_set_door_open_lamp(1);
                }
                while (elevator_get_stop_signal()) {}
            		if(at_floor){
            		    state = STOP_ST;
            		    control_set_emergency_flag(0); 
        		}
        		else{
        		    state = IDLE_ST;
        		    control_set_emergency_flag(1);
        		}
                elevator_set_stop_lamp(0);
        	    }break;


            default:
                break;
        }
    }


    return 0;
}
