#include <xc.h>
#include <pic16f18426.h>
#include "definitions.h"


int I2C_Out(int I2C_Address, char *string)
{


        // Send Start Condition
        SSP2CON2bits.SEN = 1;       // Generates a start condition on the I2C bus
//        timer2();           // clears TMR2IF & starts timer
//        while(SSP2STATbits.BF && !T2Status) {};  // while data is being shifted out - stall. buffer full 
//        if(SSP2STATbits.BF)    // still full, it timed out
//            return -1;
        timer2();
        while(SSP2CON2bits.SEN && !T2Status && !I2CBusCollision) {};        // Start bit finished
         if(I2CBusCollision) {      // I2C reset, the SSP2CON bits are all cleared - idle state
           BusCollision(1);
           return 1;
        }       
         if(SSP2CON2bits.SEN)    // still full, it timed out
            return -2;
        timer2();       
        while(!PIR3bits.SSP2IF && !T2Status && !I2CBusCollision) {};     // interrupt from completion of start
        if(I2CBusCollision) {      // I2C reset, the SSP2CON bits are all cleared - idle state
           BusCollision(1);
           return 1;
        }
        if (T2Status)
            return -3;              // timed out
        PIR3bits.SSP2IF = 0;
        timer2();

           
        // Send I2C Slave Address with Write bit set
        SSP2BUF = (I2C_Address<<1);        // R/~W, R=1,W= 0
        while(SSP2STATbits.BF && !T2Status) {};
        if(SSP2STATbits.BF)
            return -4;
 //       while(SSP2STATbits.S) {};  // start bit was detected last, it should not have been
        timer2();            
        while(!PIR3bits.SSP2IF && !T2Status) {};
        if(!PIR3bits.SSP2IF)
            return -5;
        PIR3bits.SSP2IF = 0;
        timer2();
        while ((SSP2CON2bits.ACKSTAT) && (!T2Status)) {};
        if(SSP2CON2bits.ACKSTAT)    // ACK not received from slave
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
        // Put the string out (has register address, the data)
        if(I2CBusCollision) {      // I2C reset, the SSP2CON bits are all cleared - idle state
           BusCollision(2);
           return 1;
        };     
        while(*string != 0) {
            SSP2BUF = *string++;
            timer2();
            while(SSP2STATbits.BF && !T2Status) {};
            if(SSP2STATbits.BF)
                return -7;
 //           while(SSP2STATbits.S) {};  // start bit was detected last, it should not have been
            timer2();
            while(!PIR3bits.SSP2IF && !T2Status) {};
            if(!PIR3bits.SSP2IF)
                return -8;
            PIR3bits.SSP2IF = 0;
        }
        
         if(I2CBusCollision) {      // I2C reset, the SSP2CON bits are all cleared - idle state
           BusCollision(3);
           return 1;
        }   
        timer2();
        SSP2CON2bits.PEN = 1;     // Generate a stop condition on the I2C bus
        while(SSP2STATbits.BF && !T2Status) {};
        if(SSP2STATbits.BF)
            return -9;
        timer2();
        while(!PIR3bits.SSP2IF && !T2Status) {};
        if(!PIR3bits.SSP2IF)
            return -10;
        PIR3bits.SSP2IF = 0;
         if(I2CBusCollision) {      // I2C reset, the SSP2CON bits are all cleared - idle state
           BusCollision(4);
           return 1;
        } 
    return 0;
}


