

void transType(Node *SpecifierNode, Node *ExtDecListNode)
{
	//printf("----------------%d-------------\n", SpecifierNode->expType);
	if(SpecifierNode && ExtDecListNode)
	{
		ExtDecListNode->expType = SpecifierNode->expType;
		ExtDecListNode->pExpType = SpecifierNode->pExpType;
		if(ExtDecListNode)
		{
			transType(ExtDecListNode, ExtDecListNode->child);
			if(ExtDecListNode->child && ExtDecListNode->child->bros)
			{
				transType(ExtDecListNode, ExtDecListNode->child->bros->bros);
			}
		}
	}
}

IdNode* findId(char *s)
{
	//printf("nihao2\n");
	SymNode *symnode;
	_SymTab * SymTab = SymStack.Top->SymTab;
	//printf("nihao3\n");
	symnode = SymTab->Head;
	for(;symnode;symnode = symnode->next)
	{
		printf("%s %d\n", symnode->node->idname, lvl);
		if(!strcmp(symnode->node->idname, s))
			return symnode->node;
	}
	return NULL;
}

IdNode * findIdExist(char *s)
{
	_SymTab * SymTab = SymStack.Top->SymTab;
    SymTabStackNode * stackNode = SymStack.Top;
    for(;stackNode;stackNode = stackNode->next)
    {
        SymNode *symnode;
        symnode = stackNode->SymTab->Head;
        for(; symnode; symnode = symnode->next)
        {
            if(!strcmp(s, symnode->node->idname))
            {
            	   printf("%s's dimen is %d\n", s, symnode->node->dimen);
            	   printf("find addr is %d\n", (int)symnode->node);
                return symnode->node;
            }
        }
    }
    return NULL;
}

void addId(Node *syntaxnode, int isFunc)
{
	//printf("nihao1\n");
	IdNode *temp = findId(syntaxnode->idname);
	if(temp)
	{
		if (!isFunc)
			sprintf(error[errorcnt++],"error type 3 at line %d: repeat declaration var %s\n", syntaxnode->line,syntaxnode->idname);
		else
			sprintf(error[errorcnt++],"error type 4 at line %d: repeat declaration function %s\n", syntaxnode->line,syntaxnode->idname);
	}
	else
	{
		_SymTab *SymTab = SymStack.Top->SymTab;//get the stack top
		SymNode *symnode;
		//symnode = SymTab.Head;
		int cnt = 0;
		symnode = (SymNode *)malloc(sizeof(SymNode));
		symnode->node = (IdNode *)malloc(sizeof(IdNode));
		symnode->node->idname = syntaxnode->idname;
		symnode->node->idtype = syntaxnode->expType;
		symnode->node->pType = syntaxnode->pExpType;
        		symnode->node->dimen = 0;
        		if(!isFunc)
        			symnode->node->place = newtemp();
        		else
        			symnode->node->place = -1;
        		//symnode->node->place = newvar();
        		symnode->node->size = 1;
		symnode->next = NULL;
		if(SymTab->Head ==NULL)
		{
			SymTab->Tail = symnode;
			SymTab->Head = symnode;
			printf("fisrt elem\n");
		}
		else
		{
			//printf("name:%s, type:%d\n", symnode->node->idname, symnode->node->idtype);
			SymTab->Tail->next= symnode;
			//printf("name:%s, type:%d\n", SymTab.Tail->node->idname, SymTab.Tail->node->idtype);
			SymTab->Tail =  symnode;
		}
		//printf("nihao2\n");
	}
}

char * lookForIdinVarDec(Node * VarDec)
{
    if(VarDec)
    {
         if(!strcmp(VarDec->name,"ID"))
         {
            return VarDec->idname;
         }
        else
            return lookForIdinVarDec(VarDec->child);
    }
    else
        return NULL;
}
void printfSymTab()
{
	SymNode *symnode;
	symnode = SymStack.Top->SymTab->Head;
	printf("printfSymTab: level %d\n",lvl);
	for(;symnode;symnode = symnode->next)
	{
		printf("name:%s, type:%d, pType:%d, dimen:%d, size:%d, place:t%d\n", 
			symnode->node->idname, symnode->node->idtype, symnode->node->pType, symnode->node->dimen, symnode->node->size,
			symnode->node->place);
	}
}

void addType(Node *syntaxnode)
{
	if(syntaxnode)
	{
		if(syntaxnode->expType!= -1 && syntaxnode->type == IDVT)
		{
			printf("ready to findId\n");
			IdNode *temp = findId(syntaxnode->idname);
			printf("end findId\n");
			if(temp)
			{
				temp->idtype = syntaxnode->expType;
				temp->pType = syntaxnode->pExpType;
				if(temp->idtype == INT)
		        		{
					temp->size *= SIZEOFINT;
					printf("xixi1\n");
				}
				else if(temp->idtype == FLT)
				{
					temp->size *= SIZEOFINT;
					printf("xixi2\n");
				}
		        		else if(temp->idtype == STRT)
				{
					temp->size *= ((StructType *)temp->pType)->size;
		        			printf("xixi3\n");
				}
				else if(temp->idtype == FUNC)
				{
					temp->size *= SIZEOFFUNC;
		        			printf("xixi3\n");
				}
			}
		}
		//printf("nihao1\n");
		addType(syntaxnode->child);
		addType(syntaxnode->bros);
	}
}

void pushSymTab()
{
	lvl++;
	SymTabStackNode * temp = (SymTabStackNode *)malloc(sizeof(SymTabStackNode));
	temp->level = lvl;
	temp->num = 0;
	temp->SymTab = (_SymTab *)malloc(sizeof(_SymTab));
	temp->next = SymStack.Top;
	SymStack.Top = temp;
}

void popSymTab()
{
	lvl--;
	SymTabStackNode * temp = SymStack.Top;
	// freetemp(temp->num);
	SymStack.Top = SymStack.Top->next;
	free(temp);
}




