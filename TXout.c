#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pic16f18426.h>
#include "definitions.h"

void TXout(char *string) {
    
    if (strlen(string) > 0) {
        rcack = 0;
        while (*string != 0) {

            NOP();
            NOP();
            NOP();

#ifdef pic699
            while (!PIR1bits.TXIF) {
            };
#else
            while (!PIR3bits.TX1IF) {
            };
#endif
            //            if (*string++ == -1) {
            //                TXREG = 0;
            //            } else {
            TXREG = *string++;
            //            }
            NOP();
            NOP();
            NOP();
        }
//        while (!rcack) {
//        };
    }
    __delay_ms(10);
    return;
}

