#include <xc.h>
#include <pic16f18426.h>
#include "definitions.h"


int I2C_Out( char *string)
{

        SSP2CON2bits.SEN = 1;       // Generates a start condition on the I2C bus
        timer2();           // clears TMR2IF & starts timer
        while(SSP2STATbits.BF && !PIR4bits.TMR2IF) {};  // while data is being shifted out - stall. buffer full 
        if(SSP2STATbits.BF)    // still full, it timed out
            return -1;
        timer2();
        while(SSP2CON2bits.SEN && !PIR4bits.TMR2IF) {};        // Start bit finished
         if(SSP2CON2bits.SEN)    // still full, it timed out
            return -2;
        timer2();       
        while(!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {};     // interrupt from completion of start
        if (!PIR3bits.SSP2IF)
            return -3;              // timed out
        PIR3bits.SSP2IF = 0;
        timer2();
        SSP2BUF = (MPU6050SlaveAddress<<1);        // R/~W, R=1,W= 0
        while(SSP2STATbits.BF && !PIR4bits.TMR2IF) {};
        if(SSP2STATbits.BF)
            return -4;
 //       while(SSP2STATbits.S) {};  // start bit was detected last, it should not have been
        timer2();            
        while(!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {};
        if(!PIR3bits.SSP2IF)
            return -5;
        PIR3bits.SSP2IF = 0;
        timer2();
        while ((SSP2CON2bits.ACKSTAT) && (!PIR4bits.TMR2IF)) {};
        if(SSP2CON2bits.ACKSTAT)
            return -6;
//        if(SSP2CON2bits.ACKSTAT)     {  // Did Not get an ~ACK
//            SSP2CON2bits.PEN = 1;     // Generate a stop condition on the I2C bus
//            while(SSP2STATbits.BF) {};
//            while(!PIR3bits.SSP2IF) {};   
//            PIR3bits.SSP2IF = 0; 
//                __delay_ms(50);
//            status = step;
//            goto one;
//        }
        while(*string != 0) {
            SSP2BUF = *string++;
            timer2();
            while(SSP2STATbits.BF && !PIR4bits.TMR2IF) {};
            if(SSP2STATbits.BF)
                return -7;
 //           while(SSP2STATbits.S) {};  // start bit was detected last, it should not have been
            timer2();
            while(!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {};
            if(!PIR3bits.SSP2IF)
                return -8;
            PIR3bits.SSP2IF = 0;
        }
        timer2();
        SSP2CON2bits.PEN = 1;     // Generate a stop condition on the I2C bus
        while(SSP2STATbits.BF && !PIR4bits.TMR2IF) {};
        if(SSP2STATbits.BF)
            return -9;
        timer2();
        while(!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {};
        if(!PIR3bits.SSP2IF)
            return -10;
        PIR3bits.SSP2IF = 0;
    return;
}

