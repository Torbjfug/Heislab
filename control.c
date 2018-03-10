#include "channels.h"
#include "control.h"
#include "io.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>


//types to control buttons + lights
static const elev_button_type_t button_control_list[3] = {BUTTON_CALL_UP, BUTTON_CALL_DOWN, BUTTON_COMMAND};

//Matrix of pending orders
static int button_control_matrix[N_FLOORS][N_BUTTONS] = {
        {0,0,0},  //{LIGHT_UP1, LIGHT_DOWN1, LIGHT_COMMAND1},
        {0,0,0},  //{LIGHT_UP2, LIGHT_DOWN2, LIGHT_COMMAND2},
        {0,0,0},  //{LIGHT_UP3, LIGHT_DOWN3, LIGHT_COMMAND3},
        {0,0,0},  //{LIGHT_UP4, LIGHT_DOWN4, LIGHT_COMMAND4},
};


void control_set_emergency_flag(int value){
    control_emergency_flag=value;
}
int control_get_emergency_flag(void){
    return control_emergency_flag;
}

void control_set_previous_direction(elev_motor_direction_t value){
    control_previous_direction=value;
}


elev_motor_direction_t control_get_previous_direction(void){
    return control_previous_direction;
}

void control_set_previous_floor(int floor){
    if (floor >= 0 && floor < N_FLOORS){
        control_previous_floor = floor;
    }
}

int control_get_previous_floor(){
    return control_previous_floor;
}



int control_init(void) {
    // Init hardware
    if (!io_init())
        return 0;
    control_clear_control_matrix();
    elevator_set_stop_lamp(0);
    elevator_set_door_open_lamp(0);
    elevator_set_floor_indicator(0);
    control_set_emergency_flag(0);
    return 1;
}

void control_set_button(elev_button_type_t button, int floor, int value) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS - 1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button == BUTTON_COMMAND);
    
    button_control_matrix[floor][button]=value;
    elevator_set_button_lamp(floor,button, value);
}

void control_clear_floor_order(elev_motor_direction_t direction, int floor){
    control_set_button(BUTTON_COMMAND, floor, 0);

    if (floor == 0){
        control_set_button(BUTTON_CALL_UP, floor, 0);
    }

    else if (floor == N_FLOORS-1){
        control_set_button(BUTTON_CALL_DOWN, floor, 0);
    }

    else if (direction == DIRN_STOP){
        control_set_button(BUTTON_CALL_DOWN, floor, 0);
        control_set_button(BUTTON_CALL_UP, floor, 0);
    }
    else if(direction == DIRN_DOWN){
        control_set_button(BUTTON_CALL_DOWN, floor, 0);
    }
    else if (direction == DIRN_UP){
        control_set_button(BUTTON_CALL_UP, floor, 0);
    }

}

void control_clear_control_matrix() {
    for(int floor=0; floor<N_FLOORS; floor++){
        for(int button=0; button<N_BUTTONS; button++){
            if (elevator_get_lamp_channel_matrix(floor, button)!=-1){ //If button exists
                control_set_button(button_control_list[button], floor, 0);
            }
        }

    }
}

void control_update_control_matrix(){
    for(int floor=0; floor<N_FLOORS; floor++){
        for(int button=0; button<N_BUTTONS; button++){
            if (elevator_get_button_channel_matrix(floor, button)!=-1){//If button exists{
                if (elevator_get_button_signal(button_control_list[button], floor)) {
                    control_set_button(button_control_list[button], floor, 1);
                }
            }

        }

    }
}

void control_print_control_matrix(void){
    for (int i = 0; i < N_FLOORS; ++i){
        for (int j = 0; j < 3;++j){
            printf("%i ", button_control_matrix[i][j]);
        }printf("\n");
    }
}


elev_motor_direction_t control_direction(elev_motor_direction_t direction){
    if (control_emergency_flag){
    	for (int button = 0; button < N_BUTTONS; ++button){
    	    if(button_control_matrix[control_previous_floor][button] == 1){
        		control_previous_direction *= -1; //Set direction to opposit direction		
        		return control_previous_direction;
    	    }
    	} 
    }

    //Default direction is up
    if (direction == DIRN_STOP || direction == DIRN_IDLE){
        direction = DIRN_UP;
    }

    int dirn = direction;
    int current_floor, floor_itterator ,button;
    current_floor = control_get_previous_floor();
    floor_itterator = current_floor;
    
    //Check if the up or down button is to be checked
    if (dirn == DIRN_UP){
        button = 0;
    }
    else{
        button = 1;
    }

    if(button_control_matrix[floor_itterator][button] == 1 
        || button_control_matrix[floor_itterator][2] == 1){ 
        return DIRN_STOP; //If there is an order at the current floor in the current direction
    }

    do{ 
        if (floor_itterator == N_FLOORS-1 || floor_itterator < 0){ //Looping through orders counter clockwise
            dirn *= -1;
            button = (button + 1) % 2;
        }
        if(button_control_matrix[floor_itterator][button] == 1 
            || button_control_matrix[floor_itterator][2] == 1){
            if(floor_itterator > current_floor){
                return DIRN_UP;
            }
            else if (floor_itterator < current_floor){
                return DIRN_DOWN;
            }
            else{
                return DIRN_STOP;
            }
        }
        floor_itterator += dirn;
    }while(floor_itterator != current_floor || dirn != direction); //No order found after one loop
    
    return DIRN_IDLE;


}

int control_get_control_matrix_previous_floor(){
    int floor=control_previous_floor;
    for(int button=0; button<N_BUTTONS; button++){
        if (elevator_get_button_channel_matrix(floor, button)!=-1){//If button exists{
            if (elevator_get_button_signal(button_control_list[button], floor)) {
                return 1;
            }
        }
    }
    return 0;
}








