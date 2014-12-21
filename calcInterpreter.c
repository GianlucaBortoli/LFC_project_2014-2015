#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "myCalc.h"
#include "y.tab.h"

typedef conNodeType ret;
/*
* Execute the parse tree
* returns conNodeType to manage the given types, not only int
*/
ret * ex(nodeType *p) {
    if (p == 0){ //if node doesn't exist, exit
    	return 0;
    }  

    switch(p->type) { //check wich type of node it is
    	// possible nodeDic, nodeCon, nodeId, nodeOpr (see myCalc.h enum)
        case nodeDic: {
	      	//check if var already declared earlier
	        if (getsym(p->dic.name)) { 
	            fprintf(stderr, "%s is already declared.", p->dic.name);
	            exit(1);
	        }
	        //if not decleared, put in symbol table its name and type
	        putsym(p->dic.name, p->dic.type);
	        return 0;
        }

        case nodeCon: {
        	// return directly the constant node of p
            return &(p->con);
        }

        case nodeId: {
        	//check if an id with such name is already declared
            symrec * s = getsym(p->id.name);
            if(s == NULL){
                fprintf(stdout, "%s variable does not exist\n", p->id.name);
                exit(1);
            }

            ret * r = malloc(sizeof(ret));
            r->type = s->type; //type is the one of the var in the sym table
            switch(s->type){ //assign value depending on the type
                case INTTYPE:
                    r->i = s->i;
                    break;
                case REALTYPE:
                    r->r = s->r;
                    break;
                case BOOLTYPE:
                    r->b = s->b;
                    break;
                default: //type not recognized
                    yyerror("Type not recognized.");
            }
            return r;
        }

        case nodeOpr: {
            switch(p->opr.oper) {
                case WHILE: {
                    while((*ex(p->opr.op[0])).b)
                        ex(p->opr.op[1]);
                    return 0;
                }

                case FOR: { 
                	symrec * s = getsym(p->opr.op[0]->id.name);
                	s->value = ex(p->opr.op[1]);

                	while(s->value - ex(p->opr.op[2])){
                        ex(p->opr.op[3]);
                        s->value++;
                    }
                    return 0;
                }

                case IF: {
                	if (ex(p->opr.op[0])){                  
                        ex(p->opr.op[1]);
                    }
                    else if (p->opr.nops > 2) {
                        ex(p->opr.op[2]);
                    }
                    return 0;
                }
                /*
				* The print function is already polymorphic: printInt e printBool
				* is a sort of workaround in order to pass early tests.
				* I implemented the java-style print before writing printInt e printFloat
                */
                case PRINTINT: {
                	ret * print = ex(p->opr.op[0]);
                    if (print->type != INTTYPE) {
                        yyerror("The function printInt can print only integers.");
                    }
                    return 0;
                    printf("%d\n", print->i);
                }

                case PRINTREAL: {
                	ret * print = ex(p->opr.op[0]);
                    if (print->type != REALTYPE) {
                        yyerror("The function printFloat can print only float.");
                    }
                    return 0;
                    printf("%f\n", print->r);
                }

                case PRINT: {
                	// I already do type checking in the print function
                	// switch over types in order to print in the right way the value
                    ret * print = ex(p->opr.op[0]);
                    switch(print->type){
                        case INTTYPE
                            printf("%d\n", print->i);
                            break;
                        case REALTYPE:
                        	// 46 is the maximum length of float in C
                       		// I didn't figured out a smart(er) way to do this 
                        	char * tmp = (char*)malloc(46 + 1);
                            sprintf(tmp, "%f", to_print->r);
                            // substitute comma with dot, again
                            char * ch = tmp;
                            for(int i = 0; ch[i] != '.'; i++){
                            	ch[i] = ',';
                            }
                            printf("%s\n", tmp);
                            free(tmp);
                            break;
                        case BOOLTYPE:
                            if (print->b)
                                printf("true\n");
                            else
                                printf("false\n");
                            break;
                        default:
                            yyerror("You tried to print an unauthorized type.");
                    }
                    return 0; 
                }

                case SEMICOLON: {
                    ex(p->opr.op[0]);
                    return ex(p->opr.op[1]);
                }

                case EQ: {
                    symrec * s = getsym(p->opr.op[0]->id.name);
                    if(s == NULL){
                        fprintf(stderr, "There is not such '%s' varibale in the symtable\n", p->opr.op[0]->id.name);
                        exit(1);
                    }

                    //add here coercion support
                    ret * val = ex(p->opr.op[1]);
                    switch (val->type) {
                        case INTTYPE:
                            s->i = val->i;
                            break;
                        case REALTYPE:
                            s->r = val->r;
                            break;
                        case BOOLTYPE:
                            s->b = val->b;
                            break;
                        default:
                            yyerror("Not allowed type used.");
                    }
                    return val;
                }

                case UMINUS: {
                	ret * a = ex(p->opr.op[0]);
                    return apply(&neg, a, NULL, a->type);
                }

                case PLUS:{
                    ret * a = ex(p->opr.op[0]);
                    ret * b = ex(p->opr.op[1]);

                    varTypeEnum dstType = max(a->type, b->type);

                    return apply(&sum, a, b, dstType);
                }

                case MIN:{
                    ret * a = ex(p->opr.op[0]);
                    ret * b = ex(p->opr.op[1]);

                    varTypeEnum dstType = max(a->type, b->type);

                    return apply(&min, a, b, dstType);
                }

                case MULT: {
                    ret * a = ex(p->opr.op[0]);
                    ret * b = ex(p->opr.op[1]);

                    varTypeEnum dstType = max(a->type, b->type);

                    return apply(&mul, a, b, dstType);
                }

                case DIV: {
                    ret * a = ex(p->opr.op[0]);
                    ret * b = ex(p->opr.op[1]);

                    // TODO: add here type checking
                    varTypeEnum dstType = max(a->type, b->type);

                    return apply(&dvi, a, b, dstType);
                }

                case LT: {
                    ret * a = ex(p->opr.op[0]);
                    ret * b = ex(p->opr.op[1]);

                    return apply(&lt, a, b, BOOLTYPE);
                }

                case GT: {
                    ret * a = ex(p->opr.op[0]);
                    ret * b = ex(p->opr.op[1]);

                    return apply(&gt, a, b, BOOLTYPE);
                }

                case GRE:{
                    ret * a = ex(p->opr.op[0]);
                    ret * b = ex(p->opr.op[1]);

                    return apply(&gte, a, b, BOOLTYPE);
                }

                case LRE: {
                    ret * a = ex(p->opr.op[0]);
                    ret * b = ex(p->opr.op[1]);

                    return apply(&lte, a, b, BOOLTYPE);
                }

                case NE: {
                    ret * a = ex(p->opr.op[0]);
                    ret * b = ex(p->opr.op[1]);

                    varTypeEnum dstType = max(a->type, b->type);

                    return apply(&neq, a, b, BOOLTYPE);
                }

                case DBQ: {
                    ret * a = ex(p->opr.op[0]);
                    ret * b = ex(p->opr.op[1]);

                    return apply(&deq, a, b, BOOLTYPE);
                }

                case AND: {
                    ret* a = ex(p->opr.op[0]);
                    ret* b = ex(p->opr.op[1]);

                    return apply(&and, a, b, BOOLTYPE);
                }

                case OR: {
                    ret* a = ex(p->opr.op[0]);
                    ret* b = ex(p->opr.op[1]);

                    return apply(&or, a, b, BOOLTYPE);
                }

                case NOT:{
                    ret* a = ex(p->opr.op[0]);
                    return apply(&not, a, NULL, BOOLTYPE);
                }

                default:
                	yyerror("Operator not recognized.");
            }
       		break;

        default:
        	yyerror("Node can not be matched");
        }
    yyerror("Error!");
    exit(1);
}