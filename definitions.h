/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations
#define _XTAL_FREQ  32000000

#define USE_TX_ESP8266

#define GYRO_X 0x43
#define GYRO_Y 0x45
#define GYRO_Z 0x47
#define ACCEL_X 0x3B
#define ACCEL_Y 0x3D
#define ACCEL_Z 0x3F
#define TEMPERATURE 0x41

extern const unsigned int MXC400_INT_SRC0;
extern const unsigned int MXC400_INT_CLR0;
extern const unsigned int MXC400_INT_SRC1;
extern const unsigned int MXC400_INT_CLR1;
extern const unsigned int MXC400_STATUS;
extern const unsigned int MXC400_XOUT_UPPER;
extern const unsigned int MXC400_XOUT_LOWER;
extern const unsigned int MXC400_YOUT_UPPER;
extern const unsigned int MXC400_YOUT_LOWER;
extern const unsigned int MXC400_ZOUT_UPPER;
extern const unsigned int MXC400_ZOUT_LOWER;
extern const unsigned int MXC400_TOUT;
extern const unsigned int MXC400_INTMASK0;
extern const unsigned int MXC400_INTMASK1;
extern const unsigned int MXC400_DETECTION;
extern const unsigned int MXC400_CONTROL;
extern const unsigned int MXC400_DEVICEID;
extern const unsigned int MXC400_WHOAMI;


typedef struct returnStruct {
    int level0;
    int level1;
} returnStruct;


double AtoD(unsigned int Channel);

extern char buf[];
extern char Tinside[];
extern char Toutside[];
extern char Tdiff[];
extern char Tmax[];
extern char studentTin[];
extern char studentTout[];
extern char upDoorSw[];
extern char lowDoorSw[];
//extern char doorTime[];
extern char doorOFC[];
extern char motorstat[];
extern char doorPerOpen[];
extern char processstat[];
extern char I2CErrorPosition[];
extern char TopCallErrorPosition[];
extern char openDoorsFailed[];
extern char closeDoorsFailed[];


extern int T0Count, T1Count;
extern unsigned long DoorOpenTime;  // Time required to open or close the door fully
extern int T1Overflows;
extern double DoorDegree;
//extern double DoorDegreeOpenMax;
extern double TMax, TOut, TIn;
extern double ADResult;
extern double dT, odT, idT;
extern double volts;        // Temperature control knob
extern double sti;     // student t inside temperature difference
extern double sto;     // student t outside temperature difference
extern double stv;      // student t door voltage
extern double ADPrevious_Inside, ADPrevious_Outside;
extern double ADPrevious_DoorVoltage;
extern double Zalpha;
extern char *buffer;
extern int status;
extern char thebuffer[100];

extern int DoorMoved;
extern char *rx;
extern char rcbuffer[];
extern int rcack;

//extern double DoorPosVolts;
//extern int OpenedDoorVoltage, ClosedDoorVoltage;
//extern int iDoorPosVolts;

//extern double timeInSeconds;
extern char *decimalPoint;
extern char decPt;
//extern double DoorPercentOpen;

extern int MPU6050SlaveAddress;
extern int ESP6288ModuleAddress;
extern int MXC4005XCSlaveAddress;

extern int T2Status;
extern int I2CBusCollision;

void TXout(char *string);
char *formatString (double value, int behindTheDecimalNumbers, char * additionalCharacters);
void setupI2C();
//void I2C_SetAddress ( unsigned int SlaveAddress);
//void I2C_Out(char *string);
int I2C_Out(int I2C_Address, char *string);
returnStruct init_MPU6050();
returnStruct init_MXC400();
returnStruct I2C_In (int I2C_Address, int Register, int size);
returnStruct setupDoors();
void timer2();
returnStruct openDoors( int Setup);
returnStruct closeDoors( int Setup);
void BusCollision (int i);



// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

