#include <xc.h>
#include <pic16f18426.h>
#include "definitions.h"

char I2CErrorPosition[] = "\033[11;2H"; 

void BusCollision (int i) {
    
 //   char I2CErrorPosition[] = "\033[11;2H";    
    char I2CBusText[] = "I2C Bus Collision: ";
    if(I2CBusCollision) {
        TXout(I2CErrorPosition);
        TXout(I2CBusText);
        buffer = formatString(i, 0, ".");
        TXout(buffer);
    }
    return;
}
