/*
 * Buzzer.h
 *
 *  Created on: 2023. 3. 22.
 *      Author: user
 */

#ifndef BUZZER_H_
#define BUZZER_H_

void initBuzzer(void);
void activateBuzzer(int duty);
void deactivateBuzzer(void);
void initGTM(void);
void beepSound(int interval, int state);
void signatureSound(void);

void doSound(int interval);
void leSound(int interval);
void miSound(int interval);
void paSound(int interval);
void solSound(int interval);
void laSound(int interval);
void siSound(int interval);

#endif /* BUZZER_H_ */
