#ifndef __NODE_H__
#define __NODE_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>//变长参数函数所需的头文件

#define SIZEOFINT 4
#define SIZEOFFLT 4
#define SIZEOFFUNC 4
extern char * yytext;
extern int yylineno;
extern struct _symstack SymStack;
extern int lvl;
extern int errorcnt;
extern int codecnt;
extern int TACcnt;
extern int LABLEcnt;
extern int defReturnType;
extern int defPReturnType;
extern int isReturn;
extern int t;
extern FILE * dstCode;
char error[100][100];
char code[100][100];
enum{
	VN = 0,
	INTVT,
	IDVT,
	FLTVT,
	OTHERVT
};

enum{
	INT = 0,
	FLT,
	STRT,
	FUNC
};

typedef struct _list
{
	int codeNum[100];//是第几行的代码
	int TACNum[100];//是第几行的TAC
}List;

typedef struct _boolList
{
    List * trueList;
    List * falseList;
}BoolList;


typedef struct _argList
{
	int thisArgType;
	int pthisArgType;//if it's struct or func ,pType will point to the addr of the type
	struct _argList * next;
}ArgList;

typedef struct _structDefList
{
	char *name;//save the def name
	int thisDefType;
	int pthisDefType;//if it's struct or func ,pType will point to the addr of the type
	int dimen;
	int offset;
	struct _structDefList * next;
}StructDefList;

typedef struct _funtype
{
	int DefReturnType;
	int DefPReturnType;
	int ReturnType;
	int codeline;//store code line
	int ArgNums;
	ArgList *argListHead;
}FunType;

typedef struct _structType
{
	char *name;//save the structtypeTag name
	int MemNums;//save the stucttype member NO.
	int size;//the struct size
	StructDefList *structDefListHead;//save the stucttype membertype
	struct _structType * next;
}StructType;

struct _structTab
{
	StructType * structTagTabHead;
	StructType * structTagTabTail;
}StructTab;

typedef struct _oprand
{
	int oprandType;//1=>var , 0 => const
	int oprandIsAddr;//1 => is addr 0 => not addr
	int oprandVal;
}Oprand;

typedef struct node{
	int type;//分成终结符和非终结符
	char *name;//非终结符和终结符的名字
	struct node* child;//保存儿子节点
	struct node* bros;//保存兄弟节点
	int line;//保存行号
	union{
		char* idname;//若是id，则保存id的名字
		int intval;//若是int，则保存int的值
		float fltval;//保存float的值
	};
	int expType;
	int pExpType;//if it's struct or func ,pType will point to the addr of the type
    int dimen;//表示维度，为1为1维数组，其余同理;
    int isLeftVal;//is left value
    int place;//where to store the exp or the num val
    int codeLine;//code line
    char code[100];
    int oprandType;//id => 1, num => 0
    Oprand oprand;
    union{
        BoolList * boolList;
        List * nextList;
    };
}Node;

typedef struct _idnode
{
	char * idname;
	int idtype;
	int pType;//if it's struct or func ,pType will point to the addr of the type
	int dimen;//维度，一维数组该数值为1
	int place;//where to store the id
	int size;//size
}IdNode;

typedef struct _symnode
{
	IdNode * node;
	struct _symnode * next;
}SymNode;

typedef struct symtab
{
 	SymNode* Head;
 	SymNode* Tail;
 }_SymTab;

typedef struct _symtabstacknode
{
	int level;
	_SymTab * SymTab;
	int num;
	struct _symtabstacknode * next;
}SymTabStackNode;

struct _symstack
{
	SymTabStackNode * Top;
}SymStack;

struct
{
	char * opName;
	Oprand dst;
	Oprand src1;
	Oprand src2;
}TAC[100];

enum
{
	TEMP = 0,
	VAR,
	PARAM,
	Return
};

struct 
{
	struct
	{
		int type;//temp, or var or param
		int addr;//if var or param, show the dstCode reg num or addr
	}typeTab[100];
	int varNum;
	int paramNum;
	int tempNum;
	int argNum;
	int isPush;
	int compstVarNumStack[100];
	int stackTop;
}funcAc;


void addId(Node *syntaxnode, int isFunc);

IdNode* findId(char *s);

void pushSymTab();

void popSymTab();

IdNode * findIdExist(char *s);

Node * mkNode(int type, char * name, int argnums, ...);

void printTree(Node * root, int lvl);

void printfSymTab();

void addType(Node *syntaxnode);

void makeFuncType(Node *ID, Node *VarList);

void addFuncReturnType(Node *Specifier, Node *FunDec);

int makeFuncArg(ArgList *argListHead,  Node *VarList);

StructType *  symTab2StructTab(char * OptName);

StructType *  findStructType(char * TagName);

StructDefList * findIdinStruct(StructDefList *structDefListHead, char * s);

void printFuncArg(ArgList *argListHead,int num);

void printfStructOptTab();

char * lookForIdinVarDec(Node * VarDec);

void checkTypeConflict1(Node *root, Node *left, Node *right);

void checkTypeConflict2(Node *root, Node *left, Node *op, Node *right);

void checkTypeConflict3(Node * root, Node * op, Node * left);

void checkTypeConflict4(Node *root, Node *left, Node *op, Node *right);

void checkTypeConflictForAndOr(Node *root, Node *left, Node *op, Node *right);

void checkArray(Node *root, Node *Exp, Node *index);

void checkStruct(Node * root, Node * Struct, Node * StructId);

void checkFunctionConflict1(Node * root, Node * ID, Node * Args);

int checkArgs(FunType * funType, Node * Args);

void checkID(Node * root, Node * ID);

void checkFuncReturnType(Node * returnExp);

void freetemp(int num);

int newVar();

int newtemp();

void merge(List * dst, List * src1, List * src2);

void backPatch(List * dst, int gotoStm);

void backPatchTAC(List * dst, int gotoStm);

List * initList();

BoolList * initBoolList();

void makeList(List * list, int i);

void makeTAC(char * opName, Oprand  * dst, Oprand  * src1, Oprand  * src2);

void makeTACList(List * list, int i);

void initSysFunc();

void makeDstCode();

void subStack();

void addStack();

void changeAc(int index, int type, int addr);

void lw(int varAddr, int tempRegindex, int regType);

void sw(int varAddr, int tempRegindex, int regType);

void makeOneCode(int i);

void li(int varAddr, int constVal);

void push(int tempRegindex, int regType);

void pop(int tempRegindex, int regType);

void initCode();
#endif
