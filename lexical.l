%{
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "syntax.tab.h"
enum
{
	LOOKFORDIGORLETTER = 0,
	FISRTZERO,
	ISDEC,
	ISHEX,
	ISOCT
};
int wordCount = 0;
int numCount = 0;
int col = 1;
int state = LOOKFORDIGORLETTER;
%}
%option yylineno
digit [0-9]
hexdigit [0-9a-fA-F]
octdigit [0-7]
hexnum "0x"{hexdigit}+
octnum 0{octdigit}+
decnum 0|([1-9]{digit}*)
FLOAT   [0-9]+\.[0-9]*
letter [A-Za-z]
id "_"?{letter}({digit}|{letter}|_)*
delim [" "]
whitespace {delim}+
%x COMMENT
%%
"//".*\n {printf("COMMENT  %s at line %d\n", yytext, yylineno);}
"/*".*"*/"\n {printf("COMMENT  %s at line %d\n", yytext, yylineno);}
"/*" {BEGIN COMMENT;printf("begin COMMENT  at line %d: ", yylineno);}
<COMMENT>\n {printf("\n");yylineno;}
<COMMENT>"*/" {BEGIN 0; printf("COMMENT   end at line %d\n", yylineno);}
<COMMENT>"*/"[ \t]*\n {BEGIN 0; printf("COMMENT   end at line %d\n", yylineno);}
<COMMENT>. {printf("%s", yytext);}
\n {state = LOOKFORDIGORLETTER;yylineno;col=1;}
\t {state = LOOKFORDIGORLETTER;col+=4;}
; {yylval.node = mkNode(OTHERVT, "SEMI", 0, yylineno); col+=yyleng; return SEMI;}
, {yylval.node = mkNode(OTHERVT, "COMMA", 0, yylineno); col+=yyleng; return COMMA;}
"=" {yylval.node = mkNode(OTHERVT, "ASSIGNOP", 0, yylineno); col+=yyleng; return ASSIGNOP;}
"!=" {yylval.node = mkNode(OTHERVT, "!=", 0, yylineno); col+=yyleng; return RELOP;}
"==" {yylval.node = mkNode(OTHERVT, "==", 0, yylineno); col+=yyleng; return RELOP;}
"<=" {yylval.node = mkNode(OTHERVT, "<=", 0, yylineno); col+=yyleng; return RELOP;}
">=" {yylval.node = mkNode(OTHERVT, ">=", 0, yylineno); col+=yyleng; return RELOP;}
"<" {yylval.node = mkNode(OTHERVT, "<", 0, yylineno); col+=yyleng; return RELOP;}
">" {yylval.node = mkNode(OTHERVT, ">", 0, yylineno); col+=yyleng; return RELOP;}
"+" {yylval.node = mkNode(OTHERVT, "ADD", 0, yylineno); col+=yyleng; return ADD;}
"-" {yylval.node = mkNode(OTHERVT, "SUB", 0, yylineno); col+=yyleng; return SUB;}
"*" {yylval.node = mkNode(OTHERVT, "MUL", 0, yylineno); col+=yyleng; return MUL;}
"/" {yylval.node = mkNode(OTHERVT, "DIV", 0, yylineno); col+=yyleng; return DIV;}
"&&" {yylval.node = mkNode(OTHERVT, "AND", 0, yylineno); col+=yyleng; return AND;}
"||" {yylval.node = mkNode(OTHERVT, "OR", 0, yylineno); col+=yyleng; return OR;}
"." {yylval.node = mkNode(OTHERVT, "DOT", 0, yylineno); col+=yyleng; return DOT;}
"!" {yylval.node = mkNode(OTHERVT, "NOT", 0, yylineno); col+=yyleng; return NOT;}
"(" {yylval.node = mkNode(OTHERVT, "LP", 0, yylineno); col+=yyleng; return LP;}
")" {yylval.node = mkNode(OTHERVT, "RP", 0, yylineno); col+=yyleng; return RP;}
"[" {yylval.node = mkNode(OTHERVT, "LB", 0, yylineno); col+=yyleng; return LB;}
"]" {yylval.node = mkNode(OTHERVT, "RB", 0, yylineno); col+=yyleng; return RB;}
"{" {yylval.node = mkNode(OTHERVT, "LC", 0, yylineno); col+=yyleng; return LC;}
"}" {yylval.node = mkNode(OTHERVT, "RC", 0, yylineno); col+=yyleng; return RC;}
"int" {
	yylval.node = mkNode(OTHERVT, "TYPE_INT", 0, yylineno); 
	(yylval.node)->expType = INT;
	col+=yyleng; 
	return TYPE_INT; }
"float" { 
	yylval.node = mkNode(OTHERVT, "TYPE_FLOAT", 0, yylineno); 
	(yylval.node)->expType = FLT;
	col+=yyleng; 
	return TYPE_FLOAT;  }
"struct" {
	yylval.node = mkNode(OTHERVT, "STRUCT", 0, yylineno); 
	(yylval.node)->expType = STRT;
	col+=yyleng;
	return STRUCT;}
"return" {yylval.node = mkNode(OTHERVT, "RETURN", 0, yylineno); col+=yyleng;return RETURN;}
"if" {yylval.node = mkNode(OTHERVT, "IF", 0, yylineno); col+=yyleng;return IF;}
"then"  {yylval.node = mkNode(OTHERVT, "THEN", 0, yylineno); col+=yyleng;return THEN;}
"else" {yylval.node = mkNode(OTHERVT, "ELSE", 0, yylineno); col+=yyleng;return ELSE;}
"while" {yylval.node = mkNode(OTHERVT, "WHILE", 0, yylineno); col+=yyleng;return WHILE;}
{hexnum} {yylval.node = mkNode(INTVT, "HEX", 0, yylineno); col+=yyleng;return HEX;}
{octnum} {yylval.node = mkNode(INTVT, "OCT", 0, yylineno); col+=yyleng;return OCT;}
{decnum} {  yylval.node = mkNode(INTVT, "DEC", 0, yylineno); col+=yyleng;return DEC;}
{FLOAT} {yylval.node = mkNode(FLTVT, "FLOAT", 0, yylineno);return FLOAT;}
{id} {  yylval.node = mkNode(IDVT, "ID", 0, yylineno); col+=yyleng; return ID;}
{whitespace} {state = LOOKFORDIGORLETTER; col+=yyleng;}
. {printf("Error type A at Line %d_%d: %s\n", yylineno, col, yytext);col+=yyleng;}
%%
