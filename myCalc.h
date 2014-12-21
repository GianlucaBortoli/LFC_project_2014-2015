/* Bortoli Gianluca, n° 159993 */

///////////////////////////
// FUNCTIONS DEFINITIONS //
///////////////////////////
void yyerror(char *);
int yylex();

nodeType * opr(int, int, ...);
nodeType * con(void *, varEnum);
nodeType * id(char const *);
nodeType * dic(char *, varEnum);
//nodeType * block(nodeType * next); //add it for scoping

symrec * getsym(char const *);
symrec * putsym(char const *, varEnum);

varEnum biggestType(varEnum, varEnum);

//////////////////////
// GLOBAL VARIABLES //
//////////////////////
extern symrec * symTable; 

/////////////////////
// USED STRUCTURES //
/////////////////////

/*
* allowed types for variables
* The order in which they are declared here is VERY important:
* booltype < inttype < realtype
* Do not change, biggestType function won't work any more!
*/
typedef enum{BOOLTYPE, INTTYPE, REALTYPE} varEnum; 

/* 
* used in the struct nodeType to define the type of node
*/
typedef enum {nodeDic, nodeCon, nodeId, nodeOpr} nodeEnum; // add nodeBlock for scoping

/*
* symbol table, stored as list of symrec
* see symrec * next
*/
typedef struct symrec {
	char * name; // name of record in symbol table
    union { // yylval given by yacc. one between int, real and bool
        int     i;
        float   r;
        int     b; //bool stored as int
    };
    // one of types from varEnum above
    // INTTYPE, BOOLTYPE, REALTYPE
    varEnum type; 
	struct symrec * next;
} symrec;

/*
* defines one type of node I can have from nodeEnum above 
* con -> constant
* id -> identifiers/variables
* opr -> operator
* dic -> declaration
*/
typedef struct nodeType{
    nodeEnum type;              
    union {
        conNodeType con;       
        idNodeType id;          
        oprNodeType opr;       
        dicNodeType dic;        
        //struct nodeType * blk;  /* scooping and procedure */
    };
} nodeType;

/*
* declaration
*/
typedef struct {
    char * name;
    varEnum type;
} dicNodeType;

/* 
* constant
*/
typedef struct {
    union {
        int i;
        float r;
        int b;
    };
    varEnum type;
} conNodeType;

/* 
* identifiers 
*/
typedef struct {
	char * name;
} idNodeType;

/* 
* operators 
*/
typedef struct {
    int  oper;              /* operator */
    int  nops;              /* number of operands */
    struct nodeType **op;	/* operands */
} oprNodeType;