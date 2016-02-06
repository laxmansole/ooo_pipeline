#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>
#include"main.h"

extern FILE *traceFile;

extern int robHead,robTail;

extern int currentCycle, dynamicIC;

extern instInStage *FE,*DE,*RN,*RR,*DI,*IS,*EX,*WB,*ROB;

extern RMTStruct RMT[TotalRegs];

extern int ROB_SIZE,IQ_SIZE,WIDTH, exSize;
extern int instNum,totalCycles;
int i, j, k;

char line[256];
char *strtag, *strlatency, *strdst, *strsrc1, *strsrc2; 
int tag, latency, dst, src1, src2;
int age = 0;
void regPrint(instInStage *X,char *STR,int size)
{	
  int i;
	//printf("**************************cycle %d: %s status****************************\n",currentCycle,STR);
	for(i = 0; i< size; i++)
	{
		/*printf(" fu{%d} src{%d, %d} dst{%d} FE{%d,%d} DE{%d,%d} RN{%d,%d} RR{%d,%d} DI{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,%d} RT{%d,%d}\n",
			X[i].inst.lat,X[i].inst.src1,X[i].inst.src2,X[i].inst.dst,
			X[i].inst.FEArrival,X[i].inst.FEDuration, 
			X[i].inst.DEArrival, X[i].inst.DEDuration,
			X[i].inst.RNArrival,X[i].inst.RNDuration,
			X[i].inst.RRArrival,X[i].inst.RRDuration,
			X[i].inst.DIArrival,X[i].inst.DIDuration,
			X[i].inst.ISArrival,X[i].inst.ISDuration,
			X[i].inst.EXArrival,X[i].inst.EXDuration,
			X[i].inst.WBArrival,X[i].inst.WBDuration,
			X[i].inst.RTArrival,X[i].inst.RTDuration);*/
			if(X[i].validInst)
			{
				/*printf("tag: %x-->DST = (%d--%d)  SRC1 = (%d--%d), src2 = (%d--%d) latency= %d age = %d\n",
					X[i].inst.tag,
					X[i].inst.dst,X[i].inst.dstReady,
					X[i].inst.src1,X[i].inst.src1Ready,
					X[i].inst.src2,X[i].inst.src2Ready,
					X[i].inst.latency,X[i].inst.age);
					if(X[i].inst.age == 26)
					{
						printf("FETCH ARRIVAL TIME = %d\n",X[i].inst.FEArrival);
					}*/
			}
	}
}
bool ROBHasSpace(void)
{
	//int k = robTail;
	int robSpace =0;
	for(i =0; i<ROB_SIZE;i++)
	{
		if(ROB[i].validInst == inValid)
		{
			robSpace++;
		}
	}
	if(robSpace>=WIDTH)
		return true;
	else
		return false;
	/*for(i = 0; i< WIDTH; i++)
	{
		
		if(k!=(ROB_SIZE-1))//check for this
		{
			k++;
		}
		else
		{
			k = 0;
		}
		
		if(k == (robHead))//check for this
		{
			return false;
		}
	}
	return true;*/
}
int freeEntriesInIS(void)
{
	int j;
	int freeEntries = 0; 
	for(j = 0; j <IQ_SIZE; j++)
	{
		if(IS[j].validInst == inValid)
		{
			freeEntries++;
		}
	}
	return freeEntries;
}

int dispatchBundleSize(void)
{
	int size = 0;
	for(i = 0; i < WIDTH; i++)
	{
		if(DI[i].validInst == valid)
		{
			size++;
		}
	}
	return size;
}
void sortIS(instInStage *IS)
{
	instInStage temp;
	for(i = 0; i<IQ_SIZE;i++)
	{
		for(j = i+1; j<IQ_SIZE; j++)
		{
			if(IS[i].inst.age > IS[j].inst.age)
			{
				temp = IS[i];
				IS[i] = IS[j];
				IS[j] = temp;
			}
		}
	}
}

void Retire(void)
{
	//printf("Retire\n");
	int retireNum = 0;
	//printf("robHead = %d\trobTail = %d\n",robHead,robTail);
	for(i = 0; i< WIDTH;i++)
	{
		if((ROB[robHead].inst.dstReady) && (ROB[robHead].validInst == valid))
		{	
			ROB[robHead].inst.RTDuration = (currentCycle+1) - ROB[robHead].inst.RTArrival;
			/*if(ROB[robHead].inst.dstspl != -1)
			{
				RMT[ROB[robHead].inst.dst].where = inARF;
				RMT[ROB[robHead].inst.dst].tag = ROB[robHead].inst.dst;	
			}	*/
			if(ROB[robHead].inst.dstspl != -1)
			{
				if(RMT[ROB[robHead].inst.dstspl].where == inROB)
				{
					if(RMT[ROB[robHead].inst.dstspl].tag == (robHead+100))
					{
						RMT[ROB[robHead].inst.dstspl].where = inARF;
					}
				}
			}
			ROB[robHead].validInst = inValid;
			retireNum++;
			printf("%d fu{%d} src{%d,%d} dst{%d} FE{%d,%d} DE{%d,%d} RN{%d,%d} RR{%d,%d} DI{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,%d} RT{%d,%d}\n",
					instNum,ROB[robHead].inst.lat,ROB[robHead].inst.src1spl,ROB[robHead].inst.src2spl,ROB[robHead].inst.dstspl,
					ROB[robHead].inst.FEArrival,ROB[robHead].inst.FEDuration, 
					ROB[robHead].inst.DEArrival, ROB[robHead].inst.DEDuration,
					ROB[robHead].inst.RNArrival,ROB[robHead].inst.RNDuration,
					ROB[robHead].inst.RRArrival,ROB[robHead].inst.RRDuration,
					ROB[robHead].inst.DIArrival,ROB[robHead].inst.DIDuration,
					ROB[robHead].inst.ISArrival,ROB[robHead].inst.ISDuration,
					ROB[robHead].inst.EXArrival,ROB[robHead].inst.EXDuration,
					ROB[robHead].inst.WBArrival,ROB[robHead].inst.WBDuration,
					ROB[robHead].inst.RTArrival,ROB[robHead].inst.RTDuration);	
					instNum++;
					totalCycles = ROB[robHead].inst.RTArrival+ROB[robHead].inst.RTDuration;
			
			if(robHead == (ROB_SIZE-1))
			{
				robHead = 0;
			}
			else
			{
				robHead++;
			}
		}
		else
		{
			return;
		}
		
	}
}
void WriteBack(void)
{
	//printf("WriteBack\n");
	
	for(i = 0; i< exSize; i++)
	{
		if(WB[i].validInst)
		{
			ROB[WB[i].inst.dst-100]= WB[i];
			ROB[WB[i].inst.dst-100].validInst = valid; 
			ROB[WB[i].inst.dst-100].inst.RTArrival = currentCycle+1;
			ROB[WB[i].inst.dst-100].inst.WBDuration = (ROB[WB[i].inst.dst-100].inst.RTArrival)-(ROB[WB[i].inst.dst-100].inst.WBArrival); 
			ROB[WB[i].inst.dst-100].inst.dstReady = ready;
			WB[i].validInst = inValid;
		}
	}
	
	
	
	
	
	
	
	
	
	
	/*for(i = 0; i< exSize; i++)
	{
		if(WB[i].validInst)
		{
			for(j = 0; j<ROB_SIZE; j++)
			{
				if(ROB[j].validInst && WB[i].validInst && (ROB[j].inst.tag == WB[i].inst.tag))
				{
					ROB[j] = WB[i];
					ROB[j].validInst = valid; 
					ROB[j].inst.RTArrival = currentCycle;
					ROB[j].inst.WBDuration = (ROB[j].inst.RTArrival)-(ROB[j].inst.WBArrival); 
					ROB[j].inst.dstReady = ready;
					WB[i].validInst = inValid;
				}
			}
		}
	}*/
	regPrint(ROB,"ROB",ROB_SIZE);
}
void Execute(void)
{
	//printf("Execute\n");
	for(i = 0; i< exSize; i++)
	{
		if(EX[i].validInst)
		{
			EX[i].inst.latency--;
			if(EX[i].inst.latency == 0)
			{
				for(k = 0; k<IQ_SIZE; k++)//waking up IS instruction bundle
				{
					if(IS[k].validInst && (EX[i].inst.dst != -1) && (EX[i].inst.dst == IS[k].inst.src1))
					{
						IS[k].inst.src1Ready = ready;
					}
					if(IS[k].validInst && (EX[i].inst.dst != -1) && (EX[i].inst.dst == IS[k].inst.src2))
					{
						IS[k].inst.src2Ready = ready;
					}
				}
				
				for(k = 0; k<WIDTH; k++) // waking up DI instruction bundle
				{
					if(DI[k].validInst && (EX[i].inst.dst != -1) && (EX[i].inst.dst == DI[k].inst.src1))
					{
						DI[k].inst.src1Ready = ready;
					}
					if(DI[k].validInst &&(EX[i].inst.dst != -1) && (EX[i].inst.dst == DI[k].inst.src2))
					{
						DI[k].inst.src2Ready = ready;
					}
				}
				
				for(k = 0; k<WIDTH; k++) // waking up RR instruction bundle
				{
					if(RR[k].validInst && (EX[i].inst.dst != -1) &&(EX[i].inst.dst == RR[k].inst.src1))
					{
						RR[k].inst.src1Ready = ready;
					}
					if(RR[k].validInst && (EX[i].inst.dst != -1) && (EX[i].inst.dst == RR[k].inst.src2))
					{
						RR[k].inst.src2Ready = ready;
					}
				}

				for(j = 0; j<exSize; j++)
				{
					if(WB[j].validInst == inValid)
					{
						WB[j] = EX[i];
						WB[j].validInst = valid;
						WB[j].inst.WBArrival = currentCycle+1;
						WB[j].inst.EXDuration = (WB[j].inst.WBArrival)-(WB[j].inst.EXArrival); 
						WB[j].inst.dstReady = ready;
						EX[i].validInst = inValid;
						break;
					}
				}
			}
			/*else
			{
				EX[i].inst.latency--;
			}*/
		}		
	}
	regPrint(WB,"WB",exSize);
}
void Issue(void)
{
//	printf("Issue\n");
	int noOfIssues = 0;
	sortIS(IS);
	//printf("sorted ISSUE QUE:\n");
	regPrint(IS,"IS",IQ_SIZE);
	//for(i = 0; i< WIDTH; i++)
	//{		
		for(j = 0; j< IQ_SIZE ; j++)
		{
			if(IS[j].validInst)
			{
				if((IS[j].inst.src1Ready == ready) && (IS[j].inst.src2Ready == ready) )
				{
					for(k = 0; k< exSize; k++)
					{
						if(EX[k].validInst == inValid)
						{
							noOfIssues++;
							EX[k] = IS[j];
							EX[k].validInst = valid ;
							EX[k].inst.EXArrival = currentCycle+1;
							EX[k].inst.ISDuration = (EX[k].inst.EXArrival)-(EX[k].inst.ISArrival); 
							IS[j].validInst = inValid;
							
							if(noOfIssues == WIDTH)
							{	
								//printf(" MORE IS entries ready\n");
								regPrint(EX,"EX",exSize);
								return;
							}
							break;
						}
					}
				}
			}
		}
	//}
	regPrint(EX,"EX",exSize);
}
void Dispatch(void)
{
	//printf("Dispatch\n");
	if(freeEntriesInIS()>= dispatchBundleSize())
	{
		//printf("inside DISPATCH\n");
		for(i = 0; i< WIDTH; i++)
		{
			if(DI[i].validInst == valid)
			{
				for(j = 0; j< IQ_SIZE; j++ )
				{
	
					if((IS[j].validInst == inValid))
					{
						IS[j] = DI[i];
						IS[j].validInst = valid; 
						IS[j].inst.ISArrival = currentCycle+1;
						IS[j].inst.DIDuration = (IS[j].inst.ISArrival)-(IS[j].inst.DIArrival); 
						DI[i].validInst = inValid;
						break;
					}
				}
			}
		}
		//regPrint(IS,"IS",IQ_SIZE);
	}
	regPrint(IS,"IS",IQ_SIZE);
}
void RegRead(void)
{
	//printf("Regread\n");
	if(isEmpty(DI,0,"DI"))
	{
		for(i = 0; i< WIDTH; i++)
		{
			if(RR[i].validInst)
			{
				if(RR[i].inst.src1 != -1)
				{
					if((RR[i].inst.src1<67))//&& (RMT[RR[i].inst.src1].where == inARF))
					{
						RR[i].inst.src1Ready = ready;
					}
					else if((RR[i].inst.src1>=100))//&& (RMT[RR[i].inst.src1-100].where == inROB))
					{
						if(ROB[RR[i].inst.src1-100].inst.dstReady)
						{
							RR[i].inst.src1Ready = ready;
						}
					}
				}
				else
				{
						RR[i].inst.src1Ready = ready;
				}
				
				if(RR[i].inst.src2 != -1)
				{
					if((RR[i].inst.src2<67))//&& (RMT[RR[i].inst.src2].where == inARF))
					{
						RR[i].inst.src2Ready = ready;
					}
					else if((RR[i].inst.src2>=100))//&& (RMT[RR[i].inst.src2-100].where == inROB))
					{
						if(ROB[RR[i].inst.src2-100].inst.dstReady)
						{
							RR[i].inst.src2Ready = ready;
						}
					}
				}
				else
				{
						RR[i].inst.src2Ready = ready;
				}
				
				DI[i] = RR[i];
				DI[i].validInst = valid;
				DI[i].inst.DIArrival = currentCycle+1;
				DI[i].inst.RRDuration = (DI[i].inst.DIArrival)-(DI[i].inst.RRArrival); 
				RR[i].validInst = inValid;
			}
		}
		regPrint(DI,"DI",WIDTH);
	}
	
}
void Rename(void)
{
	//printf("Rename\n");
	if(ROBHasSpace())
	{
		if(isEmpty(RR,0,"RR"))
		{
			for(i = 0; i< WIDTH; i++)
			{
				if(RN[i].validInst)
				{
					ROB[robTail] = RN[i];
								
					if(ROB[robTail].inst.src1 != -1)
					{
						if( RMT[ROB[robTail].inst.src1].where == inROB )
						{
							RN[i].inst.src1 = RMT[ROB[robTail].inst.src1].tag;//src1 renamed
						}
					}
					if(ROB[robTail].inst.src2 != -1)
					{
						if( RMT[ROB[robTail].inst.src2].where == inROB )
						{
							(RN[i].inst.src2) = RMT[ROB[robTail].inst.src2].tag;//src2 renamed
						}
					}
					if(ROB[robTail].inst.dst != -1)
					{
						RMT[ROB[robTail].inst.dst].where = inROB;//renamed destination	
						RMT[ROB[robTail].inst.dst].tag = (robTail+100);
						RN[i].inst.dst = robTail+100;
					}
					else
					{
						RN[i].inst.dst = robTail+100;
					}
					RR[i] = RN[i];
					RR[i].validInst = valid;
					RR[i].inst.RRArrival = currentCycle+1;
					RR[i].inst.RNDuration = (RR[i].inst.RRArrival)-(RR[i].inst.RNArrival); 
					RN[i].validInst = inValid;
				
					if(robTail == (ROB_SIZE-1) )
					{
						robTail = 0;
					}
					else
					{
						robTail ++;
					}
				}
			}
			regPrint(RR,"RR",WIDTH);
		}
	}
	
}

void Decode(void)
{
	//printf("Decode\n");
	if(isEmpty(RN,0,"RN"))
	{
		for(i = 0; i< WIDTH; i++)
		{
			if(DE[i].validInst)
			{
				RN[i] = DE[i];
				RN[i].validInst = valid;
				RN[i].inst.RNArrival = currentCycle+1;
				RN[i].inst.DEDuration = (RN[i].inst.RNArrival) - (RN[i].inst.DEArrival);
				DE[i].validInst = inValid;
			}
		}
		
		regPrint(RN,"RN",WIDTH);
	}
}

/*void Fetch(void)
{
	//printf("Fetch\n");
	if(isEmpty(DE,0,"DE"))
	{	
		for(i = 0; i< WIDTH; i++)
		{
			if(FE[i].validInst)
			{
				DE[i] = FE[i];
				DE[i].validInst = valid;
				DE[i].inst.DEArrival = currentCycle;
				DE[i].inst.FEDuration = (DE[i].inst.DEArrival) - (DE[i].inst.FEArrival);
				FE[i].validInst = inValid;
			}
		}
	regPrint(DE,"DE",WIDTH);
	}
}*/
void Fetchpart1(void)
{
	//printf("Fetchpart1\n");
	if(isEmpty(DE,0,"DE"))
	{
		for(i = 0; i< WIDTH; i++)
		{
			if(NULL != fgets(line,sizeof(line),traceFile))
			{
				dynamicIC++;
				strtag = strtok(line," ");
				if(strtag!=NULL)
				{
					sscanf(strtag,"%x",&tag);
					//printf("tag %d = %x\n",i,tag);
					//printf("\t%d = %x",i,tag);
				}	
				
				strlatency = strtok(NULL," ");
				if(strlatency!=NULL)
				{
					sscanf(strlatency,"%d",&latency);
					//printf("latency %d = %d\n",i,latency);
					//printf("\t%d = %d",i,latency);
				}					
				
				strdst = strtok(NULL," ");
				if(strdst!=NULL)
				{
					sscanf(strdst,"%d",&dst);
					//printf("dst %d = %d\n",i,dst);
					//printf("\t%d = %d",i,dst);
				}		
				
				strsrc1 = strtok(NULL," ");
				if(strsrc1!=NULL)
				{
					sscanf(strsrc1,"%d",&src1);
					//printf("src1 %d = %d\n",i,src1);
					//printf("\t%d = %d",i,src1);
				}				
				
				strsrc2 = strtok(NULL," ");
				if(strsrc2!=NULL)
				{
					sscanf(strsrc2,"%d",&src2);
					//printf("src2 %d = %d\n",i,src2);
					//printf("\t%d = %d\n",i,src2);
				}				
			
				
				DE[i].validInst = valid;
				DE[i].inst.tag = tag;
				DE[i].inst.dst = dst;
				DE[i].inst.dstspl = dst;
				DE[i].inst.dstReady = notReady;
				DE[i].inst.src1 = src1;
				DE[i].inst.src1spl = src1;
				DE[i].inst.age = age;
				age++;
				if(src1 == -1)
				{
					DE[i].inst.src1Ready = ready;
				}
				else
				{
					DE[i].inst.src1Ready = notReady;				
				}
				DE[i].inst.src2 = src2;
				DE[i].inst.src2spl = src2;
				if(src2 == -1)
				{
					DE[i].inst.src2Ready = ready;
				}
				else
				{
					DE[i].inst.src2Ready = notReady;				
				}
				DE[i].inst.FEArrival = currentCycle;
				DE[i].inst.FEDuration = 1;
				DE[i].inst.DEArrival = currentCycle+1;
				
				switch(latency)
				{
					case 0:
						DE[i].inst.latency = 1;
						DE[i].inst.lat = 0;
					break;
					case 1:
						DE[i].inst.latency = 2;
						DE[i].inst.lat = 1;
					break;
					case 2: 
						DE[i].inst.latency = 5;
						DE[i].inst.lat = 2;
					break;
				}
				//regPrint(instInStage *X,char *STR,int size)
				
				
			}
		}
		//if(NULL != fgets(line,sizeof(line),traceFile));
		regPrint(DE,"DE",WIDTH);
	}
	currentCycle++;
}
