#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "definitions.h"


extern double DoorInclination;
extern double OldDoorInclination;
extern double OpenedDoorInclination;
extern double ClosedDoorInclination;
extern int maxDoorCount;

extern double accelerometer_z, accelerometer_y, accelerometer_x; // variables for accelerometer raw data
extern double gyro_x, gyro_y, gyro_z; // variables for gyro raw data
extern double temperature; // variables for temperature data

returnStruct openDoors(int Setup) {
    //
    // Return Success or Failure
    //
    //   1nt i;      // used to get integer from accelerometer measure in MPU6050
    int dDoorCount;
    returnStruct Result;
    // Check the accelerometers on all axes and the temperature.  
    // Return error if the tilt sensor fails.

//    Result = I2C_ReadMPU6050(ACCEL_X);
    Result = I2C_In(MPU6050SlaveAddress, ACCEL_X,2);
    if (Result.level0) {
        Result.level1 = Result.level0;  // Put the error from I2C in level1
        Result.level0 = -1;
        return Result;
     }       
    accelerometer_x = (double) Result.level1 / 16384.0;

 //   Result = I2C_ReadMPU6050(ACCEL_Y);
    Result = I2C_In(MPU6050SlaveAddress, ACCEL_Y,2);    
    if (Result.level0) {
        Result.level1 = Result.level0;  // Put the error from I2C in level1
        Result.level0 = -2;
        return Result;
     }   
    accelerometer_y = (double) Result.level1 / 16384.0;

 //   Result = I2C_ReadMPU6050(ACCEL_Z);
     Result = I2C_In(MPU6050SlaveAddress, ACCEL_Z,2);   
    if (Result.level0) {
        Result.level1 = Result.level0;  // Put the error from I2C in level1
        Result.level0 = -3;
        return Result;
     }   
    accelerometer_z = (double) Result.level1 / 16384.0;

//    Result = I2C_ReadMPU6050(TEMPERATURE);
    Result = I2C_In(MPU6050SlaveAddress, TEMPERATURE,2);    
    if (Result.level0) {
        Result.level1 = Result.level0;  // Put the error from I2C in level1
        Result.level0 = -4;
        return Result;
     }   
    temperature = (double) Result.level1 / 340.00 + 36.53;

    // Use the temperature to determine how much the door moves so as to determine
    // how many times the software tries to shut the doors completely.


    if (temperature > 20.0)
        maxDoorCount = 6;
    else
        maxDoorCount = 12;

    dDoorCount = 0;
 //   Result = I2C_ReadMPU6050(ACCEL_Z);
    Result = I2C_In(MPU6050SlaveAddress, ACCEL_Z,2);    
    if (Result.level0) {
        Result.level1 = Result.level0;  // Put the error from I2C in level1
        Result.level0 = -5;
        return Result;
     }   
    DoorInclination = (double) Result.level1 / 16384.0;
    OldDoorInclination = -1.0;
    while ( (fabs(DoorInclination) < 0.800)  && (dDoorCount < maxDoorCount) ) {
        DoorMoved = 0;
 #ifdef USE_TX_ESP8266
        TXout(motorstat);
        TXout((char *) "Opening");
#else                      
        I2C_Out(ESP6288ModuleAddress,motorstat);
        I2C_Out(ESP6288ModuleAddress,(char *) "Opening");
#endif
        LATAbits.LATA2 = 1; // The doors are opening (voltage gets lower)    
        T1CONbits.ON = 1; // Turn on the timer
        while (!DoorMoved) {}; // Timer1 will turn off both doors when time is ended
        T1CONbits.ON = 0; // Turn off the timer
#ifdef USE_TX_ESP8266
        TXout(motorstat);
        TXout((char *) "Stopped");
#else                      
        I2C_Out(ESP6288ModuleAddress,motorstat);
        I2C_Out(ESP6288ModuleAddress,(char *) "Stopped");
#endif        
        OldDoorInclination = DoorInclination;
        do {
//            Result = I2C_ReadMPU6050(GYRO_Z);
            __delay_ms(100);
            Result = I2C_In(MPU6050SlaveAddress, GYRO_Z, 2);            
            if (Result.level0) {
                Result.level1 = Result.level0;  // Put the error from I2C in level1
                Result.level0 = -6;
                return Result;
             }   
            gyro_z = (double) Result.level1 / 131.0;
        } while (gyro_z > 1.75000);
        DoorInclination = 0.0;
//        Result = I2C_ReadMPU6050(ACCEL_Z);
        Result = I2C_In(MPU6050SlaveAddress, ACCEL_Z,2);       
        if (Result.level0) {
            Result.level1 = Result.level0;  // Put the error from I2C in level1
            Result.level0 = -7;
            return Result;
         }   
        DoorInclination = (double) Result.level1 / 16384.0;
        if ((fabs(fabs(OldDoorInclination) - fabs(DoorInclination)) > fabs(DoorInclination * 0.0302)) && OldDoorInclination > DoorInclination) {
            if (DoorInclination > 0.700)
                dDoorCount++;
        }
        if (Setup) { // Calculating Opened Door G  Force
             buffer = formatString(DoorInclination, 3, (char *) "g ");           
#ifdef USE_TX_ESP8266   // Position cursor for status message            
            TXout(lowDoorSw);
#else
            I2C_Out(ESP6288ModuleAddress,lowDoorSw);
#endif
//            buffer = ftoa((float) (DoorInclination), &status);
           
//            decimalPoint = strchr(buffer, decPt);
//            decimalPoint++;
//            decimalPoint++;
//            decimalPoint++;
//            decimalPoint++;
//            *decimalPoint++ = 'g';
//            *decimalPoint++ = ' ';
//            *decimalPoint = '\0';
#ifdef USE_TX_ESP8266              
            TXout(buffer);
//            __delay_ms(100);
#else   
            I2C_Out(ESP6288ModuleAddress,buffer);
#endif
        } else { // Door Percent Open
            TXout(doorOFC); // Position cursor for status message
//            buffer = formatString(DoorInclination, 3, (char *) "g ");
            TXout(buffer);
        }
        __delay_ms(100);
    } 
    Result.level0 = 0;
    Result.level1 = 0;
    return Result;
}
