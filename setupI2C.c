#include <xc.h>
#include <pic16f18426.h>
#include "definitions.h"

//// MPU6050 few configuration register addresses
//const unsigned int MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
//const unsigned int MPU6050_REGISTER_USER_CTRL    =  0x6A;
//const unsigned int MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
//const unsigned int MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
//const unsigned int MPU6050_REGISTER_CONFIG       =  0x1A;
//const unsigned int MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
//const unsigned int MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
//const unsigned int MPU6050_REGISTER_FIFO_EN      =  0x23;
//const unsigned int MPU6050_REGISTER_INT_ENABLE   =  0x38;
//const unsigned int MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
//const unsigned int MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;
//const unsigned int MPU6050_REGISTER_I2C_MST_CTRL = 0x24;

//extern double accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
//extern double gyro_x, gyro_y, gyro_z; // variables for gyro raw data
//extern double temperature; // variables for temperature data
//int step;

void setupI2C()
{
#ifdef REMOVE_SERIAL_ESP8266MOD
    // Setup for I2C - 1  THE ESP8266EX @ 31250Hz
    ANSELAbits.ANSA4 = 0;       // Pin 3   - Formally the door limit sensors
    ANSELAbits.ANSA5 = 0;       // Pin 2        .. Red WUre us A4
    
    PIE3bits.BCL1IE = 1;        // ENable bit collision interrupt
    // Weal pullups off
    WPUAbits.WPUA4 = 0;
    WPUAbits.WPUA5 = 0;
 
    INLVLAbits.INLVLA4 = 0;     // SMB voltage levels
    INLVLAbits.INLVLA5 = 0;
    
    ODCONAbits.ODCA4 = 0;   // Open Collector
    ODCONAbits.ODCA5 = 0;
    
    TRISAbits.TRISA4 = 1;  // Port directions
    TRISAbits.TRISA5 = 1;
    
#ifdef Test
    SSP1CLKPPS = 0x14;      // INput pins C4 & C5
    SSP1DATPPS = 0x15;
    
    RC4PPSbits.RC4PPS = 0x13;   // Functions of output pins
    RC5PPSbits.RC5PPS = 0x14;
#else
    // Input to Peripheral (MSSP2) from pins RA4 and RA5
    SSP1CLKPPS = 0x04;      // A4 - Pin 3 CLK
    SSP1DATPPS  = 0x05;     // A5   Pin 2 DAT
    
    // OUTPUT Pin Source is SCL1 or SDA1 from table in datasheet
    RA4PPSbits.RA4PPS = 0x13;   // SCL1
    RA5PPSbits.RA5PPS = 0x14;   // SDA1
#endif
   
    SSP1STATbits.SMP = 0;   // Slew rate not enabled for high speed mode
    SSP1STATbits.CKE = 0;       // SMBus voltage levels enabled
    SSP1ADD =  0xFE; //33,333//  0xFF;//31250 Hz // 0xB1;  // 45k 0x4f;     // 0x13 400KHz operation    0x4f;  // 100kHz
 //   SSP2ADD=0x4f;
    SSP1CON1bits.SSPM = 0b1000; // Master mode
    SSP1CON3bits.SDAHT = 1;     // 300ns = 1 SDA hold time (longer than 100ns))
    SSP1CON1bits.SSPEN = 1;     // I2C enable module
   
#endif
 
     //
    // SETUP I2C2 MPSS2   
    //
    // Setup fo I2C - 2
    ANSELCbits.ANSC4 = 0;
    ANSELCbits.ANSC5 = 0;
       
    
    PIE3bits.BCL2IE = 1;    // Enable bit collision interrupt
    // Weak pull ups OFF
    WPUCbits.WPUC4 = 0;
    WPUCbits.WPUC5 = 0; 
    
    INLVLCbits.INLVLC4 = 0;     // TTL Levels closer to SMB
    INLVLCbits.INLVLC5 = 0;
    
    ODCONCbits.ODCC4 = 0;
    ODCONCbits.ODCC5 = 0;

    // Assign peripherals to pins 
    TRISCbits.TRISC4 = 1;
    TRISCbits.TRISC5 = 1;    
    // No Peripheral Pin Mappings necessary since everything is already in place.
    // associate peripheral with pins

    
    // Input pin assignments
    SSP2CLKPPS = 0x14;     // Port C4 CLOCK  PIN 6
    SSP2DATPPS = 0x15;     // PORTC (0x10) and Port 5 (x5) data
    
    // Assigns pins to peripherals
    // Input Pin Peripheral pin assigments

    RC4PPSbits.RC4PPS = 0x15; // This one uses the pin number instead of the port number CLK OUTPUT RC4
    RC5PPSbits.RC5PPS = 0x16; // DATA OUTPUT
//    RC4, Pin 6 is SCLK2 - used for this program and the default location
//    RC5, Pin 5 is SDA2 - used for this program and the default location

       // Setup I2C as Master to control
        // The Arduino Slave address is in the code as software
        // The Tilt Sensor Address is somewhere else.
//    TRISCbits.= 1;
    SSP2STATbits.SMP = 0;   // Slew rate not enabled for high speed mode
    SSP2STATbits.CKE = 1;       // SMBus voltage levels enabled
    SSP2ADD =  0x4f;    //  100KHz operation    0x4f;  // 100kHz
 //   SSP2ADD=0x4f;
    SSP2CON1bits.SSPM = 0b1000; // Master mode
    SSP2CON3bits.SDAHT = 1;     // 300ns = 1 SDA hold time (longer than 100ns))
    SSP2CON1bits.SSPEN = 1;     // I2C enable module

    return;     
  
}

#ifdef STEST
int self_test_g() {
 
    int Result;
    char init[3];
    char *command = &init;
    init[0] - MPU6050_REGISTER_GYRO_CONFIG;
    init[1] = 0x20; // Z gyro self test.
    init[2] = 0;  // Return range is +/- 14.
    I2C_Out(ESP6288ModuleAddress,command);
    Result = I2C_ReadMPU6050 (0x47);
    gyro_z =  Result/131.0;
    init[1] = 0x0;  // out of self test
    I2C_Out(ESP6288ModuleAddress,command);
    return Result;
}
#endif


///*
// * PIC18F4550 I2C Source File
// * http://www.electronicwings.com
// */ 
//
//
//#include <pic18f4550.h>
//#include "I2C_Master_File.h"
//
//void I2C_Init()
//{
//    TRISB0 = 1;                     /* Set up I2C lines by setting as input */
//	TRISB1 = 1;
//	SSPSTAT = 80;                   /* Slew rate disabled, other bits are cleared */
//    SSPCON1 = 0x28;					/* Enable SSP port for I2C Master mode, clock = FOSC / (4 * (SSPADD+1))*/ 
//	SSPCON2 = 0;
//    SSPADD = BITRATE;               /* Set clock frequency */  
//    SSPIE = 1;                      /* Enable SSPIF interrupt */
//    SSPIF = 0;
//}
//
//char I2C_Start(char slave_write_address)
//{   
//    SSPCON2bits.SEN = 1;            /* Send START condition */
//    while(SSPCON2bits.SEN);         /* Wait for completion of START */
//    SSPIF=0;
//    if(!SSPSTATbits.S)              /* Return 0 if START is failed */
//    return 0;
//    return (I2C_Write(slave_write_address)); /* Write slave device address with write to communicate */
//}
//
//void I2C_Start_Wait(char slave_write_address)
//{
//  while(1)
//  {   
//    SSPCON2bits.SEN = 1;            /* Send START condition */
//    while(SSPCON2bits.SEN);         /* Wait for completion of START */
//    SSPIF = 0;
//    if(!SSPSTATbits.S)              /* Continue if START is failed */
//        continue;
//    I2C_Write(slave_write_address); /* Write slave device address with write to communicate */
//    if(ACKSTAT)                     /* Check whether Acknowledgment received or not */
//    {
//        I2C_Stop();                 /* If not then initiate STOP and continue */
//        continue;
//    }    
//    break;                          /* If yes then break loop */
//  }
//}
//
//char I2C_Repeated_Start(char slave_read_address)
//{
//    RSEN = 1;                       /* Send REPEATED START condition */
//    while(SSPCON2bits.RSEN);        /* Wait for completion of REPEATED START condition */
//    SSPIF = 0;
//    if(!SSPSTATbits.S)              /* Return 0 if REPEATED START is failed */
//    return 0;
//    I2C_Write(slave_read_address);  /* Write slave device address with read to communicate */
//    if (ACKSTAT)                    /* Return 2 if acknowledgment received else 1 */
//     return 1;
//    else
//     return 2;
//}
//
//char I2C_Write(unsigned char data)
//{
//      SSPBUF = data;                /* Write data to SSPBUF */
//      I2C_Ready();
//      if (ACKSTAT)                  /* Return 2 if acknowledgment received else 1 */
//        return 1;
//      else
//        return 2;
//}
//
//void I2C_Ack()
//{
//    ACKDT = 0;  					/* Acknowledge data 1:NACK,0:ACK */
//	ACKEN = 1;        				/* Enable ACK to send */
//    while(ACKEN);
//}
//
//void I2C_Nack()
//{
//    ACKDT = 1;          			/* Not Acknowledge data 1:NACK,0:ACK */
//	ACKEN = 1;              		/* Enable ACK to send */	          
//    while(ACKEN);
//}
//char I2C_Read(char flag)            /* Read data from slave devices with 0=Ack & 1=Nack */
//{
//    char buffer;
//    RCEN = 1;                       /* Enable receive */
//    while(!SSPSTATbits.BF);         /* Wait for buffer full flag which set after complete byte receive */
//    buffer = SSPBUF;                /* Copy SSPBUF to buffer */
//    if(flag==0)
//        I2C_Ack();                  /* Send acknowledgment */
//    else
//        I2C_Nack();                 /* Send negative acknowledgment */
//    I2C_Ready();
//    return(buffer);
//}
//
//char I2C_Stop()
//{
//    PEN = 1;                        /* Initiate STOP condition */
//    while(PEN);                     /* Wait for end of STOP condition */
//    SSPIF = 0;
//    if(!SSPSTATbits.P);             /* Return 0 if STOP failed */
//    return 0;
//}
//
//void I2C_Ready()
//{
//    while(!SSPIF);                  /* Wait for operation complete */
//    SSPIF=0;                        /*clear SSPIF interrupt flag*/
//}
//




///* 
// * MPU6050 Interfacing with PIC18F4550
// * http://www.electronicwings.com
// */
// 
//#include <pic18f4550.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include "USART_Header_File.h"      /* Include USART header file */
//#include "I2C_Master_File.h"
//#include "MPU6050_res_define.h"
//#include "Configuration_header_file.h"
//
//void MPU6050_Init()										/* Gyro initialization function */
//{
//	MSdelay(150);		/* Power up time >100ms */
//	I2C_Start_Wait(0xD0);	/* Start with device write address */
//	I2C_Write(;);	/* Write to sample rate register */
//	I2C_Write(0x07);	/* 1KHz sample rate */
//	I2C_Stop();
//
//	I2C_Start_Wait(0xD0);
//	I2C_Write(PWR_MGMT_1);	/* Write to power management register */
//	I2C_Write(0x01);	/* X axis gyroscope reference frequency */
//	I2C_Stop();
//
//	I2C_Start_Wait(0xD0);
//	I2C_Write(CONFIG);	/* Write to Configuration register */
//	I2C_Write(0x00);	/* Fs = 8KHz */
//	I2C_Stop();
//
//	I2C_Start_Wait(0xD0);
//	I2C_Write(GYRO_CONFIG);	/* Write to Gyro configuration register */
//	I2C_Write(0x18);	/* Full scale range +/- 2000 degree/C */
//	I2C_Stop();
//
//	I2C_Start_Wait(0xD0);
//	I2C_Write(INT_ENABLE);	/* Write to interrupt enable register */
//	I2C_Write(0x01);
//	I2C_Stop();
//
//}
//
//void MPU_Start_Loc()
//
//{
//	I2C_Start_Wait(0xD0);								/* I2C start with device write address */
//	I2C_Write(ACCEL_XOUT_H);							/* Write start location address from where to read */ 
//	I2C_Repeated_Start(0xD1);							/* I2C start with device read address */
//}
//
//void main()
//
//{
//	char buffer[20];
//	int Ax,Ay,Az,T,Gx,Gy,Gz;
//	float Xa,Ya,Za,t,Xg,Yg,Zg;
//    OSCCON = 0x72;
//    I2C_Init();											/* Initialize I2C */	
//	MPU6050_Init();										/* Initialize Gyro */
//	USART_Init(9600);									/* Initialize USART with 9600 baud rate */
//	
//	while(1)
//	{
//		MPU_Start_Loc();								/* Read Gyro values continuously and send to terminal over USART */
//		Ax = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
//		Ay = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
//		Az = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
//		T = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
//		Gx = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
//		Gy = (((int)I2C_Read(0)<<8) | (int)I2C_Read(0));
//		Gz = (((int)I2C_Read(0)<<8) | (int)I2C_Read(1));
//		I2C_Stop();
//        
//		
//		Xa = (float)Ax/16384.0;							/* Divide raw value by sensitivity scale factor to get real values */
//		Ya = (float)Ay/16384.0;
//		Za = (float)Az/16384.0;
//		Xg = (float)Gx/131.0;
//		Yg = (float)Gy/131.0;
//		Zg = (float)Gz/131.0;
//		t = ((float)T/340.00)+36.53;					/* Convert temperature in °/c using formula */
//
//
//		sprintf(buffer," Ax = %.2f g\t",Xa);			/* Take values in buffer to send all parameters over USART */
//		USART_SendString(buffer);
//
//		sprintf(buffer," Ay = %.2f g\t",Ya);
//		USART_SendString(buffer);
//		
//		sprintf(buffer," Az = %.2f g\t",Za);
//		USART_SendString(buffer);
//
//		sprintf(buffer," T = %.2f%cC\t",t,0xF8);           /* 0xF8 Ascii value of degree '°' on serial */
//		USART_SendString(buffer);
//
//		sprintf(buffer," Gx = %.2f%c/s\t",Xg,0xF8);
//		USART_SendString(buffer);
//
//		sprintf(buffer," Gy = %.2f%c/s\t",Yg,0xF8);
//		USART_SendString(buffer);
//		
//		sprintf(buffer," Gz = %.2f%c/s\r\n",Zg,0xF8);
//		USART_SendString(buffer);
//	}
//}
//
//
//
//
