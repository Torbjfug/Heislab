//
// Created by SigurdH on 27/02/2018.
//

#ifndef HEISLAB_CONTROL_H
#define HEISLAB_CONTROL_H

#include "elevator.h"



/**
Remembers previous floor the elevator was in
*/
static int control_previous_floor;

/**
Flag set if previous the previous state was emergency state
0 ottherwise
*/
static int control_emergency_flag;

/**
Remembers prvious direction of the elevator
*/
elev_motor_direction_t control_previous_direction;


void control_set_previous_direction(elev_motor_direction_t value);
elev_motor_direction_t control_get_previous_direction(void);

void control_set_emergency_flag(int value);
int control_get_emergency_flag(void);

void control_set_previous_floor(int floor);
int control_get_previous_floor();


/**
  Initialize elevator.
  @return Non-zero on success, 0 on failure.
*/
int control_init(void);


/**
  Updates one spesific button in control matrix
  @param button Which button type to check. Can be BUTTON_CALL_UP,
    BUTTON_CALL_DOWN or BUTTON_COMMAND (button "inside the elevator).
  @param floor Current floor of the elevator.
  @param value 0 is off, 1 is on.
*/
void control_set_button(elev_button_type_t button, int floor, int value);

/**
  Clear command button and up and/or down button from control matrix depending on direction.
  @param direction Current direction of the elevator
  @param floor Current floor of the elevator
*/
void control_clear_floor_order(elev_motor_direction_t direction, int floor);

/**
  Loops through lamp_control_matrix to clear all orders and lamps.
*/
void control_clear_control_matrix();



/**
  Loops through all buttons to update button_control_matrix.
  Also turns on lamps when order existis.
*/
void control_update_control_matrix();

/**
  Prints values button_control_matrix
*/
void control_print_contol_matrix();

/**
  Returns DIRN_STOP, DIRN_IDLE, DIRN_UP or DIRN_UP, after calculating efficient route.
  @param direction Current direction of the elevator
  @return DIRN_STOP, DIRN_IDLE, DIRN_UP or DIRN_UP
*/
elev_motor_direction_t control_direction(elev_motor_direction_t direction);








#endif //HEISLAB_CONTROL_H
