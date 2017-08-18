void makeDstCode()
{
	int i;
	initCode();
	for (i = 0; i < TACcnt; ++i)
	{
		makeOneCode(i);	
	}
}

void subStack()
{
	fprintf(dstCode, "  addi $sp, $sp, -4\n");
}

void addStack()
{
	fprintf(dstCode, "  addi $sp, $sp, 4\n");
}

void changeAc(int index, int type, int addr)
{
	funcAc.typeTab[index].type = type;
	funcAc.typeTab[index].addr = addr;
}

void lw(int varAddr, int tempRegindex, int regType)// load word from ram to reg
{
	char temp = 't';
	if(regType == PARAM)
		temp = 'a';
	if(regType == Return)
		temp = 'v';
	fprintf(dstCode, "  lw $%c%d, %d($sp)\n", temp, tempRegindex, 4 * (funcAc.varNum - varAddr - 1));
}

void li(int varAddr, int constVal)// load word from ram to reg
{
	fprintf(dstCode, "  li $t%d, %d\n", varAddr, constVal);
}

void sw(int varAddr, int tempRegindex, int regType)// save word to ram from reg
{
	char temp = 't';
	if(regType == PARAM)
		temp = 'a';
	if(regType == Return)
		temp = 'v';
	fprintf(dstCode, "  sw $%c%d, %d($sp)\n", temp, tempRegindex, 4 * (funcAc.varNum - varAddr - 1));
}

void initCode()
{
	fprintf(dstCode, ".data\n");
	fprintf(dstCode, "_prompt: .asciiz \"Enter an integer: \"\n");
	fprintf(dstCode, "_ret: .asciiz \"\\n\"\n");
	fprintf(dstCode, ".globl main\n");
	fprintf(dstCode, ".text\n");
	fprintf(dstCode, "read:\n");
	fprintf(dstCode, "  li $v0, 4\n");
	fprintf(dstCode, "  la $a0, _prompt\n");
	fprintf(dstCode, "  syscall\n");
	fprintf(dstCode, "  li $v0, 5\n");
	fprintf(dstCode, "  syscall\n");
	fprintf(dstCode, "  jr $ra\n\n");
	fprintf(dstCode, "write:\n");
	fprintf(dstCode, "  li $v0, 1\n");
	fprintf(dstCode, "  syscall\n");
	fprintf(dstCode, "  li $v0, 4\n");
  	fprintf(dstCode, "  la $a0, _ret\n");
  	fprintf(dstCode, "  syscall\n");
  	fprintf(dstCode, "  move $v0, $0\n");
  	fprintf(dstCode, "  jr $ra\n");
}

void push(int tempRegindex, int regType)
{
	subStack();
	int tempVarAddr = funcAc.varNum - 1;
	sw(tempVarAddr, tempRegindex, regType);
}

void pushra()
{
	subStack();
	fprintf(dstCode , "  sw $ra, 0($sp)\n");
}

void popra()
{
	fprintf(dstCode , "  lw $ra, 0($sp)\n");
	addStack();
}

void pop(int tempRegindex, int regType)
{
	int tempVarAddr = funcAc.varNum - 1;
	lw(tempVarAddr, tempRegindex, regType);
	addStack();
}

void makeOneCode(int i)
{
	char * op = TAC[i].opName;
	if(op[0] == 'f' && op[1] == 'u')//func begin
	{
		funcAc.stackTop = -1;
		memset(funcAc.compstVarNumStack, 0, sizeof(funcAc.compstVarNumStack));
		funcAc.varNum = 0;
		funcAc.tempNum = 0;
		funcAc.paramNum = 0;//init Ac
		fprintf(dstCode, "\n%s:\n", &op[5]);//print func label
		//fprintf(dstCode, "  move $s0, $sp\n");//save sp
	}
	else if(!strcmp(op, "Param"))//found a param put param to ram
	{
		int index = TAC[i].dst.oprandVal;//get the original t num
		int addr = funcAc.varNum++;
		funcAc.compstVarNumStack[funcAc.stackTop]++;//new a var
		changeAc(index, VAR, addr);//record ti is a var, and store param's var index
		subStack();
		sw(addr, funcAc.paramNum++, PARAM);	
		funcAc.compstVarNumStack[funcAc.stackTop + 1]++;//not meet LC

	}
	else if(!strcmp(op, "New Var"))//found a var
	{
		subStack();
		int index = TAC[i].dst.oprandVal;//get the original t num
		changeAc(index, VAR, funcAc.varNum++);//record ti is a var, and store var index
		funcAc.compstVarNumStack[funcAc.stackTop]++;
	}
	else if(!strcmp(op, "MOVE") || !strcmp(op, "ASSIGNOP"))// assign code  
	//dst is absolute a var 
	{
		int srcType = funcAc.typeTab[TAC[i].src1.oprandVal].type;
		int srcTempVal;
		int dstTempVal = funcAc.typeTab[TAC[i].dst.oprandVal].addr;
		if(TAC[i].src1.oprandType == 0)//is a const should li
		{
			int thisTempReg = funcAc.tempNum++;//start from 0;
			li(thisTempReg, TAC[i].src1.oprandVal);
			srcTempVal = thisTempReg;
		}
		else//var or tempReg
		{
			if(srcType == VAR)//src is also a var, its addr should store i the typeTab
			{
				int thisTempReg = funcAc.tempNum++;//start from 0;
				int thisVarAddr = funcAc.typeTab[TAC[i].src1.oprandVal].addr;// get the ram addr
				lw(thisVarAddr, thisTempReg, TEMP);
				srcTempVal = thisTempReg;
			}
			else//src is store in a temp reg, its addr should be the index val
			{
				srcTempVal = funcAc.typeTab[TAC[i].src1.oprandVal].addr;// get the reg addr
			}
		}
		sw(dstTempVal, srcTempVal, srcType);
	}
	else if(!strcmp(op, "Stmt"))//one stmt end, free the temp reg
	{
		funcAc.tempNum = 0;
	}
	else if(op[0] == 'C' && op[1] == 'A')//call func
	{
		int j = 0;
		for (j = 0; j < funcAc.tempNum; ++j)//protect temp reg
		{
			push(j, TEMP);
		}
		pushra();
		int index = TAC[i].dst.oprandVal;
		changeAc(index, Return, 0);//return to v0;
		//printf("%d : funcAc.typeTab[%d].oprandVal is %d\n", i ,index, funcAc.typeTab[index].type);
		fprintf(dstCode , "  jal %s\n", &op[5]);
		popra();
		for (j = funcAc.tempNum - 1; j >= 0; j--)//refresh temp reg
		{
			pop(j, TEMP);
		}
		funcAc.argNum = 0;//free argReg
		funcAc.isPush = 0;
		funcAc.argNum = 0;
	}
	else if(!strcmp(op, "LABLE"))//make label
	{
		int labelNum = TAC[i].dst.oprandVal;
		fprintf(dstCode, "label%d:\n", labelNum);
	}
	else if(!strcmp(op, "<") || !strcmp(op, "==")  || !strcmp(op, ">=") || !strcmp(op, "<=") || !strcmp(op, "!=") || !strcmp(op, ">"))
	{
		int src1TempVal = funcAc.typeTab[TAC[i].src1.oprandVal].addr;
		int src2TempVal = funcAc.typeTab[TAC[i].src2.oprandVal].addr;
		char * tempOp = "blt";
		if(!strcmp(op, "==")) tempOp = "beq";
		if(!strcmp(op, "!=")) tempOp = "bne";
		if(!strcmp(op, ">")) tempOp = "bgt";
		if(!strcmp(op, "<")) tempOp = "blt";
		if(!strcmp(op, ">=")) tempOp = "bge";
		if(!strcmp(op, "<=")) tempOp = "ble";
		if(TAC[i].src1.oprandType == 0)//left oprand is const
		{
			src1TempVal = funcAc.tempNum++;//start from 0;
			li(src1TempVal, TAC[i].src1.oprandVal);//li constNum to the temp reg
		}
		else if(funcAc.typeTab[TAC[i].src1.oprandVal].type == VAR)//left oprand is var
		{
			src1TempVal = funcAc.tempNum++;
			lw(funcAc.typeTab[TAC[i].src1.oprandVal].addr, src1TempVal, TEMP);//lw
		}
		if(TAC[i].src2.oprandType == 0)//left oprand is const
		{
			src2TempVal = funcAc.tempNum++;//start from 0;
			li(src2TempVal, TAC[i].src2.oprandVal);//li constNum to the temp reg
		}
		else if(funcAc.typeTab[TAC[i].src2.oprandVal].type == VAR)//left oprand is var
		{
			src2TempVal = funcAc.tempNum++;
			lw(funcAc.typeTab[TAC[i].src2.oprandVal].addr, src2TempVal, TEMP);//lw
		}
		char temp1 = 't', temp2 = 't';
		if(funcAc.typeTab[TAC[i].src1.oprandVal].type == Return) temp1 = 'v';
		if(funcAc.typeTab[TAC[i].src1.oprandVal].type == PARAM) temp1 = 'a';
		if(funcAc.typeTab[TAC[i].src2.oprandVal].type == Return) temp2 = 'v';
		if(funcAc.typeTab[TAC[i].src2.oprandVal].type == PARAM) temp2 = 'a';
		fprintf(dstCode , "  %s $%c%d, $%c%d, label%d\n", tempOp,temp1, src1TempVal, temp2, src2TempVal, TAC[i].dst.oprandVal);
		funcAc.tempNum = 0;//free reg
	}
	else if(!strcmp(op, "jmp"))
	{
		fprintf(dstCode, "  j label%d\n", TAC[i].dst.oprandVal);
	}
	else if(!strcmp(op, "ADD") || !strcmp(op, "SUB"))
	{
		int src1TempVal = funcAc.typeTab[TAC[i].src1.oprandVal].addr;
		int src2TempVal = funcAc.typeTab[TAC[i].src2.oprandVal].addr;
		int dstTempVal = funcAc.tempNum++;//dst is sure a newtemp
		changeAc(TAC[i].dst.oprandVal, TEMP, dstTempVal);
		char temp1 = 't', temp2 = 't';
		char * tempOP = "add";
		if(!strcmp(op, "SUB")) tempOP = "sub";
		if(funcAc.typeTab[TAC[i].src1.oprandVal].type == Return) temp1 = 'v';
		if(funcAc.typeTab[TAC[i].src1.oprandVal].type == PARAM) temp1 = 'a';
		if(funcAc.typeTab[TAC[i].src2.oprandVal].type == Return) temp2 = 'v';
		if(funcAc.typeTab[TAC[i].src2.oprandVal].type == PARAM) temp2 = 'a';
		if(TAC[i].src1.oprandType == 0)//left oprand is const
		{
			if(!strcmp(op, "ADD")) tempOP = "addi";
			fprintf(dstCode , "  %s $t%d, $%c%d, %d\n", tempOP, dstTempVal, temp2, src2TempVal, TAC[i].src1.oprandVal);
		}
		else if(funcAc.typeTab[TAC[i].src1.oprandVal].type == VAR)//left oprand is var
		{
			src1TempVal = funcAc.tempNum++;
			lw(funcAc.typeTab[TAC[i].src1.oprandVal].addr, src1TempVal, TEMP);//lw
		}
		if (TAC[i].src2.oprandType == 0)
		{
			if(!strcmp(op, "ADD")) tempOP = "addi";
			fprintf(dstCode , "  %s $t%d, $%c%d, %d\n", tempOP, dstTempVal, temp1, src1TempVal, TAC[i].src2.oprandVal);
		}
		else if(funcAc.typeTab[TAC[i].src2.oprandVal].type == VAR)//left oprand is var
		{
			src2TempVal = funcAc.tempNum++;
			lw(funcAc.typeTab[TAC[i].src2.oprandVal].addr, src2TempVal, TEMP);//lw
		}
		if(TAC[i].src1.oprandType && TAC[i].src2.oprandType)
		{
			//printf("both is not const\n");
			fprintf(dstCode , "  %s $t%d, $%c%d, $%c%d\n", tempOP, dstTempVal, temp1, src1TempVal, temp2, src2TempVal);
		}
	}
	else if (!strcmp(op, "MUL") || !strcmp(op, "DIV"))
	{
		int src1TempVal = funcAc.typeTab[TAC[i].src1.oprandVal].addr;
		int src2TempVal = funcAc.typeTab[TAC[i].src2.oprandVal].addr;
		char * tempOp = "mul";
		int dstTempVal = funcAc.tempNum++;//dst is sure a newtemp
		changeAc(TAC[i].dst.oprandVal, TEMP, dstTempVal);
		char temp1 = 't', temp2 = 't';
		if(funcAc.typeTab[TAC[i].src1.oprandVal].type == Return) temp1 = 'v';
		if(funcAc.typeTab[TAC[i].src1.oprandVal].type == PARAM) temp1 = 'a';
		if(funcAc.typeTab[TAC[i].src2.oprandVal].type == Return) temp2 = 'v';
		if(funcAc.typeTab[TAC[i].src2.oprandVal].type == PARAM) temp2 = 'a';
		if(TAC[i].src1.oprandType == 0)//left oprand is const
		{
			src1TempVal = funcAc.tempNum++;
			li(src1TempVal, TAC[i].src1.oprandVal);
		}
		else if(funcAc.typeTab[TAC[i].src1.oprandVal].type == VAR)//left oprand is var
		{
			src1TempVal = funcAc.tempNum++;
			lw(funcAc.typeTab[TAC[i].src1.oprandVal].addr, src1TempVal, TEMP);//lw
		}
		if (TAC[i].src2.oprandType == 0)
		{
			src2TempVal = funcAc.tempNum++;
			li(src2TempVal, TAC[i].src2.oprandVal);
		}
		else if(funcAc.typeTab[TAC[i].src2.oprandVal].type == VAR)//left oprand is var
		{
			src2TempVal = funcAc.tempNum++;
			lw(funcAc.typeTab[TAC[i].src2.oprandVal].addr, src2TempVal, TEMP);//lw
		}
		if(!strcmp(op, "DIV")) 
		{
			tempOp = "div";
			fprintf(dstCode, "  %s $%c%d, $%c%d\n", tempOp, temp1, src1TempVal, temp2, src2TempVal);
			fprintf(dstCode, "  mflo $t%d\n", dstTempVal);
		}
		else 
		{
			fprintf(dstCode, "  %s $t%d, $%c%d, $%c%d\n", tempOp, dstTempVal, temp1, src1TempVal, temp2, src2TempVal);
		}
	}
	else if(!strcmp(op, "PUSH"))//push param
	{
		int j, dstTempVal = funcAc.typeTab[TAC[i].dst.oprandVal].addr;
		if(TAC[i].dst.oprandType == 0)//dst oprand is const
		{
			dstTempVal = funcAc.tempNum++;//start from 0;
			li(dstTempVal, TAC[i].dst.oprandVal);//li constNum to the temp reg
		}
		else if(funcAc.typeTab[TAC[i].dst.oprandVal].type == VAR)//dst oprand is var
		{
			dstTempVal = funcAc.tempNum++;
			lw(funcAc.typeTab[TAC[i].dst.oprandVal].addr, dstTempVal, TEMP);//lw
		}
		char temp = 't';
		if(funcAc.typeTab[TAC[i].dst.oprandVal].type == Return) temp = 'v';
		if(funcAc.typeTab[TAC[i].dst.oprandVal].type == PARAM) temp = 'a';
		fprintf(dstCode , "  move $a%d, $%c%d\n", funcAc.argNum++, temp, dstTempVal);
	}
	else if(!strcmp(op, "RETURN"))
	{
		int j, dstTempVal = funcAc.typeTab[TAC[i].dst.oprandVal].addr;
		if(TAC[i].dst.oprandType == 0)//dst oprand is const
		{
			dstTempVal = funcAc.tempNum++;//start from 0;
			li(dstTempVal, TAC[i].dst.oprandVal);//li constNum to the temp reg
		}
		else if(funcAc.typeTab[TAC[i].dst.oprandVal].type == VAR)//dst oprand is var
		{
			dstTempVal = funcAc.tempNum++;
			lw(funcAc.typeTab[TAC[i].dst.oprandVal].addr, dstTempVal, TEMP);//lw
		}
		char temp = 't';
		if(funcAc.typeTab[TAC[i].dst.oprandVal].type == Return) temp = 'v';
		if(funcAc.typeTab[TAC[i].dst.oprandVal].type == PARAM) temp = 'a';
		fprintf(dstCode , "  addi $sp, $sp, %d\n", 4 * funcAc.varNum);
		printf("funcAc.varNum is %d, funcAc.paramNum is %d\n", funcAc.varNum, funcAc.paramNum);
		fprintf(dstCode , "  move $v0, $%c%d\n", temp, dstTempVal);
		//fprintf(dstCode, "  move $sp, $s0\n");//load sp
		fprintf(dstCode , "  jr $ra\n");
	}
	else if(!strcmp(op, "LC"))
	{
		funcAc.stackTop++;
	}
	else if(!strcmp(op, "RC"))
	{
		if(funcAc.stackTop >= 1)
		{
			int cmpstNum = funcAc.compstVarNumStack[funcAc.stackTop];
			funcAc.varNum -= cmpstNum;
			fprintf(dstCode , "  addi $sp, $sp, %d\n", 4 * cmpstNum);
			funcAc.stackTop--;
		}
	}
	else
	{
		printf("%d not ready\n", i);
	}
}