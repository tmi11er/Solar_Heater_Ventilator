#include <stdlib.h>
#include <math.h>
#include "definitions.h"

void setupIOC()
{
#ifdef DOOR_SWITCH SENSORS
    // This is not used since the varibale resistor door sensor was installed.
    PIE0bits.IOCIE = 1;     // Enable interrupts from the module
    // Enable the interrupt detectors
//    IOCANbits.IOCAN4 = 1;       // Change from 1 to 0, switch is closed, door in contact
    IOCANbits.IOCAN5 = 1;       //  Upper door switch, doors are completel closed.
//    IOCAPbits.IOCAP4 = 1;
    IOCAPbits.IOCAP5 = 1;       // Positive edges
#endif
}
