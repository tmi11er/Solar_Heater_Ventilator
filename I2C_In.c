#include <xc.h>
#include <pic16f18426.h>
#include "definitions.h"

returnStruct I2C_In(int SlaveAddress, int Register, int size) // ReadMPU6050(int Register) // long int I2C_ReadMPU6050(int Register) {
{
    returnStruct Result;
    int data;
    Result.level0 = 0;

    //    while(SSP2STATbits.I2C_STOP) {}; // wait for previous stop to complete
    //   do {
top:
    SSP2CON2bits.SEN = 1; // Generates a start condition on the I2C bus   
    timer2();
    while (SSP2CON2bits.SEN && !T2Status) {
    }; // Start bit finished // while start is still in progress wait.
    BusCollision(-10);
    if (SSP2CON2bits.SEN) { // still full, it timed out
        Result.level0 = -100;
        return Result;
    }
    timer2();
    while (!PIR3bits.SSP2IF && !T2Status) {
    }; // interrupt from completion of start
    BusCollision(-11);
    if (!PIR3bits.SSP2IF) {
        Result.level0 = -101; // timed out  
        return Result;
    }
    PIR3bits.SSP2IF = 0;

    SSP2BUF = (SlaveAddress << 1); // read not set because R is 1
    timer2();
    while (SSP2STATbits.BF && !T2Status) {
    }; // BF bit high while address shifts out
    BusCollision(-12);
    if (SSP2STATbits.BF) {
        Result.level0 = -102;
        return Result;
    }
    while (!PIR3bits.SSP2IF && !T2Status) {
    }; // The nineth clock bit has gone out and Ack is set or not
    BusCollision(-13);
    if (!PIR3bits.SSP2IF) {
        Result.level0 = -103;
        return Result;
    }
    PIR3bits.SSP2IF = 0;
    timer2();
    //    } while ((SSP2CON2bits.ACKSTAT) && (!PIR4bits.TMR2IF)); // while data is being shifted out - stall.
    while (SSP2CON2bits.ACKSTAT && (!T2Status)) {
    }; // Wait for the ACK
    BusCollision(-14);
    if (SSP2CON2bits.ACKSTAT) { // timed out, report error
        Result.level0 = -104;
        return Result;
    }
    //    do {
    SSP2BUF = Register;
    timer2(); // clears TMR2IF & starts timer
    while (SSP2STATbits.BF && !T2Status) {
    }; // while data is being shifted out - stall. buffer full 
    BusCollision(-15);
    if (SSP2STATbits.BF) { // still full, it timed out
        Result.level0 = -105;
        return Result;
    }
    timer2();
    while (!PIR3bits.SSP2IF && !T2Status) {
    }; // interrupt from completion of start
    BusCollision(-16);
    if (!PIR3bits.SSP2IF) {
        Result.level0 = -106; // timed out
        return Result;
    }
    PIR3bits.SSP2IF = 0;
    timer2();
    //    } while ((SSP2CON2bits.ACKSTAT) && (!PIR4bits.TMR2IF)); // while data is being shifted out - stall.
    while (SSP2CON2bits.ACKSTAT && (!T2Status)) {
    }; // Wait for the ACK  
    BusCollision(-17);
    if (SSP2CON2bits.ACKSTAT) {
        Result.level0 = -107;
        return Result;
    }
    //     while(SSP2STATbits.I2C_STOP) {}; // wait for previous stop to complete
    // Read 1st word/byte
    //    do {
    SSP2CON2bits.SEN = 1; // Generates a start condition on the I2C bus       
    timer2();
    while (SSP2CON2bits.SEN && !T2Status) {
    }; // Start bit finished // while start is still in progress wait.
    BusCollision(-18);
    if (SSP2CON2bits.SEN) { // still full, it timed out
        Result.level0 = -108;
        return Result;
    }
    timer2();
    while (!PIR3bits.SSP2IF && !T2Status) {
    }; // interrupt from completion of start
    BusCollision(-19);
    if (!PIR3bits.SSP2IF) {
        Result.level0 = -109; // timed out  
        return Result;
    }
    PIR3bits.SSP2IF = 0;
    SSP2BUF = (SlaveAddress << 1) | 1; // read bit set;
    timer2(); // clears TMR2IF & starts timer
    while (SSP2STATbits.BF && !T2Status) {
    }; // while data is being shifted out - stall. buffer full
    BusCollision(-20);
    if (SSP2STATbits.BF) { // still full, it timed out
        Result.level0 = -110;
        return Result;
    }
    timer2();
    while (!PIR3bits.SSP2IF && !T2Status) {
    }; // interrupt from completion of start
    BusCollision(-21);
    if (!PIR3bits.SSP2IF) {
        Result.level0 = -111; // timed out
        return Result;
    }
    PIR3bits.SSP2IF = 0;
    //    } while ((SSP2CON2bits.ACKSTAT) && (!PIR4bits.TMR2IF)); // while data is being shifted out - stall.
    while (SSP2CON2bits.ACKSTAT && (!T2Status)) {
    }; // Wait for the ACK
    BusCollision(-22);
    if (SSP2CON2bits.ACKSTAT) {
        Result.level0 = -112;
        return Result;
    }
    // read 16 bits
    SSP2CON2bits.RCEN = 1; // Read bit
    //    while(!SSP2STATbits.BF) {};  // while data is being shifted out - stall.
    BusCollision(-23);
    timer2();
    while (!PIR3bits.SSP2IF && !T2Status) {
    }; // interrupt from completion of start
    if (!PIR3bits.SSP2IF) {
        Result.level0 = -113; // timed out
        return Result;
    }
    PIR3bits.SSP2IF = 0;
    data = SSP2BUF; // Register to read from;
    BusCollision(-24);
    if (size == 2)
        data = data << 8;
    SSP2CON2bits.ACKDT = 0; // Ack data
    SSP2CON2bits.ACKEN = 1; // Acknowledge slave data received
    BusCollision(-25);
    timer2();
    while (!PIR3bits.SSP2IF && !T2Status) {
    }; // interrupt from completion of start
    if (!PIR3bits.SSP2IF) {
        Result.level0 = -114; // timed out
        return Result;
    }
    PIR3bits.SSP2IF = 0;

    if (size == 2) {
        SSP2CON2bits.RCEN = 1; // Read bit
        //    while(!SSP2STATbits.BF) {};  // while data is being shifted out - stall. 
        BusCollision(-26);
        timer2();
        while (!PIR3bits.SSP2IF && !T2Status) {
        }; // interrupt from completion of start
        if (!PIR3bits.SSP2IF) {
            Result.level0 = -115; // timed out
            return Result;
        }
        PIR3bits.SSP2IF = 0;
        data = data | SSP2BUF;
        BusCollision(-27);
        SSP2CON2bits.ACKDT = 1; // Ack data
        SSP2CON2bits.ACKEN = 1; // Acknowledge slave data received
        BusCollision(-28);
        timer2();
        while (!PIR3bits.SSP2IF && !T2Status) {
        }; // interrupt from completion of start
        if (!PIR3bits.SSP2IF) {
            Result.level0 = -116; // timed out
            return Result;
        }
        PIR3bits.SSP2IF = 0;

        SSP2CON2bits.PEN = 1; // Generate a stop condition on the I2C bus
        BusCollision(-29);
        timer2();
        while (!SSP2STATbits.I2C_STOP && !T2Status) {
        }; // while data is being shifted out - stall. 
        if (!SSP2STATbits.I2C_STOP) {
            Result.level0 = -117;
            return Result;
        }
        timer2();
        while (!PIR3bits.SSP2IF && !T2Status) {
        }; // interrupt from completion of start
        if (!PIR3bits.SSP2IF) {
            Result.level0 = -118; // timed out
            return Result;
        }
        PIR3bits.SSP2IF = 0;
    }
    BusCollision(-30);
    Result.level0 = 0;
    Result.level1 = data;
    return Result;
}
