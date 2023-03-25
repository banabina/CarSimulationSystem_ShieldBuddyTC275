/*
 * Motor.h
 *
 *  Created on: 2023. 3. 22.
 *      Author: user
 */

#ifndef MOTOR_H_
#define MOTOR_H_

void initMotor(void);
void motorRun(int duty, int dir);
void motorBrake(void);

typedef enum _DIR_TYPE {
    FORWARD = 1,
    BACKWARD = 2
} DIR_TYPE;

#endif /* MOTOR_H_ */
