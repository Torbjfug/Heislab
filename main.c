#include "elevator.h"
#include "control.h"
#include "timer.h"

#include <stdio.h>
#include <time.h>

//define DEBUG

enum elevator_states
{
    INITIAL_ST, STOP_ST, IDLE_ST, EMERGENCY_ST, UP_ST, DOWN_ST 
};

int main() {
    // Initialize hardware

    enum elevator_states state;
    state = INITIAL_ST;
 #ifdef DEBUG
    elev_init();
    enum tag_elev_motor_direction old_dir;
    elev_set_motor_direction(DIRN_STOP);
    int count;
    while(1){
    count = 0;
    if (elev_get_button_signal(BUTTON_CALL_DOWN,2)){
	old_dir = DIRN_DOWN;
   	elev_set_motor_direction(DIRN_DOWN);
	}
    if (elev_get_button_signal(BUTTON_CALL_UP,2)){
	elev_set_motor_direction(DIRN_UP);
	old_dir = DIRN_UP;
	}
    if(elevator_get_stop_signal()){
	elev_clear_button_control();
    }
    while(elev_get_obstruction_signal()){
	enum tag_elev_motor_direction new_dir;
	new_dir = elev_direction_control(old_dir);
	if (count < 1){
		printf("old_dir: %i\n",old_dir);
		printf("new_dir: %i\n",new_dir);
		printf("floor: %i\n",control_previous_floor);
	        printf("get_Floor: %i\n",elev_get_floor_memory());

	}

	elevator_set_motor_direction(DIRN_STOP);
	old_dir = DIRN_STOP;
	count++;
	}
    elev_set_floor_memory(elev_get_floor_sensor_signal());
    elev_update_button_control();
    //control_set_previous_floor(elevator_get_floor_sensor_signal());
    //printf("Floor: %i",control_get_previous_floor());
    elev_set_floor_indicator(control_previous_floor);
    }
#endif
#ifndef DEBUG
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
                while (elevator_get_floor_sensor_signal() == -1)  {

		}
                elevator_set_floor_indicator(elevator_get_floor_sensor_signal());
                control_set_previous_floor(elevator_get_floor_sensor_signal());
                state = STOP_ST;
                break;


            case STOP_ST:
                {
		          printf("Stop state\n");
		      //printf("etasje: %i\n",control_get_previous_floor());
                elevator_set_motor_direction(DIRN_STOP);
                elevator_set_door_open_lamp(1);
                timer_set(0.1,3);

                while (timer_get() != 2) {
		          if(timer_get() != 0){
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
                enum tag_elevator_motor_direction direction;
                direction = DIRN_IDLE;
                while (direction == DIRN_IDLE) { //Bestilling finst ikkje
                    direction = control_direction(control_get_previous_direction());
		    control_update_control_matrix(); //sjekk bestilling
		    if(elevator_get_stop_signal()){
                        state = EMERGENCY_ST;
                        goto emergency;
                   }

                }
                //set inn get-funksjon for retning
                printf("direction:%i\n",direction);
                //printed before
                control_clear_floor_order(direction, control_get_previous_floor());
		        //printed before
		if (direction == DIRN_STOP){//ETASJE =DENNE
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


                do {
                    control_update_control_matrix();
                    floor = elevator_get_floor_sensor_signal();
		    if(elevator_get_stop_signal()){
                        state = EMERGENCY_ST;
                        goto emergency;
                   }
		   if (control_get_emergency_flag() && floor == prev_floor){
                        break;
                    }


                } while (floor < 0 || floor == prev_floor);
                elevator_set_floor_indicator(floor);
                control_set_previous_floor(floor);
                enum tag_elevator_motor_direction direction;
                
		if(control_get_emergency_flag()){
                    control_set_emergency_flag(0);
                }

		
		direction = DIRN_UP;
                direction = control_direction(direction);
		
                if (direction == 0){
                    control_clear_floor_order(DIRN_UP, floor);
		    state = STOP_ST;
		    }
		    
                else
                    state = UP_ST;

                elevator_set_floor_indicator(floor);

            }break;

            case DOWN_ST: {
		control_set_previous_direction(DIRN_DOWN);

                printf("Down state\n");

                elevator_set_motor_direction(DIRN_DOWN);
                int floor, prev_floor;
		prev_floor = control_get_previous_floor();
                do {
                    control_update_control_matrix();
                    floor = elevator_get_floor_sensor_signal();
		    
		    if(elevator_get_stop_signal()){
                        state = EMERGENCY_ST;
                        goto emergency;
                   }
		    if(control_get_emergency_flag() && floor == prev_floor){
			break;
		    }

                }
                while (floor<0 || floor == prev_floor);
                elevator_set_floor_indicator(floor);
                control_set_previous_floor(floor);
		
		if(control_get_emergency_flag()){
                    control_set_emergency_flag(0);
                }


                       
                enum tag_elevator_motor_direction direction;
                direction = DIRN_DOWN;
                direction = control_direction(direction);

                if (direction == 0){
                    state = STOP_ST;
                    control_clear_floor_order(DIRN_DOWN, floor);
		    }
                else
                    state = DOWN_ST;

            }break;

            case EMERGENCY_ST: {

		emergency:
                printf("Emergency state\n");
                elevator_set_motor_direction(DIRN_STOP);
                elevator_set_stop_lamp(1);
                control_clear_control_matrix();
		
                int at_floor = (elevator_get_floor_sensor_signal()) != -1;
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

 #endif




    /*
    while (1) {
        // Change direction when we reach top/bottom floor

        if (elev_get_floor_sensor_signal() == N_FLOORS - 1) {
            elev_set_motor_direction(DIRN_DOWN);
        } else if (elevator_get_floor_sensor_signal() == 0) {
            ev_set_motor_direction(DIRN_UP);
        }

        // Stop elevator and exit program if the stop button is pressed
        if (elevator_get_stop_signal()) {
            elevator_set_motor_direction(DIRN_STOP);
            break;
        }
    }*/

    return 0;
}
