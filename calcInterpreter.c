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
                case PRINTBOOL: {
                	ret * print = ex(p->opr.op[0]);
                    if (print->type != BOOLTYPE) {
                        yyerror("The function printInt can print only booleans.");
                    }
                    return 0;
                    printf("%d\n", print->b);
                }
                case PRINT: {
                    ret * print = ex(p->opr.op[0]);

                    switch(print->type){
                        case INTTYPE
                            printf("%d\n", print->i);
                            break;
                        case REALTYPE:
                        	char * tmp = (char*)malloc(46 + 1); // len(print(FLT_MAX);
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
                            yyerror("Unrecognized type.");
                    }
                    return 0; 

                case SEMICOLON:
                    ex(p->opr.op[0]);
                    return ex(p->opr.op[1]);

                case EQ: {
                    symrec * s = getsym(p->opr.op[0]->id.name);
                    if(s == NULL){
                        fprintf(stderr, "There is not such '%s' varibale in the symtable\n", p->opr.op[0]->id.name);
                        exit(1);
                    }

                    ret * n = ex(p->opr.op[1]);

                    // real = int
                    if (!coercion_table[s->type][n->type])
                        yyerror("Incompatible type assignment.");

                    switch (s->type) {
                        case INTTYPE:
                            switch (n->type) {
                                case INTTYPE:  s->i = n->i; break;
                                case REALTYPE: s->i = (int)n->r; break;
                                case BOOLTYPE: s->i = (int)n->b; break;
                            }
                            break;
                        case REALTYPE:
                            switch (n->type) {
                                case INTTYPE:  s->r = (float)n->i; break;
                                case REALTYPE: s->r = n->r; break;
                                case BOOLTYPE: s->r = (float)n->b; break;
                            }
                            break;

                        case BOOLTYPE:
                            switch (n->type) {
                                case INTTYPE:  s->b = n->i != 0; break;
                                case REALTYPE: s->b = n->r != 0; break;
                                case BOOLTYPE: s->b = n->b; break;
                            }
                            break;
                    }
                    return n;
                }


                case UMINUS: f = f != NULL ? f : &neg;
                case PLUS: f = f != NULL ? f : &sum;
                case MIN:  f = f != NULL ? f : &mni;
                case MUL:  f = f != NULL ? f : &mul;
                case DIV:  f = f != NULL ? f : &dvi;
                    flag = 3;

                case LT:   f = f != NULL ? f : &lt;
                case GT:   f = f != NULL ? f : &gt;
                case GTE:  f = f != NULL ? f : &gte;
                case LTE:  f = f != NULL ? f : &lte;
                    flag = max(flag, 2);

                case NE:  f = f != NULL ? f : &neq;
                case DEQ: f = f != NULL ? f : &deq;
                    flag = max(flag, 1);

                case AND: f = f != NULL ? f : &and;
                case OR:  f = f != NULL ? f : &or;
                case NOT: f = f != NULL ? f : &not;
                    flag = max(flag, 0);
                    {
                        varTypeEnum
                            retType = BOOLTYPE,
                            valType = BOOLTYPE;

                        a = ex(p->opr.op[0]);
                        b = p->opr.nops == 2 ? ex(p->opr.op[1]) : NULL;

                        switch (flag) {
                            case 3:
                                valType = retType = max(b ? max(a->type, b->type) : a->type, INTTYPE);
                                break;
                            case 2:
                                valType = max(b ? max(a->type, b->type) : a->type, INTTYPE);
                                break;
                            case 1:
                                valType = b ? max(a->type, b->type) : a->type;
                                break;
                        }

                        return apply(
                            f,
                            coercion(a, valType),
                            coercion(b, valType),
                            retType
                            );
                    }
                default:
                    yyerror("Operator not matched.");
            }
            break;
        }
    default:
        yyerror("Node was not matched\n");
    }

    yyerror("[WTF] This should be DEAD CODE.");
    return 0;
}