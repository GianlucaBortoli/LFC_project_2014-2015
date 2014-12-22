/* Bortoli Gianluca, n° 159993 */

%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "myCalc.h"

	symrec * symTable = NULL; //global symbol table; check for scoping management
%}

%error-verbose // yacc stuff

%union {
    int     iValue;//int
    float   rValue;//real
    int     bValue;//bool

    nodeType * nPtr;

    char * varName;
};

////////////
// TOKENS //
////////////
%token <iValue> INTEGER
%token <rValue> REALVALUE
%token <bValue> BOOLEAN
%token <varName> VARIABLE
%token WHILE IF PRINT FOR TO INT REAL BOOL AND OR NOT PRINTINT PRINTREAL

//////////////////////////////////////////////////
// PRECEDENCE & ASSOCIATIVITY FOR ALL OPERATORS //
//////////////////////////////////////////////////
%nonassoc IFX
%nonassoc ELSE
%left EQ
%left AND OR
%left DBE NE GT LT GRE LRE
%left PLUS MIN
%left MULT DIV
%right NOT
%nonassoc UMINUS RCURLY LCURLY LP RP COMMA SEMICOLON INTEGER REALVALUE BOOLEAN MAIN

////////////////////
// EXPECTED TYPES //
////////////////////
%type <nPtr> stmt expr stmt_list opt_stmt_list var declaration opt_declaration_list

%%

//////////////////////
// GRAMMAR && RULES //
//////////////////////
program: opt_declaration_list
        MAIN
        opt_stmt_list   {
                            ex($1); // variables declared before main
                            ex($3); // execute main
                            exit(0);
                        }
        ;

opt_declaration_list: 	/* empty */       						{$$ = NULL;} // declaration of variables before main is optional -> NULL
            			| opt_declaration_list declaration  	{$$ = opr(SEMICOLON, 2, $1, $2);}
            			;

declaration:  INT  VARIABLE SEMICOLON    { $$ = dic($2, INTTYPE); } // usual variable declaration inside main
   			| REAL VARIABLE SEMICOLON    { $$ = dic($2, REALTYPE); }
   			| BOOL VARIABLE SEMICOLON    { $$ = dic($2, BOOLTYPE); }
   			;

var: VARIABLE      {$$ = id($1);}
	 ;

opt_stmt_list:	/* empty */ 	{$$ = NULL;} // this declaration is optional -> NULL
				| stmt_list
				;

stmt_list:		stmt_list stmt 	{$$ = opr(SEMICOLON, 2, $1, $2);}
				| stmt     		{$$ = $1;}
				;

stmt: SEMICOLON                                     {$$ = opr(SEMICOLON, 2, NULL, NULL);}
    | declaration 									{$$ = $1;}
    | expr SEMICOLON   								{$$ = $1;}
    | PRINT expr SEMICOLON                          {$$ = opr(PRINT,1,$2);}
    | PRINTINT expr SEMICOLON                       {$$ = opr(PRINTINT,1,$2);}
    | PRINTREAL expr SEMICOLON                      {$$ = opr(PRINTREAL,1,$2);}
    | var EQ expr SEMICOLON                   		{$$ = opr(EQ,2,$1,$3);}
    | WHILE LP expr RP stmt                         {$$ = opr(WHILE,2,$3,$5);}
    | IF LP expr RP stmt %prec IFX                  {$$ = opr(IF,2,$3,$5);}
    | IF LP expr RP stmt ELSE stmt                  {$$ = opr(IF,3,$3,$5,$7);}
    | FOR LP var EQ expr TO expr RP stmt       		{$$ = opr(FOR,4,$3,$5,$7,$9);}
    | LCURLY stmt_list RCURLY                       {$$ = $2;}
    ;

expr: var      					{$$ = $1;} // variable
    | LP expr RP    			{$$ = $2;}
    | expr PLUS expr  			{$$ = opr(PLUS,2,$1,$3);} // operations on numbers
    | expr MIN expr   			{$$ = opr(MIN,2,$1,$3);}
    | expr MULT expr   			{$$ = opr(MULT,2,$1,$3);}
    | expr DIV expr   			{$$ = opr(DIV,2,$1,$3);}
    | MIN expr %prec UMINUS    	{$$ = opr(UMINUS,1,$2);}
    | expr LT expr          	{$$ = opr(LT,2,$1,$3);} // operations on booleans
    | expr GT expr          	{$$ = opr(GT,2,$1,$3);}
    | expr LRE expr         	{$$ = opr(LRE,2,$1,$3);}
    | expr GRE expr         	{$$ = opr(GRE,2,$1,$3);}
    | expr NE expr          	{$$ = opr(NE,2,$1,$3);}
    | expr DBE expr         	{$$ = opr(DBE,2,$1,$3);}
    | expr AND expr         	{$$ = opr(AND,2,$1,$3);}
    | expr OR expr          	{$$ = opr(OR,2,$1,$3);}
    | NOT expr              	{$$ = opr(NOT,1,$2);}
    | INTEGER	      			{$$ = con($1, INTTYPE);} // types are constants nodes
    | REALVALUE      			{$$ = con($1, REALTYPE);}
    | BOOLEAN       			{$$ = con($1, BOOLTYPE);}
    ;

%%

int main(){
    yyparse();
    return 0;
}