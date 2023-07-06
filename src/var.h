#ifndef __VAR_H
#define __VAR_H
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
// #include <pic16f690.h>

//#########################################################
//#### NO OLVIDAR !!!! CAMBIAR A CERO _nSIM_SLEEP     #####
//#### EN MODO PROTOTIPO 						  	  #####
//#########################################################
/*##########*/ #define _nSIM_SLEEP 0 //####################
//#########################################################

////// OTROS DBG
#define __DBG_CLV 1
#define __DBG_SPI 1

typedef struct {
	unsigned anlzTmpRst:1;	
	unsigned anlzMtras:1;
	unsigned aprdzjRlzd:1;
	unsigned cmdRcb:1;
	unsigned dtcPriBit:1;
	unsigned dtcTmpRst:1;
	unsigned error:1;	
	unsigned nDormir:1;
	unsigned rcvDataParaRpCtrl:1;
	unsigned medFrec:1;
	unsigned vlrEnIndc:1;
	unsigned mtras:1;
	unsigned nbbl:1;
	unsigned intTmr2:1;
	unsigned TmpFuera:1;
	unsigned rpCtrl:1;
	unsigned priBitDtdo:1;
	unsigned priMtras:1;
	unsigned rstDtdo:1;
	unsigned tmp240msTMR2;
} Banderas;
Banderas band;

#define _NUM_FIL_TINDC 16
#define _NUM_FIL_TCOD 45
unsigned char DATARX, cntFl[2], cntVerf, numErr, AUX[3], tCod[_NUM_FIL_TCOD], posTindc, auxTmr2;  
unsigned char test, posTcod[2], cntErr, rxCont;
unsigned int mtras, tIndc[_NUM_FIL_TINDC], prdo[2], tRst[2], AUX16[2], mtrasPort[4];

//#define _INIDBG C2ON=0; TMR1ON=0; TMR2ON=0; TMR2IE=0;
//#define _FINDBG resetRg(); TMR1ON=0; RCIE=1;

#define _nCS RC6
#define _DIN RC2
#define _EN_TRMT RC1
#define _LED RC0
#define _nEN_MED_VBAT_AND_VREF TRISA0
#define _EN_BTH RC4

#define _TMP_TMR2_100US 125
#define _TMP_TMR2_20MS 160

#define _CONTRL 0x00
#define _CONFIG0 0x01
#define _HIFREQ1 0x02
#define _HIFREQ0 0x03
#define _LOFREQ1 0x04
#define _LOFREQ0 0x05
#define _FLOAD 0x06
#define _DATAIN 0x07
#define _CONFIG1 0x09
#define _STATUS 0x0C
#define _LEYENDO 0x00
#define _SDO 0x01
#define _ESCRITURA 0x01
#define _LECTURA 0x02
#define _CARGAR_FRECUENCIA 0x01

#define _FIN_TINDC cntVerf
#define _LNGTUD_TRAMA numErr
#define _FREC cntFl[1]

#endif