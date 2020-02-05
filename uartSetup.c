
#include <stdlib.h>
#include <math.h>
#include <pic16f18426.h>
#include "definitions.h"

void uartSetup()
{

    int j;
    
    RX1DTPPS = 0x04;      // A4 - Pin 3 TX
//    TX1DTPPS  = 0x05;     // A5   Pin 2 RX
    
//    RX1DTPPS = 0x14;        // Peripheral Pin Select RX1 PORT C RC4 Input
//    TX1CKPPS = 0x15;
    RA4PPS = 0x0;       // DT1
    RA5PPS = 0xf;   // TX
//    RC5PPS = 0xF;
    ANSELAbits.ANSA4 = 0; // ANSELCbits.ANSC4 = 0;       // Non Analog
    ANSELAbits.ANSA5 = 0;   //    ANSELCbits.ANSC5 = 0;       // Non Analog
    TRISAbits.TRISA4 = 1; //    TRISCbits.TRISC5 = 0;       // Port Digital RC5 Input
    TRISAbits.TRISA5 = 0;   
//    TRISCbits.TRISC4 = 1;       // Port RC4 Output
    TXSTAbits.BRGH = 1;         //  BRGH high speed
    BAUDCTLbits.BRG16 =1;       // 16 bit baud rate generator   
//    SPBRGH = 0x6;
//    SPBRG = 6666;              // 1200 baud   (SYNC=0, BRGH = 1, BRG16=1) @ 32MHz
//    SPBRGH = 0xD;
//    SP1BRGH = 1;
 //   SP1BRGL = 1;
//    SP1BRG = 6666L;     // 1200 baud
    SP1BRG = 68L; // 115.200k // 6666L; 1200 baud    // 115200 at 32MHz
//    SP1BRG = 32L;   // 250000baud
//    SP1BRG = 16L; // 500000baud
    // SP1BRG = 8L; // 1M baud
    // SP1BRG = 4L; // 2M baud

    TXSTAbits.SYNC = 0;         // Asynchronous mode
 
    TXSTAbits.TX9=0;           // 8 bits no parity

    PIE3bits.RC1IE = 1;         // Enable Receiver interrupts
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    PIE3bits.RC1IE = 1;
 
    TXSTAbits.TXEN = 1;         // Transmitter on,
    RCSTAbits.CREN = 1;       // Continuous receive
    BAUDCTLbits.SCKP = 1;       // do invert transmitted bits
    RCSTAbits.SPEN = 1;        // Serial Ports Enabled   
    PIR3bits.RC1IF = 0;         // Clear the interrupt flag
    PIR3bits.TX1IF = 0;
   
//    for (i = 1; i < 300; i++) {      
//        TXREG = 'T';
//        NOP();
//        while(!PIR1bits.TXIF) {};
//        TXREG = 'M';
//        NOP();
//        while(!PIR1bits.TXIF) {};       
//   }
    return;
}

