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


returnStruct closeDoors(int Setup) {

    int dDoorCount;
    returnStruct Result; 
    
    Result.level0 = 0;
    Result.level1 = 0; 
    if(DoorDegree < 10.0)     // The door is already closed
        return Result;

//    Result =  I2C_ReadMPU6050(ACCEL_Z);
    Result = I2C_In(MPU6050SlaveAddress, ACCEL_Z,2);    
    if (Result.level0) {
        Result.level1 = Result.level0;  // Put the error from I2C in level1
        Result.level0 = -1;
        return Result;
     }  
    DoorInclination = (double) Result.level1 / 16384.0;    
    
//     i = I2C_ReadMPU6050(ACCEL_Z);
//    if(i <= -100)
//        return -1;
//    DoorInclination = (double) i / 16384.0;   
//    
    dDoorCount = 0;
    
    while((OldDoorInclination > DoorInclination ) || (dDoorCount < 2)) {
        DoorMoved = 0;  
 #ifdef USE_TX_ESP8266
        TXout(motorstat);
        TXout((char *) "Opening");
#else                      
        I2C_Out(ESP6288ModuleAddress,motorstat);
        I2C_Out(ESP6288ModuleAddress,(char *) "Opening");
#endif        
        T1Count = 2;       // Timer period - Don't know the length of time yet 
        LATCbits.LATC0 = 1;          // The doors are closing (voltage gets higher)    
        T1CONbits.ON = 1;       // Turn on the timer
        while(!DoorMoved) {};   // Timer1 will turn off both doors when time is ended
        T1CONbits.ON = 0;    // Turn off the timer
#ifdef USE_TX_ESP8266
        TXout(motorstat);
        TXout((char *) "Stopped");
#else                      
        I2C_Out(ESP6288ModuleAddress,motorstat);
        I2C_Out(ESP6288ModuleAddress,(char *) "Stopped");
#endif        
//        do {
//            i = I2C_ReadMPU6050(GYRO_Z);
//            if(i <= -100)
//                return -1;
//            gyro_x = (double) i/131.0;
//        } while (gyro_x > 1.000);   
        do {
//            Result = I2C_ReadMPU6050(GYRO_Z);
             __delay_ms(100);           
            Result = I2C_In(MPU6050SlaveAddress, GYRO_Z, 2);            
            if (Result.level0) {
                Result.level1 = Result.level0;  // Put the error from I2C in level1
                Result.level0 = -1;
                return Result;
            }  
            gyro_x = (double) Result.level1/131.0;
        } while (gyro_x > 1.7500);       
        OldDoorInclination = DoorInclination;         

//        Result =  I2C_ReadMPU6050(ACCEL_Z);
        Result = I2C_In(MPU6050SlaveAddress, ACCEL_Z, 2);        
        if (Result.level0) {
            Result.level1 = Result.level0;  // Put the error from I2C in level1
            Result.level0 = -1;
            return Result;
        }  
        DoorInclination = (double) Result.level1 / 16384.0;       
        if(OldDoorInclination < DoorInclination)            
            dDoorCount++; 

        
        if(OldDoorInclination == DoorInclination)
            dDoorCount++;
        
        
         if(Setup) {     // Calculating Opened Door G  Force
            buffer = formatString(DoorInclination, 3, (char *) "g ");            
#ifdef USE_TX_ESP8266   // Position cursor for status message            
            TXout(upDoorSw);
#else
        I2C_Out(ESP6288ModuleAddress,lowDoorSw);       
#endif
//            buffer = ftoa((float)(DoorInclination),&status);
//            decimalPoint = strchr(buffer,decPt);
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
        } else {        // Door Percent Open
            TXout(doorOFC); // Position cursor for status message
//            buffer = formatString(DoorInclination, 3, (char *) "g ");
            TXout(buffer);            
        }
        __delay_ms(100);      
    }       
#ifdef USE_TX_ESP8266 
        TXout(motorstat);
        TXout((char *) "Stopped");
#else                    //                   
        I2C_Out(ESP6288ModuleAddress,motorstat);
        I2C_Out(ESP6288ModuleAddress,(char *) "Stopped");
#endif  
      
    return Result;
}
