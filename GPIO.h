/*
 * GPIO.h
 *
 *  Created on: 2023. 3. 22.
 *      Author: user
 */

#ifndef GPIO_H_
#define GPIO_H_

void initGPIO(void);
void setLED2State(int state);
void setRGBLEDState(int state);
int getAccelState(void); // switch 1
int getBrakeState(void); // switch 2


#endif /* GPIO_H_ */
