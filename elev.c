// Wrapper for libComedi Elevator control.
// These functions provides an interface to the elevators in the real time lab
//
// 2007, Martin Korsgaard


#include "channels.h"
#include "elev.h"
#include "io.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

// Number of signals and lamps on a per-floor basis (excl sensor)



static const int lamp_channel_matrix[N_FLOORS][N_BUTTONS] = {
    {LIGHT_UP1, LIGHT_DOWN1, LIGHT_COMMAND1},
    {LIGHT_UP2, LIGHT_DOWN2, LIGHT_COMMAND2},
    {LIGHT_UP3, LIGHT_DOWN3, LIGHT_COMMAND3},
    {LIGHT_UP4, LIGHT_DOWN4, LIGHT_COMMAND4},
};


static const int button_channel_matrix[N_FLOORS][N_BUTTONS] = {
    {BUTTON_UP1, BUTTON_DOWN1, BUTTON_COMMAND1},
    {BUTTON_UP2, BUTTON_DOWN2, BUTTON_COMMAND2},
    {BUTTON_UP3, BUTTON_DOWN3, BUTTON_COMMAND3},
    {BUTTON_UP4, BUTTON_DOWN4, BUTTON_COMMAND4},
};
 //types to control buttons + lights
static const elev_button_type_t button_control_list[3] = {BUTTON_CALL_UP, BUTTON_CALL_DOWN, BUTTON_COMMAND};

// Global control of lamps on floors
static int button_control_matrix[N_FLOORS][N_BUTTONS] = {
    {0,0,0},  //{LIGHT_UP1, LIGHT_DOWN1, LIGHT_COMMAND1},
    {0,0,0},  //{LIGHT_UP2, LIGHT_DOWN2, LIGHT_COMMAND2},
    {0,0,0},  //{LIGHT_UP3, LIGHT_DOWN3, LIGHT_COMMAND3},
    {0,0,0},  //{LIGHT_UP4, LIGHT_DOWN4, LIGHT_COMMAND4},
};


int control_init(void) {


    // Init hardware
    if (!io_init())
        return 0;

    // Zero all floor button lamps
    control_clear_control_matrix();

    // Clear stop lamp, door open lamp, and set floor indicator to ground floor.
    elevator_set_stop_lamp(0);
    elevator_set_door_open_lamp(0);
    elevator_set_floor_indicator(0);

    // Return success.
    return 1;
}

static int emergency_flag = 0;

void elevator_set_motor_direction(elev_motor_direction_t dirn) {


    if (dirn == 0){
        io_write_analog(MOTOR, 0);
    } else if (dirn > 0) {
        io_clear_bit(MOTORDIR);
        io_write_analog(MOTOR, 2800);
    } else if (dirn < 0) {
        io_set_bit(MOTORDIR);
        io_write_analog(MOTOR, 2800);
    }
}

void elevator_set_door_open_lamp(int value) {
    if (value)
        io_set_bit(LIGHT_DOOR_OPEN);
    else
        io_clear_bit(LIGHT_DOOR_OPEN);
}

int elevator_get_obstruction_signal(void) {
    return io_read_bit(OBSTRUCTION);
}

int elevator_get_stop_signal(void) {
    return io_read_bit(STOP);
}

void elevator_set_stop_lamp(int value) {
    if (value)
        io_set_bit(LIGHT_STOP);
    else
        io_clear_bit(LIGHT_STOP);
}

int elevator_get_floor_sensor_signal(void) {
    if (io_read_bit(SENSOR_FLOOR1))
    {

        return 0;
    }
    else if (io_read_bit(SENSOR_FLOOR2)) {
	printf("Etasjenumemr1");
        return 1;
    }
    else if (io_read_bit(SENSOR_FLOOR3)) {

        return 2;
    }
    else if (io_read_bit(SENSOR_FLOOR4)) {

        return 3;
    }
    else {
        return -1;
    }
}


void elevator_set_floor_indicator(int floor) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);

    // Binary encoding. One light must always be on.
    if (floor & 0x02)
         io_set_bit(LIGHT_FLOOR_IND1);


    else
        io_clear_bit(LIGHT_FLOOR_IND1);

    if (floor & 0x01)
        io_set_bit(LIGHT_FLOOR_IND2);
    else
        io_clear_bit(LIGHT_FLOOR_IND2);
}

int elevator_get_button_signal(elev_button_type_t button, int floor) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS - 1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button == BUTTON_COMMAND);

    if (io_read_bit(button_channel_matrix[floor][button]))
        return 1;
    else
        return 0;
}

void control_set_button(elev_button_type_t button, int floor, int value) {
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS - 1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button == BUTTON_COMMAND);

    button_control_matrix[floor][button]=value;

    if (value)
        io_set_bit(lamp_channel_matrix[floor][button]);
    else
        io_clear_bit(lamp_channel_matrix[floor][button]);
}


void control_clear_control_matrix() {
    for(int floors=0; floors<N_FLOORS; floors++)

    {
        for(int buttons=0; buttons<N_BUTTONS; buttons++) 
        {
            if (lamp_channel_matrix[floors][buttons]!=-1) //If button exists
            {
                control_set_button(button_control_list[buttons], floors, 0);
            }
        }

    }
}

void control_update_control_matrix(){
    for(int floors=0; floors<N_FLOORS; floors++)
    {
        for(int buttons=0; buttons<N_BUTTONS; buttons++) 
        {
            if (button_channel_matrix[floors][buttons]!=-1) //If button exists
            {
                if (elevator_get_button_signal(button_control_list[buttons], floors)) {
                    control_set_button(button_control_list[buttons], floors, 1);
                }
            }
                    
        }

    }
}

enum tag_elevator_motor_direction control_direction(enum tag_elevator_motor_direction direction){
    if (control_emergency_flag){
        
    }
    if (direction == DIRN_STOP || direction == DIRN_IDLE){
        direction = DIRN_UP;
    }
    int dirn = direction;
    int current_floor, i,button;
    current_floor = control_get_previous_floor();
    i = current_floor;
    if (dirn == 1){
	button = 0;
    }
    else{
	button = 1;
    }
    if(button_control_matrix[i][button] == 1 || button_control_matrix[i][2] == 1){
//	printf("button %i 2 %i\n",button_control_matrix[i][button],button_control_matrix[i][2]);
	return DIRN_STOP;
    }
    
    do{
	
	if (i == N_FLOORS-1 || i < 0){
            dirn *= -1;
            button = (button + 1) % 2;
        }
        if(button_control_matrix[i][button] == 1 || button_control_matrix[i][2] == 1){
	printf("returned\n");
;
	    if(i > current_floor){
		return DIRN_UP;
	    }
	    else if (i < current_floor){
	        return DIRN_DOWN;
 	    }
	    else{
	        return DIRN_STOP;
	    }
	}
	i += dirn;
	//printf("i: %i, Current_floor: %i\n",i,current_floor);
	//printf("dirn: %i direction: %i\n",dirn,direction);
    }while(i != current_floor || dirn != direction);
    //printf("Ended\n");
    return DIRN_IDLE;


}

/*
enum tag_elevator_motor_direction control_direction(enum tag_elevator_motor_direction direction){
    int dir_number;
    if(direction==DIRN_UP)
        dir_number=0;
    else if(direction==DIRN_DOWN)
        dir_number=1;
    else
        return elev_direction_control_still();
    
    int floor = elev_get_floor_memory();
    if(floor == 0 || floor == (N_FLOORS - 1)){
	return DIRN_STOP;
    }    
    

    if(button_control_matrix[control_previous_floor][2]||button_control_matrix[control_previous_floor][dir_number]){
        return DIRN_STOP;}
    else
        return direction;
    }

printf("button after: %i\n", button_control_matrix[2][2]);
enum tag_elevator_motor_direction elev_direction_control_still(){
    for(int floors=0; floors<N_FLOORS; floors++)
        {
            for(int buttons=0; buttons<N_BUTTONS; buttons++) 
            {
                if (button_channel_matrix[floors][buttons]!=-1) //If button exists
                    if(button_control_matrix[floors][buttons])
                        if((elev_get_floor_memory()-floors)<0)
                            return DIRN_UP;
                        else if((control_get_previous_floor()-floors)>0)
                            return DIRN_DOWN;
                        else{	
			    //control_set_button(BUTTON_COMMAND,control_previous_floor,0);
                            return DIRN_STOP;
			}
            }
        }
    return DIRN_IDLE;
}
*/
void control_clear_floor_order(enum tag_elevator_motor_direction direction, int floor){
    printf("Clear button Floor:%i, Direction: %i\n", floor, direction);
    control_set_button(BUTTON_COMMAND, floor, 0);
    printf("button before: %i\n", button_control_matrix[floor][2]);

    if (floor == 0){
        control_set_button(BUTTON_CALL_UP, floor, 0);
    }

    else if (floor == N_FLOORS-1){
        control_set_button(BUTTON_CALL_DOWN, floor, 0);
    }
    
    else if (direction == DIRN_STOP){
	printf("Cleared stop\n");
        control_set_button(BUTTON_CALL_DOWN, floor, 0);
        control_set_button(BUTTON_CALL_UP, floor, 0);
    }
    else if(direction == DIRN_DOWN){
        control_set_button(BUTTON_CALL_DOWN, floor, 0);
    }
    else if (direction == DIRN_UP){
        control_set_button(BUTTON_CALL_UP, floor, 0);
    }
    printf("button after: %i\n", button_control_matrix[floor][2]);

}

void control_set_previous_floor(int floor){
    if (floor >= 0 && floor < N_FLOORS){
        control_previous_floor = floor;
    }
}

int control_get_previous_floor(){
    return control_previous_floor;
}

void control_print_control_matrix(void){
    for (int i = 0; i < N_FLOORS; ++i){
	for (int j = 0; j < 3;++j){
	    printf("%i ", button_control_matrix[i][j]);
	}printf("\n");
    }
}
