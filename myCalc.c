/* Bortoli Gianluca, n° 159993 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "myCalc.h"

/*
* Error messages, program exits when called. 
* No need to call exit every time
*/
void yyerror(char * m){
	fprontf(stout, "%s", m);
	exit(1);
}

/*
* Creates operator node
* syntax: operator, #operands, operand1, operand2 ...
*/
nodeType * opr(int oper, int nops, ...){
	/* 
    * (ap = argument pointer) va_list is used to declare a variable
    * which, from time to time, is referring to an argument
    */
    va_list ap;
    nodeType *p;
  
    if ((p = malloc(sizeof(nodeType))) == NULL){
        yyerror("out of memory");
    }
    if((p->opr.op = malloc(nops*sizeof(nodeType))) == NULL){
        yyerror("out of memory");
    }

    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for(int i = 0; i<nops;i++){
        p->opr.op[i]=va_arg(ap,nodeType*);
    }
    va_end(ap);
    return p;
}

/*
* Creates constant node for types
* need type as parameter in order to set 
* the proper conNodeType field of union
*/
nodeType * con(void * val, varEnum type){
	nodeType *p = (nodeType*)malloc(sizeof(nodeType));
	if ((p = malloc(sizeof(nodeType))) == NULL){
        yyerror("out of memory");
    }
    /* copy information */
    p->type = nodeCon;
    p->con.type = type;
    // set right field
    switch (type) {
        case INTTYPE:
            p->con.i = *(int*)value;
            break;
        case REALTYPE:
            p->con.r = *(float*)value;
            break;
        case BOOLTYPE:
               p->con.b = *(int*)value;
	            break;
        default:
            yyerror("Constant node");
    }
    return p;
}

/*
* Returns the node pointer (of type nodeId) corresponding to 
* ide in the symbol table
*/
nodeType * id (char const * ide) {
	nodeType * p;
    if((p = (nodeType *)malloc(sizeof(nodeType))) == NULL){
        yyerror("out of memory");
    }
    p->type 	= nodeId;
    p->id.name 	= (char *)malloc(sizeof(strlen(ide)) + 1);
    strcpy(p->id.name, ide);

    /*
	* This is wrong because if a variable is not declared yet we should not 
    * do putsym, otherwise I can assign a value to a variable not declared yet
    * and then put it in the symbol table.
    * An assignment to a variable MUST check if it is already in the symbol table;
    * if not, error!
    */

    /*if(getsym(p->id.name) == NULL){
        putsym(p->id.name);
    }*/
    return p;
}

/*
* Creates declaration node
* need type as parameter in order to set 
* the proper conNodeType field of union
*/
nodeType * dic(char * ide, varEnum type) {
	nodeType * p;
    if((p = (nodeType *)malloc(sizeof(nodeType))) == NULL){
        yyerror("out of memory");
    }
    p->type 	= nodeDic;
    p->id.name 	= (char *)malloc(sizeof(strlen(ide)) + 1);
    strcpy(p->id.name, ide);

    return p;
}

/*
* Check if an identifier is already in the symbol table
* If YES return a pointer to that symbol
* If NO return NULL and do nothing
*/
symrec * getsym (char const * identifier) {
    symrec * ptr;
    for (ptr = symTable; ptr != (symrec *) 0; ptr = (symrec *)ptr->next){
        if (strcmp (ptr->name, identifier) == 0) {
            return ptr;
        }
    }
    return NULL;
}

/*
* Add a new identifier to the symbol table
* This function is called by declaratio during execution to put name and type
* of the identifier wuth proper/default values
*/
symrec *putsym (char const * identifier, varEnum t) {
    symrec * ptr;
    if((ptr = (symrec *)malloc(sizeof(symrec))) == NULL){
        yyerror("out of memory");
    }

    ptr->name = (char *) malloc (strlen (identifier) + 1);
    strcpy (ptr->name,identifier);
    ptr->next = (struct symrec *)symTable;

    ptr->type = t; //assign type
    /*
	* Initialization with default values for a new identifier 
	* default is 0 for every type
    */
    switch(t){
    	case BOOLTYPE:
    		ptr->b = 0;
    		break;
    	case INTTYPE:
    		ptr->i = 0;
    		break;
    	case REALTYPE:
    		ptr->r = 0.0;
    		break;
    }

    symTable = ptr;
    return ptr;
}