/* 
**	Part of IFJ Project 2019/2020
**	Author: Samuel Stuchly xstuch06
*/
#ifndef symtable_h
#define symtable_h
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>  // for strcmp()
#include<stdbool.h>

// type for global vars and func data
typedef struct global{
    bool defined;
    bool is_function;
    int num_of_params;
    //struct tBSTNodeLocal * LocalVarTree;   // pointer to the tree of local variables of the function, if is_funtion == False then LocalVarTree is NULL;
}global_t ;



// Tree for local variables
// Note : We dont need to know if local variable is defined because it has to be defined before it is used.
typedef struct tBSTNodeLocal {
    char * Name;                    /* key */  /* data */
	struct tBSTNodeLocal * LPtr;    /* left subtree*/
	struct tBSTNodeLocal * RPtr;    /* right subtree */
} *tBSTNodePtrLocal;


// Tree for global functions and variables
typedef struct tBSTNode {
    char * Name;                    /* key */
    global_t content;               /* data */
	struct tBSTNode * LPtr;         /* left subtree */
	struct tBSTNode * RPtr;         /* right subtree */
} *tBSTNodePtr;


/* function prototypes */

// Initialize tree for global variables or func
void BSTInit   (tBSTNodePtr *);
// Find out if the func/var is already in tree + get data
bool BSTSearch  (tBSTNodePtr, char *, global_t *);
// Insert func/var into tree + data
void BSTInsert (tBSTNodePtr *, char *, global_t);
// Delete entire tree
void BSTDispose(tBSTNodePtr *);

// Initialize tree for local variables
void BSTInitLocal   (tBSTNodePtrLocal *);
// Find out if the func/var is already in tree 
bool BSTSearchLocal  (tBSTNodePtrLocal, char *);
// Insert local var into tree
void BSTInsertLocal (tBSTNodePtrLocal *, char *);
// Delete entire tree
void BSTDisposeLocal (tBSTNodePtrLocal *);


extern tBSTNodePtr GlobalBody ;
extern tBSTNodePtrLocal LocalVariables;

extern tBSTNodePtrLocal  GFDefinedVarsTree;
extern tBSTNodePtrLocal  LFDefinedVarsTree;




#endif /* stack_h */
