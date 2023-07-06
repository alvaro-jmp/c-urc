#ifndef __FUNC_H
#define __FUNC_H
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
// #include <pic16f690.h>
#include "var.h"

#define _LOOPINF while(1);
#define _CLRWDT asm("clrwdt");
#define _SLEEP asm("sleep");

void printUARTbyte(unsigned char* pByteUartToTx) {	
	TXREG=*pByteUartToTx; while(!TRMT);
}

unsigned char printSPIbyte(unsigned char* pByteSpiToSdo) {
	SSPIF=0; SSPBUF=*pByteSpiToSdo; while (!SSPIF); return SSPBUF;	
}

void tmp20msTMR2(unsigned char pActvInt) {
	if (pActvInt) {
		TMR2IE=0; TMR2ON=0; TMR2IF=0; TMR2=0; PR2=_TMP_TMR2_20MS; T2CONbits.TOUTPS=0b1111; T2CONbits.T2CKPS=0b10;
		TMR2IE=1; TMR2ON=1;
	}	
	else {
		TMR2IE=0; TMR2ON=0; TMR2IF=0; TMR2=0; PR2=_TMP_TMR2_20MS; T2CONbits.TOUTPS=0b1111; T2CONbits.T2CKPS=0b10;
		TMR2ON=1; while(!T2IF); TMR2IE=0; TMR2ON=0; TMR2IF=0;
	}
}		


void resetRg(void) {
	asm("clrwdt");
	memset(&band,0,sizeof(band)); cntVerf=auxTmr2=0; _nEN_MED_VBAT_AND_VREF=1; 
	asm("clrwdt");
	cntFl[0]=cntFl[1]=0; TMR1=TMR2=0;  TMR2ON=0;TMR2IE=0; rxCont=0;
	posTindc=posTcod[1]=posTcod[0]=0; TMR1CS=0; tRst[0]=tRst[1]=0; SSPEN=_DIN=_EN_TRMT=0;
	asm("clrwdt");
	for(AUX[0]=0; AUX[0]<_NUM_FIL_TINDC;AUX[0]++) {tIndc[AUX[0]]=0;}	
	asm("clrwdt");
	for(AUX[0]=0; AUX[0]<_NUM_FIL_TCOD;AUX[0]++) {tCod[AUX[0]]=0;}	
	AUX[0]=AUX[1]=AUX[2]=cntFl[0]=cntFl[1]=0; prdo[0]=prdo[1]=0; AUX16[0]=AUX16[1]=tRst[0]=tRst[1]=0;
}

unsigned char buscIndSegunMtras(unsigned int *pMtras) { 
	for (AUX[0]=0; AUX[0]<posTindc; AUX[0]++) {
		if (*pMtras==tIndc[AUX[0]]) {band.vlrEnIndc = 1; break;}
	}
	if (band.vlrEnIndc) return AUX[0];
	else return 0;
}

// unsigned char buscVlrEnTindc(unsigned int *indc) { 
	// for (AUX[0]=0; AUX[0]<posTindc; AUX[0]++) {
		// if (*indc==tIndc[AUX[0]]) {band.vlrEnIndc = 1; break;}
	// }
	// if (band.vlrEnIndc) return tIndc[AUX[0]];
	// else return 0;
// }


void sec1(void) {;
	asm("clrwdt"); for(AUX[0]=0; AUX[0]<5; AUX[0]++) {printUARTbyte(&AUX[0]);}
}	

void medVoltBat() {
	resetRg(); _nEN_MED_VBAT_AND_VREF=0; ADON=1; ADIF=0; tmp20msTMR2(0); GO_nDONE=1; while(!ADIF); ADIF=0; ADON=0; _nEN_MED_VBAT_AND_VREF=1;
}	

#endif