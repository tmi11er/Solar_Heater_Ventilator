#include <xc.h>
#include <pic16f18426.h>
#include "definitions.h"

// MPU6050 few configuration register addresses
const unsigned int MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const unsigned int MPU6050_REGISTER_USER_CTRL    =  0x6A;
const unsigned int MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const unsigned int MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const unsigned int MPU6050_REGISTER_CONFIG       =  0x1A;
const unsigned int MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const unsigned int MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const unsigned int MPU6050_REGISTER_FIFO_EN      =  0x23;
const unsigned int MPU6050_REGISTER_INT_ENABLE   =  0x38;
const unsigned int MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const unsigned int MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;
const unsigned int MPU6050_REGISTER_I2C_MST_CTRL = 0x24;




returnStruct init_MPU6050() {
    
    
returnStruct returnValue;

int result;
//    I2C_SetAddress ( MPU6050SlaveAddress);        // Tilt Sensor Address. 68 1101000  
    char init1[3] = {MPU6050_REGISTER_PWR_MGMT_1, 0x80, 0x00};      // 6B 80 00 01101011 10000000 00000000  zero is end of string
    char *initializeString;
    initializeString = init1;
    do {
        result = I2C_Out(MPU6050SlaveAddress,initializeString);
        // if result == 1 then  bus collision
        if(result < 0) {    //   Reset Device
            returnValue.level0 = -10;
            returnValue.level1 = result;
            return returnValue;
        }
        if(I2CBusCollision) {      // I2C reset, the SSP2CON bits are all cleared - idle state
           BusCollision(1);
           returnValue.level0 = 1;
           returnValue.level1 = -1;
           return returnValue;
        }       
    } while(result != 0);       // Bus Collisions: result = 1
//        return ((short int)-10) | ((short int) result);      // failed.

    __delay_ms(100);
    init1[1] = 0x01;
    
    if((result = I2C_Out(MPU6050SlaveAddress,initializeString)) < 0) {    //   Reset Device
        returnValue.level0 = -11;
        returnValue.level1 = result;
        return returnValue;
    }    
//    if((result = I2C_Out_MPU6050(initializeString))  < 0)  // Get out of sleep mode
//         return -11;      // failed.       

//    result =I2C_ReadMPU6050 (MPU6050_REGISTER_PWR_MGMT_1);      // Get out of sleep mode
//    while(result & 0xC000) {
//        result =I2C_ReadMPU6050 (MPU6050_REGISTER_PWR_MGMT_1);   
//    }; // wait until out of reset.     
    init1 [0] = MPU6050_REGISTER_PWR_MGMT_2;
    init1 [1] == 0x00;
    __delay_ms(200);
 
    init1 [0] = MPU6050_REGISTER_INT_ENABLE;        // Interrupts from the MPU6050 
     if((result = I2C_Out(MPU6050SlaveAddress,initializeString)) < 0) {    //   Reset Device
        returnValue.level0 = -12;
        returnValue.level1 = result;
        return returnValue;
    }
    
    __delay_ms(200);    
    init1 [0] = MPU6050_REGISTER_FIFO_EN;
    if((result = I2C_Out(MPU6050SlaveAddress,initializeString)) < 0) {    //   Reset Device
        returnValue.level0 = -13;
        returnValue.level1 = result;
        return returnValue;
    }
//    init1 [0] = MPU6050_REGISTER_PWR_MGMT_1;  
//    I2C_Out(initializeString); 
    __delay_ms(200);    
    init1 [0] = MPU6050_REGISTER_I2C_MST_CTRL;
    if((result = I2C_Out(MPU6050SlaveAddress,initializeString)) < 0) {    //   Reset Device
        returnValue.level0 = -14;
        returnValue.level1 = result;
        return returnValue;
    }       

    init1[0] = MPU6050_REGISTER_USER_CTRL; 
    init1[1] = 0x0C;
    __delay_ms(200);
    
    init1 [0] = MPU6050_REGISTER_CONFIG; // Accel at 1kHz, Gyro at 8KHz
    init1 [1] = 0x01;
    if((result = I2C_Out(MPU6050SlaveAddress,initializeString)) < 0) {    //   Reset Device
        returnValue.level0 = -15;
        returnValue.level1 = result;
        return returnValue;
    }
    __delay_ms(200);    
    init1[1]  = 0x00;
    init1[0] = MPU6050_REGISTER_SMPLRT_DIV;
    if((result = I2C_Out(MPU6050SlaveAddress,initializeString)) < 0) {    //   Reset Device
        returnValue.level0 = -16;
        returnValue.level1 = result;
        return returnValue;
    }
     __delay_ms(200);   
    init1 [0] = MPU6050_REGISTER_GYRO_CONFIG;
    if((result = I2C_Out(MPU6050SlaveAddress,initializeString)) < 0) {    //   Reset Device
        returnValue.level0 = -17;
        returnValue.level1 = result;
        return returnValue;
    } 
    __delay_ms(200);    
    init1 [0] = MPU6050_REGISTER_ACCEL_CONFIG; // set +/- 2g full scale
    if((result = I2C_Out(MPU6050SlaveAddress,initializeString)) < 0) {    //   Reset Device
        returnValue.level0 = -18;
        returnValue.level1 = result;
        return returnValue;
    }
     __delay_ms(200);   
    init1[0] = MPU6050_REGISTER_SIGNAL_PATH_RESET;
    init1[1] = 0x00;
    if((result = I2C_Out(MPU6050SlaveAddress,initializeString)) < 0) {    //   Reset Device
        returnValue.level0 = -19;
        returnValue.level1 = result;
        return returnValue;
    }
    returnValue.level0 = 0;
    returnValue.level1 = 0;
    return returnValue;
}
