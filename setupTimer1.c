#include <stdlib.h>
#include <math.h>
#include "definitions.h"

void setupTimer1() {
    T1CONbits.CKPS = 3;         // 1:8 Prescaler
    T1CONbits.RD16 = 1;         // Read TMRH & TMRL in a single operation
    TMR1CLK = 1;                // FOSC/4
    PIE4bits.TMR1IE = 1;        // Interrupt on overflow
    TMR1 = 0;                  // 16 bit write to Counter register
    INTCONbits.PEIE = 1;    // Peripheral Interrupt Enable
    INTCONbits.GIE = 1;     // General Interrupt enable
    T1CONbits.ON = 0;       // Turn the timer on
 

}
