#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <xc.h>

//March 23, 2019
extern int i;
extern int status;

char *formatString (double value, int behindTheDecimalNumbers, char * additionalCharacters)
{
//    char *buffer;
//    static char BBUuff[10];
//    buffer = &BBUuff[0];
    int length;
    char *Obuffer;
    static char OHbuffer[256];
//    status = 0;
//    for (int a = 0; a < 10; a++)
//        OHbuffer[a] = 0;
    Obuffer = &OHbuffer[0];
//    unsigned char *decimalPoint;
//    char decPt = '.'; 
if((value > 1000.0) || (value < -1000.0)) {
            while(1)
                SLEEP(1);
        }    
    if(behindTheDecimalNumbers == 0)
        length = sprintf(OHbuffer,"%+.0f",value);
    else if(behindTheDecimalNumbers == 1)
        length = sprintf(OHbuffer, "%+.1f",value);        
    else if(behindTheDecimalNumbers == 2)
        length = sprintf(OHbuffer, "%+.2f",value); //  ftoa((float)(value),&status);
    else if (behindTheDecimalNumbers == 3)
        length = sprintf(OHbuffer, "%+.3f",value);
    else if (behindTheDecimalNumbers == 3)
        length = sprintf(OHbuffer, "%+.4f",value);
    else if (behindTheDecimalNumbers == 3)
        length = sprintf(OHbuffer, "%+.5f",value);
     else if (behindTheDecimalNumbers == 3)
        length = sprintf(OHbuffer, "%+.6f",value); 
     else 
        length = sprintf(OHbuffer, "%+f",value); 
    
//    if( value > 0)
//        *Obuffer++ = ' ';
//    memcpy(Obuffer,buffer, (size_t) 10);
//    decimalPoint = strchr(Obuffer,decPt);
//    decimalPoint++;
//    for ( i =0; i < behindTheDecimalNumbers; i++)
//            decimalPoint++;  
//    while(*additionalCharacters != 0)
//        *decimalPoint++ = *additionalCharacters++;
//    *decimalPoint = '\0';

    while(*additionalCharacters != 0)
        OHbuffer[length++] = *additionalCharacters++;
    OHbuffer[length] = 0;
    return Obuffer;
}
