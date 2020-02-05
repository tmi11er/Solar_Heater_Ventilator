// Solar Heater Vent
#ifdef pic688
#include <xc.h>

#pragma config FOSC = INTOSCIO // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF // Brown Out Detect (BOR enabled)
#pragma config IESO = OFF // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)


#include <pic16f688.h>
#endif

// PIC16F18426 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINTPLL// Power-up default value for COSC bits (HFINTOSC with 2x PLL, with OSCFRQ = 16 MHz and CDIV = 1:1 (FOSC = 32 MHz))
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; i/o or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (FSCM timer disabled)

// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR pin is Master Clear function)
#pragma config PWRTS = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config LPBOREN = OFF    // Low-Power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = ON       // Brown-out reset enable bits (Brown-out Reset Enabled, SBOREN bit is ignored)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (VBOR) set to 2.45V)
#pragma config ZCDDIS = OFF     // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR.)
#pragma config PPS1WAY = OFF     // Peripheral Pin Select one-way control (The PPSLOCK bit can be cleared and set only once in software)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

// CONFIG3
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled, SWDTEN is ignored)
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4
#pragma config BBSIZE = BB512   // Boot Block Size Selection bits (512 words boot block size)
#pragma config BBEN = OFF       // Boot Block Enable bit (Boot Block disabled)
#pragma config SAFEN = OFF      // SAF Enable bit (SAF disabled)
#pragma config WRTAPP = OFF     // Application Block Write Protection bit (Application Block not write protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block not write protected)
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration Register not write protected)
#pragma config WRTD = OFF       // Data EEPROM write protection bit (Data EEPROM NOT write protected)
#pragma config WRTSAF = OFF     // Storage Area Flash Write Protection bit (SAF not write protected)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/Vpp pin function is MCLR.)

// CONFIG5
#pragma config CP = OFF         // UserNVM Program memory code protection bit (UserNVM code protection disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pic16f18426.h>

#include "definitions.h"


void uartSetup();
void setupTimer0();
void setupTimer1();
void setupTimer2();
void TXout(char *string);
void setupAtoD();

void printDoorSwitches();
void setupIOC();

//double normsinv( double p);
//int linearCorrelationCoefficient (int numberofSamples, int ConfidenceLevel);
char buf[10];

char clearScreen[] = "\033[2J";
char intro1[] = "\033[?25l\033[H  \033[1;27HVentilator Door Software 2018" \
"\033[2;35HApril 9, 2018" \
"\033[4;45HClosed Door Accel:\033[5;45HOpened Door Accel: " \
"\033[4;1HInside Temperature      :\033[5;1HOutside Temperature     : \033[6;1HTemperature Difference  :";

char intro2[] = "\033[6;45HMax. Temperature :\033[8;1HStudent T Inside Change :" \
"\033[9;1HStudent T Outside Change:\033[7;45HDoor Pos. Accel. :" \
"\033[8;45HDoor Percent Open:\033[9;45HMotor Direction  :";
char intro3[] = "\033[7;1HAdjustment Process: ";
char Tinside[] = "\033[4;27H";
char Toutside[] = "\033[5;27H";
char Tdiff[] = "\033[6;27H";
char Tmax[] = "\033[6;64H";
char studentTin[] = "\033[8;27H";
char studentTout[] = "\033[9;27H";
char upDoorSw[] = "\033[4;64H";
char lowDoorSw[] = "\033[5;64H";
//char doorTime[] = "\033[7;66H\0";
char doorOFC[] = "\033[7;64H"; // Door COunter
char motorstat[] = "\033[9;64H";
char doorPerOpen[] = "\033[8;64H";
char processstat[] = "\033[7;21H";
char TopCallErrorPosition[] = "\033[10;2H"; 

char mpu6050Fail[] = "MPU6050 Init. Fail ";
char mxc400Fail[] = "MXC400Xxc Init. Fail ";
char openDoorsFailed[] = "OpenDoors Failure ";
char closeDoorsFailed[] = "CloseDoors Failure ";


char rcbuffer[128];
char *rx = &rcbuffer[0];
//char ibuffer[] = "+32767";

char CelciusUnits[] = {0xC2, 0xB0, 'C', '/', 0}; //  "\xC2\xB0C/";
char FarhenheitUnits[] = {0xC2, 0xB0, 'F', 0}; // "\xC2\xB0F";
//unsigned char *ib = &ibuffer[0];

//int Samples[NumberofSamples];
double ADResult;
int T1Counter;
double x;
double dT, odT, idT;
char *buffer;
char thebuffer[100];
char Celsius[100];
int status;
//unsigned long DoorOpenTime;  // Time required to open or close the door fully
double volts; // Temperature control knob
int T0Count, T1Count;
int T1Overflows;
double DoorDegree = 50.0;
int DoorMoved;

double Zalpha = 2.630330; // 99% Confidence level z  
double sti = -100.0; // student t inside temperature difference
double sto = -100.0; // student t outside temperature difference
double ADPrevious_Inside;
double ADPrevious_Outside = 1000.0;
double stv = -100.0; // student t door voltage
double ADPrevious_DoorVoltage;

//double DoorDegreeOpenMax;
//double DoorPercentOpen = 0;

unsigned char *decimalPoint;
char decPt = '.';

//#define DOORS_NOT_CLOSED  PORTAbits.RA4         // bit set means the switch is open
//#define DOORS_NOT_OPENED  PORTAbits.RA5
double TMax, TOut, TIn;
int i;

int MPU6050SlaveAddress = 0x68;
int MXC4005XCSlaveAddress = 0x15;
int ESP6288ModuleAddress = 0x8;

double accelerometer_z, accelerometer_y, accelerometer_x; // variables for accelerometer raw data
double gyro_x, gyro_y, gyro_z; // variables for gyro raw data
double temperature; // variables for temperature data


extern double DoorInclination;
extern double OldDoorInclination;
extern double OpenedDoorInclination;
extern double ClosedDoorInclination;
extern int maxDoorCount;

returnStruct rts;

int main() {

    // Declare Variables
    returnStruct Result;
    int j;


    buffer = &thebuffer[0];
    
    resetCause();
    PCON1bits.nMEMV = 1;        // No Memory Violation has occured.
    //    int DoorTime;       // Time used to open the door to its current position
    //    int HeatDrain;      // Calculated Heat loss if the door were fully opened
    //    int iDoorVoltage, oldiDoorVoltage;;
    //    int DoorLimit;

    // Configure Hardware
#ifdef pic688   
    CMCON0 = 0x7; // All CxIN pins (13, 12, 10, 9; ISCPDAT, Vref/ISCPCLK,
    // Motor Control, Max.Temp.) configured as digital and
    // all comparators off.
    ANSEL = 0; // Configure all A/D inputs as Digital -RC4 & RC5
    // (RX/TX) have no AtoD circuitry.

    TRISAbits.TRISA1 = 1; // Vref as Input
    ANSELbits.ANS1 = 1; // Vref as Analog

    TRISCbits.TRISC1 = 1; // Max Temp. Pot.
    ANSELbits.ANS5 = 1; // Max. Temp. Pot. as Analog
    TRISCbits.TRISC2 = 1; // Outside NTC as Input
    ANSELbits.ANS6 = 1; // Outside NTC as Analog
    TRISCbits.TRISC3 = 1; // Inside NTC as Input
    ANSELbits.ANS7 = 1; // Inside NTC as Analog
    ADCON0 = 0; // No Channel selected the ADC  AN1 AN6 AN7 are Max.Temp. Pot., Outside Temp., Inside Temp. Channels of the sample and hold circuit
    //    OPTION_REGbits.nRAPU = 1;   // Enable Pullups
    TRISAbits.TRISA4 = 1; // RA4 is for an Upper Door Sensor (Closed)
    ANSELbits.ANS3 = 0; // Digital I/O
    IOCAbits.IOCA4 = 0; // Interrupt On Change Door Sensor
    //    WPUAbits.WPUA4 = 1;     // Weak Pull Up RA4
    TRISAbits.TRISA5 = 1; // RA5 is for IOC for lower door sensors (Opened)
    IOCAbits.IOCA5 = 0; // Interrupt On Change Door Sensor
    //    WPUAbits.WPUA5 = 1;     // Weak Pull Up RA5
    TRISAbits.TRISA2 = 0; // Motor Control Open
    TRISCbits.TRISC0 = 0; // Motor Control Closed
    ANSELbits.ANS1 = 1; // Vref is an analog channel after PicKit is removed
    // and after 3.3V appears on it.

    // Oscillator to 8 MHz
    OSCCONbits.SCS = 0;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;


    while (!OSCCONbits.HTS) {
    }; // Wait until the clock is stable
#else
    while (!OSCSTATbits.HFOR) {
    }; // Wait until the clock is stable  
    PMD3 = 0x47; // Disable everything in the register except AtoD
    //    PMD3bits.CMP1MD = 1;            // Disable comparator 1
    //    PMD3bits.CMP2MD = 1;            // Disable comparator 2
    //    PMD3bits.DAC1MD = 1;            // Disable Digital to Analog Converter
    //    PMD3bits.ZCDMD = 1;             // Disable  Zero cross detector
    PMD4 = 0xFF; // Disable Pulse width modulators
    PMD5 = 0xFF; // Disable continuous wave modules
    PMD6 = 0xff; // Disable serial devices
#ifdef UART
    PMD6bits.U1MD = 0; // Enable EUSart
#else
    PMD6bits.MSSP2MD = 0; // Enable
    PMD6bits.U1MD = 0; // Enable uart
    PMD6bits.MSSP1MD = 1; // Emab;e
#endif
    PMD7 = 0xFF; // Disable 
    // AtoD Converters
    TRISCbits.TRISC1 = 1; // Pot/knob - max temp analog
    ANSELCbits.ANSC1 = 1; // analog Input
    TRISCbits.TRISC2 = 1; // Outside NTC
    ANSELCbits.ANSC2 = 1; // analog Input
    TRISCbits.TRISC3 = 1; // Inside NTC
    ANSELCbits.ANSC3 = 1; // analog Input  


    ANSELAbits.ANSA4 = 0; // Back Switch, Lower Pin  (Was Lower Door switch - AtoD)
    TRISAbits.TRISA4 = 1; // Now Output to gate ESP8266EX
    ANSELAbits.ANSA5 = 0; // Upper Door switch
    TRISAbits.TRISA5 = 1;

    LATAbits.LATA2 = 0;
    LATCbits.LATC0 = 0;
    ANSELAbits.ANSA2 = 0;
    TRISAbits.TRISA2 = 0; // Motor Control Output
    ANSELCbits.ANSC0 = 0;
    TRISCbits.TRISC0 = 0; // Motor Control Output
    PORTAbits.RA2 = 0;
    PORTCbits.RC0 = 0;
#endif   

    setupAtoD();
#ifdef pic688
    setupTimer1();
#else
    setupTimer0();
    setupTimer1();
    setupTimer2();
#endif


   //   setupIOC();  

    uartSetup();


    //    while(1) {
    //       TXout("Todd Matin Miller");
    //        INTCONbits.GIE=0;
    //        *rx = 0;
    //        
    //        rx = &rcbuffer[0];
    //        for (int q = 0; q< 10; q++)
    //            rcbuffer[q] = 0;
    ////        TXout("aaa");
    //        rx = &rcbuffer[0]; 
    //       INTCONbits.GIE = 1;
    //        __delay_ms(2000);
    //    }
    //    TXout("Initializing");

    TXout(clearScreen);
    TXout(intro1); // Title print
    TXout(intro2);
    TXout(intro3);
    __delay_ms(100);

    //    I2C_Out(ESP6288ModuleAddress,"Initializing\n");   
    //    I2C_Out(ESP6288ModuleAddress,clearScreen);
    //    I2C_Out(ESP6288ModuleAddress,intro1);
    //    I2C_Out(ESP6288ModuleAddress,intro2);
    //    I2C_Out(ESP6288ModuleAddress,intro3);  

    setupI2C();
#ifdef NEW_TILT_SENSOR   
    rts = init_MXC400();
    if (rts.level0 < 0) {
        TXout(processstat); // Position
        TXout(mxc400Fail); // 0 is success, -1 is failure
        buffer = formatString(rts.level0, 0, ",");
        TXout(buffer);
        buffer = formatString(rts.level1, 0, ";");
        TXout(buffer);
        __delay_ms(4000);
        RESET();
    }
#endif

    
    rts = init_MPU6050();

    if (rts.level0 != 0) {
        TXout(TopCallErrorPosition); // Position
        TXout(mpu6050Fail); // 0 is success, -1 is failure
        buffer = formatString(rts.level0, 0, ",");
        TXout(buffer);
        buffer = formatString(rts.level1, 0, ";");
        TXout(buffer);
//        if(I2CBusCollision) {      // I2C reset, the SSP2CON bits are all cleared - idle state
//                I2CBusCollision = 0;
//                BusCollision(13);
//        }        
        __delay_ms(1000);
        RESET();
    }


    //    int Result;
    //    while(1) {

    //    Result = I2C_ReadMPU6050 (0x3B);
    //    accelerometer_x = (double) Result/16384.0; /// *9.80665;
    //    Result = I2C_ReadMPU6050 (0x3D);
    //    accelerometer_y = (double) Result/16384.0; // *9.80665;  
    //    Result = I2C_ReadMPU6050 (0x3F);
    //    accelerometer_z = (double) Result/16384.0; // *9.80665;
    //    Result = I2C_ReadMPU6050 (0x41);
    //    temperature =  Result/340.00+36.53;
    //    Result = I2C_ReadMPU6050 (0x43);
    //    gyro_x =  Result/131.0;
    //    Result = I2C_ReadMPU6050 (0x45);
    //    gyro_y =  Result/131.0; 
    //    Result = I2C_ReadMPU6050 (0x47);
    //    gyro_z =  Result/131.0;    

    //   self_test_g(); 
    //    }

    //   while(1) { 
    //    I2C_SetAddress ( ESP6288ModuleAddress );        // No write - No Device 
    //    I2C_Out(ESP6288ModuleAddress,clearScreen);
    //    I2C_Out(ESP6288ModuleAddress,intro1);
    //    I2C_Out(ESP6288ModuleAddress,intro2);
    //    I2C_Out(ESP6288ModuleAddress,intro3);
    //   }

    //    int q = 0xB0;
    //    while(1) {
    //    x = 25.00;
    //             buffer = ftoa(x, &status);
    //             decimalPoint = strchr(buffer,decPt);
    //            decimalPoint++;
    //            decimalPoint++;
    ////            *decimalPoint++ = 260;
    //            *decimalPoint = '\0'; 
    //            TXout(buffer);
    //}



    //    while(1) {
    //    printDoorSwitches();
    rts = setupDoors();
    if ((rts.level0) < 0) {
        __delay_ms(1000);
        RESET();
    }


    int firstpass = 1;
    // Operate the Ventilator
    int iteration = 0;
    while (1) {
        //       I2C_SetAddress ( ESP6288ModuleAddress );        // No write - No Device 
        // Time the doors
        if (iteration == 250) {
            INTCONbits.GIE = 0;
#ifdef USE_TX_ESP8266              
            TXout(clearScreen);
            TXout(intro1); // Title print
            TXout(intro2);
            TXout(intro3);
#else              
            I2C_Out(ESP6288ModuleAddress,clearScreen);
            I2C_Out(ESP6288ModuleAddress,intro1); // Title print
            I2C_Out(ESP6288ModuleAddress,intro2);
            I2C_Out(ESP6288ModuleAddress,intro3);
#endif            
            INTCONbits.GIE = 1;
            //            printDoorSwitches();
            iteration = 0;
        }
        iteration++;
        if (firstpass == 1) {
            //            INTCONbits.GIE = 0;     // Disable interrupts 
#ifdef USE_TX_ESP8266 
            TXout(processstat);
            TXout((char *) "Acquire & Analyze");
#else
            I2C_Out(ESP6288ModuleAddress,processstat);
            I2C_Out(ESP6288ModuleAddress,(char *) "Acquire & Analyze");
#endif
            //            INTCONbits.GIE = 1;     // enable interrupts
        }
#if CHARACTERS_INPUT       
        // Received characters are printed
        char *px;
        if (rx != &rcbuffer[0]) {

            for (px = &rcbuffer[0]; px < rx;) {
                // if *px == 's' then different output
                while (!PIR3bits.TX1IF) {
                };
                TXREG = *px++;
            }
            rx = &rcbuffer[0];
        }
#endif   // CHARACTERS_INPUT 
        // Get Maximum Temperature
        if((firstpass <-3) || (firstpass  > 1)) {
            while(1)
               SLEEP();
        }         
        TMax = AtoD(0b010001); // RC1 pin 9  
        if((firstpass <-3) || (firstpass  > 1)) {
            while(1)
               SLEEP();
        }        
        x = TMax;
#ifdef USE_TX_ESP8266 
        // Telnet can't print degree character
        buffer = formatString(x, 2, &CelciusUnits[0]); // removed degree symbol
#else            
        buffer = formatString(x, 2, (char *) "\xC2\xB0C/");
#endif
        //            buffer = ftoa(x,&status);
        //            decimalPoint = strchr(buffer,decPt);           
        //            decimalPoint++;
        //            decimalPoint++;
        //            *decimalPoint++ = 0xC2;
        //            *decimalPoint++ = 0xB0;
        //            *decimalPoint++ = 'C';
        //            *decimalPoint++ = '/';
        //            *decimalPoint = '\0';
        strcpy(Celsius, buffer);
        x = x * 9.0 / 5.0 + 32.0;
#ifdef USE_TX_ESP8266 
        // Telnet can't print degree character
        buffer = formatString(x, 2, &FarhenheitUnits[0]);
#else                        
        buffer = formatString(x, 2, (char *) "\xC2\xB0F");
#endif            
        //            buffer = ftoa(x, &status);
        //            decimalPoint = strchr(buffer,decPt);
        //            decimalPoint++;
        //            decimalPoint++;
        //            *decimalPoint++ = 0xC2;
        //            *decimalPoint++ = 0xB0;
        //            *decimalPoint++ = 'F';
        //            *decimalPoint++ = ' ';
        //            *decimalPoint = '\0';
        //            INTCONbits.GIE=0;
        //            I2C_Out(Tmax);
#ifdef USE_TX_ESP8266 
        TXout(Tmax);
        TXout(Celsius);
        TXout(buffer);
#else
        I2C_Out(ESP6288ModuleAddress,Tmax);
        I2C_Out(ESP6288ModuleAddress,Celsius);
        I2C_Out(ESP6288ModuleAddress,buffer);
#endif
        //            INTCONbits.GIE=1;             

        // Get Inside Temperature
        
        // ENSURE THIS IS NOT A OUT Liner
        if((firstpass <-3) || (firstpass  > 1)) {
            while(1)
               SLEEP();
        }           
        TIn = AtoD(0b010011); // RC3 pin 7
        if((firstpass <-3) || (firstpass  > 1)) {
            while(1)
               SLEEP();
        }        
        x = TIn;        // Proceed with the new value
        if((x > 1000.0) || (x < -1000.0)) {
            while(1)
                SLEEP(1);
        }
#ifdef USE_TX_ESP8266 
        // Telnet can't print degree character
        buffer = formatString(x, 2, &CelciusUnits[0]);      // STopped Here 2/5/20
#else            
        buffer = formatString(x, 2, (char *) "\xC2\xB0C/");
#endif 
         if((firstpass <-3) || (firstpass  > 1)) {
            while(1)
               SLEEP();
        }           
        //            buffer = ftoa(x, &status);
        //            decimalPoint = strchr(buffer,decPt);
        //            decimalPoint++;
        //            decimalPoint++;
        //            *decimalPoint++ = 0xC2;
        //            *decimalPoint++ = 0xB0;
        //            *decimalPoint++ = 'C';
        //            *decimalPoint++ = '/';
        //            *decimalPoint = '\0';
        strcpy(Celsius, buffer);
        x = x * 9.0 / 5.0 + 32.0;
         if((firstpass <-3) || (firstpass  > 1)) {
            while(1)
               SLEEP();
        }           
#ifdef USE_TX_ESP8266 
        // Telnet can't print degree character
        buffer = formatString(x, 2, &FarhenheitUnits[0]);
#else                        
        buffer = formatString(x, 2, (char *) "\xC2\xB0F");
#endif            
        //            buffer = ftoa(x, &status);
        //            decimalPoint = strchr(buffer,decPt);
        //            decimalPoint++;
        //            decimalPoint++;
        //            *decimalPoint++ = 0xC2;
        //            *decimalPoint++ = 0xB0;
        //            *decimalPoint++ = 'F';
        //            *decimalPoint++ = ' ';           
        //            *decimalPoint = '\0';
        //            INTCONbits.GIE=0;
        if((firstpass <-3) || (firstpass  > 1)) {
            while(1)
               SLEEP();
        }    
#ifdef USE_TX_ESP8266 
        TXout(Tinside);
        TXout(Celsius);
        TXout(buffer);
#else
        I2C_Out(ESP6288ModuleAddress,Tinside);
        I2C_Out(ESP6288ModuleAddress,Celsius);
        I2C_Out(ESP6288ModuleAddress,buffer);
#endif
        //            INTCONbits.GIE=1;

        // Get Outside Temperature
        if((firstpass <-3) || (firstpass  > 1)) {
            while(1)
               SLEEP();
        }       
        TOut = AtoD(0b010010); // RC2 pin 8
        if((firstpass <-3) || (firstpass  > 1)) {
            while(1)
               SLEEP();
        }       
        x = TOut;        // Proceed with the new value       
#ifdef USE_TX_ESP8266 
        // Telnet can't print degree character
        buffer = formatString(x, 2, &CelciusUnits[0]);
#else            
        buffer = formatString(x, 2, (char *) "\xC2\xB0C/");
#endif           
        //            buffer = ftoa(x,&status);
        //            decimalPoint = strchr(buffer,decPt);            
        //            decimalPoint++;
        //            decimalPoint++;
        //            *decimalPoint++ = 0xC2;
        //            *decimalPoint++ = 0xB0;
        //            *decimalPoint++ = 'C';
        //            *decimalPoint++ = '/';
        //            *decimalPoint = '\0';
        strcpy(Celsius, buffer);
        x = x * 9.0 / 5.0 + 32.0;
#ifdef USE_TX_ESP8266 
        // Telnet can't print degree character
        buffer = formatString(x, 2, &FarhenheitUnits[0]);
#else                        
        buffer = formatString(x, 2, (char *) "\xC2\xB0F");
#endif             
        //            buffer = ftoa(x, &status);
        //            decimalPoint = strchr(buffer,decPt);
        //            decimalPoint++;
        //            decimalPoint++;
        //            *decimalPoint++ = 0xC2;
        //            *decimalPoint++ = 0xB0;
        //            *decimalPoint++ = 'F';
        //            *decimalPoint++ = ' ';           
        //            *decimalPoint = '\0';
        //            INTCONbits.GIE=0;

#ifdef USE_TX_ESP8266
        TXout(Toutside);
        TXout(Celsius);
        TXout(buffer);
#else
        I2C_Out(ESP6288ModuleAddress,Toutside);
        I2C_Out(ESP6288ModuleAddress,Celsius);
        I2C_Out(ESP6288ModuleAddress,buffer);
#endif
        //            INTCONbits.GIE=1;


        x = TIn - TOut;
        dT = (double) (x);
#ifdef USE_TX_ESP8266 
        // Telnet can't print degree character
        buffer = formatString(x, 2, &CelciusUnits[0]);
#else            
        buffer = formatString(x, 2, (char *) "\xC2\xB0C/");
#endif           
        //            buffer = ftoa(x,&status); 
        //            decimalPoint = strchr(buffer,decPt);           
        //            decimalPoint++;
        //            decimalPoint++;
        //            *decimalPoint++ = 0xC2;
        //            *decimalPoint++ = 0xB0;
        //            *decimalPoint++ = 'C';
        //            *decimalPoint++ = '/';
        //            *decimalPoint = '\0';
        strcpy(Celsius, buffer);
        x = x * 9.0 / 5.0;
#ifdef USE_TX_ESP8266 
        // Telnet can't print degree character
        buffer = formatString(x, 2, &FarhenheitUnits[0]);
#else                        
        buffer = formatString(x, 2, (char *) "\xC2\xB0F");
#endif            
        //            buffer = ftoa(x, &status);
        //            decimalPoint = strchr(buffer,decPt);
        //            decimalPoint++;
        //            decimalPoint++;
        //            *decimalPoint++ = 0xC2;
        //            *decimalPoint++ = 0xB0;
        //            *decimalPoint++ = 'F';
        //            *decimalPoint++ = ' ';            
        //            *decimalPoint = '\0';
        //            INTCONbits.GIE=0;          

#ifdef USE_TX_ESP8266
        TXout(Tdiff);
        TXout(Celsius);
        TXout(buffer);
#else
        I2C_Out(ESP6288ModuleAddress,Tdiff);
        I2C_Out(ESP6288ModuleAddress,Celsius);
        I2C_Out(ESP6288ModuleAddress,buffer);
#endif
        //            INTCONbits.GIE=1;
        // Set the Ventilator Door Positions
        //       I2C_Out(lowDoorSw);       // Position cursor for status message
        buffer = formatString(OpenedDoorInclination, 3, "g");
        //        buffer = ftoa((float)(OpenedDoorVoltage/1000.0),&status);
        //        decimalPoint = strchr(buffer,decPt);
        //        decimalPoint++;
        //        decimalPoint++;
        //        decimalPoint++;
        //        decimalPoint++;              
        //        *decimalPoint++ = 'V'; 
        //         *decimalPoint = '\0';
        //        INTCONbits.GIE=0;    
        //                    I2C_Out(DoorPosVolts); 
#ifdef USE_TX_ESP8266 
        TXout(lowDoorSw);
        TXout(buffer);
#else
        I2C_Out(ESP6288ModuleAddress,lowDoorSw);
        I2C_Out(ESP6288ModuleAddress,buffer);
#endif
        //        INTCONbits.GIE=1;             



        buffer = formatString(ClosedDoorInclination, 3, "g");
        //        buffer = ftoa((float)(ClosedDoorVoltage/1000.0),&status);
        //        decimalPoint = strchr(buffer,decPt);
        //        decimalPoint++;
        //        decimalPoint++;
        //        decimalPoint++;
        //        decimalPoint++;              
        //        *decimalPoint++ = 'V'; 
        //         *decimalPoint = '\0';
        //        INTCONbits.GIE=0;
#ifdef USE_TX_ESP8266 
        TXout(upDoorSw); // Position cursor for status message labeled Open Door Status       
        TXout(buffer);
#else
        I2C_Out(ESP6288ModuleAddress,upDoorSw); // Position cursor for status message labeled Open Door Status        
        I2C_Out(ESP6288ModuleAddress,buffer);
#endif
        //        INTCONbits.GIE=1; 
        TXout(processstat);
        if (firstpass == 1)
            TXout((char *) "First Pass          ");
        __delay_ms(1000);
                     if((firstpass <-3) || (firstpass  > 1)) {
                        while(1)
                           SLEEP();
                    }       
        if (TIn < TMax) { //  Heat the Room
            if (TIn < TOut) { //Heat is available

                // Equillibrium
                // Changes in Temperature
                if ((sto > Zalpha) || (firstpass == 1)) { // The outside temperature is rising
                    // (99% confidence level).

                    // Open Door Slightly
                    //                    INTCONbits.GIE = 0;
#ifdef USE_TX_ESP8266 
                    TXout(processstat);
                    TXout((char *) "Signif. Outdoor Inc.");
#else
                    I2C_Out(ESP6288ModuleAddress,processstat);
                    if (firstpass)
                        I2C_Out(ESP6288ModuleAddress,(char *) "First Pass          ");
                    else
                        I2C_Out(ESP6288ModuleAddress,(char *) "Signif. Outdoor Inc.");
#endif
                    //                    INTCONbits.GIE = 1;
#ifdef POT
                    // Adjust Door Positions
                    DoorPosVolts = AtoD(0b000100); // RA4 
                    iDoorVoltage = DoorPosVolts * 1000.0;
                    oldiDoorVoltage = 0;
                    DoorLimit = 0;
#endif
                    rts = openDoors(0);
                    if (rts.level0 < 0) {
                        TXout(TopCallErrorPosition); // Position
                        TXout(openDoorsFailed); // 0 is success, -1 is failure 
                        buffer = formatString(rts.level0, 0, ", ");       
                        TXout(buffer);
                        buffer = formatString(rts.level1, 0, ".");
                        TXout(buffer);
                        if(I2CBusCollision) 
                            BusCollision(I2CBusCollision);
                        while(1) 
                            SLEEP();
                    }
                    if((firstpass <-3) || (firstpass  > 1)) {
                        while(1)
                           SLEEP();
                    }
                    //                    int dDoorCount;
                    //    
                    // //                   I2C_SetAddress ( MPU6050SlaveAddress);        // Tilt Sensor Address.    
                    //                      i = I2C_ReadMPU6050(ACCEL_X);
                    //                      if(i == -100)
                    //                        return -1;          // I2C ERROR
                    //                      accelerometer_x = (double) i / 16384.0;
                    ////                    accelerometer_z = (double) I2C_ReadMPU6050 (ACCEL_Z)/16384.0; // *9.80665; *** 
                    //                      i = 0;
                    //                       do {
                    //                            j = (double) I2C_ReadMPU6050(TEMPERATURE);
                    //                            if(j == -100)
                    //                                return -1;
                    //                            temperature = (double) j / 340.00  + 36.53;
                    //                            i++;
                    //                        } while((temperature == 36.53) && i < 10);
                    //                        if(i > 9)
                    //                            return -1;    // I2C ERROR                      
                    //                      
                    ////                    while((temperature = I2C_ReadMPU6050 (0x41)/340.00+36.53) == 36.53 ) {
                    ////                        temperature = 0.0;
                    ////                    };  //    Result = I2C_ReadMPU6050 (0x43);
                    ////                    gyro_x =  I2C_ReadMPU6050 (0x43)/131.0; 
                    //                        if(temperature > 20.0)
                    //                            maxDoorCount = 6;
                    //                        else
                    //                            maxDoorCount = 12;                        
                    //                    dDoorCount = 0;
                    //                    i = (double) I2C_ReadMPU6050(ACCEL_Z);
                    //                    if(i <= -100)
                    //                        return -1;
                    //                    double DoorInclination = (double) i / 16384.0;                   
                    ////                    double DoorInclination = (double) I2C_ReadMPU6050 (ACCEL_Z)/16384.0;
                    //                    double OldDoorInclination = -1.0;                    
                    ////                    while ((iDoorVoltage > OpenedDoorVoltage) && (DoorLimit < 5)) { // Not completely opened
                    //                    while((DoorInclination < OpenedDoorInclination ) && dDoorCount < maxDoorCount) {
                    ////                        if((oldiDoorVoltage - iDoorVoltage) < 10)
                    ////                            DoorLimit++;
                    ////                        I2C_SetAddress ( ESP6288ModuleAddress);
                    //#ifdef USE_TX_ESP8266
                    //                        TXout(motorstat);
                    //                        TXout((char *) "Opening");                       
                    //#else                      
                    //                        I2C_Out(motorstat);
                    //                        I2C_Out((char *) "Opening");
                    //#endif
                    //                        DoorMoved = 0;
                    //                        T1Count = 2; // add 1 for sub degrees(next statement)
                    //                        LATAbits.LATA2 = 1; // open the doors  
                    //                        T1CONbits.ON = 1; // Turn on the timer
                    //                        while (!DoorMoved) {
                    //                        }; // Timer1 will turn off both doors when time is ended
                    //                        T1CONbits.ON = 0; // Turn off the timer   
                    //                        // Print the New Door Voltage
                    // //                       I2C_SetAddress ( MPU6050SlaveAddress);
                    //                        do {
                    //                            i = I2C_ReadMPU6050(GYRO_Z);
                    //                            if(i <= -100)
                    //                                return -1;
                    //                            gyro_x = (double) i/131.0;
                    //                        } while (gyro_x > 1.000);                        
                    ////                        unsigned int toolong;
                    ////                        while ((gyro_x = I2C_ReadMPU6050 (GYRO_Z)/131.0 > 1.000) && (toolong < 0xFE)) { __delay_ms(1); toolong++; }  // wait til it stops shaking 
                    //                        // get the new inclination
                    //                        OldDoorInclination = DoorInclination;
                    //                        i = I2C_ReadMPU6050(ACCEL_Z);
                    //                        if(i == -100)
                    //                            return -1;
                    //                        DoorInclination = (double) i / 16384.0;                         
                    ////                        DoorInclination = 0.0;
                    ////                        for (i = 0; i < 10; i++)
                    ////                            DoorInclination += (double) (I2C_ReadMPU6050 (ACCEL_X)/16384.0);
                    ////                        DoorInclination /= 10.0;
                    //                        // If the change is not significant
                    //                         if(OldDoorInclination <= DoorInclination) //* 0.0302) && OldDoorInclination > DoorInclination) {
                    ////                             if(OldDoorInclination > DoorInclination)
                    //                                dDoorCount++; 
                    //                        }      
                    //                       
                    //                        
                    ////                        DoorPosVolts = AtoD(0b000100); // Get new door position
                    ////                        iDoorVoltage = DoorPosVolts * 1000.0;
                    ////                        int oldiDoorVoltage = iDoorVoltage;
                    // //                       I2C_SetAddress ( ESP6288ModuleAddress);        // Tilt Sensor Address  
                    //#ifdef USE_TX_ESP8266                        
                    //                        TXout(doorOFC); // Position cursor for status message
                    //                        buffer = formatString(DoorInclination, 3, (char *) "g ");
                    //                        TXout(buffer);
                    //#else
                    //                        I2C_Out(doorOFC); // Position cursor for status message
                    //                        buffer = formatString(DoorInclination, 3, (char *) "g");
                    //                        I2C_Out(buffer);
                    //#endif
                    ////                        INTCONbits.GIE = 1;                      
                    //                
                    // //                   I2C_SetAddress ( ESP6288ModuleAddress ); 
                    // #ifdef USE_TX_ESP8266 
                    //                    TXout(motorstat);
                    //                    TXout((char *) "Stopped");
                    //#else                    //                   
                    //                    I2C_Out(motorstat);
                    //                    I2C_Out((char *) "Stopped");
                    //#endif
                    if((firstpass <-3) || (firstpass  > 1)) {
                        while(1)
                           SLEEP();
                    }
                } else if ((sto < 0.0) && (sto < (-1.0 * Zalpha)) && sto != -100.0) { // The outside temperature is falling
                    // (99% confidence level).   
                    //                   I2C_SetAddress ( ESP6288ModuleAddress );                    
                    //                    INTCONbits.GIE = 0;
#ifdef USE_TX_ESP8266                    
                    TXout(processstat);
                    TXout((char *) "Signif. Outdoor Dec. ");
#else                    
                    I2C_Out(ESP6288ModuleAddress,processstat);
                    I2C_Out(ESP6288ModuleAddress,(char *) "Signif. Outdoor Dec.");
#endif
                    //                    INTCONbits.GIE = 1;
                    if((firstpass <-3) || (firstpass  > 1)) {
                        while(1)
                           SLEEP();
                    }
#ifdef _OLD_CODE_1  
                    DoorPosVolts = AtoD(0b000100);
                    iDoorVoltage = DoorPosVolts * 1000.0;
                    if (iDoorVoltage < ClosedDoorVoltage) {
                        TXout(motorstat);
                        TXout((char *) "Closing");
                        DoorMoved = 0;
                        T1Count = 20; // add 1 for sub degrees(next statement)
                        LATCbits.LATC0 = 1; // close the doors  
                        T1CONbits.ON = 1; // Turn on the timer
                        while (!DoorMoved) {
                        }; // Timer1 will turn off both doors when time is ended
                        T1CONbits.ON = 0; // Turn off the timer                       
                        DoorPosVolts = AtoD(0b000100); // Get new door position
                        iDoorVoltage = DoorPosVolts * 1000.0;
                        TXout(doorOFC); // Position cursor for status message
                        buffer = formatString(DoorPosVolts, 3, (char *) "V");
                        INTCONbits.GIE = 0;
                        TXout(buffer);
                        INTCONbits.GIE = 1;
                    }
                    TXout(motorstat);
                    TXout((char *) "Stopped");
#endif
                }
            } else { // Heat is not available (In > TOut & TOut < TIn)  (Tin < Tout)
                //               I2C_SetAddress ( ESP6288ModuleAddress );               
                //                INTCONbits.GIE=0;
#ifdef USE_TX_ESP8266                 
                TXout(processstat);
                TXout((char *) "No Heat to Heat With");
#else
                I2C_Out(ESP6288ModuleAddress,processstat);
                I2C_Out(ESP6288ModuleAddress,(char *) "No Heat to Heat with");
#endif
                     if((firstpass <-3) || (firstpass  > 1)) {
                        while(1)
                           SLEEP();
                    }               
                rts = closeDoors(0);
                if(rts.level0)
                    return -1;
                
                ////                INTCONbits.GIE=1; 
                //  //              I2C_SetAddress ( MPU6050SlaveAddress);        // Tilt Sensor Address.
                //                i = I2C_ReadMPU6050(ACCEL_Z);
                //                if(i == -100)
                //                    return -1;
                //                DoorInclination = (double) i / 16384.0;                
                //                DoorInclination = (double) I2C_ReadMPU6050 (ACCEL_Z)/16384.0;
                ////                DoorPosVolts = AtoD(0b000100);  
                ////                iDoorVoltage = DoorPosVolts * 1000.0;
                ////                while ((iDoorVoltage < ClosedDoorVoltage ) && (fabs(stv) > fabs(Zalpha)) ){
                //                while ((DoorInclination > (ClosedDoorInclination + 0.02) ) && (fabs(stv) > fabs(Zalpha)) ){                    
                ////                    I2C_SetAddress ( ESP6288ModuleAddress );
                //#ifdef USE_TX_ESP8266 
                //                    TXout(motorstat);
                //                    TXout((char *) "Closing");
                //#else                    
                //                    I2C_Out(motorstat);
                //                    I2C_Out((char *) "Closing"); 
                //#endif
                //                    DoorMoved = 0;
                //                    T1Count = 2; // add 1 for sub degrees(next statement)
                //                    LATCbits.LATC0 = 1;          // close the doors  
                //                    T1CONbits.ON = 1;       // Turn on the timer
                //                    while(!DoorMoved) {};   // Timer1 will turn off both doors when time is ended
                //                    T1CONbits.ON = 0;    // Turn off the timer
                // //                   I2C_SetAddress ( MPU6050SlaveAddress);
                //                     unsigned int toolong;
                //                    while ((gyro_x = I2C_ReadMPU6050 (GYRO_Z)/131.0 > 1.000) && (toolong < 0xFE)) { __delay_ms(1); toolong++; }  // wait til it stops shaking 
                //                    // get the new inclination
                //                    OldDoorInclination = DoorInclination;
                //                    i = I2C_ReadMPU6050(ACCEL_Z);
                //                    if(i == -100)
                //                        return -1;
                //                    DoorInclination = (double) i / 16384.0;                     
                //                    
                ////                    DoorInclination = 0.0;
                ////                    for (i = 0; i < 10; i++)
                ////                        DoorInclination += (double) I2C_ReadMPU6050 (0x3F)/16384.0;
                ////                    DoorInclination /= 10.0;                   
                ////                    DoorPosVolts = AtoD(0b000100); // Get new door position
                ////                    iDoorVoltage = DoorPosVolts * 1000.0;
                ////                    I2C_SetAddress ( ESP6288ModuleAddress );
                //
                //                    buffer = formatString(DoorInclination,3,(char *) "g");
                ////                    INTCONbits.GIE=0; 
                //#ifdef USE_TX_ESP8266                    
                //                    TXout(doorOFC);       // Position cursor for status message                    
                //                    TXout(buffer); 
                //#else
                //                    I2C_Out(doorOFC);       // Position cursor for status message                    
                //                    I2C_Out(buffer); 
                //#endif                    
                ////                    INTCONbits.GIE=1; 
                //                }
                ////                I2C_SetAddress ( ESP6288ModuleAddress );
                // #ifdef USE_TX_ESP8266               
                //                TXout(motorstat);
                //                TXout((char *) "Stopped"); 
                //#else
                //                I2C_Out(motorstat);
                //                I2C_Out((char *) "Stopped");                
                //#endif
            }


        } else { // TIn > TMax  Stop Heating the Room
            ////            INTCONbits.GIE=0;
            //            I2C_Out(processstat);
            //            TXout((char *) "TIn > TMax            ");
            ////            INTCONbits.GIE=1;
            //            DoorPosVolts = AtoD(0b000100);  
            //            iDoorVoltage = DoorPosVolts * 1000.0; 
            //           I2C_SetAddress ( MPU6050SlaveAddress);        // Tilt Sensor Address.
//            DoorInclination = (double) I2C_ReadMPU6050(ACCEL_Z) / 16384.0;
            if (TIn > TOut) { // TOut will cool the room
                //                   I2C_SetAddress ( 0 );
                //                    INTCONbits.GIE=0;
#ifdef USE_TX_ESP8266                     
                TXout(processstat);
                TXout((char *) "TIn > TMax; Cool Room ");
#else
                I2C_Out(ESP6288ModuleAddress,processstat);
                I2C_Out(ESP6288ModuleAddress,(char *) "TIn > TMax; Cool Room ");
#endif
                rts = openDoors(0);
                if(rts.level0)
                    return -1;
                ////                    INTCONbits.GIE=1;               
                ////                     while (iDoorVoltage > OpenedDoorVoltage) { // Not completely opened
                //                    while(DoorInclination > OpenedDoorInclination) { // Not completely opened)
                // //                       I2C_SetAddress (ESP6288ModuleAddress);
                //#ifdef USE_TX_ESP8266                         
                //                        TXout(motorstat);
                //                        TXout((char *) "Opening");
                //#else
                //                        I2C_Out(motorstat);
                //                        I2C_Out((char *) "Opening");
                //#endif                        
                //                        DoorMoved = 0;
                //                        T1Count = 2; // add 1 for sub degrees(next statement)
                //                        LATAbits.LATA2 = 1; // open the doors  
                //                        T1CONbits.ON = 1; // Turn on the timer
                //                        while (!DoorMoved) {
                //                        }; // Timer1 will turn off both doors when time is ended
                //                        T1CONbits.ON = 0; // Turn off the timer   
                //                        // Print the New Door Voltage
                // //                       I2C_SetAddress ( MPU6050SlaveAddress);        // Tilt Sensor Address.
                //                        DoorInclination = (double) I2C_ReadMPU6050 (0x3F)/16384.0;                       
                ////                        DoorPosVolts = AtoD(0b000100); // Get new door position
                ////                        iDoorVoltage = DoorPosVolts * 1000.0;
                ////                        I2C_SetAddress (ESP6288ModuleAddress);
                //                        
                //                        buffer = formatString(DoorInclination, 3, (char *) "V");
                ////                        INTCONbits.GIE = 0;
                //#ifdef USE_TX_ESP8266 
                //                        TXout(doorOFC); // Position cursor for status message                        
                //                        TXout(buffer);
                //#else                        
                //                        I2C_Out(doorOFC); // Position cursor for status message                        
                //                        I2C_Out(buffer);
                //#endif
                ////                        INTCONbits.GIE = 1;
                //                    }            
            } else { // TOut  > Tin and close the doors

                // if stv is positive and greater than Z alpha the closing of the door produced a significant difference in voltage
                // so, if stv is not greater than Z the door movement produced no significant change and the door moved very very little
                //               I2C_SetAddress (ESP6288ModuleAddress);
                //                INTCONbits.GIE=0;
#ifdef USE_TX_ESP8266                
                TXout(processstat);
                TXout((char *) "TIn > TMax; Stop Heat ");
#else
                I2C_Out(ESP6288ModuleAddress,processstat);
                I2C_Out(ESP6288ModuleAddress,(char *) "TIn > TMax; Stop Heat ");
#endif  
                rts = closeDoors(0);
                if(rts.level0)
                    return -1;
                ////                INTCONbits.GIE=1;               
                ////                while ((iDoorVoltage < ClosedDoorVoltage) && (fabs(stv) > fabs(Zalpha))) {
                //                while ((DoorInclination > ClosedDoorInclination) && (fabs(stv) > fabs(Zalpha))) {
                //#ifdef USE_TX_ESP8266                     
                //                    TXout(motorstat);
                //                    TXout((char *) "Closing");
                //#else
                //                    I2C_Out(motorstat);
                //                    I2C_Out((char *) "Closing"); 
                //#endif                    
                //                    DoorMoved = 0;
                //                    T1Count = 2; // add 1 for sub degrees(next statement)
                //                    LATCbits.LATC0 = 1;          // close the doors  
                //                    T1CONbits.ON = 1;       // Turn on the timer
                //                    while(!DoorMoved) {};   // Timer1 will turn off both doors when time is ended
                //                    T1CONbits.ON = 0;    // Turn off the timer  
                // //                   I2C_SetAddress ( MPU6050SlaveAddress);        // Tilt Sensor Address.
                //                    DoorInclination = (double) I2C_ReadMPU6050 (0x3F)/16384.0;                   
                ////                    DoorPosVolts = AtoD(0b000100); // Get new door position
                ////                    iDoorVoltage = DoorPosVolts * 1000.0;
                // //                   I2C_SetAddress (ESP6288ModuleAddress);
                //                    buffer = formatString(DoorInclination,3,(char *) "V");                   
                ////                    INTCONbits.GIE=0;
                //#ifdef USE_TX_ESP8266                    
                //                    TXout(doorOFC);       // Position cursor for status message  
                //                    TXout(buffer);
                //#else
                //                    I2C_Out(doorOFC);       // Position cursor for status message  
                //                    I2C_Out(buffer);
                //#endif                    
                ////                    INTCONbits.GIE=1; 
                //                } 

                //            }
                //           I2C_SetAddress (ESP6288ModuleAddress);
                //            INTCONbits.GIE=0;
#ifdef USE_TX_ESP8266              
                TXout(motorstat);
                TXout((char *) "Stopped");
#else
                I2C_Out(ESP6288ModuleAddress,motorstat);
                I2C_Out(ESP6288ModuleAddress,(char *) "Stopped");
#endif
                //            INTCONbits.GIE=1;
            }

#ifdef LOOKS_LIKE_A_PATCH               
            // It is not as hot inside as it should be
            if ((TOut < TIn) && (dT < 0.4) && (dT > -0.4)) {
                INTCONbits.GIE = 0;
                TXout(processstat);
                TXout((char *) "TOut < Tin + dT        ");
                INTCONbits.GIE = 1;
                //                    if(DOORS_NOT_CLOSED != 0) { // The door can still close more
                //                         PORTCbits.RC0 =1;      // Turn on the motor to close the doors
                ////                         TXout(motorstat);
                ////                         TXout((char *) "Stopped"); 
                //                    } 
                //                    DoorPercentOpen = 0.0*100.0;
                //
                //                    buffer = ftoa((float)(DoorPercentOpen),&status);
                //                    decimalPoint = strchr(buffer,decPt);
                //                    decimalPoint++;
                //                    decimalPoint++;
                //                    *decimalPoint++ = '%';
                //                    *decimalPoint++ = ' ';
                //                    *decimalPoint++ = ' ';                
                //                    *decimalPoint = '\0'; 
                //                    INTCONbits.GIE=0;
                //                    TXout(doorPerOpen);                   
                //                    TXout(buffer);
                //                    INTCONbits.GIE=1;
            }
#endif
            // If the student t shows a significant change the doors should be
            // moved, but when the student t does not and in the same direction
            // for numerous tests then the following code should catch that.


            // When there is a significant change in the outside temperature
            // at the 99% confidence level, the doors open.


            // Maybe the previous temperature value being compared to the current
            // temperature value should stay the same until a significant change
            // occurs, then I would not miss a temperature change.  Then I can
            // use the difference in temperature to scale the doors movement.
            // Outside Temperature changes:



            // The inside temperature could change too.                
            if (sti > Zalpha) { // The inside temperature is rising
                //           I2C_SetAddress (ESP6288ModuleAddress);
                //            INTCONbits.GIE=0;
#ifdef USE_TX_ESP8266           
                TXout(processstat);
                TXout((char *) "T inside Rising     ");
#else      
                I2C_Out(ESP6288ModuleAddress,processstat);
                I2C_Out(ESP6288ModuleAddress,(char *) "T inside Rising    ");
#endif            
                //            INTCONbits.GIE=1;           
                // it is still not high enough until it reaches Tmax   
            } else if ((sti < 0.0) && (sti < (-1.0 * Zalpha)) && sti != -100.0) { // The inside temperature is falling
                // the outside temperature might be high enough to open the doors
                //          I2C_SetAddress (ESP6288ModuleAddress);
                //            INTCONbits.GIE=0;
#ifdef USE_TX_ESP8266             
                TXout(processstat);
                TXout((char *) "T inside Falling    ");
#else
                I2C_Out(ESP6288ModuleAddress,processstat);
                I2C_Out(ESP6288ModuleAddress,(char *) "T inside Falling    ");
#endif            
                //            INTCONbits.GIE=1;
                //                    if (dT < -1.0) {  // the outside is hotter than the outside  
                //                        INTCONbits.GIE=0;
                //                        TXout(processstat);
                //                        TXout((char *) "T inside falling    ");
                //                        INTCONbits.GIE=1;
                //                        DoorPosVolts = AtoD(0b000100);  
                //                        iDoorVoltage = DoorPosVolts * 1000.0;
                //                        TXout(motorstat);
                //                        TXout((char *) "Opening"); 
                //                        DoorLimit = iDoorVoltage - 60;
                //                        if(DoorLimit <= OpenedDoorVoltage)
                //                            DoorLimit = OpenedDoorVoltage;
                //                        while(iDoorVoltage < DoorLimit) {  // open the doors 10%  0.06*1000/0.6*1000 * 100% = 10%
                //                            DoorMoved = 0;
                //                            T1Count = 1; // add 1 for sub degrees(next statement)
                //                            LATAbits.LATA2 = 1;          // close the doors  
                //                            T1CONbits.ON = 1;       // Turn on the timer
                //                            while(!DoorMoved) {};   // Timer1 will turn off both doors when time is ended
                //                            T1CONbits.ON = 0;    // Turn off the timer                       
                //                            DoorPosVolts = AtoD(0b000100); // Get new door position
                //                            iDoorVoltage = DoorPosVolts * 1000.0;
                //                            TXout(doorOFC);       // Position cursor for status message
                //                            buffer = formatString(DoorPosVolts,3,(char *) "V");
                ////                            buffer = ftoa((float)(DoorPosVolts),&status);
                ////                            decimalPoint = strchr(buffer,decPt);
                ////                            decimalPoint++;
                ////                            decimalPoint++;
                ////                            decimalPoint++;
                ////                            decimalPoint++;              
                ////                            *decimalPoint++ = 'V'; 
                ////                            *decimalPoint = '\0';
                //                            INTCONbits.GIE=0;    
                //        //                    TXout(DoorPosVolts);                
                //                            TXout(buffer);   
                //                            INTCONbits.GIE=1; 
                //                        } 
                //                    TXout(motorstat);
                //                    TXout((char *) "Stopped");            
                //                    }                     
                //                }
                //            }
            }

        }
            //
            // Status Messages for Each Pass
            //
            //        DoorPosVolts = AtoD(0b000100);
            //       I2C_SetAddress ( MPU6050SlaveAddress);        // Tilt Sensor Address.
//        Result =  I2C_Read(MPU6050SlaveAddress,ACCEL_Z, 2);
                    if((firstpass <-3) || (firstpass  > 1)) {
                        while(1)
                           SLEEP();
                    }        
       Result = I2C_In(MPU6050SlaveAddress, ACCEL_Z, 2);
       if(Result.level0)
                return Result.level0;
       DoorInclination = (double) Result.level1 / 16384.0;         
//            DoorInclination = (double) I2C_ReadMPU6050(ACCEL_Z) / 16384.0;
            //       I2C_SetAddress (ESP6288ModuleAddress);     
        buffer = formatString(DoorInclination, 3, (char *) "G");
        if(DoorInclination < ClosedDoorInclination)
            DoorInclination = ClosedDoorInclination;
        if(DoorInclination == ClosedDoorInclination)
            DoorDegree = 0.0;
        else if(DoorInclination < 0.00000)
            DoorDegree = (fabs(ClosedDoorInclination)/(fabs(ClosedDoorInclination)+OpenedDoorInclination)* 100.0) * (1-(DoorInclination/ClosedDoorInclination)) ; //
        else
            DoorDegree = ((DoorInclination/OpenedDoorInclination) + fabs(ClosedDoorInclination)/(OpenedDoorInclination + fabs(ClosedDoorInclination))) * 100.0;
 

            //        INTCONbits.GIE=0;        
#ifdef USE_TX_ESP8266        
            TXout(doorOFC); // Position cursor for status message  
            TXout(buffer);
            // Update Display - Percentage Door Opened
            //        DoorPosVolts = AtoD(0b000100); // Get new door position
            //        DoorDegree = ((double) (ClosedDoorVoltage)/1000.0-DoorPosVolts);
            //        DoorDegree = DoorDegree/((double)(ClosedDoorVoltage)/1000.0-(double)(OpenedDoorVoltage)/1000.0);
            //        DoorDegree = DoorDegree*100.0;

            TXout(doorPerOpen); // Position cursor for status message
            // format DoorDegree with 1 significant figure behind the decimal point
            buffer = formatString(DoorDegree, 1, (char *) "% ");
            //            buffer = ftoa((float)(DoorDegree)),&status);

            //            decimalPoint = strchr(buffer,decPt);
            //            decimalPoint++;
            //            decimalPoint++;        
            //            *decimalPoint++ = '%'; 
            //            *decimalPoint++ = ' ';
            //            *decimalPoint = '\0';

            TXout(buffer);
#else
            I2C_Out(ESP6288ModuleAddress,doorOFC); // Position cursor for status message  
            I2C_Out(ESP6288ModuleAddress,buffer);
            // Update Display - Percentage Door Opened
            //        DoorPosVolts = AtoD(0b000100); // Get new door position
            //        DoorDegree = ((double) (ClosedDoorVoltage)/1000.0-DoorPosVolts);
            //        DoorDegree = DoorDegree/((double)(ClosedDoorVoltage)/1000.0-(double)(OpenedDoorVoltage)/1000.0);
            //        DoorDegree = DoorDegree*100.0;

            I2C_Out(ESP6288ModuleAddress,doorPerOpen); // Position cursor for status message
            // format DoorDegree with 1 significant figure behind the decimal point
            buffer = formatString(DoorDegree, 1, (char *) "% ");
            //            buffer = ftoa((float)(DoorDegree)),&status);

            //            decimalPoint = strchr(buffer,decPt);
            //            decimalPoint++;
            //            decimalPoint++;        
            //            *decimalPoint++ = '%'; 
            //            *decimalPoint++ = ' ';
            //            *decimalPoint = '\0';

            I2C_Out(ESP6288ModuleAddress,buffer);
#endif        

            if((firstpass <-3) || (firstpass  > 1)) {
                while(1)
                    SLEEP();
            }
#ifdef USE_TX_ESP8266 
            TXout("\033[10;1H");
            if (firstpass == 1 || firstpass == 0) {
                TXout("-");
                firstpass = -1;
            } else if (firstpass == -1) {
                TXout("\\");
                firstpass = -2;
            } else if (firstpass == -2) {
                TXout("|");
                firstpass = -3;
            } else if (firstpass == -3) {
                TXout("/");
                firstpass = 0;
            }


//            if (iteration == 250) {
//            // Data for Remote Computer
//                TXout("# "); // TIn, TOut, TMax, ClosedDoorVoltage, dDoorVoltage, DoorPosVolts, sto, sti. Zalpha]
//                //   Double, double, double, int,         int                ,double     double, double, double    
//                TXout(formatString(TIn, 1, (char *) " ")); // double TIn
//                TXout(formatString(TOut, 1, (char *) " ")); // double TOut
//                TXout(formatString(TMax, 1, (char *) " ")); // double TMax
//                TXout(formatString((double) (ClosedDoorInclination), 0, (char *) " "));
//                TXout(formatString((double) (OpenedDoorInclination), 0, (char *) " "));
//                TXout(formatString(DoorInclination, 3, " ")); // double DoorPosVolts
//                TXout(formatString(sti, 6, (char *) " ")); // double Student t inside temperature
//                TXout(formatString(sto, 6, (char *) " ")); // double Student t outside temperature
//                TXout(formatString(Zalpha, 6, (char *) " ")); // double critical value
//                TXout("$ ");
//            }
#else
            I2C_Out(ESP6288ModuleAddress,"\033[10;1H\0");

            // Data for Remote Computer
            I2C_Out(ESP6288ModuleAddress,"# "); // TIn, TOut, TMax, ClosedDoorVoltage, dDoorVoltage, DoorPosVolts, sto, sti. Zalpha]
            //   Double, double, double, int,         int                ,double     double, double, double    
            I2C_Out(ESP6288ModuleAddress,formatString(TIn, 1, (char *) " ")); // double TIn
            I2C_Out(ESP6288ModuleAddress,formatString(TOut, 1, (char *) " ")); // double TOut
            I2C_Out(ESP6288ModuleAddress,formatString(TMax, 1, (char *) " ")); // double TMax
            I2C_Out(ESP6288ModuleAddress,formatString((double) (ClosedDoorInclination), 0, (char *) " "));
            I2C_Out(ESP6288ModuleAddress,formatString((double) (OpenedDoorInclination), 0, (char *) " "));
            I2C_Out(ESP6288ModuleAddress,formatString(DoorInclination, 3, " ")); // double DoorPosVolts
            I2C_Out(ESP6288ModuleAddress,formatString(sti, 6, (char *) " ")); // double Student t inside temperature
            I2C_Out(ESP6288ModuleAddress,formatString(sto, 6, (char *) " ")); // double Student t outside temperature
            I2C_Out(ESP6288ModuleAddress,formatString(Zalpha, 6, (char *) " ")); // double critical value
            I2C_Out(ESP6288ModuleAddress,"$ ");
#endif
            //        INTCONbits.GIE=1;
            //        firstpass = 0;


    }
    return 0;
}