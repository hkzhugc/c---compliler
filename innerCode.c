int newtemp()
{
	return t++;
}

void freetemp(int num)
{
	t -= num;
}

void merge(List * dst, List * src1, List * src2)
{
	int i = 0;
	int j = 0;
	for(; ; i++)
	{
		if(src1->codeNum[i] != -1)
		{
			dst->codeNum[i] = src1->codeNum[i];
		}
		else if(src2->codeNum[j] != -1)
		{
			dst->codeNum[i] = src2->codeNum[j];
			j++;
		}else
		{
			break;
		}
	}
	i = 0;
	j = 0;
	for(; ; i++)
	{
		if(src1->TACNum[i] != -1)
		{
			dst->TACNum[i] = src1->TACNum[i];
		}
		else if(src2->TACNum[j] != -1)
		{
			dst->TACNum[i] = src2->TACNum[j];
			j++;
		}else
		{
			break;
		}
	}
}

void backPatch(List * dst, int gotoStm)
{
	int i = 0;
	for(; dst->codeNum[i] != -1; i++)
	{
		int j = 0;
		int index = dst->codeNum[i];
		printf("%s\n", code[index]);
		for(; code[index][j] != '_' && code[index][j]; j++);
		sprintf(&code[index][j], "%d\n", gotoStm);
	}
	i = 0;
	for(; dst->TACNum[i] != -1; i++)
	{
		int j = 0;
		TAC[dst->TACNum[i]].dst.oprandVal = gotoStm;
		TAC[dst->TACNum[i]].dst.oprandType = 0;
	}
}


void backPatchTAC(List * dst, int gotoStm)
{
    	int i = 0;
	for(; dst->TACNum[i] != -1; i++)
	{
		TAC[dst->TACNum[i]].dst.oprandVal = gotoStm;
		TAC[dst->TACNum[i]].dst.oprandType = 0;
	}
}

List * initList()
{
    List * temp = (List * )malloc(sizeof(List));
    int i = 0;
    for(;i < 100; i++)
    {
        temp->codeNum[i] = -1;
        temp->TACNum[i] = -1;
    }
    return temp;
}

BoolList * initBoolList()
{
	BoolList  * temp = (BoolList * )malloc(sizeof(BoolList));
	temp->trueList = initList();
	temp->falseList = initList();
	return temp;
}

void makeList(List * list, int i)
{
    list->codeNum[0] = i;
}

void makeTACList(List * list, int i)
{
    list->TACNum[0] = i;
}

void makeTAC(char * opName, Oprand  * dst, Oprand  * src1, Oprand  * src2)
{
	TAC[TACcnt].opName = opName;
	//name
	if(dst)
	{
		TAC[TACcnt].dst.oprandVal = dst->oprandVal;
		TAC[TACcnt].dst.oprandType = dst->oprandType;
		TAC[TACcnt].dst.oprandIsAddr = dst->oprandIsAddr;
	}
	else
	{
		TAC[TACcnt].dst.oprandVal = -1;
		TAC[TACcnt].dst.oprandType = -1;
		TAC[TACcnt].dst.oprandIsAddr = -1;
	}
	//dst
	if(src1)
	{
		TAC[TACcnt].src1.oprandVal = src1->oprandVal;
		TAC[TACcnt].src1.oprandType = src1->oprandType;
		TAC[TACcnt].src1.oprandIsAddr = src1->oprandIsAddr;
	}
	else
	{
		TAC[TACcnt].src1.oprandVal = -1;
		TAC[TACcnt].src1.oprandType = -1;
		TAC[TACcnt].src1.oprandIsAddr = -1;
	}
	//sr1
	if(src2)
	{
		TAC[TACcnt].src2.oprandVal = src2->oprandVal;
		TAC[TACcnt].src2.oprandType = src2->oprandType;
		TAC[TACcnt].src2.oprandIsAddr = src2->oprandIsAddr;
	}
	else
	{
		TAC[TACcnt].src2.oprandVal = -1;
		TAC[TACcnt].src2.oprandType = -1;
		TAC[TACcnt].src2.oprandIsAddr = -1;
	}
	//sr2
	TACcnt++;
}

/*void printFuncArg2Code(ArgList *argListHead)
{
	if(argListHead&&argListHead->thisArgType!=-1)
	{
		sprintf(code[codecnt++], "Param t%d\n", num, argListHead->thisArgType);
		printFuncArg(argListHead->next,num+1);
	}
}*/