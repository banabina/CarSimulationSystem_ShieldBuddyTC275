/*
 * Buzzer.c
 *
 *  Created on: 2023. 3. 22.
 *      Author: user
 */

#include "IfxPort.h"
#include "IfxGtm_reg.h"
#include "Buzzer.h"

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

// GTM registers
#define FXCLK_SEL_BIT_LSB_IDX   0
#define EN_FXCLK_BIT_LSB_IDX    22
#define SEL7_BIT_LSB_IDX        14
#define SEL1_BIT_LSB_IDX        2

#define UPEN_CTRL3_BIT_LSB_IDX  22
#define ENDIS_CTRL3_BIT_LSB_IDX 6
#define OUTEN_CTRL3_BIT_LSB_IDX 6

// GTM - TOM0 registers
#define UPEN_CTRL1_BIT_LSB_IDX  18
#define HOST_TRIG_BIT_LSB_IDX   0
#define ENDIS_CTRL1_BIT_LSB_IDX 2
#define OUTEN_CTRL1_BIT_LSB_IDX 2
#define CLK_SRC_SR_BIT_LSB_IDX  12
#define OSM_BIT_LSB_IDX         26
#define TRIGOUT_BIT_LSB_IDX     24
#define SL_BIT_LSB_IDX          11

#define SEL3_BIT_LSB_IDX        6

// scu registers
#define LCK_BIT_LSB_IDX         1
#define ENDINT_BIT_LSB_IDX      0

// CCU60 registers
#define DISS_BIT_LSB_IDX        1
#define DISR_BIT_LSB_IDX        0
#define CTM_BIT_LSB_IDX         7
#define T12PRE_BIT_LSB_IDX      3
#define T12CLK_BIT_LSB_IDX      0
#define T12STR_BIT_LSB_IDX      6
#define T12RS_BIT_LSB_IDX       1
#define INPT12_BIT_LSB_IDX      10
#define ENT12PM_BIT_LSB_IDX     7


void initGTM(void)
{
    // Password Access to unlock SCU_WDCSCON0
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) & ~(1 << LCK_BIT_LSB_IDX)) | (1 << ENDINT_BIT_LSB_IDX);
    while ((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) != 0);  // wait until unlocked

    // Modify Access to clear ENDINIT
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) | (1 << LCK_BIT_LSB_IDX)) & ~(1 << ENDINT_BIT_LSB_IDX);
    while ((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) == 0);  // wait until locked

    GTM_CLC.U &= ~(1 << DISR_BIT_LSB_IDX);

    // Password Access to unlock SCU_WDCSCON0
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) & ~(1 << LCK_BIT_LSB_IDX)) | (1 << ENDINT_BIT_LSB_IDX);
    while ((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) != 0);  // wait until unlocked

    // Modify Access to clear ENDINIT
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) | (1 << LCK_BIT_LSB_IDX)) | (1 << ENDINT_BIT_LSB_IDX);
    while ((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) == 0);  // wait until locked

    while ((GTM_CLC.U & (1 << DISS_BIT_LSB_IDX)) != 0); // wait until GTM module enabled


    // GTM clock configuration
    GTM_CMU_FXCLK_CTRL.U &= ~(0xF << FXCLK_SEL_BIT_LSB_IDX);    // input clock of CUM FXCLK --> CMU_GCLK_EN
    GTM_CMU_CLK_EN.U |= 0x2 << EN_FXCLK_BIT_LSB_IDX;

    // GTM TOM0 PWM configuration
    GTM_TOM0_TGC0_GLB_CTRL.U |= 0x2 << UPEN_CTRL1_BIT_LSB_IDX;      //  TOM channel 1 update enable
    GTM_TOM0_TGC0_ENDIS_CTRL.U |= 0x2 << ENDIS_CTRL1_BIT_LSB_IDX;   // enable channel 1 on update trigger
    GTM_TOM0_TGC0_OUTEN_CTRL.U |= 0x2 << OUTEN_CTRL1_BIT_LSB_IDX;   // enable channel 1 output on trigger

    GTM_TOM0_CH1_CTRL.U |= (0x1 << SL_BIT_LSB_IDX);            // high signal level for duty cycle
    GTM_TOM0_CH1_CTRL.U |= (0x1 << CLK_SRC_SR_BIT_LSB_IDX);    // clock source --> MU FXCLK(1) = 6250kHz
    GTM_TOM0_CH1_CTRL.U &= ~(0x1 << OSM_BIT_LSB_IDX);          // continuous mode enable
    GTM_TOM0_CH1_CTRL.U &= ~(0x1 << TRIGOUT_BIT_LSB_IDX);       // TRIG[x] = TRIG[x-1]

    GTM_TOM0_CH1_SR0.U = 12500 - 1;                             // PWM freq. = 6250kHz / 12500 = 500 Hz
    GTM_TOM0_CH1_SR1.U = 1250 - 1;                              // duty cycle = 1250 / 12500 = 10 % (temporary)

    GTM_TOUTSEL6.U &= ~(0x3 << SEL7_BIT_LSB_IDX);               // TOUT103 --> TOM 0 channel 1
                                                                // 103 = 16 * 6 + 7

}


void initBuzzer(void) // Buzzer P2.3, TOUT3, TOM0_11 (channel 11)
{

    P02_IOCR0.U &= ~(0X1F << PC3_BIT_LSB_IDX);
    P02_IOCR0.U |= (0X11 << PC3_BIT_LSB_IDX);

    // GTM clock configuration
    GTM_CMU_FXCLK_CTRL.U &= ~(0xF << FXCLK_SEL_BIT_LSB_IDX);    // input clock of CUM FXCLK --> CMU_GCLK_EN
    GTM_CMU_CLK_EN.U |= 0x2 << EN_FXCLK_BIT_LSB_IDX;

    // GTM TOM0 PWM configuration
    GTM_TOM0_TGC1_GLB_CTRL.U |= 0x2 << UPEN_CTRL3_BIT_LSB_IDX;      //  TOM channel 1 update enable
    GTM_TOM0_TGC1_ENDIS_CTRL.U |= 0x2 << ENDIS_CTRL3_BIT_LSB_IDX;   // enable channel 1 on update trigger
    GTM_TOM0_TGC1_OUTEN_CTRL.U |= 0x2 << OUTEN_CTRL3_BIT_LSB_IDX;   // enable channel 1 output on trigger

    GTM_TOM0_CH11_CTRL.U |= (0x1 << SL_BIT_LSB_IDX);            // high signal level for duty cycle
    GTM_TOM0_CH11_CTRL.U |= (0x1 << CLK_SRC_SR_BIT_LSB_IDX);    // clock source --> MU FXCLK(1) = 6250kHz
    GTM_TOM0_CH11_CTRL.U &= ~(0x1 << OSM_BIT_LSB_IDX);          // continuous mode enable
    GTM_TOM0_CH11_CTRL.U &= ~(0x1 << TRIGOUT_BIT_LSB_IDX);       // TRIG[x] = TRIG[x-1]

    GTM_TOM0_CH11_SR0.U = 12500 - 1;                             // PWM freq. = 6250kHz / 12500 = 500 Hz
    GTM_TOM0_CH11_SR1.U = 0;                              // duty cycle = 1250 / 12500 = 10 % (temporary)

    GTM_TOUTSEL0.U &= ~(0x3 << SEL3_BIT_LSB_IDX);               // TOUT103 --> TOM 0 channel 1
                                                                // 103 = 16 * 6 + 7
}

void activateBuzzer(int duty)
{
    GTM_TOM0_CH11_SR1.U = duty;
}

void deactivateBuzzer(void)
{
    GTM_TOM0_CH11_SR1.U = 0;
}

void doSound(int interval)
{
    activateBuzzer(781);
    for(int i = 0; i < interval; ++i); // do
    deactivateBuzzer();
}

void leSound(int interval)
{
    activateBuzzer(694);
    for(int i = 0; i < interval; ++i); // le
    deactivateBuzzer();

}
void miSound(int interval)
{
    activateBuzzer(594);
    for(int i = 0; i < interval; ++i); // mi
    deactivateBuzzer();
}
void paSound(int interval)
{
    activateBuzzer(568);
    for(int i = 0; i < interval; ++i); // pa
    deactivateBuzzer();
}
void solSound(int interval)
{
    activateBuzzer(500);
    for(int i = 0; i < interval; ++i); // sol
    deactivateBuzzer();
}
void laSound(int interval)
{
    activateBuzzer(446);
    for(int i = 0; i < interval; ++i); // la
    deactivateBuzzer();
}
void siSound(int interval)
{
    activateBuzzer(403);
    for(int i = 0; i < interval; ++i); // si
    deactivateBuzzer();
}

void signatureSound(void)
{
    int interval = 10000000;

    doSound(interval);
    for(int i = 0; i < interval; ++i);
    miSound(interval);
    for(int i = 0; i < interval; ++i);
    solSound(interval*2);
    for(int i = 0; i < interval; ++i);
    miSound(interval);
    for(int i = 0; i < interval; ++i);
    doSound(interval);
    for(int i = 0; i < interval; ++i);
    /*
    miSound(interval);
    for(int i = 0; i < interval; ++i);
    leSound(interval);
    for(int i = 0; i < interval; ++i);
    doSound(interval);
    for(int i = 0; i < interval; ++i);
    leSound(interval);
    for(int i = 0; i < interval; ++i);
    miSound(interval);
    for(int i = 0; i < interval; ++i);
    miSound(interval);
    for(int i = 0; i < interval; ++i);
    miSound(interval);
    for(int i = 0; i < interval * 2; ++i);
    */
}

void beepSound(int interval, int state)
{
    activateBuzzer(2500-1);
    for(int i = 0; i < interval; ++i);
    deactivateBuzzer();
    if (state == 1)
        interval = 0;
    for(int i = 0; i < interval; ++i);
}
