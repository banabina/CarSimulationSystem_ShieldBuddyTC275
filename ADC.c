/*
 * ADC.c
 *
 *  Created on: 2023. 3. 22.
 *      Author: user
 */
//test
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "IfxCcu6_reg.h"
#include "IfxVadc_reg.h"
#include "IfxGtm_reg.h"
#include <stdio.h>
#include "ADC.h"
#include "GPIO.h"



// SCU registers
#define LCK_BIT_LSB_IDX     1
#define ENDINIT_BIT_LSB_IDX 0
#define EXIS0_BIT_LSB_IDX   4
#define FEN0_BIT_LSB_IDX    8
#define EIEN0_BIT_LSB_IDX   11
#define INP_BIT_LSB_IDX     12
#define IGP0_BIT_LSB_IDX    14

// CCU60 registers
#define DISS_BIT_LSB_IDX    1
#define DISR_BIT_LSB_IDX    0


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
#define TREV_BIT_LSB_BIT 9


//Port register index
#define PC1_BIT_LSB_IDX     11
#define PC2_BIT_LSB_IDX     19
#define PC3_BIT_LSB_IDX     27
#define PC5_BIT_LSB_IDX     11
#define PC7_BIT_LSB_IDX     27
#define P1_BIT_LSB_IDX      1
#define P2_BIT_LSB_IDX      2
#define P3_BIT_LSB_IDX      3
#define P5_BIT_LSB_IDX      5
#define P7_BIT_LSB_IDX      7


//Port register index
#define PC1_BIT_LSB_IDX 11
#define PC2_BIT_LSB_IDX 19
#define P1_BIT_LSB_IDX 1
#define P2_BIT_LSB_IDX 2
#define LCK_BIT_LSB_IDX 1
#define ENDINIT_BIT_LSB_IDX 0
#define DISR_BIT_LSB_IDX 0
#define DISS_BIT_LSB_IDX 1
#define T12CLK_BIT_LSB_IDX 0
#define T12PRE_BIT_LSB_IDX 3
#define CTM_BIT_LSB_IDX 7
#define T12STR_BIT_LSB_IDX 6
#define ENT12PM_BIT_LSB_IDX 7
#define INPT12_BIT_LSB_IDX 10
#define SRPN_BIT_LSB_IDX 0
#define SRE_BIT_LSB_IDX 10
#define TOS_BIT_LSB_IDX 11
#define T12RS_BIT_LSB_IDX 1

// scu registers
#define LCK_BIT_LSB_IDX         1
#define ENDINT_BIT_LSB_IDX      0

void initVADC(void)
{
    // Password Access to unlock SCU_WDCSCON0
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) & ~(1 << LCK_BIT_LSB_IDX)) | (1 << ENDINT_BIT_LSB_IDX);
    while ((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) != 0);  // wait until unlocked

    // Modify Access to clear ENDINIT
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) | (1 << LCK_BIT_LSB_IDX)) & ~(1 << ENDINT_BIT_LSB_IDX);
    while ((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) == 0);  // wait until locked

    VADC_CLC.U &= ~(1 << DISR_BIT_LSB_IDX);    // enable CLC

    // Password Access to unlock SCU_WDCSCON0
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) & ~(1 << LCK_BIT_LSB_IDX)) | (1 << ENDINT_BIT_LSB_IDX);
    while ((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) != 0);  // wait until unlocked

    // Modify Access to clear ENDINIT
    SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) | (1 << LCK_BIT_LSB_IDX)) | (1 << ENDINT_BIT_LSB_IDX);
    while ((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) == 0);  // wait until locked

    // VADC configuration
    while ((VADC_CLC.U & (1 << DISS_BIT_LSB_IDX)) != 0); // wait until VADC module enabled

    VADC_G4_ARBPR.U |= (0x3 << PRIO0_BIT_LSB_IDX);      // highest priority for request source 0
    VADC_G4_ARBPR.U &= ~(0x1 << CSM0_BIT_LSB_IDX);      // wait-for-start mode
    VADC_G4_ARBPR.U |= (0x1 << ASEN0_BIT_LSB_IDX);      // arbitration source input 0 enable

    VADC_G4_QMR0.U &= ~(0x3 << ENGT_BIT_LSB_IDX);
    VADC_G4_QMR0.U |= (0x1 << ENGT_BIT_LSB_IDX);        // enable conversion request
    VADC_G4_QMR0.U |= (0x1 << FLUSH_BIT_LSB_IDX);       // clear ADC queue

    VADC_G4_ARBCFG.U |= (0x3 << ANONC_BIT_LSB_IDX);     // ADC normal operation

    VADC_G4_ICLASS0.U &= ~(0x7 << CMS_BIT_LSB_IDX);     // Class 0 Standard Conversion (12 bit)

    // VADC Group 4 Channel 7 configuration
    VADC_G4_CHCTR7.U |= (0x1 << RESPOS_BIT_LSB_IDX);    // result right-aligned
    VADC_G4_CHCTR7.U &= ~(0xF << RESREG_BIT_LSB_IDX);   // store result @ Result Register G4RES0
    VADC_G4_CHCTR7.U &= ~(0x3 << ICLSEL_BIT_LSB_IDX);   // Class 0

    VADC_G4_CHASS.U |= (0x1 << ASSCH7_BIT_LSB_IDX);
}

void VADC_startconversion(void)
{
    VADC_G4_QINR0.U |= 0x07;                            // no. of Request Channel = 7
    VADC_G4_QMR0.U |= (0x1 << TREV_BIT_LSB_IDX);        // Generate Conversion start Trigger Event
}

unsigned int VADC_readResult(void)
{
    unsigned int result;

    while ( (VADC_G4_RES0.U & (0x1 << VF_BIT_LSB_IDX)) == 0);       // wait until read available
    result = VADC_G4_RES0.U & (0xFFFF << RESULT_BIT_LSB_IDX);       // read ADC value

    return result;
}

/*
__interrupt(0x0B) __vector_table(0)
void lightCtrl(void)
{
    VADC_startConversion();
    unsigned int adcResult=VADC_readResult();

    if((adcResult>=2048)&&(getAccelState()==1)){

        P10_OUT.U ^= 0x1 << P1_BIT_LSB_IDX;
    }
    else if(getBrakeState()==1){
        P10_OUT.U |= (0x1 << P1_BIT_LSB_IDX);
    }
    else{
       P10_OUT.U &= ~(0x1 << P1_BIT_LSB_IDX);
    }
}
*/

/** SCU_WDTCPU0_CON0 == CPU WDT Control Register */
void initADC_test(void){

  //1. SCU_WDTCPU0_CON*****************
       // Password Access to unlock SCU_WDTSCON0
       SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) & ~(1 << LCK_BIT_LSB_IDX)) | (1 << ENDINIT_BIT_LSB_IDX);
       while((SCU_WDTCPU0_CON0.U & (1 <<LCK_BIT_LSB_IDX)) != 0);   // wait until unlocked

       // Modify Access to clear ENDINIT
       SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) | (1 << LCK_BIT_LSB_IDX)) & ~(1 << ENDINIT_BIT_LSB_IDX);
       while((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) == 0);  // wait until locked

  //2. VADC_CLC ***********************
       VADC_CLC.U &= ~(1 << DISR_BIT_LSB_IDX);    // enable VADC


  //3.
       // Password Access to unlock SCU_WDTSCON0
       SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) & ~(1 << LCK_BIT_LSB_IDX)) | (1 << ENDINIT_BIT_LSB_IDX);
       while((SCU_WDTCPU0_CON0.U & (1 <<LCK_BIT_LSB_IDX)) != 0);   // wait until unlocked

       // Modify Access to clear ENDINIT
       SCU_WDTCPU0_CON0.U = ((SCU_WDTCPU0_CON0.U ^ 0xFC) | (1 << LCK_BIT_LSB_IDX)) | (1 << ENDINIT_BIT_LSB_IDX);
       while((SCU_WDTCPU0_CON0.U & (1 << LCK_BIT_LSB_IDX)) == 0);  // wait until locked

       // VADC configurations
       while((VADC_CLC.U & (1 << DISS_BIT_LSB_IDX)) != 0);    // wait until VADC module

       VADC_G4_ARBPR.U |= 0x3 << PRIO0_BIT_LSB_IDX;        // highest priority for Request Source 0
       VADC_G4_ARBPR.U &= ~(0x1 << CSM0_BIT_LSB_IDX);      // Wait-for-Start Mode
       VADC_G4_ARBPR.U |= 0x1 << ASEN0_BIT_LSB_IDX;        // Arbitration Source Input 0 Enable

       VADC_G4_QMR0.U &= ~(0x3 << ENGT_BIT_LSB_IDX);
       VADC_G4_QMR0.U |= 0x1 << ENGT_BIT_LSB_IDX;          // enable conversion request
       VADC_G4_QMR0.U |= 0x1 << FLUSH_BIT_LSB_IDX;         // clear ADC queue
  //6.
       VADC_G4_ARBCFG.U |= 0x3 << ANONC_BIT_LSB_IDX;       // ADC normal operation
  //7.
       VADC_G4_ICLASS0.U &= ~(0x7 << CMS_BIT_LSB_IDX);     // Class 0 Standard Conversion (12-bit)

  //8.
       VADC_G4_CHCTR7.U |= 0x1 << RESPOS_BIT_LSB_IDX;      // result right-aligned
       VADC_G4_CHCTR7.U &= ~(0xF << RESREG_BIT_LSB_IDX);   // store result @ Result Register G4RES0
       VADC_G4_CHCTR7.U &= ~(0x3 << ICLSEL_BIT_LSB_IDX);   // Class 0

       VADC_G4_CHASS.U |= 0x1 << ASSCH7_BIT_LSB_IDX;

}

void initrgbLED_test(void)
{
    P02_IOCR4.U &= ~(0x1F<<PC7_BIT_LSB_IDX);    // reset P02_IOCR4 PC7
    P10_IOCR4.U &= ~(0x1F<<PC5_BIT_LSB_IDX);    // reset P10_IOCR4 PC5
    P10_IOCR0.U &= ~(0x1F<<PC3_BIT_LSB_IDX);    // reset P10_IOCR0 PC3

    P02_IOCR4.U |= (0x10<<PC7_BIT_LSB_IDX);     // set P02.7 push-pull general output
    P10_IOCR4.U |= (0x10<<PC5_BIT_LSB_IDX);     // set P10.5 push-pull general output
    P10_IOCR0.U |= (0x10<<PC3_BIT_LSB_IDX);     // set P10.3 push-pull general output
}


void initLED_test(void)
{
    P10_IOCR0.U &= ~(0x1F<<PC1_BIT_LSB_IDX);    // reset P10_IOCR0 PC1
    P10_IOCR0.U &= ~(0x1F<<PC2_BIT_LSB_IDX);    // reset P10_IOCR0 PC2

    P10_IOCR0.U |= (0x10<<PC1_BIT_LSB_IDX);     // set P10.1 push-pull general output
    P10_IOCR0.U |= (0x10<<PC2_BIT_LSB_IDX);     // set P10.2 push-pull general output
}


void initbutton_test(void)
{
    P02_IOCR0.U &= ~(0x1F<<PC1_BIT_LSB_IDX);    // reset P02_IOCR0 PC1
    P02_IOCR0.U |= (0x02<<PC1_BIT_LSB_IDX);     // set P02.1 push-pull general input
}

void initCCU60_test(void)
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
        CCU60_TCTR0.U |= 0x2 << T12CLK_BIT_LSB_IDX;     // f_CCU6 = 50MHz, prescaler = 1024
        CCU60_TCTR0.U |= 0x1 << T12PRE_BIT_LSB_IDX;     // f_T12 = 48,828 Hz

        CCU60_TCTR0.U &= ~(0x1 << CTM_BIT_LSB_IDX);     // T12 auto reset when period match(PM) occur

        CCU60_T12PR.U = 24414 -1;                       // PM interrupt freq. = f_T12 / (T12PR + 1)
        CCU60_TCTR4.U |= 0x1 << T12STR_BIT_LSB_IDX;     // load T12PR from shadow register

        CCU60_T12.U = 0;    // clear T12 counter register

        // CCU60 T12 PM interrupt setting
        CCU60_IEN.U |= 0x1 << ENT12PM_BIT_LSB_IDX;      // enable T12 PM interrupt
        CCU60_INP.U &= ~(0x3 << INPT12_BIT_LSB_IDX);    // service request output SR0 selected


        // SRC setting for CCU60
        SRC_CCU6_CCU60_SR0.U &= ~(0xFF << SRPN_BIT_LSB_IDX);
        SRC_CCU6_CCU60_SR0.U |= 0x0B << SRPN_BIT_LSB_IDX;       // set priority 0x0B
        SRC_CCU6_CCU60_SR0.U &= ~(0x3 << TOS_BIT_LSB_IDX);      // CPU0 service T12 PM interrupt
        SRC_CCU6_CCU60_SR0.U |= 0x1 << SRE_BIT_LSB_IDX;         // SR0 enabled

        CCU60_TCTR4.U = 0x1 << T12RS_BIT_LSB_IDX;       // T12 start counting


}




