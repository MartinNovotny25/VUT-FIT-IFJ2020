/*
*  Predmet: Formalne jazyky a prekladace (IFJ) - FIT VUT v Brne
*  Hlavickovy subor tabulky symbolov(sucast semantickej analyzy) pre projekt IFJ2020
*  Vytvoril: Peter Cellar - xcella01
*
*  Datum: 10/2020
*/

#ifndef symtable_h
#define symtable_h
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
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
    int numOfParams;                    //pocet parametrov
    int numOfReturns;                   //pocetnavratovych hodnot
    char *params[MAXPARAMS];            //pole ukladajuce mena parametrov
    int paramsType[MAXPARAMS];          //pole typov parametrov
    int returns[MAXPARAMS];             //pole navratovych typov
}functionData ;



// Lokalny strom. resp. pre premenne,konstanty,... vo funkcii,if-else,for
typedef struct tBSTNodeLocal {
    char* Name;                         //kluc
    int Type;                           //typ premennej
    char *Data;                         //momentalne nevyuzite
	struct tBSTNodeLocal* LPtr;         //lavy podstrom
	struct tBSTNodeLocal* RPtr;         //pravy podstrom
} *tBSTNodePtrLocal;


// Strom pre funkcie resp. Globalny strom
typedef struct tBSTNodeGlobal {
    char * Name;                        //kluc
    functionData content;               //informacie o funkcii
	struct tBSTNodeGlobal * LPtr;       //lavy podstrom
	struct tBSTNodeGlobal * RPtr;       //pravy podstrom
} *tBSTNodePtrGlobal;

// Hlavny zasobnik. Uklada ukazatele na stromy
typedef struct	{                          
    int top;                            //vrchol zasobnika(pocet aktualne ulozenych ukazatelov)
    tBSTNodePtrLocal *a[MAXSTACK];      //pole ukazatelov
}MainStack;


/* prototypy funkcii */


void BSTInitGlobal   (tBSTNodePtrGlobal *);
bool BSTSearchGlobal  (tBSTNodePtrGlobal, char *, functionData *);
void BSTInsertGlobal (tBSTNodePtrGlobal *, char *, functionData);
void BSTDisposeGlobal (tBSTNodePtrGlobal *);



void BSTInitLocal   (tBSTNodePtrLocal *);
bool BSTSearchLocal (tBSTNodePtrLocal RootPtr, char* name, int *Type, char *Data);
void BSTInsertLocal (tBSTNodePtrLocal* RootPtr, char * Name, int *Type, char *Data);
void ReplaceByRightmost (tBSTNodePtrLocal PtrReplaced, tBSTNodePtrLocal *RootPtr);
void BSTDelete (tBSTNodePtrLocal *RootPtr, char *K);
void BSTDisposeLocal(tBSTNodePtrLocal *);



void InitMainStack (MainStack *S);
void PushTreeMain (MainStack *S, tBSTNodePtrLocal *ptrLocal);
tBSTNodePtrLocal PopTreeMain (MainStack *S);
bool EmptyMainStack (MainStack *S);

#endif
