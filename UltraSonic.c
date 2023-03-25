/*
 * UltraSonic.c
 *
 *  Created on: 2023. 3. 22.
 *      Author: user
 */

#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include "IfxCcu6_reg.h"
#include "IfxVadc_reg.h"
#include "IfxGtm_reg.h"
#include <stdio.h>
#include "Ultrasonic.h"


//Port register index
#define PC1_BIT_LSB_IDX     11
#define PC2_BIT_LSB_IDX     19
#define PC3_BIT_LSB_IDX     27
#define PC4_BIT_LSB_IDX     3
#define PC5_BIT_LSB_IDX     11
#define PC6_BIT_LSB_IDX     19
#define PC7_BIT_LSB_IDX     27
#define P1_BIT_LSB_IDX      1
#define P2_BIT_LSB_IDX      2
#define P3_BIT_LSB_IDX      3
#define P4_BIT_LSB_IDX      4
#define P5_BIT_LSB_IDX      5
#define P6_BIT_LSB_IDX      6
#define P7_BIT_LSB_IDX      7

// SCU registers
#define LCK_BIT_LSB_IDX     1
#define ENDINIT_BIT_LSB_IDX 0
#define EXIS0_BIT_LSB_IDX   4
#define FEN0_BIT_LSB_IDX    8
#define EIEN0_BIT_LSB_IDX   11
#define INP_BIT_LSB_IDX     12
#define IGP0_BIT_LSB_IDX    14
#define REN0_BIT_LSB_IDX    9


// CCU60 registers
#define DISS_BIT_LSB_IDX    1
#define DISR_BIT_LSB_IDX    0
#define CTM_BIT_LSB_IDX     7
#define T12PRE_BIT_LSB_IDX  3
#define T12CLK_BIT_LSB_IDX  0
#define T12STR_BIT_LSB_IDX  6
#define T12RS_BIT_LSB_IDX   1
#define INPT12_BIT_LSB_IDX  10
#define ENT12PM_BIT_LSB_IDX 7
#define T12SSC_BIT_LSB_IDX  0


// SRC registers
#define SRPN_BIT_LSB_IDX    0
#define TOS_BIT_LSB_IDX     11
#define SRE_BIT_LSB_IDX     10
#define INP0_BIT_LSB_IDX    12

// VDAC registers
#define ANONC_BIT_LSB_IDX   0
#define ASEN0_BIT_LSB_IDX   24
#define CSM0_BIT_LSB_IDX    3
#define PRIO0_BIT_LSB_IDX   0
#define CMS_BIT_LSB_IDX     8
#define FLUSH_BIT_LSB_IDX   10
#define TREV_BIT_LSB_IDX    9
#define ENGT_BIT_LSB_IDX    0
#define RESPOS_BIT_LSB_IDX  21
#define RESREG_BIT_LSB_IDX  16
#define ICLSEL_BIT_LSB_IDX  0
#define VF_BIT_LSB_IDX      31
#define RESULT_BIT_LSB_IDX  0
#define ASSCH7_BIT_LSB_IDX  7
#define TREV_BIT_LSB_BIT    9

// GTM registers
#define SEL7_BIT_LSB_IDX            14
#define EN_FXCLK_BIT_LSB_IDX        22
#define FXCLK_SEL_BIT_LSB_IDX       0

// GTM - TOM0 registers
#define UPEN_CTRL1_BIT_LSB_IDX      18
#define HOST_TRIG_BIT_LSB_IDX       0
#define ENDIS_CTRL1_BIT_LSB_IDX     2
#define OUTEN_CTRL1_BIT_LSB_IDX     2
#define RSCNT0_CN1_BIT_LSB_IDX      18
#define FUPD_CTRL1_BIT_LSB_IDX      2
#define CLK_SRC_SR_BIT_LSB_IDX      12
#define OSM_BIT_LSB_IDX             26
#define TRIGOUT_BIT_LSB_IDX         24
#define SL_BIT_LSB_IDX              11

unsigned int g_range;
unsigned char g_range_valid_flag;

__interrupt(0x0A) __vector_table(0)
void ERU0_ISR(void)
{
    if ((P00_IN.U &(0x1 << P4_BIT_LSB_IDX)) != 0)   //rising edge of echo
    {
        CCU61_TCTR4.U = 0x1 << T12RS_BIT_LSB_IDX;   //start CCU61 T12 counter
    }                                                  // falling edge of echo
    else
    {
        CCU61_TCTR4.B.T12RR = 0x1;                  // stop CCU61 T12 counter

        g_range = ((CCU61_T12.B.T12CV * 1000000)/48828) /58;
        g_range_valid_flag = 1;

        CCU61_TCTR4.B.T12RES = 0x1; // reset CCU61_T12_counter

    }
}

__interrupt(0x0B) __vector_table(0)
void CCU60_T12_ISR(void)
{
    P02_OUT.U &= ~(0x1 << P5_BIT_LSB_IDX);
}



void initERU(void)
{
    SCU_EICR1.U &= ~(0x7 << EXIS0_BIT_LSB_IDX);
    SCU_EICR1.U |= 0x02 << EXIS0_BIT_LSB_IDX;

    SCU_EICR1.U |= 0x01 << FEN0_BIT_LSB_IDX; //falling edge

    SCU_EICR1.U |= 0x01 << REN0_BIT_LSB_IDX; //rising edge

    SCU_EICR1.U |= 0x01 << EIEN0_BIT_LSB_IDX;

    SCU_EICR1.U &= ~(0x7 << INP0_BIT_LSB_IDX);

    SCU_IGCR0.U &= ~(0x03 << IGP0_BIT_LSB_IDX);
    SCU_IGCR0.U |= 0x01 << IGP0_BIT_LSB_IDX;

    // SRC interrupt setting
    SRC_SCU_SCU_ERU0.U &= ~(0xFF << SRPN_BIT_LSB_IDX);
    SRC_SCU_SCU_ERU0.U |= (0x0A << SRPN_BIT_LSB_IDX);

    SRC_SCU_SCU_ERU0.U |= 0x01 << SRE_BIT_LSB_IDX;

    SRC_SCU_SCU_ERU0.U &= ~(0x03 << TOS_BIT_LSB_IDX);
}

void initCCU60(void)
{
    // Password Access to unlock SCU_WDTSCON0
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) & ~(1 << LCK_BIT_LSB_IDX)) | (1 << ENDINIT_BIT_LSB_IDX);
    while((SCU_WDTCPU0_CON0.U & (1 <<LCK_BIT_LSB_IDX)) != 0);   // wait until unlocked

    // Modify Access to clear ENDINIT
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) | (1 << LCK_BIT_LSB_IDX)) & ~(1 << ENDINIT_BIT_LSB_IDX);
    while((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) == 0);  // wait until locked

    CCU60_CLC.U &= ~(1 << DISR_BIT_LSB_IDX);    // enable CCY

    // Password Access to unlock SCU_WDTSCON0
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) & ~(1 << LCK_BIT_LSB_IDX)) | (1 << ENDINIT_BIT_LSB_IDX);
    while((SCU_WDTCPU0_CON0.U & (1 <<LCK_BIT_LSB_IDX)) != 0);   // wait until unlocked

    // Modify Access to clear ENDINIT
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) | (1 << LCK_BIT_LSB_IDX)) | (1 << ENDINIT_BIT_LSB_IDX);
    while((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) == 0);  // wait until locked

    // CCU60 T12 configurations
    while((CCU60_CLC.U & (1 << DISS_BIT_LSB_IDX)) != 0);    // wait until CCU60 module

    CCU60_TCTR0.U &= ~(0x7 << T12CLK_BIT_LSB_IDX);  // f_T12 = f_CCU6 / prescaler
    CCU60_TCTR0.U |= 0x2 << T12CLK_BIT_LSB_IDX;     // f_CCU6 = 50MHz, prescaler = 4
    // CCU60_TCTR0.U |= 0x1 << T12PRE_BIT_LSB_IDX;     // f_T12 = 48,828 Hz

    CCU60_TCTR0.U &= ~(0x1 << CTM_BIT_LSB_IDX);     // T12 auto reset when period match(PM) occur

    CCU60_T12PR.U = 125 -1;                       // PM interrupt freq. = f_T12 / (T12PR + 1)
    CCU60_TCTR4.U |= 0x1 << T12STR_BIT_LSB_IDX;     // load T12PR from shadow register

    CCU60_TCTR2.B.T12SSC = 0x1; // Single Shot Control


    CCU60_T12.U = 0;    // clear T12 counter register

    // CCU60 T12 PM interrupt setting
    CCU60_IEN.U |= 0x1 << ENT12PM_BIT_LSB_IDX;      // enable T12 PM interrupt
    CCU60_INP.U &= ~(0x3 << INPT12_BIT_LSB_IDX);    // service request output SR0 selected


    // SRC setting for CCU60
    SRC_CCU6_CCU60_SR0.U &= ~(0xFF << SRPN_BIT_LSB_IDX);
    SRC_CCU6_CCU60_SR0.U |= 0x0B << SRPN_BIT_LSB_IDX;       // set priority 0x0B
    SRC_CCU6_CCU60_SR0.U &= ~(0x3 << TOS_BIT_LSB_IDX);      // CPU0 service T12 PM interrupt
    SRC_CCU6_CCU60_SR0.U |= 0x1 << SRE_BIT_LSB_IDX;         // SR0 enabled

    // CCU60_TCTR4.U = 0x1 << T12RS_BIT_LSB_IDX;       // T12 start counting
}

void initCCU61(void)
{
    // Password Access to unlock SCU_WDTSCON0
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) & ~(1 << LCK_BIT_LSB_IDX)) | (1 << ENDINIT_BIT_LSB_IDX);
    while((SCU_WDTCPU0_CON0.U & (1 <<LCK_BIT_LSB_IDX)) != 0);   // wait until unlocked

    // Modify Access to clear ENDINIT
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) | (1 << LCK_BIT_LSB_IDX)) & ~(1 << ENDINIT_BIT_LSB_IDX);
    while((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) == 0);  // wait until locked

    CCU61_CLC.U &= ~(1 << DISR_BIT_LSB_IDX);    // enable CCY

    // Password Access to unlock SCU_WDTSCON0
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) & ~(1 << LCK_BIT_LSB_IDX)) | (1 << ENDINIT_BIT_LSB_IDX);
    while((SCU_WDTCPU0_CON0.U & (1 <<LCK_BIT_LSB_IDX)) != 0);   // wait until unlocked

    // Modify Access to clear ENDINIT
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) | (1 << LCK_BIT_LSB_IDX)) | (1 << ENDINIT_BIT_LSB_IDX);
    while((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) == 0);  // wait until locked

    // CCU60 T12 configurations
    while((CCU61_CLC.U & (1 << DISS_BIT_LSB_IDX)) != 0);    // wait until CCU60 module

    CCU61_TCTR0.U &= ~(0x7 << T12CLK_BIT_LSB_IDX);  // f_T12 = f_CCU6 / prescaler
    CCU61_TCTR0.U |= 0x2 << T12CLK_BIT_LSB_IDX;     // f_CCU6 = 50MHz, prescaler = 1024
    CCU61_TCTR0.U |= 0x1 << T12PRE_BIT_LSB_IDX;     // f_T12 = 48,828 Hz

    CCU61_T12PR.U = 100000 -1;                       // PM interrupt freq. = f_T12 / (T12PR + 1)
    CCU61_TCTR4.U |= 0x1 << T12STR_BIT_LSB_IDX;     // load T12PR from shadow register

    CCU61_T12.U = 0;    // clear T12 counter register

}



void initUSonic(void)
{
    P02_IOCR4.U &= ~(0x1F << PC5_BIT_LSB_IDX);
    P00_IOCR4.U &= ~(0x1F << PC4_BIT_LSB_IDX);

    P02_IOCR4.U |= 0x10 << PC5_BIT_LSB_IDX;
    P00_IOCR4.U |= 0x01 << PC4_BIT_LSB_IDX;

    P02_OUT.U &= ~(0x1 << P5_BIT_LSB_IDX);
}

void usonicTrigger(void)
{
   //start of 10us Trigger Pulse
    //GPIO_P02.6 -> HIGH
    P02_OUT.U |= 0x1 << P5_BIT_LSB_IDX;
    g_range_valid_flag = 0;

    CCU60_TCTR4.U = 0x1 << T12RS_BIT_LSB_IDX;       // T12 start counting
}

