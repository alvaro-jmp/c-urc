// #include <pic16f690.h>
#include <stdlib.h>
#include <xc.h>

//#####################################
//########  REVISARRRR var.h ##########
//#####################################

#include "var.h"
#include "func.h"
#include "int.h"

#define _XTAL_FREQ 8000000

#pragma CONFIG CONFIG1L = 0xDC // WDT ON // 1101 1100
#pragma CONFIG CONFIG1H = 0x00 // 0000 0000


void main() {
	// OSCCON | 8 Mhz | Interno | 01110001
	OSCCON=0x7C; OSCTUNE=0;
	SCS=1;
	
	// Puertos
	ANSEL = 0b00000100; TRISA = 0b111110; TRISC = 0b00001000; PORTA = PORTB = PORTC = 0;
	TRISB7 = 0; TRISB6 = 0; TRISB5 = 1; TRISB4 = 1;
	ANSELH = 0; WPUA = 0; IOCA = 0; WPUB = 0; IOCB = 0;
	
    // UART | 9600 baudios | 1 stop bit | prdad no
	TXSTA=0x26; RCSTA=0x90; BAUDCTL=0x48;
	SYNC=0; BRG16=0; BRGH=0; SPBRGH=0; SPBRG=12; //SPBRG=207, SPBRG=220
		
	// SPI | Master | CLK=FOSC/4
	SMP=0; CKP=0; CKE=1; SSPCONbits.SSPM=0b0010;	
	
	// CMP1 | POL INV | CVREF = 1.44375
	C1OE=0; C1POL=1; C1R=1; CM1CON0bits.C1CH=0b11; 
	C1VREN=1; VP6EN=0; VRR=0; VRCONbits.VR=11; 	
	
	// AN | MOD READ LEFT TO RIGHT | VREF = VREF PIN | CHS = AN2 | TAD = FOSC/32
	ADFM = 1; VCFG = 1; ADCON0bits.CHS = 0b10; ADCON1bits.ADCS = 0b10;
	
	// TMR1 | MOD CONT | PRE 8 | NO SYNC | EXT CLK
	T1CON=0x04;
		
	// TMR0 | PSA = TMR0
	OPTION_REG=0xFD;
	
	// WDT | 16384 | 16 s
	WDTCON=0x12;
	
	// EN Modulos
	_nEN_MED_VBAT_AND_VREF=1; 
	_EN_BTH=1;

	// INT
	PEIE=GIE=RCIE=1; PIR1=PIR2=0;
				
	while(1) {		
		if (!band.nDormir) {
			 //Medir bateria y tomar acciones
			#if !_nSIM_SLEEP
				WDTCON=0x12; asm("clrwdt");
				//medVtjBat();AUX16[0]=ADRESH<<8;	AUX16[0]&=0x0300; AUX16[0]|=ADRESL;
				test=11; AUX[0]='d'; printUARTbyte(&AUX[0]); WUE=1; asm("sleep");
			#else				
				test=10; _LED^=1;
			#endif
		}					
		if (band.cmdRcb) {			
			if ((rxCont>1) && !band.rcvDataParaRpCtrl) {
				TMR2IE=0; TMR2ON=0;
				switch ( DATARX ) {
					case 'a':						
						asm("clrwdt"); WDTCON=0x12;
						printUARTbyte(&DATARX); resetRg(); TMR1ON=1;
						C1ON=1; T1CONbits.T1CKPS=0b11; cntErr=0; band.nDormir=1; C1IF=0; C1IE=1;
						break;
					case 'r':
						C1ON=0; TMR1ON=0; TMR2ON=0; TMR2IE=0; C1IE=0;
						resetRg(); band.rcvDataParaRpCtrl=1; 
						band.nDormir=1; ++cntVerf; rxCont=0; 
						asm("clrwdt"); WDTCON=0x08;
						break;
					case 'b':
						medVoltBat(); AUX[1]=ADRESH; AUX[0]=ADRESL;
						printUARTbyte(&AUX[1]); printUARTbyte(&AUX[0]); 
				}
			}
			//else if((rxCont==1) && !band.rcvDataParaRpCtrl) {; /*asm("clrwdt"); WDTCON=0x08;*/}	
			else if (band.rcvDataParaRpCtrl) {
				if (rxCont==2) _FIN_TINDC=DATARX; // fin tIndc
				else if (rxCont==3) _LNGTUD_TRAMA=DATARX; // Longitud de la trama
				else if (rxCont==4) cntFl[0]=DATARX; // cntFl
				else if (rxCont==5) tRst[0]=DATARX<<8; // tRst H
				else if (rxCont==6) tRst[0]|=DATARX; // tRst L
				else if (rxCont==7) _FREC=DATARX; // _FREC=cntFl[1]
				else if (rxCont>7 && rxCont<=_FIN_TINDC) {
				 	if (rxCont%2==1) tIndc[posTindc++]|=(DATARX<<8);
				 	else tIndc[posTindc]=DATARX;
				}
				else if (rxCont>_FIN_TINDC && rxCont<=_LNGTUD_TRAMA) {
					tCod[posTcod[0]++]=DATARX;
					if (rxCont==_LNGTUD_TRAMA) {
						band.rcvDataParaRpCtrl=0; band.rpCtrl=1; rxCont=0; // No quitar rxCont=0. De lo contrario hay que esperar a resetRg() para que sea cero y se envía un byte descontrola el código
					} 
				} 
			}				
			band.cmdRcb = 0;
		}
		if (band.intTmr2) {			
			TMR2ON=0;TMR2IE=0;TMR2IF=0;			
			if (band.medFrec) {
				AUX16[0]=TMR1; mtrasPort[cntVerf-9]=AUX16[0];
				if ((cntVerf-9)>0) {
					if (mtrasPort[cntVerf-9]>mtrasPort[cntVerf-10]) {AUX[1]=mtrasPort[cntVerf-9]-mtrasPort[cntVerf-10];}
					else {AUX[1]=mtrasPort[cntVerf-10]-mtrasPort[cntVerf-9];}
					if (AUX[1]>5){
						band.error=1; numErr=3; ++cntErr;
					}
				 }
				++cntVerf; if (cntVerf>12 && !band.error) {
					C1IE=0; C1ON=0; TMR1ON=0; asm("clrwdt"); WDTCON=0x12;
					band.aprdzjRlzd=1; band.medFrec=0;
				} 			
			 }
			if (band.rcvDataParaRpCtrl) {
				++cntVerf;
			}
			if (band.tmp240msTMR2) {
				++auxTmr2;
				if (auxTmr2==12) {
					band.tmp240msTMR2=0; auxTmr2=0; ++cntVerf;
				}	
				else {TMR2IF=0; TMR2=0; TMR2ON=1; TMR2IE=1;}
			}	
			band.intTmr2=0;
		}
		if (band.rpCtrl) {
			
//			asm("clrwdt"); WDTCON=0x12; C1ON=0; TMR1ON=0; TMR2ON=0; TMR2IE=0; C1IE=0;			
//			//cntFl[0]
//			printUARTbyte(&cntFl[0]);
//			//tRst[0]
//			AUX[1]=tRst[0]>>8; AUX[0]=tRst[0]&0xFF;
//			printUARTbyte(&AUX[1]); printUARTbyte(&AUX[0]);
//			//_FREC=cntFl[1]
//			printUARTbyte(&_FREC);
//			//tIndc
//			for(AUX[0]=0; AUX[0]<=posTindc; AUX[0]++) { //probablemente el ultimo indc sea cero o un valor
//				asm("clrwdt"); 
//				if (tIndc[AUX[0]]==0) break;
//				else {
//					AUX[2]=(tIndc[AUX[0]])>>8; AUX[1]=tIndc[AUX[0]]&0xFF;
//					printUARTbyte(&AUX[2]); printUARTbyte(&AUX[1]);
//				}				
//			}	
//			sec1();
//			//tCod
//			AUX[1]=(cntFl[0]>>1)-1;
//			for(AUX[0]=0; AUX[0]<=AUX[1]; AUX[0]++) {asm("clrwdt"); printUARTbyte(&tCod[AUX[0]]);}	
//			Secuencia Final
//			for(AUX[0]=0; AUX[0]<=5; AUX[0]++) {asm("clrwdt"); printUARTbyte(&AUX[0]);}	
			
		#if __DBG_SPI
			SSPEN=1; _EN_TRMT=1; _nCS=1; tmp20msTMR2(0);
		 	// ### CONFIG1 #####
		 	_nCS=0; 
			AUX[0]=_ESCRITURA; printSPIbyte(&AUX[0]); AUX[0]=_CONFIG1; printSPIbyte(&AUX[0]); 
			AUX[0]=0x01; printSPIbyte(&AUX[0]);
			_nCS=1;

			 //### CONTRL #####
			_nCS=0;		
			AUX[0]=_ESCRITURA; printSPIbyte(&AUX[0]); AUX[0]=_CONTRL; printSPIbyte(&AUX[0]); 
			AUX[0]=0x00; printSPIbyte(&AUX[0]);
			_nCS=1;
		
			//### CONFIG0 #####
			_nCS=0;
			AUX[0]=_ESCRITURA; printSPIbyte(&AUX[0]); AUX[0]=_CONFIG0; printSPIbyte(&AUX[0]); 
			AUX[0]=0x00; printSPIbyte(&AUX[0]);
			_nCS=1;

			//### LOFREQ0 #####
			_nCS=0; AUX[0]=_ESCRITURA; printSPIbyte(&AUX[0]); AUX[0]=_LOFREQ0; printSPIbyte(&AUX[0]);
			AUX[0]=0x00; printSPIbyte(&AUX[0]);
			_nCS=1;
			
			//### LOFREQ1 #####
			_nCS=0;
			AUX[0]=_ESCRITURA; printSPIbyte(&AUX[0]); AUX[0]=_LOFREQ1; printSPIbyte(&AUX[0]);
			AUX[0]=_FREC; printSPIbyte(&AUX[0]);
			//AUX[0]=0x3B; printSPIbyte(&AUX[0]);			
			_nCS=1;
					
			//### FLOAD #####
			_nCS=0;
			AUX[0]=_ESCRITURA; printSPIbyte(&AUX[0]); AUX[0]=_FLOAD; printSPIbyte(&AUX[0]);
			AUX[0]=_CARGAR_FRECUENCIA; printSPIbyte(&AUX[0]);
			_nCS=1;		
		#endif
		
		#if !__DBG_CLV
			cntFl[1]=1; AUX[0]=posTcod[0]=0; TMR1ON=1; T1CONbits.T1CKPS=0b00;
			do {
				asm("clrwdt");
				TMR1=0; TMR1IF=0; 
				if (cntFl[1]%2==1) {_LED=1;_DIN=1; ++cntFl[1]; TMR1=tIndc[((tCod[posTcod[0]])&0x0F)]; TMR1+=46;} // Resto X us que consume la rutina
				else {_LED=0;_DIN=0; ++cntFl[1]; TMR1=tIndc[((tCod[(posTcod[0]++)])>>4)]; TMR1+=71;} // Resto X us que consume la rutina
				while(!TMR1IF);
				if (cntFl[1]==cntFl[0]) {
					asm("clrwdt");
					_DIN=0; ++AUX[0]; TMR1=0; TMR1IF=0; T1CONbits.T1CKPS=0b11; TMR1=tRst[0]; while(!TMR1IF);
					posTcod[0]=0; T1CONbits.T1CKPS=0b00; cntFl[1]=1;
				}
			} while(AUX[0]<10);	
		#else
			cntFl[1]=1; AUX[0]=posTcod[0]=0; TMR1ON=1; T1CONbits.T1CKPS=0b00;
			do {
				asm("clrwdt");
				TMR1=0; TMR1IF=0; 
				if (cntFl[1]%2==1) {_LED=1;_DIN=1; ++cntFl[1]; TMR1=tIndc[((tCod[posTcod[0]])&0x0F)]; TMR1+=46;} // Resto X us que consume la rutina
				else {_LED=0;_DIN=0; ++cntFl[1]; TMR1=tIndc[((tCod[(posTcod[0]++)])>>4)]; TMR1+=71;} // Resto X us que consume la rutina
				while(!TMR1IF);
				if (cntFl[1]==cntFl[0]) {
					asm("clrwdt");
					_DIN=0; ++AUX[0]; TMR1=0; TMR1IF=0; T1CONbits.T1CKPS=0b11; TMR1=tRst[0]; while(!TMR1IF);
					posTcod[0]=0; T1CONbits.T1CKPS=0b00; cntFl[1]=1;
				}
			} while(AUX[0]<255);			
		#endif			
			SSPEN=0; _EN_TRMT=_DIN=TMR1ON=_LED=0;
			asm("clrwdt"); WDTCON=0x12; resetRg();
		}	
		if (band.mtras) {
			if (cntVerf==4) {
				++cntFl[0]; band.rstDtdo=0;
				if (cntFl[0]>1) {
					for (AUX[0]=0; AUX[0]<=posTindc; AUX[0]++) {
						if (mtras<tIndc[AUX[0]]) AUX16[0]=tIndc[AUX[0]]-mtras;
						else AUX16[0]=mtras-tIndc[AUX[0]];
						if (AUX16[0]<65) {band.vlrEnIndc=1; break;}
					}	
					if (posTindc==15 && !band.vlrEnIndc && tIndc[15]!=0) {band.error=1; numErr=4; --AUX[0];}
					if (!band.vlrEnIndc) {tIndc[posTindc++]=mtras; if(posTindc==16)posTindc=15; --AUX[0];}					
					band.vlrEnIndc=0;
					if (posTcod[0]<=45) {
						if (band.nbbl) {AUX[0]=AUX[0]<<4; tCod[posTcod[0]++]|=AUX[0]; band.nbbl=0;}
						else {tCod[posTcod[0]]=AUX[0]; band.nbbl=1;}
					}
					else {
						band.error=1; numErr=5;
					}
				}
			}	
			else if (cntVerf>4 && !band.medFrec) {
				++cntFl[0]; band.rstDtdo=0;	band.vlrEnIndc=0;			
				if (cntFl[0]>1) {
					if (posTcod[0]<=45) {
						if (band.nbbl) {AUX[0]=tCod[posTcod[0]++]>>4; band.nbbl=0;}
						else {AUX[0]=tCod[posTcod[0]]&0x0F; band.nbbl=1;}
						AUX16[0]=tIndc[AUX[0]];
						if (mtras>AUX16[0]) {AUX16[0]=mtras-AUX16[0];}
						else {AUX16[0]=AUX16[0]-mtras;}	
						if (AUX16[0]>65){
							band.error=1; numErr=1; ++cntErr;
						}
					}	
				}
			}	
			else if (band.medFrec) {
				if (C1OUT) {
					TMR2IF=0; TMR2=0; PR2=_TMP_TMR2_100US; T2CONbits.T2CKPS=0; T2CONbits.TOUTPS=0; 
					TMR1=0; TMR2IE=TMR2ON=1;
				}
				else {TMR2ON=0;TMR2IE=0;}	
			}
			else if (!cntVerf) {
				band.tmp240msTMR2=1; tmp20msTMR2(1); ++cntVerf; asm("clrwdt"); WDTCON=0x08; //512ms WDT
			}	
			else if (cntVerf==1) {
				++cntFl[0];	if (cntFl[0]>1) {if (tRst[0]<mtras) tRst[0]=mtras;} if (cntFl[0]==2) cntFl[0]==1;
			}	
			else if (cntVerf==2) {
				if (tRst[0]>7500) tRst[1]=60000;
				else tRst[1]=tRst[0]*6;
				TMR1=0; T1CONbits.T1CKPS=0; band.dtcTmpRst=1; cntVerf++;				
			}	
			band.mtras=0;					
		}		
		if (band.dtcTmpRst && !C1OUT && (TMR1>tRst[1]) && !band.rstDtdo) {
			if (cntVerf==8) {
				band.medFrec=1; band.dtcTmpRst=0; cntErr=0; TMR1CS=1;
			} 
			cntFl[1]=cntFl[0]; band.rstDtdo=1; cntFl[0]=posTcod[0]=0; band.nbbl=0; ++cntVerf;			
		}	

		if (band.error) {
			asm("clrwdt");
			if (cntErr<=5) {
				if (cntVerf<9) {resetRg(); T1CONbits.T1CKPS=0b11; band.nDormir=1;}
				else {cntVerf=9;}
				band.error=0;
			}
			else {
				asm("clrwdt"); WDTCON=0x12; C1ON=0;TMR1ON=0;TMR2ON=0;TMR2IE=0;TMR2IF=0;
				//numErr
				printUARTbyte(&numErr); AUX[1]=0xFF;				
				for(AUX[0]=0; AUX[0]<=5; AUX[0]++) {asm("clrwdt");  printUARTbyte(&AUX[1]);}	
				resetRg();
			}
			band.error=0;
		}
		if (band.aprdzjRlzd) {
			//cntFl[1]
			asm("clrwdt"); 
			printUARTbyte(&cntFl[1]);
			//tRst[0]
			AUX[1]=tRst[0]>>8; AUX[0]=tRst[0]&0xFF;
			printUARTbyte(&AUX[1]); printUARTbyte(&AUX[0]);
			//mtrasPort[0]
			asm("clrwdt"); 
			AUX[1]=mtrasPort[0]>>8; AUX[0]=mtrasPort[0]&0xFF;
			printUARTbyte(&AUX[1]); printUARTbyte(&AUX[0]);
			//tIndc
			for(AUX[0]=0; AUX[0]<=posTindc; AUX[0]++) { //probablemente el ultimo indc sea cero o un valor
				asm("clrwdt"); 
				if (tIndc[AUX[0]]==0) break;
				else {
					AUX[2]=(tIndc[AUX[0]])>>8; AUX[1]=tIndc[AUX[0]]&0xFF;
					printUARTbyte(&AUX[2]);
					printUARTbyte(&AUX[1]);
				}				
			}	
			sec1();
			//tCod
			AUX[1]=(cntFl[1]>>1)-1;
			for(AUX[0]=0; AUX[0]<=AUX[1]; AUX[0]++) {asm("clrwdt"); printUARTbyte(&tCod[AUX[0]]);}	
			//Secuencia Final
			for(AUX[0]=0; AUX[0]<=5; AUX[0]++) {asm("clrwdt"); printUARTbyte(&AUX[0]);}			
			resetRg();
		}		
	}	
}	
