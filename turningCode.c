#pragma config(Sensor, S1,     HTMC,           sensorI2CCustomFastSkipStates)
#pragma config(Motor,  motorB,          motorLeft,     tmotorNXT, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorC,          motorRight,    tmotorNXT, PIDControl, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JoystickDriver.c"
#include "drivers/hitechnic-compass.h"

#define PI 4.0 atan(1.0)

#define PID_SENSOR_SCALE 1
#define PID_MOTOR_SCALE -1
#define PID_DRIVE_MAX 127
#define PID_DRIVE_MIN (-127)

#define PID_INTEGRAL_LIMIT 50

float pid_Kp = 2.0;
float pid_Ki = 0.04;
float pid_Kd = 0.0;

static int pidRunning = 1;
static float pidRequestedValue;

void Arcade(int x, int y){
	int powY;
	int powX;
	int powRightMotor;
	int powLeftMotor;

	powY = y * 100 / 127;
	powX = x * 100 / 127;

	if (powX < 0) // turning left
	{
		powLeftMotor = powY * (100 + (2 * powX)) / 100;
		powRightMotor = powY;
	}
	else // turning right
	{
		powRightMotor = (powY * (100 - (2 * powX)) / 100);
		powLeftMotor = powY;
	}

	motor[motorRight] = powRightMotor;
	motor[motorLeft] = powLeftMotor;
}

task pidController()
{
    float  pidSensorCurrentValue;

    float  pidError;
    float  pidLastError;
    float  pidIntegral;
    float  pidDerivative;
    float  pidDrive;

    pidLastError  = 0;
    pidIntegral   = 0;

    while( true )
        {

        if( pidRunning )
            {
            // Read the sensor value and scale
            pidSensorCurrentValue = HTMCreadRelativeHeading(HTMC) * PID_SENSOR_SCALE;

            // calculate error
            pidError = pidSensorCurrentValue - pidRequestedValue;

            // integral - if Ki is not 0
            if( pid_Ki != 0 )
                {
                // If we are inside controlable window then integrate the error
                if( abs(pidError) < PID_INTEGRAL_LIMIT )
                    pidIntegral = pidIntegral + pidError;
                else
                    pidIntegral = 0;
                }
            else
                pidIntegral = 0;

            // calculate the derivative
            pidDerivative = pidError - pidLastError;
            pidLastError  = pidError;

            // calculate drive
            pidDrive = (pid_Kp * pidError) + (pid_Ki * pidIntegral) + (pid_Kd * pidDerivative);

            // limit drive
            if( pidDrive > PID_DRIVE_MAX )
                pidDrive = PID_DRIVE_MAX;
            if( pidDrive < PID_DRIVE_MIN )
                pidDrive = PID_DRIVE_MIN;

            // send to motor
            motor[motorLeft] = pidDrive * PID_MOTOR_SCALE;
            motor[motorRight] = - pidDrive * PID_MOTOR_SCALE;
            }
        else
            {
            // clear all
            pidError      = 0;
            pidLastError  = 0;
            pidIntegral   = 0;
            pidDerivative = 0;
            motor[motorLeft] = 0;
            motor[motorRight] = 0;
            }

        // Run at 50Hz
        wait1Msec( 25 );
        }
}

task main () {
  //int _target = 0;
  pidRequestedValue = 0;

  startTask(pidController);

  while (true)
  {
  	getJoystickSettings(joystick);
		pidRequestedValue = atan(joystick.joy1_y1/joystick.joy1_x1) * 180/PI ;
		wait1Msec(50);
	}
  /*while(true) {

  	eraseDisplay();
    nxtDisplayTextLine(1, "Reading");
    nxtDisplayTextLine(2, "Target: %4d", _target);
    nxtDisplayTextLine(4, "Abs:   %4d", HTMCreadHeading(HTMC));
    nxtDisplayTextLine(5, "Rel:   %4d", HTMCreadRelativeHeading(HTMC));
    wait1Msec(100);

  }
  */
}
