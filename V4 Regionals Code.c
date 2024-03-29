#pragma config(Sensor, S2,     frontIR,        sensorI2CCustom)
#pragma config(Sensor, S3,     backIR,         sensorI2CCustom)
#pragma config(Sensor, S4,     compass,        sensorEV3_GenericI2C)
#pragma config(Motor,  motorA,          FLmotor,       tmotorEV3_Large, openLoop, encoder)
#pragma config(Motor,  motorB,          BLmotor,       tmotorEV3_Large, openLoop, encoder)
#pragma config(Motor,  motorC,          BRmotor,       tmotorEV3_Large, openLoop, encoder)
#pragma config(Motor,  motorD,          FRmotor,       tmotorEV3_Large, openLoop, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//NOTE: Copy to Test A to identify the issue and see if RobotC is being stupid or not. Google the error

#include "hitechnic-irseeker-v2.h"
#include "PF_Motors.h"
#include "PF_Motors.c"

#define COMP_MULTI 0.7

/*
Run #include "RobotC_PF_Motors/PF_Motors.h" and #include "RobotC_PF_Motors/PF_Motors.c" to iniate power function motors
Initiate initPFMotors(); and startMotorTask(); to run motors
Use motorIn.structure.A,B,C,D to run motors
Set Motor Config to PID off and Large Motors
*/

int frontStr;
int backStr;

/* Values for the motors */

int motorSpeeds[18][4] = {
	{ -100, -100,  100,  100}, // Forward - Works
	{ -100,   20, -100,   20}, // Forward-Right - More Forward
	{ -100,  100,  100, -100}, // Right - Works
	{    0,  100,    0, -100}, // Backward-Right
	{  100,  100, -100, -100}, // Backward
	{  100,    0, -100,    0}, // Backward-Left
	{  100, -100, -100,  100}, // Left
	{   20, -100,   20,  100}, // Forward-Left - More Forward
	{    0,    0,    0,    0}, // Still
	{  100,  100, -100, -100}, // Forward
	{  100,    0, -100,    0}, // Forward-Right
	{  100, -100, -100,  100}, // Right
	{    0, -100,    0,  100}, // Backward-Right
	{ -100, -100,  100,  100}, // Backward
	{ -100,    0,  100,    0}, // Backward-Left
	{ -100,  100,  100, -100}, // Left
	{    0,  100,    0, -100}, // Forward-Left
	{    0,    0,    0,    0}, // Still
};

int motorVal[4] = {0, 0, 0, 0};

float max(float a, float b) {
	return a > b? a : b;
}

//variable -= (motorSpeed > 0)? correction : -correction;

float caluculateCorrection(int target, int current) { // This section works fine
	float correction = (target - current) * 2.0;

	if(correction > 180) {
		correction -= 360;
	}

	else if(correction < -180) {
		correction += 360;
	}

	if(correction > 100) {
		correction = 100;
	}

	if(correction < -100) {
		correction = -100;
	}
	correction *= COMP_MULTI;

	return correction;
}

int calculateBallDir(int frontStr, int backStr, int frontDir, int backDir) {
	int ballDir;
	if(frontStr >= backStr){
		switch(frontDir) {
			case 1:
				ballDir = 4;
				break;
			case 2:
				ballDir = 5;
				break;
			case 3:
				ballDir = 6;
				break;
			case 4:
				ballDir = 7;
				break;
			case 5:
				ballDir = 0;
				break;
			case 6:
				ballDir = 1;
				break;
			case 7:
				ballDir = 2;
				break;
			case 8:
				ballDir = 3;
				break;
			case 9:
				ballDir = 4;
				break;
			default:
				ballDir = 4;
		}
	} else {
		switch(backDir) { // NOTE: Find out how to get AROUND the ball, not directly at it
			case 1:
				ballDir = 16;
				break;
			case 2:
				ballDir = 17;
				break;
			case 3:
				ballDir = 9;
				break;
			case 4:
				ballDir = 10;
				break;
			case 5:
				ballDir = 11;
				break;
			case 6:
				ballDir = 17;
				break;
			case 7:
				ballDir = 9;
				break;
			case 8:
				ballDir = 10;
				break;
			case 9:
				ballDir = 11;
				break;
			default:
				ballDir = 9;
		}
	}
	/* HARD CODE
	if(frontStr >= backStr) {
			ballDir = SensorValue(frontIR);
			//print value of ir here
	} else if(frontStr < backStr) {
		ballDir = SensorValue(backIR);
		//Print value of Ir here
	}
	*/
	return ballDir;
}

void moving(int ballDir, float correction) {
	for(int i = 0; i < 4; i++) {
		motorVal[i] = motorSpeeds[ballDir][i];
		motorVal[i] -= correction;
		displayVariableValues(line1, motorVal[i]);
	}

	float highestVal = max(abs(motorVal[0]), abs(motorVal[1]));
	highestVal = max(highestVal, abs(motorVal[2]));
	highestVal = max(highestVal, abs(motorVal[3]));
	if(highestVal > 100) {
		if(highestVal != 0) {
			for(int i = 0; i <= 3; i++) {
				motorVal[i] /= highestVal;
				motorVal[i] *= 100;
			}
		}
	}
	motorIn.structure.A = motorVal[0];
	motorIn.structure.B = motorVal[1];
	motorIn.structure.C = motorVal[2];
	motorIn.structure.D = motorVal[3];
}

task main()
{
	tHTIRS2 irSeeker1;
	tHTIRS2 irSeeker2;
	initSensor(&irSeeker1, frontIR);
	initSensor(&irSeeker2, backIR);

	float correction;
	int ballDir;
	int target = SensorValue(compass);
	int current = target;
	for(int i = 0; i < 4; i++) {
		motorIn.array[i] = 0;
	}
	initPFMotors();
	startMotorTask();
	while(true) {
		readSensor(&irSeeker1);
		readSensor(&irSeeker2);
		current = SensorValue(compass);
		ballDir = calculateBallDir(irSeeker1.enhStrength, irSeeker2.enhStrength, irSeeker1.acDirection, irSeeker2.acDirection);
		correction = caluculateCorrection(target, current);
		moving(ballDir, correction);
		//motorIn.structure.A = 100;
		//motorIn.structure.B = 100;
		//motorIn.structure.C = 100;
		//motorIn.structure.D = 100;
		displayVariableValues(line5, correction);
		displayVariableValues(line6, ballDir);
	}
}
