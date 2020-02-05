#include <stdlib.h>
#include <math.h>
#include <pic16f18426.h>
#include "definitions.h"
int rcack;
int I2CBusCollision = 0;

void __interrupt Interrupts ()
{
    static int T2Counter = 0;   // assigned only once during program execution
#ifdef pic688
    if(PIR1bits.TMR1IF) { // Timer 1 interrupt
        T1Counter++;
        PIR1bits.TMR1IF = 0;
        return;
    }
#else
    if(PIR0bits.TMR0IF) { // Timer 0 interrupt Counts UP
   //     T1Counter = (TMR0H << 8) | TMR0L;
        if(T0Count == -1)
           T0CON0bits.EN = 0;       // Done
        T0Count++;
        PIR0bits.TMR0IF = 0;
        return;
    }
    
    if(PIR4bits.TMR1IF) { // Timer 1 interrupt - controlling the motor Counts Down
        if(T1Count <= 0) {
            // Turn off both the motors
            LATAbits.LATA2 = 0;         // Turn the motor off; the doors are completely opened
                                        // at the lowest possible voltage
            LATCbits.LATC0 = 0;        // Turn the Motor off - The doors are completely closed, and 
                                        // the highest voltage is now stored in DoorClosedPos.  
            T1CONbits.ON = 0;       // Turn off the timer
            DoorMoved = 1;            
        } else {
            T1Count--;
        }
        PIR4bits.TMR1IF = 0;
        return;
    }
    if(PIR4bits.TMR2IF) {
        T2Counter++;
        if(T2Counter == 0x1f) {
            T2Status = 1;
            T2Counter = 0;
        }
        PIR4bits.TMR2IF = 0;
    }
    if(PIE3bits.RC1IE && PIR3bits.RC1IF) { // RS232 receiver interrupt
        if(RC1STAbits.FERR)
            ;
        *rx++ = RC1REG;
        if(RC1STAbits.OERR) {
            RCSTAbits.CREN = 0;           // Clear overflow error
            RCSTAbits.CREN = 1;         // Re-enable device
        }
        rcack = 1;
    }
    
    if(PIR0bits.IOCIF) {        // Interrupt on Change
        while(IOCAF4 | IOCAF5 | IOCAP4 | IOCAP5) {
            // OPENED DOOR - THE BACK/OUTSIDE SWITCH I4F
            if(IOCAF4) {    // Door is completely opened - falling edge switch closes
                LATAbits.LATA2 = 0; //PORTAbits.RA2 = 0;      // Turn the motor off that was opening the doors
 #ifdef USE_TX_ESP8266               
                if(!PORTCbits.RC0) {    // If the motor is off the doors are closed
                    TXout(motorstat);
                    TXout((char *) "Stopped");
                }
                TXout(lowDoorSw);
                TXout((char *) "I4F Max. Opened ");     // Top/Upper Door Switch  
#else
                if(!PORTCbits.RC0) {    // If the motor is off the doors are closed
                    I2C_Out(ESP6288ModuleAddress,motorstat);
                    I2C_Out(ESP6288ModuleAddress,(char *) "Stopped");
                }
                I2C_Out(ESP6288ModuleAddress,lowDoorSw);
                I2C_Out(ESP6288ModuleAddress,(char *) "I4F Max. Opened ");     // Top/Upper Door Switch 
#endif                
                IOCAF4 &= 0;
                DoorMoved = 1;   
            } else if (IOCAF5) {  // Door is completely closed
                LATCbits.LATC0 = 0;
    //            PORTCbits.RC0 = 0;      // Turn the motor off that was closing the doors
#ifdef USE_TX_ESP8266                
                if(!PORTAbits.RA2) {
                    TXout(motorstat);
                    TXout((char *) "Stopped");
                }
                TXout(upDoorSw);
                TXout((char *) "I5F Max. Closed ");     // Top/Upper Door Switch
#else
                if(!PORTAbits.RA2) {
                    I2C_Out(ESP6288ModuleAddress,motorstat);
                    I2C_Out(ESP6288ModuleAddress,(char *) "Stopped");
                }
                I2C_Out(ESP6288ModuleAddress,upDoorSw);
                I2C_Out(ESP6288ModuleAddress,(char *) "I5F Max. Closed ");     // Top/Upper Door Switch
#endif                
                IOCAF5 &= 0;
                DoorMoved = 1;                 
            } else if (IOCAP5) {
                LATCbits.LATC0 = 0;
    //            PORTCbits.RC0= 0;       // Turn the motor off that was closing the doors
    //            I2C_Out(ESP6288ModuleAddress,motorstat);
    //            I2C_Out(ESP6288ModuleAddress,(char *) "Stopped");
 #ifdef USE_TX_ESP8266                
                TXout(upDoorSw);
                TXout((char *) "I5P Not Sealed  ");
                if(!PORTAbits.RA2) {
                    TXout(motorstat);
                    TXout((char *) "Stopped");
                }
#else
                I2C_Out(ESP6288ModuleAddress,upDoorSw);
                I2C_Out(ESP6288ModuleAddress,(char *) "I5P Not Sealed  ");
                if(!PORTAbits.RA2) {
                    I2C_Out(ESP6288ModuleAddress,motorstat);
                    I2C_Out(ESP6288ModuleAddress,(char *) "Stopped");
                }
#endif                
                IOCAP5 &= 0;
               
            } else if (IOCAP4) {
                LATAbits.LATA2 = 0;       // Turn the motor off that was opening the doors
    //            I2C_Out(ESP6288ModuleAddress,motorstat);
    //            I2C_Out(ESP6288ModuleAddress,(char *) "Stopped");
   #ifdef USE_TX_ESP8266               
                TXout(lowDoorSw);
                TXout((char *) "I4P Not Flat    ");
                if(!PORTCbits.RC0) {
                    TXout(motorstat);
                    TXout((char *) "Stopped");
                } 
#else
                I2C_Out(ESP6288ModuleAddress,lowDoorSw);
                I2C_Out(ESP6288ModuleAddress,(char *) "I4P Not Flat    ");
                if(!PORTCbits.RC0) {
                    I2C_Out(ESP6288ModuleAddress,motorstat);
                    I2C_Out(ESP6288ModuleAddress,(char *) "Stopped");
                }                
#endif
                IOCAP4 &= 0; 
               
            }
        }
        PIR0bits.IOCIF = 0;     // Clear the interrupt flag       
//        when port is 0 the switch is closed, so it is 1 when the switch is opened;
//        when a port closes and the switch closes the door closes and the port goes
//        from 1 to zero, that is a falling edge.
    }
    if (PIR3bits.BCL2IF) {
        I2CBusCollision = 1;
        PIR3bits.BCL2IF = 0;        // Start condition aborted, module reset.
    }
//    if (PIR3bits.BCL1IF) {
//        PIR3bits.BCL1IF = 0;
//    }
#endif

}
