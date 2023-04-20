/* file: motor.c
 * Motor control file for radio-telescope project
 * Authors: Zach Martin & Aaron Olsen
 * Date: 4/20/2023
 * 
 * TODO:
 *  generic motor update function for both motors               DONE
               (can I pass a struct maybe?)
 *  interface with just one motor_update function in main       DONE
 *  handle PWM-ing so we can go to a place wihtout overshoot
 *  for some reason motor controls are also occuring in the 
               command processing function. don't do that.
 *  add motor max and min in struct so we can prevent 
               breaking the antenna
 * 
*/

#include "motor.h"

// Motor movement types. Manual +/- or degrees
enum {
  MOVE_NEG,
  MOVE_POS,
  MOVE_STOP,
  MOVE_DEGREES
};

// to keep track of PWM cycle high or low
enum
{
    PWM_PWL = 0,
    PWM_PWH,
    PWM_NONE
};

// Public function for updating in main loop
void update_motors_new(void);

// Private functions
static void _update_alt(void);
static void _update_az(void);
static void _motor_update(struct _Motor *, uint8_t desired_move, uint16_t target_degrees); // Pass by reference. &motor.alt for example



// public function for updating in main
void update_motors_new(void)
{
    static uint8_t desired_move_alt = MOVE_STOP;
    static uint8_t desired_move_az = MOVE_STOP;
    static commands_t old_command = CMD_NONE;
    
    // If we got a new command, then update the move type
    if(command.command != old_command)
    {
        switch(command.command)
        {
            case CMD_ALT_NEG:
                desired_move_alt = MOVE_NEG;
                break;

        }
    }
    // Make decisions on how to rotate motors
    _motor_update(&motor.alt, desired_move_alt, command.alt_deg);
    _motor_update(&motor.az, desired_move_az, command.az_deg);
    
    // Now act on decisions for each axis
    // Altitude axis (AKA elevation AKA up/down )
    // Stop motor
    if(motor.alt.dir == MOTOR_STOP || motor.alt.pwm == PWM_PWL)
    {
        EL_CONTROL1_SetLow();
        EL_CONTROL2_SetLow();
    }
    // Altitude positive (UP)
    if(motor.alt.dir == MOTOR_POS && motor.alt.pwm > 0)
    {
       EL_CONTROL2_SetLow(); // Just to be sure
       EL_CONTROL1_SetHigh();
    }
    // Altitude negative (down)
    if(motor.alt.dir == MOTOR_POS && motor.alt.pwm > 0)
    {
       EL_CONTROL1_SetLow(); // Just to be sure
       EL_CONTROL2_SetHigh();
    }
    
    // Aziumuth axis (AKA north-south left-right axis)
    // Stop motor
    if(motor.az.dir == MOTOR_STOP || motor.alt.pwm == PWM_PWL)
    {
        AZ_CONTROL1_SetLow();
        AZ_CONTROL2_SetLow();
    }
    // Azimuth positive
    if(motor.az.dir == MOTOR_POS && motor.alt.pwm > 0)
    {
       AZ_CONTROL2_SetLow(); // Just to be sure
       AZ_CONTROL1_SetHigh();
    }
    // Azimuth negative
    if(motor.az.dir == MOTOR_POS && motor.alt.pwm > 0)
    {
       AZ_CONTROL1_SetLow(); // Just to be sure
       AZ_CONTROL2_SetHigh();
    }
}

// Generic motor update for either alt or az
// Pass by reference. &motor.alt for example
// access members by dereferencing, eg mptr->degrees
// Move_type can be   MOVE_NEG,  MOVE_POS,  MOVE_STOP, and  MOVE_DEGREES
#define DIR_CHANGE_DELAY 600 // #ms until motor is allowed to switch directions
#define PWM_HIGH_TIME 50        // $ms high
#define PWM_LOW_TIME 100        // 33% pwm



static void _motor_update(struct _Motor *mptr, uint8_t desired_move, uint16_t target_degrees) 
{
    // Handle commands to stop
    if(desired_move == MOVE_STOP)
    {
        mptr->dir = MOTOR_STOP;                 // OK, we can stop
        mptr->off_timestamp = timestamp_raw(); // Make sure we allow everything to settle so we dont accidentally break something by doing go + -> stop -> go -
        return; // TODO: return early so we get quickest response
    }
    
    // Handle commands to go to a certain location
    if(desired_move == MOVE_DEGREES)
    {
        if(mptr->degrees > target_degrees)
        {
            if(mptr->degrees - target_degrees > 10) // within 1 degree
            {
                mptr->speed = 1; // slow down
            }
            desired_move = MOVE_NEG;
        }
        else if(mptr->degrees < target_degrees)
        {
            if(target_degrees - mptr->degrees > 10) // within 1 degree
            {
                mptr->speed = 1; // slow down
            }
            desired_move = MOVE_POS;
        }
        else // We have reached the location!
        {
            // return to previous speed TODO
            mptr->speed = 3;
            mptr->pwm = PWM_NONE;
            return; // TODO: early return bad idea?
        }
    }
    
    // Handle commands to move negative direction
    if(desired_move == MOVE_NEG)
    {
        // We're already going in negative direction, so do nothing.
        if(mptr->dir == MOTOR_NEG )
        {
            if(mptr->speed < 3) // 3 is max speed TODO
            {
                if(mptr->pwm == PWM_NONE) // initialize PWM
                {
                    mptr->pwm = PWM_PWH;
                    mptr->pwm_timestamp = timestamp_raw();
                }
                
            }
            // We are PWM-ing and currently are in the high part
            if(mptr->pwm == PWM_PWH && timestamp_to_ms(timestamp_raw() - mptr->on_timestamp) > PWM_HIGH_TIME)
            {
                mptr->dir = MOTOR_STOP;
                mptr->off_timestamp = timestamp_raw();
                mptr->pwm = PWM_PWL;
            }
                
        }

        /* We were going in positive direction, so we have to stop, 
         * wait, and then change direction. */
        else if(mptr->dir == MOTOR_POS)
        {
            mptr->dir = MOTOR_STOP;
            mptr->off_timestamp = timestamp_raw();
        }
        
        /* We have stopped, and are waiting for everything to settle before
         * going again*/
        else if(mptr->dir == MOTOR_STOP)
        {
            // We are switching direction
            if(timestamp_to_ms(timestamp_raw() - mptr->off_timestamp) > DIR_CHANGE_DELAY))
            {
                mptr->dir = MOTOR_NEG;
                
                if(mptr->speed < 3)
                {
                    mptr->pwm = PWM_PWH; // start the high part
                }
            }
            
        }
    }
    
    
    // Handle commands to move positive direction
    else if(desired_move == MOVE_POS)
    {
        // We're already going in positive direction, so do nothing.
        if(mptr->dir == MOTOR_POS) {}

        /* We were going in negative direction, so we have to stop, 
         * wait, and then change direction. */
        else if(mptr->dir == MOTOR_NEG)
        {
            mptr->dir = MOTOR_STOP;
            mptr->off_timestamp = timestamp_raw();
        }
        
        /* We have stopped, and are waiting for everything to settle before
         * going again*/
        else if(mptr->dir == MOTOR_STOP && timestamp_to_ms(timestamp_raw() - mptr->off_timestamp) > DIR_CHANGE_DELAY)
        {
            mptr->dir = MOTOR_POS;
        }
    }
    
}

// for reference, Aaron's code
void update_motors_old(void)
{
    // Stop Motors
    if (motor.alt.dir == MOTOR_STOP)
    {
        AZ_CONTROL1_SetLow();
        AZ_CONTROL2_SetLow();
        motor.alt.dir = MOTOR_STOP;
    }
    if (motor.az.dir == MOTOR_STOP)
    {
        EL_CONTROL1_SetLow();
        EL_CONTROL2_SetLow();
        motor.az.dir = MOTOR_STOP;
    }
    
    // Altitude
    if (motor.alt.dir == MOTOR_POS) // Up
    {
        EL_CONTROL2_SetLow();
        __delay_ms(600);    // 600 ms is the motor turnoff time -- change this later to not delay whole program (use timer1)
        EL_CONTROL1_SetHigh();
    }
    else if (motor.alt.dir == MOTOR_NEG) // Down
    {
        EL_CONTROL1_SetLow();
        __delay_ms(600);    // 600 ms is the motor turnoff time -- change this later to not delay whole program (use timer1)
        EL_CONTROL2_SetHigh();
    }
    
    // Azimuth
    if (motor.az.dir == MOTOR_POS) // Clockwise
    {
        AZ_CONTROL2_SetLow();
        __delay_ms(600);    // 600 ms is the motor turnoff time -- change this later to not delay whole program (use timer1)
        AZ_CONTROL1_SetHigh();
    }
    else if (motor.az.dir == MOTOR_NEG) // Counter-Clockwise
    {
        AZ_CONTROL1_SetLow();
        __delay_ms(600);    // 600 ms is the motor turnoff time -- change this later to not delay whole program (use timer1)
        AZ_CONTROL2_SetHigh();
    }
    
    // these were used to decide if we needed to update the motor directions
//    prev_alt_dir = motor.alt.dir;
//    prev_az_dir = motor.az.dir;
}

