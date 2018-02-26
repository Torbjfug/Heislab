#include "elev.h"
#include <stdio.h>
#include <time.h>
//define DEBUG

enum elevator_states
{
    INITIAL_ST, STOP_ST, IDLE_ST, EMERGENCY_ST, UP_ST, DOWN_ST 
};

int main() {
    // Initialize hardware
    enum tag_elev_motor_direction prev_dirn;
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
    if(elev_get_stop_signal()){
	elev_clear_button_control();
    }
    while(elev_get_obstruction_signal()){
	enum tag_elev_motor_direction new_dir;
	new_dir = elev_direction_control(old_dir);
	if (count < 1){
		printf("old_dir: %i\n",old_dir);
		printf("new_dir: %i\n",new_dir);
		printf("floor: %i\n",elev_floor_memory);
	        printf("get_Floor: %i\n",elev_get_floor_memory());

	}

	elev_set_motor_direction(DIRN_STOP);
	old_dir = DIRN_STOP;
	count++;
	}
    elev_set_floor_memory(elev_get_floor_sensor_signal());
    elev_update_button_control();
    //elev_set_floor_memory(elev_get_floor_sensor_signal());
    //printf("Floor: %i",elev_get_floor_memory());
    elev_set_floor_indicator(elev_floor_memory);
    }
#endif
#ifndef DEBUG
     while(1) {
	if(elev_get_obstruction_signal()){
	  elev_set_motor_direction(DIRN_STOP);
	  break;
   	}
	
        switch (state) {
            case INITIAL_ST:
                printf("Initial state\n");
                if (!elev_init()) {
                    printf("Unable to initialize elevator hardware!\n");
                    return 1;
                }
                elev_set_motor_direction(DIRN_UP);
		prev_dirn = DIRN_UP;
                while (elev_get_floor_sensor_signal() == -1)  {

		}
		elev_set_floor_indicator(elev_get_floor_sensor_signal());
		elev_set_floor_memory(elev_get_floor_sensor_signal());
                state = STOP_ST;
                break;


            case STOP_ST:		
		printf("Stop state\n");
		//printf("etasje: %i\n",elev_get_floor_memory());		
                elev_set_motor_direction(DIRN_STOP);
                elev_set_door_open_lamp(1);
                time_t start_time, current_time;
                start_time = time(NULL);
                current_time = time(NULL);
                while (current_time - start_time < 3) {
		   if(current_time - start_time > 0.1)
	                   elev_update_button_control(); 
		   if(elev_get_stop_signal()){
		        state = EMERGENCY_ST;
			goto emergency;
    		   }
                    current_time = time(NULL);

                }

                state = IDLE_ST;
                break;

            case IDLE_ST: {

                printf("Idle state\n");

                elev_set_door_open_lamp(0);
                enum tag_elev_motor_direction direction;
                direction = DIRN_IDLE;
                while (elev_direction_control(prev_dirn) == DIRN_IDLE) { //Bestilling finst ikkje
                    elev_update_button_control(); //sjekk bestilling
		    if(elev_get_stop_signal()){
                        state = EMERGENCY_ST;
                        goto emergency;
                   }

                }


                direction = elev_direction_control(prev_dirn); //set inn get-funksjon for retning
                printf("direction:%i\n",direction);
		elev_print_control_matrix();
		elev_clear_button_floor(direction,elev_get_floor_memory());
		elev_print_control_matrix();
		if (direction == DIRN_STOP){//ETASJE =DENNE
		    state = STOP_ST;
		}
                else if (direction == DIRN_UP)
                    state = UP_ST;
                else if (direction == DIRN_DOWN)
                    state = DOWN_ST;

            }break;

            case UP_ST: {
		prev_dirn = DIRN_UP;

                printf("Up state\n");

                elev_set_motor_direction(DIRN_UP);
                int floor, prev_floor;
		prev_floor = elev_get_floor_memory();


                do {
                    elev_update_button_control();
                    floor = elev_get_floor_sensor_signal();
		    if(elev_get_stop_signal()){
                        state = EMERGENCY_ST;
                        goto emergency;
                   }

                } while (floor < 0 || floor == prev_floor);
                elev_set_floor_indicator(floor);
		elev_set_floor_memory(floor);
                enum tag_elev_motor_direction direction;
                direction = DIRN_UP;
                direction = elev_direction_control(direction);
		
                if (direction == 0){
                    elev_clear_button_floor(DIRN_UP, floor);
		    state = STOP_ST;
		    }
		    
                else
                    state = UP_ST;

                elev_set_floor_indicator(floor);

            }break;

            case DOWN_ST: {
		prev_dirn = DIRN_DOWN;

                printf("Down state\n");

                elev_set_motor_direction(DIRN_DOWN);
                int floor, prev_floor;
		prev_floor = elev_get_floor_memory();
                do {
                    elev_update_button_control();
                    floor = elev_get_floor_sensor_signal();
		    
		    if(elev_get_stop_signal()){
                        state = EMERGENCY_ST;
                        goto emergency;
                   }

                }
                while (floor<0 || floor == prev_floor);
                elev_set_floor_indicator(floor);
                elev_set_floor_memory(floor);

                enum tag_elev_motor_direction direction;
                direction = DIRN_DOWN;
                direction = elev_direction_control(direction);

                if (direction == 0){
                    state = STOP_ST;
		    elev_clear_button_floor(DIRN_DOWN, floor);
		    }
                else
                    state = DOWN_ST;

            }break;

            case EMERGENCY_ST: {

		emergency:
                printf("Emergency state\n");
                elev_set_motor_direction(DIRN_STOP);
                elev_clear_button_control();
		
                int at_floor = (elev_get_floor_sensor_signal()) != -1;
		if (at_floor) {
                    elev_set_door_open_lamp(1);
                }
                while (elev_get_stop_signal()) {}
		if(at_floor){
		    state = STOP_ST;
		    elev_emergency_flag = 0;
		}
		else{
		    state = IDLE_ST;
		    elev_emergency_flag = 1;
		}break;
	    }


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
        } else if (elev_get_floor_sensor_signal() == 0) {
            ev_set_motor_direction(DIRN_UP);
        }

        // Stop elevator and exit program if the stop button is pressed
        if (elev_get_stop_signal()) {
            elev_set_motor_direction(DIRN_STOP);
            break;
        }
    }*/

    return 0;
}
