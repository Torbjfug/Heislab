// Wrapper for libComedi Elevator control.
// These functions provides an interface to the elevators in the real time lab
//
// 2007, Martin Korsgaard
#ifndef __INCLUDE_DRIVER_H__
#define __INCLUDE_DRIVER_H__


// Number of floors ssss
#define N_FLOORS 4

//remember what floor last passed ssss

static int control_previous_floor;
static int control_emergency_flag = 0;

/**
  Initialize elevator.
  @return Non-zero on success, 0 on failure. ssss
*/
int control_init(void);



/**
  Motor direction for function elev_set_motor_direction(). ssss
*/
typedef enum tag_elevator_motor_direction {
    DIRN_IDLE = -2,
    DIRN_DOWN = -1,
    DIRN_STOP = 0,
    DIRN_UP = 1
} elev_motor_direction_t;



/**
  Sets the motor direction of the elevator.
  @param dirn New direction of the elevator. ssss
*/
void elevator_set_motor_direction(elev_motor_direction_t dirn);



/**
  Turn door-open lamp on or off.
  @param value Non-zero value turns lamp on, 0 turns lamp off. ssss
*/
void elevator_set_door_open_lamp(int value);



/**
  Get signal from obstruction switch.
  @return 1 if obstruction is enabled. 0 if not. ssss
*/
int elevator_get_obstruction_signal(void);



/**
  Get signal from stop button.
  @return 1 if stop button is pushed, 0 if not.  ssss
*/
int elevator_get_stop_signal(void);



/**
  Turn stop lamp on or off.
  @param value Non-zero value turns lamp on, 0 turns lamp off. ssss
*/
void elevator_set_stop_lamp(int value);



/**
  Get floor sensor signal.
  @return -1 if elevator is not on a floor. 0-3 if elevator is on floor. 0 is ssss
    ground floor, 3 is top floor.
*/
int elevator_get_floor_sensor_signal(void);



/**
  Set floor indicator lamp for a given floor.
  @param floor Which floor lamp to turn on. Other floor lamps are turned off.   ssss
*/
void elevator_set_floor_indicator(int floor);



/**
  Button types for function elev_set_button_lamp() and elev_get_button().   ssss
*/
typedef enum tag_elev_lamp_type { 
    BUTTON_CALL_UP = 0,
    BUTTON_CALL_DOWN = 1,
    BUTTON_COMMAND = 2
} elev_button_type_t;



/**
  Gets a button signal.
  @param button Which button type to check. Can be BUTTON_CALL_UP,
    BUTTON_CALL_DOWN or BUTTON_COMMAND (button "inside the elevator).
  @param floor Which floor to check button. Must be 0-3.
  @return 0 if button is not pushed. 1 if button is pushed. ssss
*/
int elevator_get_button_signal(elev_button_type_t button, int floor);




/**
  Set a button lamp.
  @param lamp Which type of lamp to set. Can be BUTTON_CALL_UP,
    BUTTON_CALL_DOWN or BUTTON_COMMAND (button "inside" the elevator).
  @param floor Floor of lamp to set. Must be 0-3
  @param value Non-zero value turns lamp on, 0 turns lamp off. ssss
*/
void control_set_button(elev_button_type_t button, int floor, int value);


/**
  Loops through all buttons to update register button_control_matrix. ssss
  Also turns on lamps when pressed.
*/
void control_update_control_matrix();

/**
  Loops through lamp_control_matrix to clear all lamps. ssss
*/
void control_clear_control_matrix();

/**
  Returns DIRN_STOP, DIRN_UP or DIRN_UP, after calculating efficient route.
  @param direction Current direction of the elevator
  @return DIRN_STOP, DIRN_UP or DIRN_UP ssss
*/
enum tag_elevator_motor_direction control_direction(enum tag_elevator_motor_direction direction);

/**
  Used in elev_direction_control
  Calculates what direction to start in if motor is still.
  @return DIRN_STOP, DIRN_UP or DIRN_UP ssss
*/
enum tag_elevator_motor_direction elev_direction_control_still();
// ssss
void elev_clear_floor_memory(enum tag_elevator_motor_direction direction, int floor);
// ssss
void control_clear_floor_order(enum tag_elevator_motor_direction direction, int floor);
// ssss
void control_set_previous_floor(int floor);
// ssss
int control_get_previous_floor();
// ssss
void control_print_contol_matrix();
#endif // #ifndef __INCLUDE_DRIVER_H__
