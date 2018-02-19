#include "elev.h"
#include <stdio.h>
#include <time.h>

enum elevator_states
{
    INITIAL_ST, STOP_ST, IDLE_ST, EMERGENCY_STOP_ST, UP_ST, DOWN_ST 
}

int main() {
    // Initialize hardware
    

    elevator_states states;
    states = INITIAL_ST;

    switch states
    {
        case INITIAL_ST:
        
            if (!elev_init()) 
            {
                printf("Unable to initialize elevator hardware!\n");
                return 1;
            }
            elev_set_motor_direction(DIRN_UP);
            while (elev_get_floor_sensor_signal == -1){}
            states = STOP_ST;
            
        break;

        case STOP_ST:
        
            elev_set_motor_direction(DIRN_STOP);
            elev_set_door_open_lamp(1);
            time_t start_time, current_time;
            start_time = time(NULL);
            current_time = time(NULL);
            while(current_time - start_time < 3)
            {
                
                //SJEKK BESTILLING

                current_time = time(NULL);

            }

            states = INITIAL_ST;
        break;

        case IDLE_ST:
        
            elev_set_door_open_lamp(0);

            while(0){}//Bestilling finst ikkje
                //sjekk bestilling

            
                tag_elev_motor_direction direction;
                direction = 1; //set inn get-funksjon for retning
                if(direction == DIRN_STOP)//ETASJE == DENNE
                    states = STOP_ST;
                else if(direction == DIRN_UP)
                    states == UP_ST;
                else if(direction == DIRN_DOWN)
                    states == DOWN_ST;
        break;

        case UP_ST:
        {
           elev_set_motor_direction(DIRN_UP);
           int floor;
           

           do
           {
                //Oppdater knappar
                floor = elev_get_floor_sensor_signal();
           } while(floor < 0 || floor == elev_floor_memory);
           elev_set_floor_indicator(floor);
           elev_floor_memory = floor;
           tag_elev_motor_direction direction;
           direction = 0;//Få retnig eller stopp

           if (direction == 0)
                states = STOP_ST;
            else 
                states = UP_ST;
        break;

        case DOWN_ST:
        
           elev_set_motor_direction(DIRN_DOWN);
           int floor;
           {
                //Oppdater knappar
                floor = elev_get_floor_sensor_signal();
           } while(floor < 0 || floor == elev_floor_memory);
           elev_set_floor_indicator(floor);
           elev_floor_memory = floor;
           
           tag_elev_motor_direction direction;
           direction = 0;//Få retnig eller stopp

           if (direction == 0)
                states = STOP_ST;
            else 
                states = DOWN_ST;
        break;

        case EMERGENCY_STOP_ST:

            elev_set_motor_direction(DIRN_STOP);
            //Slett bestilling
            if(elev_get_floor_sensor_signal() != -1)
            {
                elev_set_door_open_lamp(1)
            }

            while(elev_get_stop_signal())
            {}


        default:
            break;
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
