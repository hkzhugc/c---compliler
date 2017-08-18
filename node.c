#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>//变长参数函数所需的头文件
#include "node.h"
extern char * yytext;
extern int yylineno;
extern struct _symstack SymStack;
extern int lvl;
int str2Int(char * str)
{
	int res = 0;
	int radix;
	if(*str != '0')//DEC
	{
		radix = 10;
	}
	else if(str[1] == 'x'||str[1] == 'X')//HEX
	{
		radix = 16;
		str += 2;
	}
	else
	{
		radix = 8;
		str += 1;
	}
	while(*str)
	{
		int remain;
		if(*str <='9' && *str >= '0') remain = *str - '0';
		else  remain = (*str | 0x20) - 'a' + 10;
		res = res * radix +remain;
		str++;
	}
	return res;
}

Node * mkNode(int type, char * name, int argnums, ...)
/*argnums为0时，表示是终结符，参数是行号，为-1表示为空规约，正数表示参数是node*非终结符，且
变长参数中参数的个数，变长参数中保存的是该node的儿子*/
{
	va_list valist;
	Node* node = (Node*)malloc(sizeof(Node));
	node->child = NULL;
	node->bros = NULL;
	Node* temp  = (Node*)malloc(sizeof(Node));;
	node->type = type;//
	node->name = name;
	node->expType = -1;
	printf(" %s :", node->name);
	if(node->pExpType) exit(0);
	va_start(valist, argnums);
	if(argnums > 0)//非终结符
	{
		printf(" VN");
		temp = va_arg(valist, Node *);
		node->child = temp;
		node->line = temp->line;
		if (argnums > 1)//将node的儿子与他的bros链接成链表
		{
			int i;
			for(i = 0; i < argnums - 1; i++)
			{
				//printf(" %s\n", node->name);
				temp->bros = va_arg(valist, Node *);
				temp = temp->bros;
			}
		}
	}
	else if(argnums == 0)
	{
		printf(" %s", yytext);
		node->line = va_arg(valist, int);//读取非终结符的行号
		if(node->type == INTVT)
		{
			node->intval = str2Int(yytext);
			//printf(" %d", node->intval);
		}
		else if (node->type == FLTVT)
		{
			node->fltval = atof(yytext);
			//printf(" %f", node->fltval);
		}
		else if (node->type == IDVT)
		{
			char * tmp = (char *)malloc(sizeof(char)*40);
			strcpy(tmp, yytext);
			node->idname = tmp;
			//printf(" %s", node->idname);
		}
		else
		{
			//printf(" %s", yytext);
		}
	}
	else
	{
		printf(" VN");
		node->line = -1;//表示为空规约
	}
	printf("\n");
	return node;
}

void printTree(Node *node, int lvl)
{
	if(node && node->line > 0)
	{
		int i;
		for(i = 0; i < lvl; i++)
		{
			printf("  ");
		}
		printf("%s  %d", node->name, node->expType);
		switch(node->type)
		{
			case INTVT:
				if(!strcmp(node->name,"DEC"))
					printf(": %d\n", node->intval);
				else if(!strcmp(node->name,"HEX"))
					printf(": 0x%x\n", node->intval);
				else 
					printf(": 0%o\n", node->intval);
				break;
			case IDVT:
				printf(": %s\n", node->idname);
				break;
			case FLTVT:
				printf(": %f\n", node->fltval);
				break;
			case VN:
				printf("  (%d)\n",node->line);
				break;
			case OTHERVT:
				printf("  \n");
				break;
		}
	}
	if(node)
	{
		printTree(node->child, lvl + 1);
		//printf("print bros\n");
		printTree(node->bros, lvl);
	}
		
}

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

void addId(Node *syntaxnode)
{
	//printf("nihao1\n");
	IdNode *temp = findId(syntaxnode->idname);
	if(temp)
	{
		printf("repeat declaration at line %d\n", syntaxnode->line);
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

void printfSymTab()
{
	SymNode *symnode;
	symnode = SymStack.Top->SymTab->Head;
	printf("printfSymTab: level %d\n",lvl);
	for(;symnode;symnode = symnode->next)
	{
		printf("name:%s, type:%d, pType:%d\n ", symnode->node->idname, symnode->node->idtype, symnode->node->pType);
	}
}

void addType(Node *syntaxnode)
{
	if(syntaxnode)
	{
		if(syntaxnode->expType!= -1 && syntaxnode->type == IDVT)
		{
			printf("read to findId\n");
			IdNode *temp = findId(syntaxnode->idname);
			printf("end findId\n");
			if(temp)
			{
				temp->idtype = syntaxnode->expType;
				temp->pType = syntaxnode->pExpType;
			}
		}
		//printf("nihao1\n");
		addType(syntaxnode->child);
		addType(syntaxnode->bros);
	}

}

FunType * makeFuncType(int returnType, Node *VarList)
{
	FunType * funType = (FunType *)malloc(sizeof(FunType));
	funType->DefReturnType = returnType;
	funType->argListHead = (ArgList *)malloc(sizeof(ArgList));
	funType->argListHead->thisArgType = -1;
	funType->argListHead->next = NULL;
	funType->ArgNums = makeFuncArg(funType->argListHead, VarList);
	//printFuncArg(funType->argListHead, 1);
	return funType;
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

void pushSymTab()
{
	lvl++;
	SymTabStackNode * temp = (SymTabStackNode *)malloc(sizeof(SymTabStackNode));
	temp->level = lvl;
	temp->SymTab = (_SymTab *)malloc(sizeof(_SymTab));
	temp->next = SymStack.Top;
	SymStack.Top = temp;
}

void popSymTab()
{
	lvl--;
	SymTabStackNode * temp = SymStack.Top;
	SymStack.Top = SymStack.Top->next;
	free(temp);
}

StructType * symTab2StructTab(char * OptName)
{
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

