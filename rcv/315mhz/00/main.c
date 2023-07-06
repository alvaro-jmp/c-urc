#include <pic16f690.h>
#include <stdlib.h>
#include <xc.h>

//#####################################
//########  REVISARRRR var.h ##########
//#####################################

#include "var.h"
#include "func.h"
#include "int.h"

#define _XTAL_FREQ 8000000
#pragma config CONFIG = 0x00DC; // WDT ON // 0000 0000 1101 1100


void main() {
	// OSCCON | 8 Mhz | Interno | 01110001
	OSCCON=0x7C; OSCTUNE=0; SCS=1;
	
	// Puertos
	ANSEL=TRISA=0; TRISC=0b00001000;
	TRISB7=0; TRISB6=0; TRISB5=0; TRISB4=0; ANSELH=WPUA=IOCA=WPUB=IOCB=0;
	
	// CMP1 | POL INV | CVREF = 1.44375
	C1OE=0; C1POL=1; C1R=1; CM1CON0bits.C1CH=0b11; 
	C1VREN=1; VP6EN=0; VRR=0; VRCONbits.VR=11; 	
	
	// TMR2 | PRE=16 | POST=16 
	PR2=58; T2CON=0b01111011;
	
	// TMR1 | MOD CONT | PRE 8 | NO SYNC | EXT CLK
	T1CON=0x01;
		
	// TMR0 | PSA = TMR0
	OPTION_REG=0xFD;
	
	// WDT | 16384 | 65 s
	WDTCON=0x16;
	
	// INT
	PORTA=PORTB=PORTC=0;
	GIE=PEIE=TMR2IE=1; PIR1=PIR2=0; C1ON=1; C1IF=0; C1IE=1;
				
	while(1) {	
		if (band.mtras) {
			if (!C1OUT&&cntFl>1) {
				auxChar[0]=tCod[posTcod]<<4; auxChar[0]=auxChar[0]>>4;
				if (mtras>=tIndc[auxChar[0]]) {auxInt[0]=mtras-tIndc[auxChar[0]];}	
				else {auxInt[0]=tIndc[auxChar[0]]-mtras; }
				if (auxInt[0]<=errCalib) {++cntSec;}
				else {cntSec=posTcod=0;cntFl=0;}
			}
			else if(C1OUT&&cntFl>1) {
				auxChar[0]=tCod[posTcod]>>4;
				if (mtras>=tIndc[auxChar[0]]) {auxInt[0]=mtras-tIndc[auxChar[0]];}	
				else {auxInt[0]=tIndc[auxChar[0]]-mtras;}
				if (auxInt[0]<=errCalib) {++cntSec;++posTcod;}
				else {cntSec=posTcod=0;cntFl=0;}
			}	
			band.mtras=0;
		}	
		// Secuencia detectada
		if (cntSec==12) {
			cntSec=posTcod=0;cntFl=0; TMR2=0; TMR2IF=0; TMR2ON=1;
			EN_LED=0; EN_SOUND=1;
		}
	}	
}	
