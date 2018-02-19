#include "elev.h"
#include <stdio.h>
#include <time.h>

enum elevator_states
{
    INITIAL_ST, STOP_ST, IDLE_ST, EMERGENCY_STOP_ST, UP_ST, DOWN_ST 
};

int main() {
    // Initialize hardware
    

    enum elevator_states state;

    state = INITIAL_ST;
    while(1) {
        switch (state) {
            case INITIAL_ST:
                printf("Initial state\n");
                if (!elev_init()) {
                    printf("Unable to initialize elevator hardware!\n");
                    return 1;
                }
                elev_set_motor_direction(DIRN_UP);
                while (elev_get_floor_sensor_signal() == -1) {}
                state = STOP_ST;
                break;


            case STOP_ST:

                printf("Stop state\n");

                elev_set_motor_direction(DIRN_STOP);
                elev_set_door_open_lamp(1);
                time_t start_time, current_time;
                start_time = time(NULL);
                current_time = time(NULL);
                while (current_time - start_time < 3) {

                    //SJEKK BESTILLING

                    current_time = time(NULL);

                }

                state = IDLE_ST;
                break;

            case IDLE_ST: {

                printf("Idle state\n");

                elev_set_door_open_lamp(0);
                enum tag_elev_motor_direction direction;
                direction = DIRN_IDLE;
                while (elev_direction_control(direction) == DIRN_IDLE) { //Bestilling finst ikkje
                    elev_update_button_control(); //sjekk bestilling
                }


                direction = elev_direction_control(direction); //set inn get-funksjon for retning
                if (direction == DIRN_STOP)//ETASJE == DENNE
                    state = STOP_ST;
                else if (direction == DIRN_UP)
                    state = UP_ST;
                else if (direction == DIRN_DOWN)
                    state = DOWN_ST;

            }break;

            case UP_ST: {

                printf("Up state\n");

                elev_set_motor_direction(DIRN_UP);
                int floor;


                do {
                    elev_update_button_control();
                    floor = elev_get_floor_sensor_signal();
                } while (floor < 0 || floor == elev_floor_memory);
                elev_set_floor_indicator(floor);
                elev_floor_memory = floor;
                enum tag_elev_motor_direction direction;
                direction = DIRN_UP;
                direction = elev_direction_control(direction);

                if (direction == 0)
                    state = STOP_ST;
                else
                    state = UP_ST;

                elev_set_floor_indicator(floor);

            }break;

            case DOWN_ST: {

                printf("Down state\n");

                elev_set_motor_direction(DIRN_DOWN);
                int floor;
                do {
                    elev_update_button_control();
                    floor = elev_get_floor_sensor_signal();
                }
                while (floor < 0 || floor == elev_floor_memory);
                elev_set_floor_indicator(floor);
                elev_floor_memory = floor;

                enum tag_elev_motor_direction direction;
                direction = DIRN_DOWN;
                direction = elev_direction_control(direction);

                if (direction == 0)
                    state = STOP_ST;
                else
                    state = DOWN_ST;
                elev_set_floor_indicator(floor);

            }break;

            case EMERGENCY_STOP_ST:

                printf("Emergency state\n");

                elev_set_motor_direction(DIRN_STOP);
                //Slett bestilling
                if (elev_get_floor_sensor_signal() != -1) {
                    elev_set_door_open_lamp(1);
                }

                while (elev_get_stop_signal()) {}


            default:
                break;
        }
    }

    





    /*
    while (1) {
        // Change direction when we reach top/bottom floor

        if (elev_get_floor_sensor_signal() == N_FLOORS - 1) {
            elev_set_motor_direction(DIRN_DOWN);
        } else if (elev_get_floor_sensor_signal() == 0) {
            elev_set_motor_direction(DIRN_UP);
        }

        // Stop elevator and exit program if the stop button is pressed
        if (elev_get_stop_signal()) {
            elev_set_motor_direction(DIRN_STOP);
            break;
        }
    }*/

    return 0;
}
