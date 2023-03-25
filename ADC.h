/*
 * ADC.h
 *
 *  Created on: 2023. 3. 22.
 *      Author: user
 */

#ifndef ADC_H_
#define ADC_H_

void initVADC (void);
void VADC_startconversion(void);
unsigned int VADC_readResult(void);

void initADC_test(void);    //
void initrgbLED_test(void);
void VADC_startConversion(void);
unsigned int VADC_readResult(void); //
void initCCU60_test(void);  //
void initbutton_test(void); //
void initLED_test(void);    //

void lightCtrl(void);

#endif /* ADC_H_ */
