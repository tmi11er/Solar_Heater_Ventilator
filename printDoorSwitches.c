#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "definitions.h"



void printDoorSwitches() 
{
    
 #ifdef _DOOR_STOPS   
   // Prints the door switch status
//    INTCONbits.GIE=0;;
//    TXout(upDoorSw);
//
//    if(PORTAbits.RA5==0)
//        TXout((char *) "Closed");            // Top/Upper Door Switch
//    else
//        TXout((char *) "Opened");
//    INTCONbits.GIE=1;;
////    __delay_ms(1000);
//   
//    INTCONbits.GIE=0;;
//    TXout(lowDoorSw);
//
//    if(PORTAbits.RA4==0)        // Bottom/Lower Door Switch
//        TXout((char *) "Closed");
//    else
//        TXout((char *) "Opened");
////     __delay_ms(1000);
//    INTCONbits.GIE=1;;
//    
    buffer = ftoa((float) (timeInSeconds),&status);
//    TXout(doorOFC);
//    TXout(buf);
//    ultoa(buf,DoorOpenTime,10);
 
    decimalPoint = strchr(buffer,decPt);
    decimalPoint++;
    decimalPoint++;
    *decimalPoint++ = ' ';
    *decimalPoint++ = 's';
    *decimalPoint++ = '\0';
    INTCONbits.GIE=0;;
    TXout(doorTime);    
    TXout(buffer);
    INTCONbits.GIE=1;;
    if((PORTAbits.RA2 == 0) && (PORTCbits.RC0 == 0)) {
        INTCONbits.GIE=0;;
        TXout(motorstat);
        TXout("Stopped");
        INTCONbits.GIE=1;;
    }
    
    buffer = ftoa((float)(DoorPercentOpen),&status);
    decimalPoint = strchr(buffer,decPt);
    decimalPoint++;
    decimalPoint++;
    decimalPoint++;
    *decimalPoint++ = '%';
    *decimalPoint++ = ' ';
    *decimalPoint++ = ' ';    
    *decimalPoint = '\0'; 
    INTCONbits.GIE=0;;
    TXout(doorPerOpen);
    TXout(buffer);
    INTCONbits.GIE=1;;
   #endif // _DOOR_STOPS  
    
    return;
}

