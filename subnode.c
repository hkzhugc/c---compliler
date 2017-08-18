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
	if(!strcmp(node->name,"Stmt") || !strcmp(node->name,"N"))
	{
		node->nextList = initList();
		//printf("List is %d\n", node->nextList);
	}
	else if(!strcmp(node->name,"Exp"))
	{
		node->boolList = initBoolList();
		//printf("List is %d\n", node->boolList ->trueList);
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


