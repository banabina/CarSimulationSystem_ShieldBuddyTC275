/*
 * UltraSonic.h
 *
 *  Created on: 2023. 3. 22.
 *      Author: user
 */

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

void initCCU60(void);
void initCCU61(void);
void initERU(void);
void initUSonic(void);
void usonicTrigger(void);
void ERU0_ISR(void);
void CCU60_T12_ISR(void);

extern unsigned int g_range;
extern unsigned char g_range_valid_flag;

#endif /* ULTRASONIC_H_ */
