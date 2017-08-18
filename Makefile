compiler:
	flex lexical.l 
	bison -v -d syntax.y
	gcc syntax.tab.c  parser.c main.c -lfl -ly -o parser -g




