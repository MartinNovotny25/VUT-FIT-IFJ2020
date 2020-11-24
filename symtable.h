#ifndef symtable_h
#define symtable_h
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>  // for strcmp()
#include<stdbool.h>

#define MAXSTACK 1000

// Datova struktura pre ukladanie informacii o funkcii (glob. premenne nie su sucastou ifj20)
typedef struct global{
    bool defined;
    bool is_function;
    int num_of_params;
    int num_of_return_vars;
    //struct tBSTNodeLocal * LocalVarTree;   // pointer to the tree of local variables of the function, if is_funtion == False then LocalVarTree is NULL;
}global_t ;



// Lokalny strom. resp. pre premenne,konstanty,... vo funkcii,if-else,for
// Note : We dont need to know if local variable is defined because it has to be defined before it is used.
typedef struct tBSTNodeLocal {
    char* Name;                    /* key */  /* data */
    char *Type;
    char *Data;
	struct tBSTNodeLocal* LPtr;    /* left subtree*/
	struct tBSTNodeLocal* RPtr;    /* right subtree */
} *tBSTNodePtrLocal;


// Strom pre funkcie resp. Globalny strom
typedef struct tBSTNodeGlobal {
    char * Name;                    /* key */
    global_t content;               /* data */
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
bool BSTSearchGlobal  (tBSTNodePtrGlobal, char *, global_t *);
// Insert func/var into tree + data
//void BSTInsertGlobal (tBSTNodePtrGlobal *, char *, global_t);
// Delete entire tree
void BSTDisposeGlobal (tBSTNodePtrGlobal *);


void BSTInitLocal   (tBSTNodePtrLocal *);
// Find out if the func/var is already in tree + get data
bool BSTSearchLocal  (tBSTNodePtrLocal RootPtr, char *Name);
// Insert func/var into tree + data
void BSTInsertLocal (tBSTNodePtrLocal* RootPtr, char * Name, char *Type, char *Data);
// Delete entire tree
void BSTDisposeLocal(tBSTNodePtrLocal *);
#endif
