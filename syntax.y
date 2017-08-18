%{
#include <stdio.h>
#include <unistd.h>
#include "node.h"
#include "lex.yy.c"
int flag = 1;
Node *temp;	
%}
%union{
	Node* node;
}
%token <node> SEMI COMMA ASSIGNOP RELOP ADD SUB MUL DIV AND OR DOT  NOT LP RP LB RB LC RC
%token <node> TYPE_INT TYPE_FLOAT STRUCT RETURN IF ELSE THEN WHILE HEX OCT DEC ID FLOAT
%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FuncDec VarList
ParamDec CompSt StmtList  Stmt DefList Def DecList Dec Exp Args INT M N
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left ADD SUB
%left MUL DIV
%right NOT
%left LB RB LC RC LP RP DOT
%nonassoc ELSE
%%

Program : 
	ExtDefList
    { 
		$$ = mkNode(VN, "Program", 1, $1);
		if(flag) printf("BEGIN TO PRINT TREE:\n");  
		if(flag) printTree($$, 0);
		//addType($$);
		printfSymTab();
        int i;
        if(errorcnt)
        {
        	printf("\nthere is %d error:\n\n", errorcnt);
            for(i = 0; i < errorcnt; i++)
            {
                printf("%s",error[i]);
            }
        }
        if(codecnt && !errorcnt)
        {
        	printf("\nthere is %d code:\n\n", codecnt);
            for(i = 0; i < codecnt; i++)
            {
                printf("%4d : %s", i + 1, code[i]);
            }
            for(i = 0; i < TACcnt; i++)
            {
            	char tempdst = '#', tempsrc1 = '#', tempsrc2 = '#';
            	if(TAC[i].dst.oprandType) tempdst = 't';
            	if(TAC[i].src1.oprandType) tempsrc1 = 't';
            	if(TAC[i].src2.oprandType) tempsrc2 = 't';
            	if(TAC[i].dst.oprandVal == -1) tempdst = ' ';
            	if(TAC[i].src1.oprandVal == -1) tempsrc1 = ' ';
            	if(TAC[i].src2.oprandVal == -1) tempsrc2 = ' ';
            	  printf("%4d : %s\t %c%d\t%c%d\t%c%d \n", i +1, TAC[i].opName, 
            	  	tempdst, TAC[i].dst.oprandVal, tempsrc1, TAC[i].src1.oprandVal, tempsrc2, TAC[i].src2.oprandVal);
            }
        }
        makeDstCode();
	}
	;

ExtDefList : 
	ExtDef ExtDefList { $$ = mkNode(VN, "ExtDefList", 2, $1, $2);}
	| { $$ = mkNode(VN, "ExtDefList", -1);}
	;

ExtDef : 
	Specifier ExtDecList SEMI 
	{ 
		$$ = mkNode(VN, "ExtDef", 3, $1, $2, $3);
		//printf("ready to transType\n");
		transType($1, $2);
		//printf("ready to addType\n");
		addType($$);//注意，如果是结构的话，这里已经pop过一次符号表，因此在Specifier中的ID不在符号表中
		//printf("end addType\n");
	}
	| Specifier SEMI { $$ = mkNode(VN, "ExtDef", 2, $1, $2);}
	| Specifier FuncDec 
	{
		addFuncReturnType($1, $2);
		//sprintf(code[codecnt++], "func ---%s--- begin\n", $2->child->idname);
	} 
	CompSt 
	{ 
		$$ = mkNode(VN, "ExtDef", 3, $1, $2, $4);
		if(!isReturn)
		    sprintf(error[errorcnt++], "error type 8 at line %d : return type conflict(no return type)\n", yylineno);
		isReturn = 0;
		printfSymTab();popSymTab();
	}
	;

ExtDecList : 
	 VarDec { $$ = mkNode(VN, "ExtDecList", 1, $1);}
	| VarDec COMMA ExtDecList  { $$ = mkNode(VN, "ExtDecList", 3, $1, $2, $3);}
	;

Specifier : 
	TYPE_INT 
	{ 
		$$ = mkNode(VN, "Specifier", 1, $1);
		($$)->expType = INT;
	}
	| TYPE_FLOAT 
	{ 
		$$ = mkNode(VN, "Specifier", 1, $1);
		($$)->expType = FLT;
	}
	| StructSpecifier 
	{ 
		$$ = mkNode(VN, "Specifier", 1, $1);
		($$)->expType = STRT;
		$$->pExpType = $1->pExpType;
	}
	;

StructSpecifier : 
	STRUCT OptTag LC {pushSymTab();} DefList RC 
	{ 
		$$ = mkNode(VN, "StructSpecifier", 5, $1, $2, $3, $5, $6);
		/*cod*/ //code to trans symTab to StructType and add it to structStructTab
		printfSymTab();
		if($2->child)
			$$->pExpType = (int)symTab2StructTab($2->child->idname);
		else
			$$->pExpType = (int)symTab2StructTab(NULL);
		freetemp(SymStack.Top->num);
		popSymTab();
		printfStructOptTab();
	}
	| STRUCT Tag 
	{ 
		$$ = mkNode(VN, "StructSpecifier", 2, $1, $2);
		$$->pExpType = (int)findStructType($2->child->idname);
		if(!$$->pExpType)
			sprintf(error[errorcnt++],"error type 17 at line %d: undefined struct %s\n", yylineno, $2->child->idname);
	}
	;

OptTag : 
	ID { $$ = mkNode(VN, "OptTag", 1, $1);}
	| { $$ = mkNode(VN, "OptTag", -1);}
	;

Tag : 
	ID
	{ 
	    $$ = mkNode(VN, "Tag", 1, $1);
	}
	;

VarDec : 
	ID 
	{ 
		$$ = mkNode(VN, "VarDec", 1, $1); 
		addId($1, 0); 
		IdNode *temp = findIdExist($1->idname); 
		$$->place = temp->place;
		$$->oprand.oprandType = 1;
		$$->oprand.oprandVal = temp->place;
	}
	| VarDec LB INT RB
	{
		$$ = mkNode(VN, "VarDec", 4, $1, $2, $3, $4);
		IdNode * temp = findIdExist(lookForIdinVarDec($$));
		temp->dimen++;
		//printf("%d\n", temp->size);
		temp->size *= $3->child->intval;
		$$->place = $1->place;
	}
	;


FuncDec : 
	ID LP 
	{
		addId($1, 1); 
		char * temp = ((char *)malloc(sizeof(char) * 100));
		sprintf(temp, "func %s", $1->idname);
		makeTAC(temp, NULL, NULL, NULL);
		sprintf(code[codecnt++], "func ---%s--- begin\n", $1->idname); 
		pushSymTab();
	} 
	VarList RP 
	{
		printf("make a FuncDec node\n" );
		$$ = mkNode(VN, "FuncDec", 4, $1, $2, $4, $5);
		makeFuncType($1, $4);
		printf("the Func has %d args\n", ((FunType *)($1->pExpType))->ArgNums);
		printFuncArg(((FunType *)($1->pExpType))->argListHead,1);
		//printFuncArg2Code(((FunType *)($1->pExpType))->argListHead);
		//printfSymTab();
	}
	| ID LP 
	{
		addId($1, 1); 
		char * temp = ((char *)malloc(sizeof(char) * 100));
		sprintf(temp, "func %s", $1->idname);
		makeTAC(temp, NULL, NULL, NULL);
		sprintf(code[codecnt++], "func ---%s--- begin\n", $1->idname); 
		pushSymTab();
	}  
	RP 
	{ 
		//printf("make a FuncDec node\n" );
		$$ = mkNode(VN, "FuncDec", 3, $1, $2, $4);
		makeFuncType($1, NULL);
		printf("the Func has %d args\n", ((FunType *)($1->pExpType))->ArgNums);	
		printFuncArg(((FunType *)($1->pExpType))->argListHead,1);
		//printfSymTab();
	}
	;

VarList : 
	ParamDec COMMA VarList { $$ = mkNode(VN, "VarList", 3, $1, $2, $3);}
	| ParamDec { $$ = mkNode(VN, "VarList", 1, $1);}
	;

ParamDec : 
	Specifier VarDec 
	{ 
		transType($1, $2);
		addType($2);
		$$ = mkNode(VN, "ParamDec", 2, $1, $2);
		$$->expType = $2->expType = $1->expType;
		Oprand temp;
		temp.oprandType = 1;
		temp.oprandVal = $2->place;
		makeTAC("Param", &$2->oprand, NULL, NULL);
		sprintf(code[codecnt++], "Param t%d\n", $2->place);
	}
	;

CompSt : 
	LC 
	{
		pushSymTab();
		makeTAC("LC", NULL, NULL, NULL);
	} 
	DefList StmtList RC 
	{ 
		$$ = mkNode(VN, "CompSt", 4, $1, $3, $4, $5); 
		makeTAC("RC", NULL, NULL, NULL);
		printfSymTab();
		popSymTab();
	}
	;

StmtList : 
	Stmt StmtList { $$ = mkNode(VN, "StmtList", 2, $1, $2);}
	| { $$ = mkNode(VN, "StmtList", -1);}
	;

M :
	{ 
		$$ = mkNode(VN, "M", -1); 
		$$->place = LABLEcnt; 
		sprintf(code[codecnt++], "LABLE%d:\n",LABLEcnt++);
		Oprand temp;
		temp.oprandType = 0;
		temp.oprandVal = $$->place;
		makeTAC("LABLE", &temp, NULL, NULL);
	};

N :
	{ 
		$$ = mkNode(VN, "N", -1); 
		makeList($$->nextList, codecnt); 
		makeTACList($$->nextList, TACcnt); 
		makeTAC("jmp", NULL, NULL, NULL);
		sprintf(code[codecnt++], "goto LABLE_\n");};

Stmt : 
	Exp SEMI 
	{ 
		$$ = mkNode(VN, "Stmt", 2, $1, $2);
		sprintf(code[codecnt++], "one stmt end\n");
		makeTAC("Stmt", NULL, NULL, NULL);
	}
	| CompSt { $$ = mkNode(VN, "Stmt", 1, $1);}
	| RETURN Exp SEMI
	{
		$$ = mkNode(VN, "Stmt", 3, $1, $2, $3);
		checkFuncReturnType($2);
		makeTAC("RETURN", &($2->oprand), NULL, NULL);
		char temp = '#';
		if($2->oprand.oprandType) temp = 't';
		sprintf(code[codecnt++], "RETURN %c%d\n", temp,$2->oprand.oprandVal);
		sprintf(code[codecnt++], "func end\n\n");
	}
	| IF LP Exp RP THEN M Stmt 
	{ 
		$$ = mkNode(VN, "Stmt", 5, $1, $2, $3, $4, $5, $7);
		backPatch($3->boolList->trueList, $6->place);
		backPatch($3->boolList->falseList, LABLEcnt);
		Oprand temp;
		temp.oprandType = 0;
		temp.oprandVal = LABLEcnt;
		makeTAC("LABLE", &temp, NULL, NULL);
		sprintf(code[codecnt++], "LABLE%d:\n",LABLEcnt++);
		merge($$->nextList, $3->boolList->falseList, $7->nextList);
	}
	| IF LP Exp RP THEN M Stmt  N ELSE  M Stmt  
	{ 
    		$$ = mkNode(VN, "Stmt", 7, $1, $2, $3, $4, $5, $7, $9, $11);
    		backPatch($8->nextList, LABLEcnt);
    		Oprand temp;
    		temp.oprandType = 0;
		temp.oprandVal = LABLEcnt;
		makeTAC("LABLE", &temp, NULL, NULL);
    		sprintf(code[codecnt++], "LABLE%d:\n",LABLEcnt++);
    		backPatch($3->boolList->trueList, $6->place);
		backPatch($3->boolList->falseList, $10->place);
	}
	| WHILE LP M Exp RP  M Stmt N
	{ 
		$$ = mkNode(VN, "Stmt", 5, $1, $2, $4, $5, $7);
		backPatch($8->nextList, $3->place);
		backPatch($4->boolList->trueList, $6->place);
		backPatch($4->boolList->falseList, LABLEcnt);
		Oprand temp;
		temp.oprandType = 0;
		temp.oprandVal = LABLEcnt;
		makeTAC("LABLE", &temp, NULL, NULL);
		sprintf(code[codecnt++], "LABLE%d:\n",LABLEcnt++);
	}
	| error SEMI {flag  =  0; }
	;

DefList : 
	Def DefList { $$ = mkNode(VN, "DefList", 2, $1, $2);}
	| { $$ = mkNode(VN, "DefList", -1);}
	;

Def : 
	Specifier DecList SEMI 
	{ 
		$$ = mkNode(VN, "Def", 3, $1, $2, $3);
		transType($1, $2);
		addType($$);
		sprintf(code[codecnt++], "one stmt end\n");
		makeTAC("Stmt", NULL, NULL, NULL);
	}
	;

DecList : 
	Dec { $$ = mkNode(VN, "DecList", 1, $1);}
	| Dec COMMA DecList { $$ = mkNode(VN, "DecList", 3, $1, $2, $3);}
	;

Dec : 
	VarDec 
	{ 
		$$ = mkNode(VN, "Dec", 1, $1);
		Oprand temp1;
		temp1.oprandType = 1;
		temp1.oprandVal = $1->place;
		makeTAC("New Var", &temp1, NULL,NULL);
	}
	| VarDec ASSIGNOP Exp  
	{ 
		$$ = mkNode(VN, "DecList", 3, $1, $2, $3);
		Oprand temp1;
		temp1.oprandType = 1;
		temp1.oprandVal = $1->place;
		makeTAC("New Var", &temp1, NULL,NULL);
		Oprand temp2;
		temp2.oprandType = $3->oprand.oprandType;
		temp2.oprandVal = $3->oprand.oprandVal;
		makeTAC("MOVE", &temp1, &temp2,NULL);
		 char tempchar1 = 't', tempchar2 = '#';
        		if($3->oprand.oprandType) tempchar2 = 't';
        		sprintf(code[codecnt++], "%c%d := %c%d\n",tempchar1, $1->place, tempchar2, $3->oprand.oprandVal);
	}
	;

Exp :
	Exp ASSIGNOP Exp
    	{ 
        		$$ = mkNode(VN, "Exp", 3, $1, $2, $3);
        		checkTypeConflict1($$, $1, $3);
	}
	| Exp AND M Exp
	{ 
        		$$ = mkNode(VN, "Exp", 3, $1, $2, $4);
        		checkTypeConflictForAndOr($$, $1, $2, $4);
        		backPatch($1->boolList->trueList, $3->place);
        		merge($$->boolList->falseList, $1->boolList->falseList, $4->boolList->falseList);
        		$$->boolList->trueList = $4->boolList->trueList;
	}
	| Exp OR  M Exp
	{ 
        		$$ = mkNode(VN, "Exp", 3, $1, $2, $4);
        		checkTypeConflictForAndOr($$, $1, $2, $4);
        		backPatch($1->boolList->falseList, $3->place);
        		merge($$->boolList->trueList, $1->boolList->trueList, $4->boolList->trueList);
        		$$->boolList->falseList = $4->boolList->falseList;
	}
	| Exp RELOP Exp
	{ 
        		$$ = mkNode(VN, "Exp", 3, $1, $2, $3);
        		checkTypeConflict4($$, $1, $2, $3);
	}
	| Exp ADD Exp
	{ 
        		$$ = mkNode(VN, "Exp", 3, $1, $2, $3);
        		checkTypeConflict2($$, $1, $2, $3);
	}
	| Exp SUB Exp
	{ 
        		$$ = mkNode(VN, "Exp", 3, $1, $2, $3);
        		checkTypeConflict2($$, $1, $2, $3);
	}
	| Exp MUL Exp
	{ 
        		$$ = mkNode(VN, "Exp", 3, $1, $2, $3);
        		checkTypeConflict2($$, $1, $2, $3);
	}
	| Exp DIV Exp
	{ 
        		$$ = mkNode(VN, "Exp", 3, $1, $2, $3);
        		checkTypeConflict2($$, $1, $2, $3);
	}
	| LP Exp RP 
	{ 
		$$ = mkNode(VN, "Exp", 3, $1, $2, $3);
		$$->expType = $2->expType;
        		$$->pExpType = $2->pExpType;
        		$$->dimen = $2->dimen;
        		$$->isLeftVal =  $2->isLeftVal;
        		$$->place = $2->place;
        		$$->boolList = $2->boolList;
        		$$->oprand.oprandVal = $2->oprand.oprandVal;
        		$$->oprand.oprandType = $2->oprand.oprandType;
	}
	| SUB Exp 
	{ 
		$$ = mkNode(VN, "Exp", 2, $1, $2);
		checkTypeConflict3($$, $1, $2);
	}
	| NOT Exp
	{ 
		$$ = mkNode(VN, "Exp", 2, $1, $2);
		checkTypeConflict3($$, $1, $2);
	}
	| Exp LB Exp RB 
	{ 
		$$ = mkNode(VN, "Exp", 4, $1, $2, $3, $4);
		checkArray($$, $1, $3);
	}
	| Exp DOT ID 
	{ 
		$$ = mkNode(VN, "Exp", 3, $1, $2, $3);
		checkStruct($$, $1, $3);
	}
	| ID LP Args RP 
	{ 
		$$ = mkNode(VN, "Exp", 4, $1, $2, $3, $4);
		checkFunctionConflict1($$, $1, $3);

	}
	| ID LP RP 
	{ 
		$$ = mkNode(VN, "Exp", 3, $1, $2, $3);
		checkFunctionConflict1($$, $1, NULL);
	}
	| ID 
	{ 
		$$ = mkNode(VN, "Exp", 1, $1);
		checkID($$, $1);
	}
	| INT 
	{ 
		$$ = mkNode(VN, "Exp", 1, $1);
		$$->expType = INT;
        		$$->pExpType = 0;
        		$$->dimen = 0;
        		$$->isLeftVal = 0;
        		$$->place = $1->child->intval;
        		$$->oprandType = 0;
        		$$->codeLine = codecnt + 1;
        		$$->oprand.oprandType = 0;
        		$$->oprand.oprandVal = $1->child->intval;
	}
	| FLOAT 
	{ 
		$$ = mkNode(VN, "Exp", 1, $1);
		$$->expType = FLT;
        		$$->pExpType = 0;
        		$$->dimen = 0;
        		$$->isLeftVal = 0;
        		$$->place = (int)$1->fltval;
        		$$->oprandType = 0;
        		$$->codeLine = codecnt + 1;
        		$$->oprand.oprandType = 0;
        		$$->oprand.oprandVal = $$->place;
	}
	;

Args : 
	Exp 
	{
		makeTAC("PUSH", &($1->oprand), NULL, NULL);
	}
	COMMA Args 
	{ 
		$$ = mkNode(VN, "Args", 3, $1, $3, $4);
		sprintf($$->code, "Param t%d, ", $1->oprand.oprandVal);
		strcat($$->code, $4->code);
	}
	| Exp 
	{ 
		$$ = mkNode(VN, "Args", 1, $1);
		makeTAC("PUSH", &($1->oprand), NULL, NULL);
		sprintf($$->code, "Param t%d\n", $1->oprand.oprandVal);
	}
	;

INT : 
	HEX { $$ = mkNode(VN, "INT", 1, $1);}
	| OCT { $$ = mkNode(VN, "INT", 1, $1);}
	| DEC { $$ = mkNode(VN, "INT", 1, $1);}
	;
%%
yyerror(char *msg)
{
	fprintf(stderr, "%s Error type B at %s LINE %d.%d-%d.%d:  ", msg, yytext, yylineno, col - yyleng,yylineno, col);
	//printf("missing ;\n");
}
