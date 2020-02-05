#include <stdlib.h>
#include <math.h>
#include <pic16f18426.h>
#include "definitions.h"

int T2Status;

void setupTimer2 () {
    T2CLKCON  = 0x1;        // Fosc/4 as clock source
    T2CONbits.CKPS = 0b111; // Max 1:128 prescaler.
    T2CONbits.OUTPS =0b1111;    // Max 1:16 prescaler.
    T2CON = 0x7F;       // Postscaler ignored in this mode
    T2HLTbits.T2MODE = 0b00100;     // One shot Mode
    T2RST = 0xf;        // External reset - reserved (none)
    T2PR = 0xff;     // Period register (to which T2TMR counts then interrupts out postscaler)
    PIE4bits.TMR2IE = 1;    // Enable timer2 interrupts
    PIR4bits.TMR2IF = 0;    // clear the interrupt            
}

void timer2() {
    T2CONbits.ON = 0;
    PIR4bits.TMR2IF = 0;
    T2Status = 0;
    T2CONbits.ON = 1;
}