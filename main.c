#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>//变长参数函数所需的头文件
#include "node.h"
int lvl = 0;
int errorcnt = 0;
int codecnt = 0;
int TACcnt = 0;
int LABLEcnt = 1;
int defReturnType = -1;
int defPReturnType = 0;
int isReturn = 0;
int t = 1;
FILE * dstCode;
int main(int argc,char** argv)
{
    memset(error, 0, sizeof(error));
	SymStack.Top = (SymTabStackNode *)malloc(sizeof(SymTabStackNode));
	SymStack.Top->SymTab = (_SymTab *)malloc(sizeof(_SymTab));
	SymStack.Top->SymTab->Head = NULL;
	SymStack.Top->SymTab->Tail = NULL;
	SymStack.Top->next = NULL;
	SymStack.Top->level = lvl;
	StructTab.structTagTabHead = StructTab.structTagTabTail = NULL;
	initSysFunc();
	if(argc>1)
	{
		FILE *f = fopen(argv[1],"r");
		if(argc > 2)
			dstCode = fopen(argv[2],"w");
		else
			dstCode = stdout;
		if( !f )
		{
			perror(argv[1]);
			return 1;
		}
		yyrestart(f);
		yyparse();
		return 0;
	}
	return 1;
}
/*asasas


*/
