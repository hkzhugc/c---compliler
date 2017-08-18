void checkTypeConflict1(Node *root, Node *left, Node *right)//ASSIGNOP
{
    if(left->expType == -1 || right->expType == -1)
    {
        sprintf(error[errorcnt++], "error at line %d: previous expression type conflict\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else if(left->isLeftVal != 1)
    {
        sprintf(error[errorcnt++],"error type 6 at line %d : can't ASSIGN a left value\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
        root->dimen = 0;
        root->isLeftVal = 0;
    }
    else if(left->expType != right->expType || left->pExpType != right->pExpType)
    {
        sprintf(error[errorcnt++], "error type 5 at line %d: expression type conflict when ASSIGNOP\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else
    {
        root->expType = left->expType;
        root->pExpType = left->pExpType;
        root->place = left->place;
        root->oprand.oprandType = left->oprand.oprandType;
        root->oprand.oprandVal = left->oprand.oprandVal;
        root->codeLine = codecnt + 1;
        makeTAC("ASSIGNOP", &left->oprand, &right->oprand, NULL);
        char temp1 = '#', temp2 = '#';
        if(left->oprand.oprandType) temp1= 't';
        if(left->oprand.oprandType == 2) temp1 = 'v';
        if(right->oprand.oprandType) temp2 = 't';
        if(right->oprand.oprandType == 2) temp2 = 'v';
        sprintf(code[codecnt++], "%c%d := %c%d\n",temp1, left->oprand.oprandVal, temp2, right->oprand.oprandVal);
    }
    root->dimen = right->dimen;
    root->isLeftVal = 1;
}

void checkTypeConflict2(Node *root, Node *left, Node *op, Node *right)//ADD SUB MUL DIV  
{
    if(left->expType == -1 || right->expType == -1)
    {
        sprintf(error[errorcnt++], "error at line %d: previous expression type conflict\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else if(left->expType != INT && left->expType != FLT && right->expType != INT && right->expType != FLT)
    {
        sprintf(error[errorcnt++], "error type 7 at line %d: this operation only allow INT or FLOAT expression\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else if(left->expType != right->expType || left->pExpType != right->pExpType)
    {
        sprintf(error[errorcnt++], "error type 7 at line %d: expression type conflict\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else
    {
        root->expType = left->expType;
        root->pExpType = left->pExpType;
        root->oprand.oprandVal = newtemp();
        root->oprand.oprandType = 1;
        root->codeLine = codecnt + 1;
        makeTAC(op->name, &root->oprand, &left->oprand, &right->oprand);
        char temp1 = '#', temp2 = '#';
        if(left->oprand.oprandType) temp1 = 't';
        if(right->oprand.oprandType) temp2 = 't';
        sprintf(code[codecnt++], "t%d := %c%d %s %c%d\n", 
            root->oprand.oprandVal, temp1, left->oprand.oprandVal, op->name, temp2, right->oprand.oprandVal);

    }
    root->dimen = 0;
    root->isLeftVal = 0;
}

void checkTypeConflictForAndOr(Node *root, Node *left, Node *op, Node *right)//AND OR  
{
    if(left->expType == -1 || right->expType == -1)
    {
        sprintf(error[errorcnt++], "error at line %d: previous expression type conflict\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else if(left->expType != INT && left->expType != FLT && right->expType != INT && right->expType != FLT)
    {
        sprintf(error[errorcnt++], "error type 7 at line %d: this operation only allow INT or FLOAT expression\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else if(left->expType != right->expType || left->pExpType != right->pExpType)
    {
        sprintf(error[errorcnt++], "error type 7 at line %d: expression type conflict\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else
    {
        root->expType = left->expType;
        root->pExpType = left->pExpType;
    }
    root->dimen = 0;
    root->isLeftVal = 0;
}

void checkTypeConflict3(Node * root, Node * op, Node * left)//one exp NOT SUB
{
    if(left->expType == -1)
    {
        sprintf(error[errorcnt++], "error at line %d: previous expression type conflict\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else if(left->expType != INT && left->expType != FLT)
    {
        sprintf(error[errorcnt++], "error at line %d: this operation only allow INT or FLOAT expression\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else
    {
        root->expType = left->expType;
        root->pExpType = left->pExpType;
        root->oprand.oprandVal = newtemp();
        root->oprand.oprandType = 1;
        root->codeLine = codecnt + 1;
        makeTAC(op->name, &root->oprand, &left->oprand, NULL);
        char temp1 = '#', temp2 = '#';
        if(left->oprand.oprandType) temp1 = 't';
        if(left->oprand.oprandType == 2) temp1 = 'v';
        sprintf(code[codecnt++], "t%d := %s %c%d\n", root->oprand.oprandVal , op->name, temp1, left->oprand.oprandVal);
    }
    root->dimen = left->dimen;
    root->isLeftVal = 0;
}

void checkTypeConflict4(Node *root, Node *left, Node *op, Node *right)//RELOP
{
    if(left->expType == -1 || right->expType == -1)
    {
        sprintf(error[errorcnt++], "error at line %d: previous expression type conflict\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else if(left->expType != INT && left->expType != FLT && right->expType != INT && right->expType != FLT)
    {
        sprintf(error[errorcnt++], "error type 7 at line %d: this operation only allow INT or FLOAT expression\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else if(left->expType != right->expType || left->pExpType != right->pExpType)
    {
        sprintf(error[errorcnt++], "error type 7 at line %d: expression type conflict\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
    }
    else
    {
        root->expType = left->expType;
        root->pExpType = left->pExpType;
        root->oprand.oprandVal = 0;
        root->oprand.oprandType = 0;
        root->codeLine = codecnt + 1;
        makeTACList(root->boolList->trueList, TACcnt);
        makeTACList(root->boolList->falseList, TACcnt + 1);
        makeTAC(op->name, NULL, &left->oprand, &right->oprand);//cmp jmp
        makeTAC("jmp", NULL, NULL, NULL);//directed jmp
        makeList(root->boolList->trueList, codecnt);
        makeList(root->boolList->falseList, codecnt + 1);
        char temp1 = '#', temp2 = '#';
        if(left->oprand.oprandType) temp1 = 't';
        if(left->oprand.oprandType == 2) temp1 = 'v';
        if(right->oprand.oprandType) temp2 = 't';
        if(right->oprand.oprandType == 2) temp2 = 'v';
        sprintf(code[codecnt++], "if %c%d %s %c%d then goto LABLE_\n", 
            temp1, left->oprand.oprandVal, op->name, temp2, right->oprand.oprandVal);
        sprintf(code[codecnt++], "goto LABLE_\n");
    }
    root->dimen = 0;
    root->isLeftVal = 0;
}

void checkArray(Node *root, Node *Exp, Node *index)
{
    if(index->expType != INT)
    {
        sprintf(error[errorcnt++],"error type 12 at line %d : index is not an integer\n", yylineno);
    }
    else if(Exp->dimen <= 0)
    {
        sprintf(error[errorcnt++],"error type 10 at line %d : Exp is not an array\n", yylineno);
    }
    else
    {
        root->expType = Exp->expType;
        root->pExpType = Exp->pExpType;
        root->dimen = Exp->dimen - 1;
        root->isLeftVal = 1;
        root->oprand.oprandVal = newtemp();
        root->oprand.oprandType = 1;
        root->codeLine = codecnt + 1;
        makeTAC("LEA", &root->oprand, &Exp->oprand, &index->oprand);
        char temp = '#', temp1 = '#';
        if(index->oprand.oprandType) temp = 't';
        if(Exp->oprand.oprandType) temp1 = 't';
        sprintf(code[codecnt++], "t%d := %c%d + offset %c%d\n", 
            root->oprand.oprandVal, temp1, Exp->oprand.oprandVal, temp, index->oprand.oprandVal);
    }
}

void checkStruct(Node * root, Node * Struct, Node * StructId)
{
    if(Struct->expType != STRT)
    {
        sprintf(error[errorcnt++],"error type 13 at line %d : Exp is not an struct\n", yylineno);
        root->expType = -1;
        root->pExpType = 0;
        root->dimen = -1;
        root->isLeftVal = 0;
    }
    else
    {
        printf("hehe\n");
        StructDefList * temp = findIdinStruct(((StructType *)(Struct->pExpType))->structDefListHead, StructId->idname);
        if(temp == NULL)
        {
            sprintf(error[errorcnt++],"error type 14 at line %d : struct doesn't have this field\n", yylineno);
            root->expType = -1;
            root->pExpType = 0;
            root->dimen = -1;
            root->isLeftVal = 0;
        }
        else
        {
            root->expType = temp->thisDefType;
            root->pExpType = temp->pthisDefType;
            root->dimen = temp->dimen;
            root->isLeftVal = 1;
            root->oprand.oprandVal = newtemp();
            root->oprand.oprandType = 1;
            root->codeLine = codecnt + 1;
            Oprand tempOprand;
            tempOprand.oprandType = 0;
            tempOprand.oprandVal = temp->offset;
            makeTAC("LEA", &root->oprand, &Struct->oprand, &tempOprand);
            char temp = '#';
            if(Struct->oprand.oprandType) temp = 't';
            sprintf(code[codecnt++], "t%d := %c%d + offset #%d\n", root->oprand.oprandVal, temp, Struct->oprand.oprandVal, tempOprand.oprandVal);
        }
    }
}

void checkFunctionConflict1(Node * root, Node * ID, Node * Args)
{
    printf("find func\n");
    IdNode *temp = findIdExist(ID->idname);
    int codeLine;
    if(temp)
    {
        if(temp->idtype != FUNC)
        {
            sprintf(error[errorcnt++],"error type 11 at line %d : var %s is not a function\n", yylineno, ID->idname);
             root->expType = -1;
            root->pExpType = 0;
        }
        else if(temp->pType == 0)
        {
            sprintf(error[errorcnt++],"error type 2 at line %d : function %s is not defined\n", yylineno, ID->idname);
             root->expType = -1;
            root->pExpType = 0;
        }
        else if(((FunType *)temp->pType)->ArgNums != 0 && Args == NULL)
        {
            sprintf(error[errorcnt++],"error type 9 at line %d : function args type not match.  ", yylineno);
            sprintf(error[errorcnt++],"function %s type should have %d argnums\n", ID->idname, ((FunType *)temp->pType)->ArgNums);
             root->expType = -1;
            root->pExpType = 0;
        }
        else if (!(codeLine = checkArgs(((FunType *)temp->pType), Args)))
        {
            sprintf(error[errorcnt++],"error type 9 at line %d : codeLine is %d.  \n", yylineno, codeLine);
        }
        else
        {
            root->expType = ((FunType *)temp->pType)->DefReturnType;
            root->pExpType= ((FunType *)temp->pType)->DefPReturnType;
            root->codeLine = codecnt + 1;
            root->oprand.oprandVal = newtemp();
            root->oprand.oprandType = 1;
            char * temp = (char *)malloc(sizeof(char) * 100);
            sprintf(temp, "CALL %s", ID->idname);
            makeTAC(temp, &root->oprand, NULL, NULL);
            root->oprandType = 1;
            if (Args)
                sprintf(code[codecnt++], "%s       call func %s jmp to line %d\n       return to t%d\n", 
                    Args->code, ID->idname, codeLine, root->oprand.oprandVal);
            else
                sprintf(code[codecnt++], "call func %s jmp to line %d\n       return to t%d\n", 
                    ID->idname, codeLine, root->oprand.oprandVal);
        }
    }
    else
    {
        printf("xixi\n");
        sprintf(error[errorcnt++],"error type 2 at line %d : function %s is not defined\n", yylineno, ID->idname);
         root->expType = -1;
        root->pExpType= 0;
    }
    root->dimen = 0;
    root->isLeftVal = 0;
}

int checkArgs(FunType * funType, Node * Args)
{
    Node * temp = Args;
    ArgList * argList = funType->argListHead;
    printf("function should have %d args\n", funType->ArgNums);
    int ArgsNum = 0;
    while(temp && argList)
    {
        printf("temp->name is %s\n", temp->name);
        printf("%d arglist args type is %d\n", ArgsNum+1, temp->child->expType);
        printf("def: %d arglist args type is %d\n", ArgsNum+1, argList->thisArgType);
        if(strcmp(temp->name,"Exp") && strcmp(temp->name, "Args")){
            //printf("fisrt cmp = %d\n", strcmp(temp->name,"Exp"));
            printf("temp->name is %s\n", temp->name);
            printf("out of range\n");
            break;
        }
        if(temp->child->expType != argList->thisArgType || temp->child->pExpType != argList->pthisArgType)
        {
            sprintf(error[errorcnt++],"error type 9 at line %d : function type not match\n", yylineno);
            return 0;
        }
        else
        {
            ArgsNum++;
            argList = argList->next;
            if(temp->child->bros)
            {
                temp = temp->child->bros->bros;
            }
            else
            {
                break;
            }
        }
    }
    printf("call function have %d args\n", ArgsNum);
    if(ArgsNum != funType->ArgNums)
    {
        sprintf(error[errorcnt++],"error type 9 at line %d : function type not match\n", yylineno);
        return 0;
    }
    else
        return funType->codeline;
}


void checkID(Node * root, Node * ID)
{
    IdNode *temp = findIdExist(ID->idname);
    if(temp)
    {
        root->expType = temp->idtype;
        root->pExpType = temp->pType;
        root->dimen = temp->dimen;
        printf(" %s's dimen is %d type is %d\n", ID->idname, temp->dimen, temp->idtype );
        root->isLeftVal = 1;
        root->oprand.oprandType = 1;
        root->oprand.oprandVal = temp->place;
    }
    else
    {
        sprintf(error[errorcnt++],"error type 1 at line %d : %s is not defined\n", yylineno, ID->idname);
        root->expType = -1;
        root->pExpType = 0;
        root->dimen = -1;
        root->oprandType = 1;
        root->isLeftVal = 0;
    }
}


void checkFuncReturnType(Node * returnExp)
{
    if(returnExp->expType != defReturnType || returnExp->pExpType != defPReturnType)
    {
        sprintf(error[errorcnt++], "error type 8 at line %d : return type conflict\n", yylineno);
    }
    isReturn = 1;
}
