//
// Created by SigurdH on 27/02/2018.
//

#ifndef HEISLAB_CONTROL_H
#define HEISLAB_CONTROL_H

#include "elevator.h"

static int control_previous_floor;
static int control_emergency_flag = 0;
enum tag_elevator_motor_direction control_previous_direction;

void control_set_previous_direction(enum tag_elevator_motor_direction value);
enum tag_elevator_motor_direction control_get_previous_direction(void);

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
  Loops through lamp_control_matrix to clear all lamps.
*/
void control_clear_control_matrix();


/**
  Loops through all buttons to update register button_control_matrix.
  Also turns on lamps when pressed.
*/
void control_update_control_matrix();


/**
  Returns DIRN_STOP, DIRN_UP or DIRN_UP, after calculating efficient route.
  @param direction Current direction of the elevator
  @return DIRN_STOP, DIRN_UP or DIRN_UP
*/
enum tag_elevator_motor_direction control_direction(enum tag_elevator_motor_direction direction);

/**
  Used in elev_direction_control
  Calculates what direction to start in if motor is still.
  @return DIRN_STOP, DIRN_UP or DIRN_UP
*/
//enum tag_elevator_motor_direction elev_direction_control_still();

//void elev_clear_floor_memory(enum tag_elevator_motor_direction direction, int floor);

/**
  Clear command button and up and/or down button from control matrix.
  @param direction Current direction of the elevator
  @param floor Current floor of the elevator
*/
void control_clear_floor_order(enum tag_elevator_motor_direction direction, int floor);

/**
  In case of feilsøking. Prints control matrix in matrix form.
*/
void control_print_contol_matrix();

/**
  Updates one spesific button in control matrix
  @param button Which button type to check. Can be BUTTON_CALL_UP,
    BUTTON_CALL_DOWN or BUTTON_COMMAND (button "inside the elevator).
  @param floor Current floor of the elevator.
  @param value 0 is off, 1 is on.
*/
void control_set_button(elev_button_type_t button, int floor, int value);

#endif //HEISLAB_CONTROL_H
