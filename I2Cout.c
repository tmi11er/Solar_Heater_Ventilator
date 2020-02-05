#include <xc.h>
#include <pic16f18426.h>
#include "definitions.h"





//double accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
//double gyro_x, gyro_y, gyro_z; // variables for gyro raw data
//int temperature; // variables for temperature data
//char tmp_str[7]; // temporary variable used in convert function
//char* convert_int16_to_str(long int i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
//  sprintf(tmp_str, "%6d", i);
//  return tmp_str;
//}
//extern int step;


void I2C_Out(char *string)
{

       while(*string != 0)  {
        do {
            SSP1CON2bits.SEN = 1;       // Generates a start condition on the I2C bus
            while(SSP1CON2bits.SEN) {};   // while start is still in progress wait.      
            while(!PIR3bits.SSP1IF) {};
            PIR3bits.SSP1IF = 0;

            SSP1BUF = (ESP6288ModuleAddress << 1);  // read not set because R is 1
            while(SSP1STATbits.BF) {}; // BF bit high while address shifts out
            while(!PIR3bits.SSP1IF) {};     // The nineth clock bit has gone out and Ack is set or not
            PIR3bits.SSP1IF = 0;
        } while(SSP1CON2bits.ACKSTAT);  // while data is being shifted out - stall.  
    
        SSP1BUF = *string++;
        while(SSP1STATbits.BF) {};
//           while(SSP2STATbits.S) {};  // start bit was detected last, it should not have been           
        while(!PIR3bits.SSP1IF) {};
        PIR3bits.SSP1IF = 0;
        
        SSP1CON2bits.PEN = 1;     // Generate a stop condition on the I2C bus
        while(SSP1STATbits.BF) {};
        while(!PIR3bits.SSP1IF) {};   
        PIR3bits.SSP1IF = 0;
        __delay_ms(30);
        
    }
    return;
}






