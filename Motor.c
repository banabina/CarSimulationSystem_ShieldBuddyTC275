/*
 * Motor.c
 *
 *  Created on: 2023. 3. 22.
 *      Author: user
 */
#include "Motor.h"
#include "IfxPort.h"
#include "IfxGtm_reg.h"

// Port register index
#define PC1_BIT_LSB_IDX     11
#define PC2_BIT_LSB_IDX     19
#define PC3_BIT_LSB_IDX     27
#define PC5_BIT_LSB_IDX     11
#define PC6_BIT_LSB_IDX     19
#define PC7_BIT_LSB_IDX     27

#define P1_BIT_LSB_IDX      1
#define P2_BIT_LSB_IDX      2
#define P3_BIT_LSB_IDX      3
#define P5_BIT_LSB_IDX      5
#define P6_BIT_LSB_IDX      6
#define P7_BIT_LSB_IDX      7

// scu registers
#define LCK_BIT_LSB_IDX         1
#define ENDINT_BIT_LSB_IDX      0

// GTM registers
#define FXCLK_SEL_BIT_LSB_IDX   0
#define EN_FXCLK_BIT_LSB_IDX    22
#define SEL7_BIT_LSB_IDX        14
#define SEL1_BIT_LSB_IDX        2

#define UPEN_CTRL3_BIT_LSB_IDX  22
#define ENDIS_CTRL3_BIT_LSB_IDX 6
#define OUTEN_CTRL3_BIT_LSB_IDX 6

// CCU60 registers
#define DISS_BIT_LSB_IDX        1
#define DISR_BIT_LSB_IDX        0
#define CTM_BIT_LSB_IDX         7
#define T12PRE_BIT_LSB_IDX      3
#define T12CLK_BIT_LSB_IDX      0
#define T12STR_BIT_LSB_IDX      6
#define T12RS_BIT_LSB_IDX       1
#define INPT12_BIT_LSB_IDX      10
#define ENT12PM_BIT_LSB_IDX

void initMotor(void)  // TOUT105, TOM2_11, TGC 1, CTRL 3
{
    // Direction: P10.2
    // Speed(PWM): P10.3
    // Break: P02.6
    P10_IOCR0.U &= ~(0X1F << PC2_BIT_LSB_IDX);
    P10_IOCR0.U &= ~(0X1F << PC3_BIT_LSB_IDX);
    P02_IOCR4.U &= ~(0X1F << PC6_BIT_LSB_IDX);

    P10_IOCR0.U |= (0X10 << PC2_BIT_LSB_IDX);
    P10_IOCR0.U |= (0X11 << PC3_BIT_LSB_IDX);
    P02_IOCR4.U |= (0X10 << PC6_BIT_LSB_IDX);

    GTM_CMU_FXCLK_CTRL.U &= ~(0xF << FXCLK_SEL_BIT_LSB_IDX);
    GTM_CMU_CLK_EN.U |= 0x2 << EN_FXCLK_BIT_LSB_IDX;

    // GTM TOM0 PWM configuration

    GTM_TOM2_TGC1_GLB_CTRL.B.UPEN_CTRL3 |= 0x2;      //  TOM channel 1 update enable
    GTM_TOM2_TGC1_ENDIS_CTRL.B.ENDIS_CTRL3 |= 0x2;
    GTM_TOM2_TGC1_OUTEN_CTRL.B.OUTEN_CTRL3 |= 0x2;

    GTM_TOM2_CH11_CTRL.B.SL = 0x1;
    GTM_TOM2_CH11_CTRL.B.CLK_SRC_SR = 0x1;

    GTM_TOM2_CH11_SR0.U = 12500 - 1;
    GTM_TOM2_CH11_SR1.U = 0;

    GTM_TOUTSEL6.U |= (0x1 << 18);
}

void motorBrake(void)
{
    /*
    P10_OUT.U |= (0x1 << P3_BIT_LSB_IDX);
    P02_OUT.U &= ~(0x1 << P6_BIT_LSB_IDX);
    GTM_TOM0_CH3_SR1.U = 1200;
    */
    GTM_TOM2_CH11_SR1.U = 0;
}
void motorRun(int duty, int DIR_TYPE)
{
    // Direction: P10.2
    // Speed(PWM): P10.3
    // Break: P02.6
    if (DIR_TYPE == FORWARD) // drive mode D
    {
        P10_OUT.U |= 0x1 << P2_BIT_LSB_IDX;
    }
    else if (DIR_TYPE == BACKWARD) // drive mode R
    {
        P10_OUT.U &= ~(0x1 << P2_BIT_LSB_IDX);
    }
    P10_OUT.U |= (0x1 << P3_BIT_LSB_IDX);
    P02_OUT.U &= ~(0x1 << P6_BIT_LSB_IDX);
    GTM_TOM2_CH11_SR1.U = duty;
    /*
    if (DIR_TYPE == FORWARD) // drive mode D
    {
        P10_OUT.U |= 0x1 << P2_BIT_LSB_IDX;
    }
    else if (DIR_TYPE == BACKWARD) // drive mode R
    {
        P10_OUT.U &= ~(0x1 << P2_BIT_LSB_IDX);
    }
    P10_OUT.U |= (0x1 << P3_BIT_LSB_IDX);
    P02_OUT.U &= ~(0x1 << P6_BIT_LSB_IDX);
    GTM_TOM0_CH3_SR1.U = duty;
    */
}
