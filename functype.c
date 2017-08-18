
void makeFuncType(Node *ID, Node *VarList)
{
	IdNode * funcIdNode;
	FunType * funType = (FunType *)malloc(sizeof(FunType));
	funType->argListHead = (ArgList *)malloc(sizeof(ArgList));
	funType->argListHead->thisArgType = -1;
	funType->argListHead->next = NULL;
	printf("xixi1\n");
	funType->ArgNums = makeFuncArg(funType->argListHead, VarList);
	ID->expType = FUNC;
	ID->pExpType = (int)funType;
	funcIdNode =  findIdExist(ID->idname);
	funcIdNode->idtype = FUNC;
	funcIdNode->pType = (int)funType;
	//printFuncArg(funType->argListHead, 1);
}

void initSysFunc()
{
	Node * readFuncId = mkNode(IDVT, "ID", -1, 0);
	readFuncId->idname = "read";
	addId(readFuncId, 1);
	makeFuncType(readFuncId, NULL);
	FunType * funType = (FunType *)(readFuncId->pExpType);
	funType->DefReturnType = INT;
	funType->codeline = -1;
	Node * writeFuncId = mkNode(IDVT, "ID", -1, 0);
	writeFuncId->idname = "write";
	addId(writeFuncId, 1);
	makeFuncType(writeFuncId, NULL);
	funType = (FunType *)(writeFuncId->pExpType);
	funType->DefReturnType = INT;
	funType->ArgNums = 1;
	funType->codeline = -1;
	funType->argListHead->thisArgType = INT;
	funType->argListHead->next = NULL;
}


void addFuncReturnType(Node *Specifier, Node *FunDec)
{
	FunDec->expType = Specifier->expType;
	IdNode * temp = findIdExist(FunDec->child->idname);
	FunType * funType = (FunType *)(temp->pType);
	printf("1 = %d\n", Specifier->expType);
	printf("2 = %d\n", (int)funType);
		printf("fuck\n");
	funType->DefReturnType = Specifier->expType;
		printf("fuck1\n");
	funType->DefPReturnType = Specifier->pExpType;
		printf("fuck2\n");
	funType->codeline = codecnt + 1;
    defReturnType = funType->DefReturnType;
    defPReturnType = funType->DefPReturnType;
}

int makeFuncArg(ArgList *argListHead,  Node *VarList)
{
	if(VarList == NULL)
		return 0;
	else
	{
		printf("this node is %s\n", VarList->name);
		if(VarList->child)
			printf("this nodechild is %s\n", VarList->child->name);
		if(strcmp(VarList->child->name,"ParamDec"))
		{
			printf("sthwrong\n");
			return 0;
		}
		else
		{
			argListHead->thisArgType = VarList->child->expType;
			printf("sucess\n");
			if(!argListHead->next)
			{
				argListHead->next= (ArgList *)malloc(sizeof(ArgList));
				argListHead->next->thisArgType = -1;
			}
			if(VarList->child->bros)
				return 1 + makeFuncArg(argListHead->next, VarList->child->bros->bros);
			else
				return 1;
		}

	}
}

void printFuncArg(ArgList *argListHead,int num)
{
	if(argListHead&&argListHead->thisArgType!=-1)
	{
		printf("%d arg type is %d\n", num, argListHead->thisArgType);
		printFuncArg(argListHead->next,num+1);
	}
}


