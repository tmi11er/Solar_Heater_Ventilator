#include <stdlib.h>
#include <math.h>
#include "definitions.h"


void setupAtoD()
{
#ifdef pic688
    ADCON0bits.ADFM = 1;        // Right Justified output
    ADCON0bits.VCFG = 0;        // Vdd as comparison voltage
    ADCON1bits.ADCS = 5;        // FOSC/16 2.0 us clock period
#else
    ADREF = 0;                  // AtoD V+ is Vdd, V- is Vss
    ADCON0bits.CS = 0;          // FOSC source divided by ADCLK
    ADCLK = 0b111111;             // 4uS period of Tad
    ADCPCON0bits.CPON = 1;      // Charge pump on, CPRDY set when finished
    ADCON0bits.FM = 1;           // Right justified
    ADACQH = 0;
    ADACQL = 81;                 // 81 Tcy periods between automatic conversions
    ADCON2bits.MD = 2;           // Average Mode
    ADCON3bits.SOI = 1;
    ADCON0bits.CONT = 1;
    ADRPT = 47;                 // Number of repetitions 
    ADCNT = 0;                  // Threshold check and divide after each conversion.
    ADCON2bits.CRS = 0;         // Shift after every conversion.
    ADCON3bits.TMD = 7;         // Interrupt at the end of conversion averaged
    ADCON0bits.ADON = 1;       // Turn On  
//    double InternalTemp = AtoD(0b111100);         // Temperature
#endif
}
