#include "int.h"
void interrupt rutina_de_interrupciones(void) {
	asm("clrwdt");
	if (C1IF) {
		//if (C1OUT) RA1=1; else RA1=0;
		TMR1ON=0; mtras=TMR1;  TMR1=40; TMR1ON=1; // TMR1=20, por que cada conteo de TMR1=500ns con PRE TMR1 en 0
		mtras-=40; ++cntFl; band.mtras=1; 
		
		C1IF=0;
	}
	else if (TMR2IF) {
		EN_LED=1; EN_SOUND=0; TMR2ON=0; TMR2IF=0; 
	}
}