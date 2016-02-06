#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>
#include"main.h"



FILE *traceFile;

int robHead,robTail;
int i;//iterator
int currentCycle = 0;
instInStage *FE,*DE,*RN,*RR,*DI,*IS,*EX,*WB,*ROB;

RMTStruct RMT[TotalRegs];
int instNum = 0;
int totalCycles = 0;
int ROB_SIZE,IQ_SIZE,WIDTH, exSize;
int dynamicIC = 0;
//char * fileName;
double IPC;
int xx = 100;

bool isEmpty(instInStage *X, int size, char *x)
{
	int i; 
	switch(size)
	{
		case 0:
			for(i = 0; i< WIDTH; i++)
			{
				if(X[i].validInst == notEmpty)
				{
					//printf("%s is NOT EMPTY\n",x);
					return false;
				}
			}
			//printf("%s is EMPTY\n",x);
			return true;
		//break;
		case 1:
			for(i = 0; i< IQ_SIZE; i++)
			{
				if(X[i].validInst == notEmpty)
				{
					//printf("%s is NOT EMPTY\n",x);
					return false;
				}
			}
			//printf("%s is EMPTY\n",x);
			return true;
		//break;
		case 2:
			for(i = 0; i< exSize; i++)
			{
				if(X[i].validInst == notEmpty)
				{
					//printf("%s is NOT EMPTY\n",x);
					return false;
				}
			}
			//printf("%s is EMPTY",x);
			return true;
		//break;
		case 3:
			for(i = 0; i< ROB_SIZE; i++)
			{
				if(X[i].validInst == notEmpty)
				{
					//printf("%s is NOT EMPTY\n",x);
					return false;
				}
			}
			//printf("%s is EMPTY",x);
			return true;
		default:
			printf("ERROR: Wrong argument to function isEmpty\n"); 
			return false;
		//break;
	}
}
bool Advance_Cycle(void)
{
	//getchar();
	//currentCycle++;
	if(isEmpty(FE,0,"FE") && isEmpty(DE,0,"DE") && isEmpty(RN,0,"RN") && isEmpty(RR,0,"RR") && isEmpty(DI,0,"DI") &&
	   isEmpty(IS,1,"IS") && isEmpty(EX,2,"EX") && isEmpty(WB,2,"WB") && isEmpty(ROB,3,"ROB")/*isEmpty(RT,2)*/)
	{
		return false;
	}
	else
		return true;
}
int main(int argc, char *argv[])
{
//	int k = 2;
	if(argc!=5)
	{
		printf("ERROR: Less or more no of argument than expected\n");
		exit(0);
	}	
	
	ROB_SIZE = atoi(argv[1]);
	IQ_SIZE  = atoi(argv[2]);
	WIDTH    = atoi(argv[3]);
	//fileName = argv[4];
	exSize 	 = WIDTH*5;
	robHead	 = 0;
	robTail	 = 0;
	
	traceFile = fopen(argv[4],"r");
	if(traceFile == NULL)
	{
		printf("ERROR: Unable to open file %s\n",argv[4]);
		return 1;
	}
	
	ROB = malloc(ROB_SIZE* sizeof(instInStage));//rob created
	for(i = 0; i< ROB_SIZE;i++)
	{
		ROB[i].validInst = inValid;   // ROB initialize to invalid instructions
	}
	
	for(i = 0; i< TotalRegs; i++)
	{
		RMT[i].where = inARF;//all register values are from arf  
		RMT[i].tag   = i;
	}
	FE = malloc(WIDTH*sizeof(instInStage));
	for(i = 0; i<WIDTH; i++)
	{
		FE[i].validInst = inValid;  //FE initialized with invalid instructions
	}
	
	DE = malloc(WIDTH*sizeof(instInStage));
	for(i = 0; i<WIDTH; i++)
	{
		DE[i].validInst = inValid;  //DE initialized with invalid instructions
	}
	RN = malloc(WIDTH*sizeof(instInStage));
	for(i = 0; i<WIDTH; i++)
	{
		RN[i].validInst = inValid;  //RN initialized with invalid instructions
	}
	RR = malloc(WIDTH*sizeof(instInStage));
	for(i = 0; i<WIDTH; i++)
	{
		RR[i].validInst = inValid;  //RR initialized with invalid instructions
	}
	DI = malloc(WIDTH*sizeof(instInStage));
	for(i = 0; i<WIDTH; i++)
	{
		DI[i].validInst = inValid;  //DI initialized with invalid instructions
	}
	IS = malloc(IQ_SIZE*sizeof(instInStage));
	{
		IS[i].validInst = inValid; //IS is initialized with invalid instructions		
	}
	EX = malloc(exSize*sizeof(instInStage));
	{
		EX[i].validInst = inValid; //EX is initialized with invalid instructions		
	}
	WB = malloc(exSize*sizeof(instInStage));
	{
		WB[i].validInst = inValid; //WB is initialized with invalid instructions		
	}
	/*RT = malloc(WIDTH*sizeof(instInStage));
	for(i = 0; i<WIDTH; i++)
	{
		RT[i].validInst = inValid;  //RT initialized with invalid instructions
	}*/
	
	do
	{
		Retire();
		WriteBack();
		Execute();
		Issue();
		Dispatch();
		RegRead();
		Rename();
		Decode();
		//Fetch();
		Fetchpart1();
	}
	while(Advance_Cycle());
		
	//printf(""); print stats
	IPC = (double)(instNum)/totalCycles;
	printf("# === Simulator Command =========\n");// print stats
	printf("# ./sim %d %d %d %s\n",ROB_SIZE,IQ_SIZE,WIDTH,argv[4]);
	printf("# === Processor Configuration ===\n");
	printf("# ROB_SIZE = %0d\n# IQ_SIZE  = %0d\n# WIDTH    = %0d\n",ROB_SIZE,IQ_SIZE,WIDTH);
	printf("# === Simulation Results ========\n");
	printf("# Dynamic Instruction Count    = %0d\n",instNum);
	printf("# Cycles                       = %0d\n",totalCycles);
	printf("# Instructions Per Cycle (IPC) = %.2f\n",IPC);
	return 0;
}
