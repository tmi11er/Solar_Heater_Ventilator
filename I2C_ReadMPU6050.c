#include <xc.h>
#include <pic16f18426.h>
#include "definitions.h"

returnStruct I2C_In(int SlaveAddress, int Register, int size)  // ReadMPU6050(int Register) // long int I2C_ReadMPU6050(int Register) {
{
    returnStruct Result;
    int data;
    Result.level0 = 0;

    //    while(SSP2STATbits.I2C_STOP) {}; // wait for previous stop to complete
    do {
        SSP2CON2bits.SEN = 1; // Generates a start condition on the I2C bus   
        timer2();
        while (SSP2CON2bits.SEN && !PIR4bits.TMR2IF) {
        }; // Start bit finished // while start is still in progress wait.  
        if (SSP2CON2bits.SEN) { // still full, it timed out
            Result.level0 - 100;
            return Result;
        }
        timer2();
        while (!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {
        }; // interrupt from completion of start
        if (!PIR3bits.SSP2IF) {
            Result.level0 - 101; // timed out  
            return Result;
        }
        PIR3bits.SSP2IF = 0;

        SSP2BUF = (SlaveAddress << 1); // read not set because R is 1
        timer2();
        while (SSP2STATbits.BF && !PIR4bits.TMR2IF) {
        }; // BF bit high while address shifts out
        if (SSP2STATbits.BF) {
            Result.level0 - 102;
            return Result;
        }
        while (!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {
        }; // The nineth clock bit has gone out and Ack is set or not
        if (!PIR3bits.SSP2IF) {
            Result.level0 - 103;
            return Result;
        }
        PIR3bits.SSP2IF = 0;
        timer2();
    } while ((SSP2CON2bits.ACKSTAT) && (!PIR4bits.TMR2IF)); // while data is being shifted out - stall.
    if (SSP2CON2bits.ACKSTAT) {
        Result.level0 - 104;
        return Result;
    }


    do {
        SSP2BUF = Register;
        timer2(); // clears TMR2IF & starts timer
        while (SSP2STATbits.BF && !PIR4bits.TMR2IF) {
        }; // while data is being shifted out - stall. buffer full 
        if (SSP2STATbits.BF) { // still full, it timed out
            Result.level0 - 105;
            return Result;
        }
        timer2();
        while (!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {
        }; // interrupt from completion of start
        if (!PIR3bits.SSP2IF) {
            Result.level0 - 106; // timed out
            return Result;
        }
        PIR3bits.SSP2IF = 0;
        timer2();
    } while ((SSP2CON2bits.ACKSTAT) && (!PIR4bits.TMR2IF)); // while data is being shifted out - stall.
    if (SSP2CON2bits.ACKSTAT) {
        Result.level0 - 107;
        return Result;
    }
    //     while(SSP2STATbits.I2C_STOP) {}; // wait for previous stop to complete

        // Read 1st word/byte
    do {
        SSP2CON2bits.SEN = 1; // Generates a start condition on the I2C bus       
        timer2();
        while (SSP2CON2bits.SEN && !PIR4bits.TMR2IF) {
        }; // Start bit finished // while start is still in progress wait.  
        if (SSP2CON2bits.SEN) { // still full, it timed out
            Result.level0 - 108;
            return Result;
        }
        timer2();
        while (!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {
        }; // interrupt from completion of start
        if (!PIR3bits.SSP2IF) {
            Result.level0 - 109; // timed out  
            return Result;
        }
        PIR3bits.SSP2IF = 0;

        SSP2BUF = (SlaveAddress << 1) | 1; // read bit set;
        timer2(); // clears TMR2IF & starts timer
        while (SSP2STATbits.BF && !PIR4bits.TMR2IF) {
        }; // while data is being shifted out - stall. buffer full 
        if (SSP2STATbits.BF) { // still full, it timed out
            Result.level0 - 110;
            return Result;
        }
        timer2();
        while (!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {
        }; // interrupt from completion of start
        if (!PIR3bits.SSP2IF) {
            Result.level0 - 111; // timed out
            return Result;
        }
        PIR3bits.SSP2IF = 0;

    } while ((SSP2CON2bits.ACKSTAT) && (!PIR4bits.TMR2IF)); // while data is being shifted out - stall.
    if (SSP2CON2bits.ACKSTAT) {
        Result.level0 - 112;
        return Result;
    }
    // read 16 bits
    SSP2CON2bits.RCEN = 1; // Read bit
    //    while(!SSP2STATbits.BF) {};  // while data is being shifted out - stall.
    timer2();
    while (!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {
    }; // interrupt from completion of start
    if (!PIR3bits.SSP2IF) {
        Result.level0 - 113; // timed out
        return Result;
    }
    PIR3bits.SSP2IF = 0;
    data = SSP2BUF; // Register to read from;
    if(size == 2)
        data = data << 8;

    SSP2CON2bits.ACKDT = 0; // Ack data
    SSP2CON2bits.ACKEN = 1; // Acknowledge slave data received
    timer2();
    while (!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {
    }; // interrupt from completion of start
    if (!PIR3bits.SSP2IF) {
        Result.level0 - 114; // timed out
        return Result;
    }
    PIR3bits.SSP2IF = 0;
    
    if(size == 2) {
        SSP2CON2bits.RCEN = 1; // Read bit
        //    while(!SSP2STATbits.BF) {};  // while data is being shifted out - stall. 
        timer2();
        while (!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {
        }; // interrupt from completion of start
        if (!PIR3bits.SSP2IF) {
            Result.level0 - 115; // timed out
            return Result;
        }
        PIR3bits.SSP2IF = 0;
        data = data| SSP2BUF;

        SSP2CON2bits.ACKDT = 1; // Ack data
        SSP2CON2bits.ACKEN = 1; // Acknowledge slave data received
        timer2();
        while (!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {
        }; // interrupt from completion of start
        if (!PIR3bits.SSP2IF) {
            Result.level0 - 116; // timed out
            return Result;
        }
        PIR3bits.SSP2IF = 0;

        SSP2CON2bits.PEN = 1; // Generate a stop condition on the I2C bus
        timer2();
        while (!SSP2STATbits.I2C_STOP && !PIR4bits.TMR2IF) {
        }; // while data is being shifted out - stall. 
        if (!SSP2STATbits.I2C_STOP) {
            Result.level0 - 117;
            return Result;
        }
        timer2();
        while (!PIR3bits.SSP2IF && !PIR4bits.TMR2IF) {
        }; // interrupt from completion of start
        if (!PIR3bits.SSP2IF) {
            Result.level0 - 118; // timed out
            return Result;
        }
        PIR3bits.SSP2IF = 0;
    }

    Result.level0 = 0;
    Result.level1 = data;
    return Result;

    //    SSP2CON2bits.ACKDT = 1; // Ack data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received
    //    while(SSP2STATbits.BF) {};  // while data is being shifted out - stall.    
    //    while(!PIR3bits.SSP2IF) {};
    //    PIR3bits.SSP2IF = 0;
    //    
    //    SSP2BUF = (slaveAddress <<1);    // write to slave
    //    while(SSP2STATbits.BF) {};  // while data is being shifted out - stall.     
    //    while(SSP2CON2bits.ACKSTAT) {};
    //    while(!PIR3bits.SSP2IF) {};
    //    PIR3bits.SSP2IF = 0;
    //    SSP2BUF = 0x3D;         // Register to read from;
    //    while(SSP2STATbits.BF) {};  // while data is being shifted out - stall. 
    //    while(SSP2CON2bits.ACKSTAT) {};    
    //    while(!PIR3bits.SSP2IF) {};
    //    PIR3bits.SSP2IF = 0;   
    //
    //    SSP2CON2bits.SEN = 1;       // Another start
    //    while(SSP2STATbits.BF) {};  // while data is being shifted out - stall. 
    //    while(SSP2CON2bits.ACKSTAT) {};       
    //    while(!PIR3bits.SSP2IF) {};
    //    PIR3bits.SSP2IF = 0;
    //    SSP2CON2bits.RCEN = 1;
    //    while (!PIR3bits.SSP2IF) {};    // wait
    //    PIR3bits.SSP2IF = 0;        // clear interrupt          
    //    unsigned int ayh = SSP2BUF;   
    //
    //    SSP2CON2bits.PEN = 1;       // Generate a stop condition on the I2C bus
    //    while(SSP2STATbits.BF) {};
    //    while(!PIR3bits.SSP2IF) {};   
    //    PIR3bits.SSP2IF = 0;          
    //       return;
    //    
    //     SSP2CON2bits.ACKDT = 1; // Ack data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received
    //    while (!PIR3bits.SSP2IF) {}
    //    PIR3bits.SSP2IF = 0;
    //     SSP2CON2bits.RCEN = 1;
    //    while (!PIR3bits.SSP2IF) {};    // wait
    //    PIR3bits.SSP2IF = 0;        // clear interrupt          
    //    unsigned int ayl = SSP2BUF; 
    //    
    //     SSP2CON2bits.ACKDT = 1; // Ack data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received
    //    while (!PIR3bits.SSP2IF) {}
    //    PIR3bits.SSP2IF = 0;
    //     SSP2CON2bits.RCEN = 1;
    //    while (!PIR3bits.SSP2IF) {};    // wait
    //    PIR3bits.SSP2IF = 0;        // clear interrupt          
    //    unsigned int azh = SSP2BUF;   
    //
    //     SSP2CON2bits.ACKDT = 1; // Ack data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received
    //    while (!PIR3bits.SSP2IF) {}
    //    PIR3bits.SSP2IF = 0;
    //     SSP2CON2bits.RCEN = 1;
    //    while (!PIR3bits.SSP2IF) {};    // wait
    //    PIR3bits.SSP2IF = 0;        // clear interrupt          
    //    unsigned int azl = SSP2BUF;   
    //   
    //     SSP2CON2bits.ACKDT = 1; // Ack data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received
    //    while (!PIR3bits.SSP2IF) {}
    //    PIR3bits.SSP2IF = 0;
    //     SSP2CON2bits.RCEN = 1;
    //    while (!PIR3bits.SSP2IF) {};    // wait
    //    PIR3bits.SSP2IF = 0;        // clear interrupt          
    //    unsigned int th = SSP2BUF;   
    //  
    //    SSP2CON2bits.ACKDT = 1; // Ack data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received
    //    while (!PIR3bits.SSP2IF) {}
    //    PIR3bits.SSP2IF = 0;
    //     SSP2CON2bits.RCEN = 1;
    //    while (!PIR3bits.SSP2IF) {};    // wait
    //    PIR3bits.SSP2IF = 0;        // clear interrupt          
    //    unsigned int tl = SSP2BUF;   
    //
    //    SSP2CON2bits.ACKDT = 1; // Ack data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received
    //    while (!PIR3bits.SSP2IF) {}
    //    PIR3bits.SSP2IF = 0;
    //     SSP2CON2bits.RCEN = 1;
    //    while (!PIR3bits.SSP2IF) {};    // wait
    //    PIR3bits.SSP2IF = 0;        // clear interrupt          
    //    unsigned int gyro_xh = SSP2BUF;   
    //
    //    SSP2CON2bits.ACKDT = 1; // Ack data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received
    //    while (!PIR3bits.SSP2IF) {}
    //    PIR3bits.SSP2IF = 0;
    //     SSP2CON2bits.RCEN = 1;
    //    while (!PIR3bits.SSP2IF) {};    // wait
    //    PIR3bits.SSP2IF = 0;        // clear interrupt          
    //    unsigned int gyro_xl = SSP2BUF;   
    //
    //    SSP2CON2bits.ACKDT = 1; // Ack data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received
    //    while (!PIR3bits.SSP2IF) {}
    //    PIR3bits.SSP2IF = 0;
    //     SSP2CON2bits.RCEN = 1;
    //    while (!PIR3bits.SSP2IF) {};    // wait
    //    PIR3bits.SSP2IF = 0;        // clear interrupt          
    //    unsigned int gyro_yh = SSP2BUF;   
    //
    //    SSP2CON2bits.ACKDT = 1; // Ack data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received
    //    while (!PIR3bits.SSP2IF) {}
    //    PIR3bits.SSP2IF = 0;
    //     SSP2CON2bits.RCEN = 1;
    //    while (!PIR3bits.SSP2IF) {};    // wait
    //    PIR3bits.SSP2IF = 0;        // clear interrupt          
    //    unsigned int gyro_yl = SSP2BUF;   
    //
    //    SSP2CON2bits.ACKDT = 1; // Ack data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received
    //    while (!PIR3bits.SSP2IF) {}
    //    PIR3bits.SSP2IF = 0;
    //     SSP2CON2bits.RCEN = 1;
    //    while (!PIR3bits.SSP2IF) {};    // wait
    //    PIR3bits.SSP2IF = 0;        // clear interrupt          
    //    unsigned int gyro_zh = SSP2BUF;   
    //    
    //    SSP2CON2bits.ACKDT = 1; // Ack data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received
    //    while (!PIR3bits.SSP2IF) {}
    //    PIR3bits.SSP2IF = 0;
    //     SSP2CON2bits.RCEN = 1;
    //    while (!PIR3bits.SSP2IF) {};    // wait
    //    PIR3bits.SSP2IF = 0;        // clear interrupt          
    //    unsigned int gyro_zl = SSP2BUF;   
    //    

    //    SSP2STATbits.BF = 0;    // Clear buffer full bit
    //    SSP2CON2bits.ACKDT = 0; // NAck data
    //    SSP2CON2bits.ACKEN = 1;     // Acknowledge slave data received   


    //    accelerometer_x = axh << 8 | axl;
    //    accelerometer_y = ayh << 8 | ayl;
    //    accelerometer_z = azh << 8 | azl;
    //
    //    temperature = th << 8 | tl;
    //    
    //    gyro_x = gyro_xh << 8 | gyro_xl;
    //    gyro_y = gyro_yh << 8 | gyro_yl;
    //    gyro_z = gyro_zh << 8 | gyro_zl;


    // Wire.beginTransmission(MPU6050SlaveAddress);
    //  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
    //  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
    //  Wire.requestFrom(MPU6050SlaveAddress, 7*2); // request a total of 7*2=14 registers
    //  accelerometer_x = Wire.available();
    //  Serial.print(accelerometer_x);
    //  Serial.println();
    //    temperature = temperature /131;    
    // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
    //  accelerometer_x = (int16_t) (Wire.read()<<8 | Wire.read()); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
    //  accelerometer_y = (int16_t) (Wire.read()<<8 | Wire.read()); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
    //  accelerometer_z = (int16_t)(Wire.read()<<8 | Wire.read()); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
    // temperature = (int16_t)Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
    //  gyro_x = (int16_t) (Wire.read()<<8 | Wire.read()); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
    //  gyro_y = (int16_t) (Wire.read()<<8 | Wire.read()); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
    // // signed int gz1 = (Wire.read() << 8 | Wire.read());
    //  gyro_z = (int16_t) (Wire.read()<<8 | Wire.read()); // /131.0; // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
    //
    //// print out data
    // Serial.print("accelerometer_y = ");Serial.println(accelerometer_y);
    //    accelerometer_x = accelerometer_x/16384.0*9.80665;
    //    accelerometer_y = accelerometer_y/16384.0*9.80665;
    //    accelerometer_z = accelerometer_z/16384.0*9.80665;
    //  Serial.print("aX = "); Serial.print(accelerometer_x/16384.0*9.80665);Serial.print("m/s^2");
    //  Serial.print(" | aY = "); Serial.print(accelerometer_y/16384.0*9.80665);Serial.print("m/s^2");
    //  Serial.print(" | aZ = "); Serial.print(accelerometer_z/16384.0*9.80665);Serial.print("m/s^2");
    //  // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]

    //  Serial.print(" | tmp = "); Serial.print(temperature/340.00+36.53);Serial.print("°C");
    //  Serial.print(" | gX = "); Serial.print(gyro_x/131); Serial.print("°/sec");
    //  Serial.print(" | gY = "); Serial.print(gyro_y/131); Serial.print("°/sec");
    //  Serial.print(" | gZ = "); Serial.print(gyro_z/131); Serial.print("°/sec");
    //  Serial.println();
    //
    //    
}