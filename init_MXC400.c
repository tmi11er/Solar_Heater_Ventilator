#include <xc.h>
#include <pic16f18426.h>
#include "definitions.h"

const unsigned int MXC400_INT_SRC0 = 0x00;
const unsigned int MXC400_INT_CLR0 = 0x00;
const unsigned int MXC400_INT_SRC1 = 0x01;
const unsigned int MXC400_INT_CLR1 = 0x01;
const unsigned int MXC400_STATUS = 0x02;
const unsigned int MXC400_XOUT_UPPER = 0x03;
const unsigned int MXC400_XOUT_LOWER = 0x04;
const unsigned int MXC400_YOUT_UPPER = 0x05;
const unsigned int MXC400_YOUT_LOWER = 0x06;
const unsigned int MXC400_ZOUT_UPPER = 0x07;
const unsigned int MXC400_ZOUT_LOWER = 0x08;
const unsigned int MXC400_TOUT = 0x09;
const unsigned int MXC400_INTMASK0 = 0x0A;
const unsigned int MXC400_INTMASK1 = 0x0B;
const unsigned int MXC400_DETECTION = 0x0C;
const unsigned int MXC400_CONTROL = 0x0D;
const unsigned int MXC400_DEVICEID = 0x0E;
const unsigned int MXC400_WHOAMI = 0x0F;


returnStruct init_MXC400() {
    
int result;   
returnStruct returnValue;
//                  Register, value, EOS
   char init1[3] = {MXC400_CONTROL, 0x00, 0x00};      // 6B 80 00 01101011 10000000 00000000
    char *initializeString;
    initializeString = init1;
    if((result = I2C_Out(MXC4005XCSlaveAddress,initializeString)) < 0) {    //   Reset Device
        returnValue.level0 = -10;
        returnValue.level1 = result;
        return returnValue;
    }
    
    //read DEVICEID for a 2
    returnValue = I2C_In(MXC4005XCSlaveAddress, MXC400_DEVICEID, 1);
    if (returnValue.level0)
        return returnValue;   
   return returnValue;     
}