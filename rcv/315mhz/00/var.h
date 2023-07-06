#ifndef __VAR_H
#define __VAR_H
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <pic16f690.h>

#define errCalib 500
#define EN_LED RC0
#define EN_SOUND RA1


typedef struct {
	unsigned mtras;
	unsigned errEnSec;
	unsigned secDtc;
} Banderas;
extern Banderas band;

extern unsigned char tCod[8], posTcod, cntSec, auxChar[3],cntFl;
extern unsigned int tIndc[4], auxInt[3], mtras; 

#endif