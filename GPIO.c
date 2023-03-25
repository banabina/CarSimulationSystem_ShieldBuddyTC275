/*
 * GPIO.c
 *
 *  Created on: 2023. 3. 22.
 *      Author: user
 */




#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "GPIO.h"
#include <stdio.h>

// Port register index
// LED index
#define PC0_BIT_LSB_IDX     3
#define PC1_BIT_LSB_IDX     11
#define PC2_BIT_LSB_IDX     19
#define P0_BIT_LSB_IDX      0
#define P1_BIT_LSB_IDX      1
#define P2_BIT_LSB_IDX      2

// RGB LED index

#define PC7_BIT_LSB_IDX         27
#define PC5_BIT_LSB_IDX         11
#define PC3_BIT_LSB_IDX         27

#define P7_BIT_LSB_IDX          7
#define P5_BIT_LSB_IDX          5
#define P3_BIT_LSB_IDX          3

void initGPIO(void)
{
    // initLED

    P10_IOCR0.U &= ~(0x1F << PC1_BIT_LSB_IDX);  // reset P10_IOCR0 PC1
    //P10_IOCR0.U &= ~(0x1F << PC2_BIT_LSB_IDX);  // reset P10_IOCR0 PC2

    P10_IOCR0.U |= (0x10 << PC1_BIT_LSB_IDX);  // set p10.1 push-pull general output
    //P10_IOCR0.U |= (0x10 << PC2_BIT_LSB_IDX);  // set p10.2 push-pull general output

    // initButton

    P02_IOCR0.U &= ~(0x1F << PC0_BIT_LSB_IDX);  // reset P02_IOCR0 PC0 (SW D2)

    P02_IOCR0.U |= 0x02 << PC0_BIT_LSB_IDX;     // set P02.0 general input (pull-up connected)

    P02_IOCR0.U &= ~(0x1F << PC1_BIT_LSB_IDX);  // reset P02_IOCR0 PC1 (SW D3)

    P02_IOCR0.U |= 0x02 << PC1_BIT_LSB_IDX;     // set P02.1 general input (pull-up connected)

    // initRGBLED

    // reset Port IOCR register
    P02_IOCR4.U &= ~(0x1F << PC7_BIT_LSB_IDX);
    P10_IOCR4.U &= ~(0x1F << PC5_BIT_LSB_IDX);

    // set Port as general purpose output (push-pull)
    P02_IOCR4.U |= 0x10 << PC7_BIT_LSB_IDX;
    P10_IOCR4.U |= 0x10 << PC5_BIT_LSB_IDX;
}


void setLED2State(int state) // state == 0 : off, state == 1 : on, D12 Red led
{

    if ( state == 0 )   // LED off
    {
        P10_OUT.U &= ~(0x1 << P1_BIT_LSB_IDX);
    }
    else if ( state == 1 )   // LED on
    {
        P10_OUT.U |= (0x1 << P1_BIT_LSB_IDX);
    }
}
void setRGBLEDState(int state) // white = 0 , Red = 1, Blue = 2, Green = 3
{
    if ( state == 0 )   // White
    {
        P02_OUT.U |= 0x1 << P7_BIT_LSB_IDX;
        P10_OUT.U |= 0x1 << P5_BIT_LSB_IDX;
    }

    if ( state == 1 )    // Red
    {
        P02_OUT.U |= 0x1 << P7_BIT_LSB_IDX;
        P10_OUT.U &= ~(0x1 << P5_BIT_LSB_IDX);
    }

    if ( state == 2 )   // Blue
    {
        P02_OUT.U &= ~(0x1 << P7_BIT_LSB_IDX);
        P10_OUT.U &= ~(0x1 << P5_BIT_LSB_IDX);
    }

    if ( state == 3 ) // Green
    {
        P02_OUT.U &= ~(0x1 << P7_BIT_LSB_IDX);
        P10_OUT.U |= 0x1 << P5_BIT_LSB_IDX;
    }
}

int getAccelState(void) // SW D2 on == 1, off == 0
{
    if ( (P02_IN.U & (0x1 << P0_BIT_LSB_IDX)) == 0) // SW D2 on
    {
        int state = 1;
        return state;
    }
    else                                            // SW D2 off
    {
        int state = 0;
        return state;
    }
}

int getBrakeState(void) // SW D3 on == 1, off == 0
{
    if ( (P02_IN.U & (0x1 << P1_BIT_LSB_IDX)) == 0) // SW D3 on
    {
        int state = 1;
        return state;
    }
    else                                            // SW D3 off
    {
        int state = 0;
        return state;
    }
}
