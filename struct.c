#include "node.h"

StructType * symTab2StructTab(char * OptName)
{
    StructType * isExit = findStructType(OptName);
    if(isExit)
    {
        sprintf(error[errorcnt++],"error type 16 at line %d: repeating declaration of struct %s\n",yylineno,OptName);
        return NULL;
    }
    int offset = 0;
    _SymTab * symTab = SymStack.Top->SymTab;
	SymNode * temp;
	StructType * structNode = (StructType *)malloc(sizeof(StructType));
	structNode->name = OptName;
	structNode->MemNums = 0;
	structNode->structDefListHead = NULL;
	StructDefList * tempStruct;
	for(temp = symTab->Head; temp; temp = temp->next)
	{
		StructDefList * structDefNode = (StructDefList *)malloc(sizeof(StructDefList));
		structDefNode->name = temp->node->idname;
		structDefNode->thisDefType = temp->node->idtype;
		structDefNode->pthisDefType = temp->node->pType;
		structDefNode->dimen = temp->node->dimen;
		structDefNode->offset = offset;
		if(structDefNode->thisDefType == STRT)
		{
			offset += ((StructType *)structDefNode->pthisDefType)->size;
		}
		else if(structDefNode->thisDefType == INT)
		{
			offset += temp->node->size;
		}
		else if(structDefNode->thisDefType == FLT)
		{
			offset += temp->node->size;
		}
		structNode->MemNums++;
		if(structNode->structDefListHead == NULL)
		{
			structNode->structDefListHead = structDefNode;
			tempStruct = structDefNode;
		}
		else
		{
			tempStruct-> next = structDefNode;
			tempStruct = tempStruct->next;
		}
	}
	structNode->size = offset;
	if(StructTab.structTagTabHead == StructTab.structTagTabTail)
	{
		StructTab.structTagTabHead = StructTab.structTagTabTail = structNode;
		StructTab.structTagTabTail = StructTab.structTagTabTail->next;
	}
	else
	{
		StructTab.structTagTabTail = structNode;
		StructTab.structTagTabTail = StructTab.structTagTabTail->next;
	}
	return structNode;
}

void printfStructOptTab()
{
	StructType * structType = StructTab.structTagTabHead;
	for(; structType; structType = structType->next)
	{
		printf("this stuct name is %s has %d Members\n", structType->name, structType->MemNums);
		StructDefList * structDefList = structType->structDefListHead;
		int cnt = 1;
		for(;structDefList; structDefList = structDefList->next)
		{
			printf("%d members name is %s\n", cnt, structDefList->name);
			printf("type is %d pType is %d\n", structDefList->thisDefType, structDefList->pthisDefType);
		}
	}
}

StructType * findStructType(char * TagName)
{
    StructType * temp = StructTab.structTagTabHead;
    printf("TagName is %s",TagName);
    for(; temp; temp = temp->next)
    {
        printf("name is %s\n",temp->name);
        if(!strcmp(TagName, temp->name))
        {
            printf("find it!\n");
            return temp;
        }
    }
    return NULL;
}

StructDefList * findIdinStruct(StructDefList *structDefListHead, char * s)
{
	StructDefList * DefList = structDefListHead;
	printf("wori\n");
	for(;DefList; DefList = DefList->next)
	{
		if(!strcmp(s, DefList->name))
		{
			return DefList;
		}
	}
	return NULL;
}