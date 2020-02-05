#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "definitions.h"

// Modified for variable position sensor 2/19/19

//double timeInSeconds;
//    double DoorPosVolts;
//    int OpenedDoorVoltage, ClosedDoorVoltage;
//    int iDoorPosVolts;


    double DoorInclination;
    double OldDoorInclination;
    double OpenedDoorInclination;
    double ClosedDoorInclination;
    int maxDoorCount;



    
    extern int MPU6050SlaveAddress;
    extern int ESP6288ModuleAddress;
    
    
    extern double accelerometer_z, accelerometer_y, accelerometer_x; // variables for accelerometer raw data
    extern double gyro_x, gyro_y, gyro_z; // variables for gyro raw data
    extern double temperature; // variables for temperature data
       
    extern int i;

returnStruct setupDoors() {  // Calibrates Door Movement times 
    
returnStruct returnValue;

returnValue.level0 = 0;
returnValue.level1 = 0;


#ifdef USE_TX_ESP8266   // Position cursor for status message     
    TXout(processstat);
    TXout((char *) "Initializing");
#else
//    I2C_SetAddress ( ESP6288ModuleAddress );
    I2C_Out(ESP6288ModuleAddress,processstat);
    I2C_Out(ESP6288ModuleAddress,(char *) "Initializing");
#endif

#ifdef _DOOR_STOPS
    if(PORTAbits.RA4 == 0) {  // The doors are completely opened 
//        T0Counter = 0;         // Set timer counter to 0
        TXout(motorstat);
        TXout((char *) "Closing");
        T0Count = 0;
        T0CON0bits.EN = 1;   // Turn Timer on
        LATCbits.LATC0 = 1;
//        PORTCbits.RC0 = 1;      // Turn the Motor on to close the doors
        while(PORTAbits.RA5 !=0) {};    // Wait for door completely closed signal
        T0CON0bits.EN = 0;
//        PORTAbits.RA2 = 0;        // Interrupt Routine turns off motor
//        TXout(motorstat);
//        TXout((char *) "Stopped");
//        T0CON0bits.EN = 1;   // Turn Timer on       

//        itoa(buf,T1Counter,10);
//        TXout(buf);
//        buf[0] = 13;
//        buf[1] = 10;
//        buf[2] = 0;
//        TXout(buf);
     
    } else if (PORTAbits.RA5 == 0) {  // The doors are closed
//        T0Counter = 0;          // Set timer counter to 0
        TXout(motorstat);
        TXout((char *) "Opening");
        T0Count = 0;
        T0CON0bits.EN = 1;   // Turn Timer on
        LATAbits.LATA2 = 1; //PORTAbits.RA2 = 1;       // Turn Motor On to open the doors
        while(PORTAbits.RA4!=0) {};    // Wait for door completely open signal
        T0CON0bits.EN = 0;
        // Interrupt Routine turned motor off
        TXout(motorstat);
        TXout((char *) "Closing"); 
        
        // now begin with the doors completely closed
        PORTCbits.RC0 = 1;      // Turn the Motor on to close the doors
        while(PORTAbits.RA5 !=0) {};    // Wait for door completely closed signal
//        TXout(motorstat);
//        TXout((char *) "Stopped");        
        
    } else {        // The door is neither opened or closed
        TXout(motorstat);
        TXout((char *)"Opening");       
        LATAbits.LATA2 = 1; 
//        PORTAbits.RA2 = 1;      // Turn the motor on to open the doors    
        while(PORTAbits.RA4 != 0) {      // Completely open doors if 0 it is closed
            // Interrupt routine turned the motor off

        }
        T0Count = 0;
         TXout(motorstat);
        TXout((char *) "Closing");       
        T0CON0bits.EN = 1;   // Turn Timer on
        PORTCbits.RC0 = 1;      // Turn the Motor on to close the doors
        while(PORTAbits.RA5 !=0) {};    // Wait for door completely closed signal
        T0CON0bits.EN = 0;
//        PORTAbits.RA2 = 0;        // Interrupt Routine turns off motor
//        TXout(motorstat);
//        TXout((char *) "Stopped");         
        
    }
    
    DoorOpenTime = TMR0;       // Total Door Opening Time
    // Overflows were recorded in T0count
    // In this model the door opens 1/16th of the opening for each degree
    // difference between Tin and Tout
//            Adjust the doors
    // Calculate timer settings to control motors to move the doors for 1 degree
    //  T0Count is the number of overflows from timing the door close/open
    // DoorOpenTime is the excess time beyond the overflows
    // Both TMR0 and TMR1 operate at the same rate of speed.
    // First, I get the total clock ticks into a real number
    double TotalTimeForDoor = (double)(T0Count)*(pow(2.0,16.0)-1.0) + (double)(DoorOpenTime);
    // Next, divide it by the number of degrees between the lowest and highest operating
    // Temperatures, and round it.
    double OneDegreeForDoor = TotalTimeForDoor/16.0;
    // Close the door 1 degree
    if(OneDegreeForDoor > 65535.0) {
        T1Overflows = T1Count = OneDegreeForDoor/65535.0;
        // T1Count is the number of overflows for 1 degree of movement of the doors,
        // that one degree being 1/16th of the total door opening time, and as it is
        // an integer the division above leaves only the integer part.
        // The fractional part is what beyond that number of overflows
        // needs to be counted to establish the correct waiting period for
        // the motor to run during.  fmod leaves the fractional part.
        // fmod() does not work properly.
        DoorDegree = (double)(OneDegreeForDoor) - ((double)(T1Count) * (double)(65535.0));
        // the remainder is a fraction of 65535. Multiplying the faction
        // by 6553 gives the number of clock ticks needed to complete the
        // required time period.  Since timer1 counts upwards or to greater 
        // than instead of downwards or to less than, the number of clock ticks
        // needed to complete the period is 65535 minus that product of the faction
        // and 65535 (its complement).
        DoorDegree = round(DoorDegree);
        TMR1 = (unsigned long) (long)(65535) - ((long) (65535) - (DoorDegree ));

    } else {
        T1Overflows = T1Count = 0.0;
        TMR1 = (unsigned long) (long)(65535) - ((long) (65535) - (DoorDegree ));
    }
    // Move the Door
    // T1 will increment until it overflows
    // I can begin with the remainder or, if T1Count is 0, the DoorDegree value;
    // then I can end if T1Count = 0 or overflow T1Count times to reach the end
    // of the period for the motor to be turned on.
    // To load the fractional value into TMR1H I need to subtract it from the
    // overflow value (65535), then load that so that it counts that many clock
    // ticks before it interrupts to turn the motor off.
//                TMR1H = 65535 - DoorDegree;
    // For example, if the TMR1H counter counted to 10 and DoorDegree was three, then
    // I want to count only three ticks.  I subtract 10-3 and get 7 and I put that
    // in the register.  When I start the clock it will count to 8,9, and 10 and at
    // 11 it will overflow and send an interrupt.  Thus I will have counted to 3.
    DoorDegreeOpenMax = 16.0*T1Overflows*65535.0 + DoorDegree;
    
    timeInSeconds =  TotalTimeForDoor * 0.000001;
    
 #endif // _DOOR_STOPS
    // Variable Resistor Senses Door Position
    // RC0 closes doors
    // RA2 opens doors
    
//    // Reconfigure AtoD for Door Position
//    ADCON0bits.ADON = 0;        // Turn the AtoD off
//    ADCON2bits.MD = 0;          // C  // Basic or legacy mode, 2 = Average Mode
//    ADCON3bits.SOI = 0;         // Stop on Interrupt bit;1 Go is cleared by threshold
//    ADCON0bits.CONT = 0;        // C // Go cleared after each conversion; 
//                                //1 = GO triggered until TIF (threshold interrupt flag) set
//    ADCON0bits.ADON = 1;       // Turn On     
    
    //  Get a door position
    
#ifdef _POT_POSITION _SENSOR
    float x;
    int iOldDoorPos;
    int dDoorCount;
    DoorPosVolts = AtoD(0b000100);       // RA4, Pin 3 channel.
//    TXout(upDoorSw);       // Position cursor for status message labeled Closed Door Status
//    buffer = ftoa((float)(DoorPosVolts),&status);
//    decimalPoint = strchr(buffer,decPt);
//    decimalPoint++;
//    decimalPoint++;
//    decimalPoint++;
//    decimalPoint++;              
//    *decimalPoint++ = 'V';               
//    *decimalPoint = '\0';
//    INTCONbits.GIE=0;    
////                    TXout(DoorPosVolts);                
//    TXout(buffer);   
    __delay_ms(1000);
    DoorPosVolts = AtoD(0b000100);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       INTCONbits.GIE=1;               
    iDoorPosVolts = DoorPosVolts * 1000.0;
    iOldDoorPos = 3000;
 
    dDoorCount = 0;
//    TXout(upDoorSw);           // Position of door status message on terminal
//    TXout((char*) "Opening");
#ifdef OLDOPENDOORS
    while ((iOldDoorPos > iDoorPosVolts) || dDoorCount < 5) {  // if the old door position is greater than the current
        DoorMoved = 0; 
        if(iDoorPosVolts > 20000)
            T1Count = 25;       // Timer period - Don't know the length of time yet 
        else
            T1Count = 20;
        LATAbits.LATA2 = 1;          // The doors are opening (voltage gets lower)    
        T1CONbits.ON = 1;       // Turn on the timer
        while(!DoorMoved) {};   // Timer1 will turn off both doors when time is ended
        T1CONbits.ON = 0;    // Turn off the timer
        if((iOldDoorPos - iDoorPosVolts) > 1) 
                dDoorCount++;

        iOldDoorPos = iDoorPosVolts;
        DoorPosVolts = AtoD(0b000100); // Get new door position
        iDoorPosVolts = DoorPosVolts * 10000.0;
//        if(iDoorPosVolts > savDoorPos)
//            iOldDoorPos = savDoorPos;
        TXout(lowDoorSw);       // Position cursor for status message
        buffer = ftoa((float)(DoorPosVolts),&status);
        decimalPoint = strchr(buffer,decPt);
        decimalPoint++;
        decimalPoint++;
        decimalPoint++;
        decimalPoint++;              
        *decimalPoint++ = 'V'; 
         *decimalPoint = '\0';
        INTCONbits.GIE=0;    
//                    TXout(DoorPosVolts);                
        TXout(buffer);   
        INTCONbits.GIE=1;  
    }
    OpenedDoorVoltage = iDoorPosVolts/10;
#else
    OpenedDoorVoltage = 1900; // 1780;    // Open door at 1.8V fixed
    iOldDoorPos = iDoorPosVolts;    
     while (iOldDoorPos > OpenedDoorVoltage) {  // if the old door position is greater than the current
        DoorMoved = 0; 
        T1Count = 1;
        LATAbits.LATA2 = 1;          // The doors are opening (voltage gets lower)    
        T1CONbits.ON = 1;       // Turn on the timer
        while(!DoorMoved) {};   // Timer1 will turn off both doors when time is ended
        T1CONbits.ON = 0;    // Turn off the timer

        DoorPosVolts = AtoD(0b000100); // Get new door position
        iOldDoorPos = DoorPosVolts * 1000.0;
//        if(iDoorPosVolts > savDoorPos)
//            iOldDoorPos = savDoorPos;
        TXout(lowDoorSw);       // Position cursor for status message
        buffer = ftoa((float)(DoorPosVolts),&status);
        decimalPoint = strchr(buffer,decPt);
        decimalPoint++;
        decimalPoint++;
        decimalPoint++;
        decimalPoint++;              
        *decimalPoint++ = 'V'; 
         *decimalPoint = '\0';
        INTCONbits.GIE=0;    
//                    TXout(DoorPosVolts);                
        TXout(buffer);   
        INTCONbits.GIE=1;  
        __delay_ms(3000);        
    }
    OpenedDoorVoltage = iOldDoorPos;    // Open door at 1.8V fixed   
#endif           
//    LATAbits.LATA2 = 0;
//    TXout(upDoorSw);           // Position of door status message on terminal
//    TXout((char*) "Finished");
        

//    DoorPosVolts = AtoD(0b000100);       // RA4, Pin 3 channel.
//    iDoorPosVolts = DoorPosVolts * 1000.0;
    iOldDoorPos = -10000;   
    
//    TXout(lowDoorSw);       // Position cursor for status message
//    buffer = ftoa((float)(DoorPosVolts),&status);
//    decimalPoint = strchr(buffer,decPt);
//    decimalPoint++;
//    decimalPoint++;
//    decimalPoint++;
//    decimalPoint++;              
//    *decimalPoint++ = 'V';               
//    *decimalPoint = '\0';
//    INTCONbits.GIE=0;    
////                    TXout(DoorPosVolts);                
//    TXout(buffer);   
//    INTCONbits.GIE=1; 
//  
//#define CloseDoorSample  
#ifdef CloseDoorSample
    long int t = 10000;
    while(t--) {
 

    dDoorCount = 0;
    stv = 100.0;
//      OpenedDoorVoltage = 1780;    // Open door at 1.8V fixed   
    while((iOldDoorPos < iDoorPosVolts) && (fabs(stv) > 3.0) || (dDoorCount < 5)) {
//     while ((iOldDoorPos < iDoorPosVolts)  || (dDoorCount < 5)) {
        DoorMoved = 0;        
        T1Count = 2;       // Timer period - Don't know the length of time yet 
        LATCbits.LATC0 = 1;          // The doors are closing (voltage gets higher)    
        T1CONbits.ON = 1;       // Turn on the timer
        while(!DoorMoved) {};   // Timer1 will turn off both doors when time is ended
        T1CONbits.ON = 0;    // Turn off the timer
        if((iOldDoorPos - iDoorPosVolts) < 1)
            dDoorCount++;       
        iOldDoorPos = iDoorPosVolts;
        DoorPosVolts = AtoD(0b000100); // Get new door position
        iDoorPosVolts = DoorPosVolts * 1000.0;
        TXout(upDoorSw);       // Position cursor for status message labeled Open Door Status
        buffer = ftoa((float)(DoorPosVolts),&status);
        decimalPoint = strchr(buffer,decPt);
        decimalPoint++;
        decimalPoint++;
        decimalPoint++;
        decimalPoint++;              
        *decimalPoint++ = 'V'; 
         *decimalPoint = '\0';
        INTCONbits.GIE=0;                  
        TXout(buffer);   
        INTCONbits.GIE=1;     
    }     
      ClosedDoorVoltage = iDoorPosVolts;
    OpenedDoorVoltage = 1900; // 1780;

    iOldDoorPos = iDoorPosVolts;    

     while (iOldDoorPos > OpenedDoorVoltage) {  // if the old door position is greater than the current
        DoorMoved = 0; 
        T1Count = 2;
        LATAbits.LATA2 = 1;          // The doors are opening (voltage gets lower)    
        T1CONbits.ON = 1;       // Turn on the timer
        while(!DoorMoved) {};   // Timer1 will turn off both doors when time is ended
        T1CONbits.ON = 0;    // Turn off the timer
        DoorPosVolts = AtoD(0b000100); // Get new door position
        iOldDoorPos = DoorPosVolts * 1000.0;
//        if(iDoorPosVolts > savDoorPos)
//            iOldDoorPos = savDoorPos;
        TXout(lowDoorSw);       // Position cursor for status message
        buffer = ftoa((float)(DoorPosVolts),&status);
        decimalPoint = strchr(buffer,decPt);
        decimalPoint++;
        decimalPoint++;
        decimalPoint++;
        decimalPoint++;              
        *decimalPoint++ = 'V'; 
         *decimalPoint = '\0';
        INTCONbits.GIE=0;    
//                    TXout(DoorPosVolts);                
        TXout(buffer);   
        INTCONbits.GIE=1;  
    }     
     OpenedDoorVoltage = DoorPosVolts*1000.0;       
        // Data for Remote Computer
        TXout("# ");  // TIn, TOut, TMax, ClosedDoorVoltage, OpenedDoorVoltage, DoorPosVolts, sto, sti. Zalpha]
                        //   Double, double, double, int,         int                ,double     double, double, double  
        TXout(formatString(TIn,1,(char *) " "));       // double TIn
        TXout(formatString(TOut,1,(char *) " "));      // double TOut
        TXout(formatString(TMax,1,(char *) " "));      // double TMax
        TXout(formatString((double)(ClosedDoorVoltage),0,(char *) " "));
        TXout(formatString((double)(OpenedDoorVoltage),0,(char *) " "));
        TXout(formatString(DoorPosVolts,3," "));       // double DoorPosVolts
        TXout(formatString(sti,6,(char *) " "));       // double Student t inside temperature
        TXout(formatString(sto,6,(char *) " "));       // double Student t outside temperature
        TXout(formatString(Zalpha,6,(char *) " "));    // double critical value
        TXout("$ ");    
    }
    exit(0);
#else
    dDoorCount = 0;
    stv = 100.0;
    while((iOldDoorPos < iDoorPosVolts) && (fabs(stv) > 3.0) || (dDoorCount < 3)) {
//     while ((iOldDoorPos < iDoorPosVolts)  || (dDoorCount < 5)) {
        DoorMoved = 0;        
        T1Count = 1;       // Timer period - Don't know the length of time yet 
        LATCbits.LATC0 = 1;          // The doors are closing (voltage gets higher)    
        T1CONbits.ON = 1;       // Turn on the timer
        while(!DoorMoved) {};   // Timer1 will turn off both doors when time is ended
        T1CONbits.ON = 0;    // Turn off the timer
        if((iOldDoorPos - iDoorPosVolts) <= 10) {
            iDoorPosVolts = iOldDoorPos;
            dDoorCount++;  
        } else {
            dDoorCount = 0;
        }
        iOldDoorPos = iDoorPosVolts;     
        DoorPosVolts = AtoD(0b000100); // Get new door position
        iDoorPosVolts = DoorPosVolts * 1000.0;
        TXout(upDoorSw);       // Position cursor for status message labeled Open Door Status
        buffer = ftoa((float)(DoorPosVolts),&status);
        decimalPoint = strchr(buffer,decPt);
        decimalPoint++;
        decimalPoint++;
        decimalPoint++;
        decimalPoint++;              
        *decimalPoint++ = 'V'; 
         *decimalPoint = '\0';
        INTCONbits.GIE=0;                  
        TXout(buffer);   
        INTCONbits.GIE=1;
        __delay_ms(3000);
          
    } 
#endif
    ClosedDoorVoltage = iDoorPosVolts;
//    LATCbits.LATC0 = 0;
 
    
//    TXout(motorstat);       // Position cursor for status message
//    buffer = ftoa(x,&status);
//    TXout(buffer);
//    TXout((char *) "V");
        
//    // reconfigure AtoD for Temperature
//    ADCON0bits.ADON = 0;        // Turn the AtoD off
//    ADCON2bits.MD = 2;          // C  // Basic or legacy mode, 2 = Average Mode
//    ADCON3bits.SOI = 1;         // Stop on Interrupt bit;1 Go is cleared by threshold
//    ADCON0bits.CONT = 1;        // C // Go cleared after each conversion; 
//                                //1 = GO triggered until TIF (threshold interrupt flag) set
//    ADCON0bits.ADON = 1;        // Turn On
     
#endif
//     float x;
//    int iOldDoorPos;
 //   int dDoorCount;
    
//    I2C_SetAddress ( MPU6050SlaveAddress);        // Tilt Sensor Address.    
//    DoorPosVolts = AtoD(0b000100);       // RA4, Pin 3 channel.
//    Result = I2C_ReadMPU6050 (0x3B);
//    int a =  I2C_ReadMPU6050(ACCEL_X);
//    if(a <= -100)
//        return -1;
//    accelerometer_x = (double) a / 16384.0;
//    
//    a =  I2C_ReadMPU6050(ACCEL_Y);
//    if(a <= -100)
//        return -1;
//    accelerometer_y = (double) a / 16384.0; 
//    
//    a = I2C_ReadMPU6050(ACCEL_Z);
//    if(a <= -100)
//        return -1;
//    accelerometer_z = (double) a / 16384.0;
// //   for (i = 0; i < 10; i++) {
// //  accelerometer_x = (double) I2C_ReadMPU6050 (ACCEL_X)/16384.0; /// *9.80665;
// //  if(accelerometer)
////    Result = I2C_ReadMPU6050 (0x3D);
////   accelerometer_y = (double) I2C_ReadMPU6050 (ACCEL_Y)/16384.0; // *9.80665;  
////    Result = I2C_ReadMPU6050 (0x3F);
// //   accelerometer_z = (double) I2C_ReadMPU6050 (ACCEL_Z)/16384.0; // *9.80665; ***
////    Result = I2C_ReadMPU6050 (0x41);
//    dDoorCount = 0;  // Wrong use of variable because of name, but hasn't been used yet.
//    do {
//        a = I2C_ReadMPU6050(TEMPERATURE);
//        if(a <= -100)
//            return -1;
//        temperature = (double) a / 340.00  + 36.53;
//        dDoorCount++;
//    } while((temperature == 36.53) && dDoorCount < 10);
//    if(dDoorCount > 9)
//        return -1;
// //       temperature = 0.0;
// //   };  //    Result = I2C_ReadMPU6050 (0x43);
//    if(temperature > 20.0)
//        maxDoorCount = 6;
//    else
//        maxDoorCount = 12;
//    
////    gyro_x =  I2C_ReadMPU6050 (GYRO_X)/131.0;
//////    Result = I2C_ReadMPU6050 (0x45);
////    gyro_y =  I2C_ReadMPU6050 (GYRO_Y)/131.0; 
//////    Result = I2C_ReadMPU6050 (0x47);
////    gyro_z =  I2C_ReadMPU6050 (GYRO_Z)/131.0;    
////    }
////    __delay_ms(1000);
////    DoorPosVolts = AtoD(0b000100);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       INTCONbits.GIE=1;               
////    iDoorPosVolts = DoorPosVolts * 1000.0;
////    iOldDoorPos = 3000;
////    DoorInclination = accelerometer_x;
////    DoorInclination = accelerometer_y;
////    DoorInclination = accelerometer_z;
////    DoorInclination = gyro_x;
////    DoorInclination = gyro_y;
////    DoorInclination = gyro_z;   
//    // DOORS ARE CLOSED, accel_z is at < 0.0 and much less than +0.9
//    // the larger the door position value, the more the door is closed
//    
//    dDoorCount = 0;
//    a = I2C_ReadMPU6050(ACCEL_Z);
//    if(a <= -100)
//        return -1;
//    DoorInclination = (double) a / 16384.0;
//   for (i = 0; i < 10; i++)
//        DoorInclination += (double) I2C_ReadMPU6050 (ACCEL_Z)/16384.0 ;
//   DoorInclination /= 10.0;
    OldDoorInclination = -1.0;
//    OpenedDoorVoltage = 1900; // 1780;    // Open door at 1.8V fixed
//    iOldDoorPos = -100.0; 
 //    while ((OldDoorInclination < DoorInclination) || (dDoorCount < 3) || (DoorInclination < 0.1)) {  // if the old door position is greater than the current
    returnValue = openDoors(1);
    if (returnValue.level0 < 0) {
        TXout(TopCallErrorPosition); // Position
        TXout(openDoorsFailed); // 0 is success, -1 is failure 
        buffer = formatString(returnValue.level0, 0, ", ");       
        TXout(buffer);
        buffer = formatString(returnValue.level1, 0, ".");
        TXout(buffer);
        if(I2CBusCollision) 
            BusCollision(I2CBusCollision);        
        __delay_ms(1000);   
        return returnValue;
    }

//    while( (fabs(DoorInclination) < 0.800 )  && (dDoorCount < maxDoorCount ) ) {
//        DoorMoved = 0; 
//        T1Count = 2;
//        LATAbits.LATA2 = 1;          // The doors are opening (voltage gets lower)    
//        T1CONbits.ON = 1;       // Turn on the timer
//        while(!DoorMoved) {};   // Timer1 will turn off both doors when time is ended
//        T1CONbits.ON = 0;    // Turn off the timer
//        // make sure the door has stopped moving
////        double temporarily = fabs(gyro_x = I2C_ReadMPU6050(0x43)/131.0);
////        while(temporarily > 0.9000) { __delay_ms(1); }
//        // get the new inclination
//        OldDoorInclination = DoorInclination;
//        do {
//        a = I2C_ReadMPU6050(GYRO_Z);
//        if(a <= -100)
//            return -1;
//        gyro_x = (double) a/131.0;
//        } while (gyro_x > 1.000);
////        unsigned int toolong = 0;
////        while ((gyro_x = I2C_ReadMPU6050 (GYRO_Z)/131.0 > 1.000) && (toolong < 0xFE)) { __delay_ms(1); toolong++; }  // wait til it stops shaking        
//        DoorInclination = 0.0;
////       for (i = 0; i < 10; i++)
////            DoorInclination += (double) I2C_ReadMPU6050 (ACCEL_Z)/16384.0 ;
////       DoorInclination /= 10.0;
//        a =  I2C_ReadMPU6050(ACCEL_Z);
//        if(a <= -100)
//            return -1;
//        DoorInclination = (double) a / 16384.0;       
//       // Three percent of -0.1741 is -0.0052
//        // If the change is not significant
//        if((fabs(fabs(OldDoorInclination) - fabs(DoorInclination)) > fabs(DoorInclination * 0.0302)) && OldDoorInclination > DoorInclination) {
////            if(OldDoorInclination > DoorInclination) {
//            if(DoorInclination > 0.700)
//                dDoorCount++; 
//        }      
//
//        
////        DoorPosVolts = AtoD(0b000100); // Get new door position
////        iOldDoorPos = DoorPosVolts * 1000.0;
//#ifdef USE_TX_ESP8266   // Position cursor for status message            
//       TXout(lowDoorSw);
//#else
//        I2C_Out(lowDoorSw);       
//#endif
//        buffer = ftoa((float)(DoorInclination),&status);
//        decimalPoint = strchr(buffer,decPt);
//        decimalPoint++;
//        decimalPoint++;
//        decimalPoint++;
//        decimalPoint++;              
//        *decimalPoint++ = 'g';
//        *decimalPoint++ = ' '; 
//        *decimalPoint = '\0';
/////        INTCONbits.GIE=0;    
////                    I2C_Out(DoorPosVolts);
//#ifdef USE_TX_ESP8266              
//       TXout(buffer);
//#else   
//        I2C_Out(buffer);
//#endif
////        INTCONbits.GIE=1;  
////        __delay_ms(3000);        
//    }
//    OpenedDoorVoltage = iOldDoorPos;    // Open door at 1.8V fixed  
    OpenedDoorInclination = DoorInclination ;
    

//    double DoorInclination = (double) I2C_ReadMPU6050 (0x3F)/16384.0;
    OldDoorInclination = +10.0;

    returnValue = closeDoors(1);
    if (returnValue.level0 < 0) {
        TXout(TopCallErrorPosition); // Position
        TXout(closeDoorsFailed); // 0 is success, -1 is failure 
        buffer = formatString(returnValue.level0, 0, ", ");       
        TXout(buffer);
        buffer = formatString(returnValue.level1, 0, "."); 
        TXout(buffer); 
        if(I2CBusCollision) 
            BusCollision(I2CBusCollision);        
        __delay_ms(1000);   
        return returnValue;
    }

    
//    dDoorCount = 0;
//    while((dDoorCount < 10)) {
////     while ((iOldDoorPos < iDoorPosVolts)  || (dDoorCount < 5)) {
//        DoorMoved = 0;        
//        T1Count = 2;       // Timer period - Don't know the length of time yet 
//        LATCbits.LATC0 = 1;          // The doors are closing (voltage gets higher)    
//        T1CONbits.ON = 1;       // Turn on the timer
//        while(!DoorMoved) {};   // Timer1 will turn off both doors when time is ended
//        T1CONbits.ON = 0;    // Turn off the timer
//        do {
//            a = I2C_ReadMPU6050(GYRO_Z);
//            if(a <= -100)
//                return -1;
//            gyro_x = (double) a/131.0;
//        } while (gyro_x > 1.000);        
////        unsigned int toolong;
////        while ((gyro_x = I2C_ReadMPU6050 (GYRO_Z)/131.0 > 1.000) && (toolong < 0xFE)) { __delay_ms(1); toolong++; }  // wait til it stops shaking 
//        OldDoorInclination = DoorInclination;         
////        DoorInclination = 0.0;
//        a = I2C_ReadMPU6050(ACCEL_Z);
//        if(a == -100)
//            return -1;
//        DoorInclination = (double) a / 16384.0;         
////        DoorInclination += (double) I2C_ReadMPU6050 (ACCEL_Z)/16384.0;        
////        for (i = 0; i < 10; i++)
////            DoorInclination += (double) I2C_ReadMPU6050 (0x3F)/16384.0;
////        DoorInclination /= 10;
// //       if(fabs(OldDoorInclination - DoorInclination) > (DoorInclination * 0.0302) || OldDoorInclination < DoorInclination) {
//            if(OldDoorInclination < DoorInclination)            
//            dDoorCount++; 
//  //      }
//        
//        if(OldDoorInclination == DoorInclination)
//            dDoorCount++;
//        
//#ifdef USE_TX_ESP8266   // Position cursor for status message            
//       TXout(upDoorSw);
//#else       
//        I2C_Out(upDoorSw);      // Position cursor for status message labeled Open Door Status 
//#endif
//        buffer = ftoa((float)(DoorInclination),&status);  // stats gives number of ascii characters in
//        decimalPoint = strchr(buffer,decPt);
//        decimalPoint++;
//        decimalPoint++;
//        decimalPoint++;
//        decimalPoint++;              
//        *decimalPoint++ = 'g';
//        *decimalPoint++ = ' '; 
//         *decimalPoint = '\0';
////        INTCONbits.GIE=0; 
// #ifdef USE_TX_ESP8266   // Position cursor for status message            
//       TXout(buffer);
//#else       
//        I2C_Out(buffer);      // Position cursor for status message labeled Open Door Status 
//#endif          
// //       INTCONbits.GIE=1;
////        __delay_ms(3000);   
//    } 
    ClosedDoorInclination = DoorInclination;
    return returnValue;
}
    

