#ifndef symtable_h
#define symtable_h
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>  // for strcmp()
#include<stdbool.h>

#define MAXSTACK 1000
#define MAXPARAMS 256

/*
#define INT 1
#define FLOAT64 2
#define STRING 3
*/
 
// Datova struktura pre ukladanie informacii o funkcii (glob. premenne nie su sucastou ifj20)
typedef struct global{
    //bool defined;
    //bool is_function;
    int numOfParams;
    int numOfReturns;
    //pole parametrov
    char *params[MAXPARAMS];
    //pole typov parametrov
    int paramsType[MAXPARAMS];
    //pole navratovych typov
    int returns[MAXPARAMS];
    //struct tBSTNodeLocal * LocalVarTree;   // pointer to the tree of local variables of the function, if is_funtion == False then LocalVarTree is NULL;
}functionData ;



// Lokalny strom. resp. pre premenne,konstanty,... vo funkcii,if-else,for
// Note : We dont need to know if local variable is defined because it has to be defined before it is used.
typedef struct tBSTNodeLocal {
    char* Name;                    /* key */  /* data */
    int Type;
    char *Data;
	struct tBSTNodeLocal* LPtr;    /* left subtree*/
	struct tBSTNodeLocal* RPtr;    /* right subtree */
} *tBSTNodePtrLocal;


// Strom pre funkcie resp. Globalny strom
typedef struct tBSTNodeGlobal {
    char * Name;                    /* key */
    functionData content;               /* data */
	struct tBSTNodeGlobal * LPtr;         /* left subtree */
	struct tBSTNodeGlobal * RPtr;         /* right subtree */
} *tBSTNodePtrGlobal;

// Hlavny zasobnik. Uklada Stromy obsahujuce info
typedef struct	{                          
    int top;
    tBSTNodePtrLocal a[MAXSTACK];
    tBSTNodePtrGlobal b[MAXSTACK];
}MainStack;

// Pomocny zasobnik. Uklada ukazatel na osobite ramce(stromy) 
typedef struct {
    tBSTNodePtrLocal *a[MAXSTACK];
    tBSTNodePtrGlobal *b[MAXSTACK];
    int top;
}HelpStack;


/* function prototypes */

// Initialize tree for global variables or func
void BSTInitGlobal   (tBSTNodePtrGlobal *);
// Find out if the func/var is already in tree + get data
bool BSTSearchGlobal  (tBSTNodePtrGlobal, char *, functionData *);
// Insert func/var into tree + data
void BSTInsertGlobal (tBSTNodePtrGlobal *, char *, functionData);
// Delete entire tree
void BSTDisposeGlobal (tBSTNodePtrGlobal *);
// Inicializacia functionData
//void BSTFunctionDataInit (functionData *);


void BSTInitLocal   (tBSTNodePtrLocal *);
// Find out if the func/var is already in tree + get data
bool BSTSearchLocal (tBSTNodePtrLocal RootPtr, char* name, int *Type, char *Data);
// Insert func/var into tree + data
void BSTInsertLocal (tBSTNodePtrLocal* RootPtr, char * Name, int *Type, char *Data);
// Delete entire tree
void BSTDisposeLocal(tBSTNodePtrLocal *);
#endif
