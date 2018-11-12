#include "msp.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

volatile long *adc14_control_register_0 = (volatile long *) 0x40012000; // ADCTL0
volatile long *adc14_control_register_1 = (volatile long *) 0x40012004; // ADCTL1
volatile long *adc14_memory_control_register_0 = (volatile long *) 0x40012018; // ADCMEMCTL0
volatile long *adc14_memory_register_0 = (volatile long *) 0x40012098; // ADC14MEM0
volatile long *adc14_interrupt_flag_0_register = (volatile long *) 0x40012144; // ADC14IFGR0
volatile long *adc14_interrupt_enable_0_register = (volatile long *) 0x4001213C; // ADC14IER0
volatile long *adc14_interrupt_enable_1_register = (volatile long *) 0x4001213C; // ADC14IER1

#define ADC14CTL0     *adc14_control_register_0
#define ADC14CTL1     *adc14_control_register_1
#define ADC14MCTL0    *adc14_memory_control_register_0
#define ADC14MEM0     *adc14_memory_register_0
#define ADC14IFGR0    *adc14_interrupt_flag_0_register
#define ADC14IER0     *adc14_interrupt_enable_0_register
#define ADC14IER1     *adc14_interrupt_enable_1_register

long period = 3018000; //one second

volatile long *systick_control_reg = (volatile long *) 0xE000E010;  // STCSR
volatile long *systick_reload_value_reg = (volatile long *) 0xE000E014; // STRVR
volatile long *systick_current_value_reg = (volatile long *) 0xE000E018; // STCVR
volatile long *systick_calibration_reg = (volatile long *) 0xE000E01C;  // STCR

int adc[10];
int count1 = 1;
int i;
int count2 = 0;
float voltage[10], percent[10];
int Nx2[100][2];
uint32_t numAdc = 0;
uint32_t IRTable[10] = {15968, 14378, 13544, 12842, 11580, 11186, 10999, 10872, 10662, 10356};

/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    for (i = 0; i < 10; i++)
    {
        adc[i] = 0;
    }

    *systick_reload_value_reg = period;
    // Start timer.  Per table 2-54 of the MSP432P401R Technical reference manual.
    *systick_control_reg = 1;

    // Diagnostic printing...
    printf("Set up ADC for input from channel 6\n");

    // Set up the ADC.  These bits are all defined in the MSP432P4xx Technical Reference Manual...
    ADC14CTL0 &= 0xFFFFFFFD; // Note that in this bit mask, only bit 1 is a zero.
    ADC14CTL0 |= 0x00000010;    // ADC14 on
    ADC14CTL0 |= 0x04000000; // Source signal from the sampling timer       ****

    // You must consult the TRM and decide how to set up this register.  The other have been done for you.
    ADC14CTL1 = 0x00000030; // Students must set this one up:  ADC14MEM0, 14-bit, ref on, regular power

    ADC14MCTL0 = 0x0000008E; // 0 to 3.3V, channel A6?? (student must figure this out in Lab 0.4)
    ADC14IER0 = 0;             // no interrupts
    ADC14IER1 = 0;             // no interrupts
    ADC14CTL0 |= 0x00000002; // enable    But doesn't the core have to be on? (bit 4).  Original code.

    // P6.1 Set as analog input
    P6DIR &= ~BIT1;
    P6SEL0 |= BIT1;
    P6SEL1 |= BIT1;

    while(1)
    {
        ADC14CTL0 |= 0x0000003;

        while(ADC14CTL0 & BIT(16)){}
        numAdc = ADC14MEM0;
        float ActVal = 69.6 - ((6.88*pow(10,-3))*numAdc) + ((1.99*pow(10,-7))*pow(numAdc,2));
        printf("%f\n", ActVal);
    }
}


