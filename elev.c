// Wrapper for libComedi Elevator control.
// These functions provides an interface to the elevators in the real time lab
//
// 2007, Martin Korsgaard


#include "channels.h"
#include "elev.h"
#include "io.h"

#include <assert.h>
#include <stdlib.h>

// Number of signals and lamps on a per-floor basis (excl sensor)
#define N_BUTTONS 3

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


int elev_init(void) {
    int i;

    // Init hardware
    if (!io_init())
        return 0;

    // Zero all floor button lamps
    for (i = 0; i < N_FLOORS; ++i) {
        if (i != 0)
            elev_set_button_lamp(BUTTON_CALL_DOWN, i, 0);

        if (i != N_FLOORS - 1)
            elev_set_button_lamp(BUTTON_CALL_UP, i, 0);

        elev_set_button_lamp(BUTTON_COMMAND, i, 0);
    }

    // Clear stop lamp, door open lamp, and set floor indicator to ground floor.
    elev_set_stop_lamp(0);
    elev_set_door_open_lamp(0);
    elev_set_floor_indicator(0);

    // Return success.
    return 1;
}

void elev_set_motor_direction(elev_motor_direction_t dirn) {
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

void elev_set_door_open_lamp(int value) {
    if (value)
        io_set_bit(LIGHT_DOOR_OPEN);
    else
        io_clear_bit(LIGHT_DOOR_OPEN);
}

int elev_get_obstruction_signal(void) {
    return io_read_bit(OBSTRUCTION);
}

int elev_get_stop_signal(void) {
    return io_read_bit(STOP);
}

void elev_set_stop_lamp(int value) {
    if (value)
        io_set_bit(LIGHT_STOP);
    else
        io_clear_bit(LIGHT_STOP);
}

int elev_get_floor_sensor_signal(void) {
    if (io_read_bit(SENSOR_FLOOR1))
    {
        elev_floor_memory = 0;
        return 0;
    }
    else if (io_read_bit(SENSOR_FLOOR2)) {
        elev_floor_memory = 1;
        return 1;
    }
    else if (io_read_bit(SENSOR_FLOOR3)) {
        elev_floor_memory = 2;
        return 2;
    }
    else if (io_read_bit(SENSOR_FLOOR4)) {
        elev_floor_memory = 3;
        return 3;
    }
    else {
        return -1;
    }
}

void elev_set_floor_indicator(int floor) {
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

int elev_get_button_signal(elev_button_type_t button, int floor) {
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

void elev_set_button_lamp(elev_button_type_t button, int floor, int value) {
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

void elev_clear_button_control() {
    for(int floors=0; floors<N_FLOORS; floors++)
    {
        for(int buttons=0; buttons<N_BUTTONS; buttons++) 
        {
            if (lamp_channel_matrix[floors][buttons]!=-1) //If button exists
            {
                elev_set_button_lamp(button_control_list[buttons], floors, 0);
            }
        }

    }
}

void elev_update_button_control(){
    for(int floors=0; floors<N_FLOORS; floors++)
    {
        for(int buttons=0; buttons<N_BUTTONS; buttons++) 
        {
            if (button_channel_matrix[floors][buttons]!=-1) //If button exists
            {
                if (elev_get_button_signal(button_control_list[buttons],floors)) {
                    elev_set_button_lamp(button_control_list[buttons], floors, 1);
                }
            }
                    
        }

    }
}


enum tag_elev_motor_direction elev_direction_control(enum tag_elev_motor_direction direction){
    int dir_number;
    if(direction==DIRN_UP)
        dir_number=0;
    else if(direction==DIRN_DOWN)
        dir_number=1;
    else
        return elev_direction_control_still();

    if(button_control_matrix[elev_floor_memory][2]||button_control_matrix[elev_floor_memory][dir_number]){
        return DIRN_STOP;}
    else
        return direction;
    }

enum tag_elev_motor_direction elev_direction_control_still(){
    for(int floors=0; floors<N_FLOORS; floors++)
        {
            for(int buttons=0; buttons<N_BUTTONS; buttons++) 
            {
                if (button_channel_matrix[floors][buttons]!=-1) //If button exists
                    if(button_control_matrix[floors][buttons])
                        if((elev_floor_memory-floors)<0)
                            return DIRN_UP;
                        else if((elev_floor_memory-floors)>0)
                            return DIRN_DOWN;
                        else{	
			    //elev_set_button_lamp(BUTTON_COMMAND,elev_floor_memory,0);
                            return DIRN_STOP;
			}
            }
        }
    return DIRN_IDLE;
}

void elev_clear_button_floor(enum tag_elev_motor_direction direction,int floor){
    elev_set_button_lamp(BUTTON_COMMAND,floor,0);
    if (direction == DIRN_DOWN){
	elev_set_button_lamp(BUTTON_CALL_DOWN,floor,0);
    }
    else if (direction == DIRN_UP){
        elev_set_button_lamp(BUTTON_CALL_UP,floor,0);
    }
}

