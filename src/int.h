#ifndef __INT_H
#define __INT_H
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
// #include <pic16f690.h>
#include "var.h"
#include "func.h"

// #pragma interrupt_level 1
void __interrupt() rutina_de_interrupciones(void) {
	asm("clrwdt");
	if (C1IF) {
		mtras=TMR1; TMR1=6; band.mtras=1; C1IF=0;
	}	
	if (TMR2IF) {
		band.intTmr2=1; TMR2IF=0;
	}	
	if (RCIF) {
		DATARX=RCREG; band.nDormir=1; band.cmdRcb=1; ++rxCont; RCIF=0;
	}
}

#endif