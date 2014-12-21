/* Bortoli Gianluca, n° 159993 */

%{
	#include <stdlib.h>
	#include <string.h>
	#include "myCalc.h"
	#include "y.tab.h"
%}

%%

"main"					return MAIN; //main statement at the beginning of input

"while"					return WHILE; // all passible statements
"for"					return FOR;
"to"					return TO;
"if"					return IF;
"else"					return ELSE;
"print"					return PRINT;
"printInt"				return PRINTINT;
"printReal"				return PRINTREAL;

"int"					return INT; // types
"real"					return REAL;
"bool"					return BOOL;

"true"					{yylval.bValue = 1; return BOOLEAN;}
"false"					{yylval.bValue = 0; return BOOLEAN;}

"<="					return GRE; //bool operations
">="					return LRE;
"=="					return DBE;
"="						return EQ;
"!="					return NE;
"or"					return OR; 
"not"					return NOT;
"and"					return AND;

"+"						return PLUS; // numpers operations
"-"						return MIN;
"*"						return MULT;
"/"						return DIV;
"<"						return LT;
">"						return GT;

[ \t\n]         		; // symbols 
\;              		return SEMICOLON; 
\,              		return COMMA;
\(              		return LP;
\)              		return RP;
\{              		return LCURLY;
\}              		return RCURLY;
.            		   	{ECHO; yyerror("Previous char is unknown to lexer");}

[1-9][0-9]* 		    {
		                    yylval.iValue = atoi(yytext);
		                    return INTEGER;
		                }

0             			{
                    		yylval.iValue = atoi(yytext);
                    		return INTEGER;
                		}

[a-zA-Z][a-zA-Z0-9]*  	{	// +1 for the \0 at the end of the string
							yylval.varName = (char *)malloc(strlen(yytext)+1);
							// copy assigned name to varName
							strcpy(yylval.varName, yytext);
							return VARIABLE;
						}

(0|[1-9][0-9]*),[0-9]+  {   // takes the hole string yytext and replces DOT with COMMA
							// REQ: write real numbers with COMMA, not with DOTS
							// +1 for the \0 at the end of the string
                            char * tmp = (char *)malloc(strlen(yytext)+1);
                            strcpy(tmp, yytext);

                            char * ch = tmp; 
                            // find the comma inside the string
                            for (int i = 0; ch[i] != ','; i++){
                            	if(ch[i] == ','){
                            		// comma is in there for sure when I match this regexp
                            		// so when I find it, I replace it with the dot to use internal
                            		// c rappresentation for float numpers
                            		ch[i] = '.'; 
                            	}
                          	}
                            yylval.rValue = atof(tmp);
                            // return number with replacement
                            return REALVALUE;
                        }
%%