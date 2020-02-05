#include <xc.h>
#include <pic16f18426.h>
#include "definitions.h"


int resetCause()
{
    
//    STATUS STATUS
//    Bit     4       3       2       1       0
//            ~TO     ~PD     Z       DC      C
//            
//            
//    PCON0  Power Control Register 0
//            Bit 7       6       5       4       3       2       1       0 
//                STKOVF  STKUNF  ~WDTWV  ~RWDT   ~RMCLR  ~RI     ~POR    ~BOR
//            
//    PCON1 Power Control Register 1
//            Bit 1
//            ~MEMV 

//    if((STATUS & 0x18) == 0x18) {   // Not a time out, not a power down
//        if (((PCON0 & 0x3E) == 0x3E) && (PCON1 & 0x2) )// POWER ON RESET &  not a memory violation
//                return 0;       // A  power on reset
//        if ((PCON0 & 0x3E) == 0x3D) && !(PCON1 & 0x2)) {    // A memory violation
//        } else if (PCON0 & 0x3D == 0x3D) {
//        }
//    }
}
