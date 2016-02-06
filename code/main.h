#ifndef MAIN_H
#define MAIN_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>

#define TotalRegs 67
#define inARF false
#define inROB true
#define empty false
#define notEmpty true
#define valid true
#define inValid false
#define ready true
#define notReady false

typedef struct
{
	int tag;
	int dst;
	int dstspl;
	bool dstReady;
	int src1;
	int src1spl;
	bool src1Ready;
	int src2;
	int src2spl;
	bool src2Ready;
	int latency;
	int lat;
	int FEArrival;
	int FEDuration;
	int DEArrival;
	int DEDuration;
	int RNArrival;
	int RNDuration;
	int RRArrival;
	int RRDuration;
	int DIArrival;
	int DIDuration;
	int ISArrival;
	int ISDuration;
	int EXArrival;
	int EXDuration;
	int WBArrival;
	int WBDuration;
	int RTArrival;
	int RTDuration;	
	int age;
}instContents;

typedef struct
{
	bool validInst;
	instContents inst;
}instInStage;

typedef struct
{
	bool where;
	int tag;
}RMTStruct;
bool Advance_Cycle(void);
bool isEmpty(instInStage *X, int size, char *x);
void Retire(void);
void WriteBack(void);
void Execute(void);
void Issue(void);
void Dispatch(void);
void RegRead(void);
void Rename(void);
void Decode(void);
void Fetchpart1(void);
void Fetch(void);
bool ROBHasSpace(void);
int  dispatchBundleSize(void);
int  freeEntriesInIS(void);

#endif
