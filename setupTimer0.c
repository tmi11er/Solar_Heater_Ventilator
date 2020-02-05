#include <stdlib.h>
#include <math.h>
#include "definitions.h"


void setupTimer0() {
    T0CON0bits.T016BIT = 1;     // 16 bit timer
    T0CON1bits.CS = 2;          // Fosc/4 = 32MHz/4 = 8MHz
    // Since the door probably takes about 10 seconds to fully open or close,
    // and the Timer has a 16 bit register to count with, and 16 bits holds about
    // 131071 counts,  if I round 131071 to 150000 then 10 into 150000 is 15,000 per
    // second, so I want a counter that works at about 15kHz or if I choose the other
    // way and round down to 100000, then I want a 10kHz clock.
    T0CON1bits.CKPS = 3;       // 1:8 Prescaler
    PIE0bits.TMR0IE = 1;
    TMR0 = 0;               // Counter registers
    INTCONbits.PEIE = 1;    // Peripheral Interrupt Enable
    INTCONbits.GIE = 1;     // General Interrupt enable
    T0CON0bits.EN = 1;
    T0Count = -1;
//    T0CON0bits.RD16 = 1;
}

#ifdef pic688 
void setupTimer1() {
  
T1CONbits.TMR1CS = 0;       // Clock source is FOSC/4 (2MHz)
T1CONbits.T1CKPS = 0;       // No Prescaler
TMR1H = 0;                  // Counter
TMR1L = 0;                  // Clear
PIE1bits.TMR1IE = 1;        // Peripheral Interrupt Enable Timer 1
INTCONbits.PEIE = 1;        // Peripheral interrupts are enabled
INTCONbits.GIE = 1;         // General interrupt enable

// Did not start the timer yet.
}
#endif
