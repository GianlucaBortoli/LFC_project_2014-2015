/* Bortoli Gianluca, n° 159993 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "myCalc.h"
#include "y.tab.h"
/*
* Execute the parse tree
* returns conNodeType to manage the given types, not only int
*/
conNodeType * ex(nodeType *p) {
    if (p == 0){ //if node doesn't exist, exit
    	return 0;
    }  

    switch(p->type) { //check wich type of node it is
    	// possible nodeDic, nodeCon, nodeId, nodeOpr (see myCalc.h enum)
        case nodeDic: {
	      	//check if var already declared earlier
	        if (getsym(p->dic.name)) { 
	            fprintf(stderr, "%s is already declared\n", p->dic.name);
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

            conNodeType * r = malloc(sizeof(r));
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
                default:
                    yyerror("Type not recognized.");
            }
            return r;
        }

        case nodeOpr: {
            switch(p->opr.oper) {
                case WHILE: {
                    while(coercion(ex(p->opr.op[0]), BOOLTYPE)->b)
                        ex(p->opr.op[1]);
                    return 0;
                }

                case FOR: { 
                    //assignment before for statement
                	ex(opr(EQ, 2, p->opr.op[0], p->opr.op[1]));
                    conNodeType * a;
                    //fake for with while
                	while(ex(opr(LT, 2, p->opr.op[0], p->opr.op[2]))->b) {
                        ex(p->opr.op[3]);
                        // 3:05 AM coded -_-
                        ex(opr(EQ, 2, p->opr.op[0], 
                            con(getTyped(a = ex(opr(PLUS, 2, p->opr.op[0], con(1,INTTYPE)))), a->type)
                            ));
                    }
                    return 0;
                }

                case IF: {
                	if (coercion(ex(p->opr.op[0]), BOOLTYPE)->b){                  
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
                	conNodeType * print = ex(p->opr.op[0]);
                    if (print->type != INTTYPE) {
                        yyerror("The function printInt can print only integers.");
                    }
                    printf("%d\n", print->i);
                    return 0;
                }

                case PRINTREAL: {
                	conNodeType * print = ex(p->opr.op[0]);
                    if (print->type != REALTYPE) {
                        yyerror("The function printFloat can print only float.");
                    }
                    //a label can only be part of a statement and a declaration is not a statement
                    // 46 is the maximum length of float in C
                    // I didn't figured out a smart(er) way to do this 
                    char * tmp = (char *)malloc(46 + 1);
                    sprintf(tmp, "%f", print->r);

                    // substitute comma with dot, again
                    for(int i = 0 ; ; i++){
                        if(tmp[i] == '.'){
                            tmp[i] = ',';
                            break;
                        }
                    }
                    printf("%s\n", tmp);
                    free(tmp);
                    return 0;
                }

                case PRINT: {
                	// I already do type checking in the print function
                	// switch over types in order to print in the right way the value
                    conNodeType * print = ex(p->opr.op[0]);
                    switch(print->type){
                        case INTTYPE:
                            printf("%d\n", print->i);
                            break;
                        case REALTYPE: 
                        	//a label can only be part of a statement and a declaration is not a statement
                        	; 
                        	// 46 is the maximum length of float in C
                       		// I didn't figured out a smart(er) way to do this 
                        	char * tmp = (char *)malloc(46 + 1);
                            sprintf(tmp, "%f", print->r);

                            // substitute comma with dot, again
                            for(int i = 0 ; ; i++){
                                if(tmp[i] == '.'){
                                    tmp[i] = ',';
                                    break;
                                }
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
                        fprintf(stderr, "There is not '%s' varibale in the symbol table\n", p->opr.op[0]->id.name);
                        exit(1);
                    }

					conNodeType * val = coercion(ex(p->opr.op[1]), s->type);
                    if(s->type == val->type){
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
                    } else {
                    	if(val->type == INTTYPE){
                    		if(s->type == BOOLTYPE){
                    			s->b = val->i != 0;
                    		} else if(s->type == REALTYPE) {
                    			s->r = val->i;
                    		} else {
                                yyerror("Not allowed type used.");
                    		}
                    	}
    					yyerror("Not allowed type used.");
                    }
                    return val;
                }

                case UMINUS: {
                	conNodeType * a = ex(p->opr.op[0]); 
                	//type checking
                	switch(a->type) {
                		case INTTYPE:
                			return ex(con(-(a->i), INTTYPE));
                		case REALTYPE:
                			return ex(con(-(a->r), REALTYPE));
                		case BOOLTYPE:
                		default:
                			yyerror("Unable to execute operation");
                	}
                }

                case PLUS:{
					conNodeType * a = ex(p->opr.op[0]);
                    conNodeType * b = ex(p->opr.op[1]);
                    varEnum d = biggestType(a->type,b->type);

                    coercion(a, d);
                    coercion(b, d);

                    if(a->type != BOOLTYPE){
                    	return ex(con((getTyped(a) + getTyped(b)), a->type));
                    } else {
                    	yyerror("Unable to execute operation");
                    }
                }

                case MIN:{
                	conNodeType * a = ex(p->opr.op[0]);
                    conNodeType * b = ex(p->opr.op[1]);
                    varEnum d = biggestType(a->type,b->type);

                    coercion(a, d);
                    coercion(b, d);

                    if(a->type != BOOLTYPE){
                    	return ex(con((getTyped(a) - getTyped(b)), a->type));
                    } else {
                    	yyerror("Unable to execute operation");
                    }
                }

                case MULT:{
                	conNodeType * a = ex(p->opr.op[0]);
                    conNodeType * b = ex(p->opr.op[1]);
                    varEnum d = biggestType(a->type,b->type);

                    coercion(a, d);
                    coercion(b, d);

                    if(a->type != BOOLTYPE){
                    	return ex(con((getTyped(a) * getTyped(b)), a->type));
                    } else {
                    	yyerror("Unable to execute operation");
                    }
                }

                case DIV:{
                	conNodeType * a = ex(p->opr.op[0]);
                    conNodeType * b = ex(p->opr.op[1]);
                    varEnum d = biggestType(a->type,b->type);

                    coercion(a, d);
                    coercion(b, d);

                    if(a->type != BOOLTYPE){
                    	return ex(con((getTyped(a) / getTyped(b)), a->type));
                    } else {
                    	yyerror("Unable to execute operation");
                    }
                }

                case LT:{
                	conNodeType * a = ex(p->opr.op[0]);
                    conNodeType * b = ex(p->opr.op[1]);
                    varEnum d = biggestType(a->type,b->type);

                    coercion(a, d);
                    coercion(b, d);

                    if(a->type != BOOLTYPE){
                    	return ex(con((getTyped(a) < getTyped(b)), BOOLTYPE));
                    } else {
                    	yyerror("Unable to execute operation");
                    }
                }

                case GT:{
                	conNodeType * a = ex(p->opr.op[0]);
                    conNodeType * b = ex(p->opr.op[1]);
                    varEnum d = biggestType(a->type,b->type);

                    coercion(a, d);
                    coercion(b, d);

                    if(a->type != BOOLTYPE){
                    	return ex(con((getTyped(a) > getTyped(b)), BOOLTYPE));
                    } else {
                    	yyerror("Unable to execute operation");
                    }
                }

                case GRE:{
                	conNodeType * a = ex(p->opr.op[0]);
                    conNodeType * b = ex(p->opr.op[1]);
                    varEnum d = biggestType(a->type,b->type);

                    coercion(a, d);
                    coercion(b, d);

                    if(a->type != BOOLTYPE){
                    	return ex(con((getTyped(a) >= getTyped(b)), BOOLTYPE));
                    } else {
                    	yyerror("Unable to execute operation");
                    }
                }

                case LRE:{
                	conNodeType * a = ex(p->opr.op[0]);
                    conNodeType * b = ex(p->opr.op[1]);
                    varEnum d = biggestType(a->type,b->type);

                    coercion(a, d);
                    coercion(b, d);

                    if(a->type != BOOLTYPE){
                    	return ex(con((getTyped(a) <= getTyped(b)), BOOLTYPE));
                    } else {
                    	yyerror("Unable to execute operation");
                    }
                }

                case NE:{
                	conNodeType * a = ex(p->opr.op[0]);
                    conNodeType * b = ex(p->opr.op[1]);
                    varEnum d = biggestType(a->type,b->type);

                    coercion(a, d);
                    coercion(b, d);

                    return ex(con((getTyped(a) != getTyped(b)), BOOLTYPE));
                }

                case DBE:{
                	conNodeType * a = ex(p->opr.op[0]);
                    conNodeType * b = ex(p->opr.op[1]);
                    varEnum d = biggestType(a->type,b->type);

                    coercion(a, d);
                    coercion(b, d);

                    return ex(con((getTyped(a) == getTyped(b)), BOOLTYPE));
                }

                case AND:{
                	conNodeType * a = ex(p->opr.op[0]);
                    conNodeType * b = ex(p->opr.op[1]);

                    coercion(a, BOOLTYPE);
                    coercion(b, BOOLTYPE);

                    return ex(con((getTyped(a) && getTyped(b)), BOOLTYPE));
                }

                case OR:{
                	conNodeType * a = ex(p->opr.op[0]);
                    conNodeType * b = ex(p->opr.op[1]);

                    coercion(a, BOOLTYPE);
                    coercion(b, BOOLTYPE);
                    
                    return ex(con((getTyped(a) || getTyped(b)), BOOLTYPE));
                }

                case NOT:{
                	conNodeType * a = ex(p->opr.op[0]);
                    coercion(a, BOOLTYPE);
                    
                    return ex(con(!getTyped(a), BOOLTYPE));
                }

                default:{
                	yyerror("Operator not recognized.");
                }
            }
       		break;
       	}
       	
        default:
        	yyerror("Node can not be matched");
    }
    yyerror("Error!");
    exit(1);
}