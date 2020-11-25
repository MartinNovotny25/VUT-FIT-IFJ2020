//  Predmet : Formalne jazyky a prekladace (IFJ)
//  tokenList.h - hlavickovy subor pre tokenList.c
//  
//
//  Created by Andrej Hyros on 11/2020.
//

#ifndef tokenList_h
#define tokenList_h

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "scanner.h"


typedef struct TDLLElem {                 /* prvek dvousměrně vázaného seznamu */
        TOKEN tdata;                                  /* užitečná data - token */
        struct TDLLElem *lptr;          /* ukazatel na předchozí prvek seznamu */
        struct TDLLElem *rptr;        /* ukazatel na následující prvek seznamu */
} *TDLLElemPtr;


typedef struct {                           /* dvousměrně vázaný seznam tokenov */
    TDLLElemPtr First;                      /* ukazatel na první prvek seznamu */
    TDLLElemPtr Act;                     /* ukazatel na aktuální prvek seznamu */
    TDLLElemPtr Last;                    /* ukazatel na posledni prvek seznamu */
} TDLList;

/* ---------------------PROTOTYPY FUNKCII NAD DL ZOZNAMOM TOKENOV--------------------- */

void TDLLPrintAllTokens(TDLList *);    //vytlaci vsetky tokeny v liste
void TDLLInitList (TDLList *);          //inicializacia zoznamu
void TDLLDisposeList (TDLList *);       //zrusenie vsetkych prvkov
void TDLLInsertFirst (TDLList *, TOKEN);  //vlozenie prvku na zaciatok zoznmu
void TDLLInsertLast(TDLList *, TOKEN);    //vlozenie prvku na koniec zoznamu
void TDLLFirst (TDLList *);             //nastavenie aktivity na prvy prvok
void TDLLLast (TDLList *);              //nastavenie aktivity na posedny prvok
void TDLLCopyFirst (TDLList *, TOKEN *);  //vracia hodnotu prveho prvku
void TDLLCopyLast (TDLList *, TOKEN *);   //vracia hodnotu posledneho prvku
void TDLLDeleteFirst (TDLList *);       //odstranenie prveho prvku
void TDLLDeleteLast (TDLList *);        //odstranenie posledneho prvku
void TDLLPostDelete (TDLList *);        //odstranenie prvku za aktivnym prvkom
void TDLLPreDelete (TDLList *);         //odstranenie prvku pred aktivnym prvkom
void TDLLPostInsert (TDLList *, TOKEN);   //vlozenie prvku za aktivny prok
void TDLLPreInsert (TDLList *, TOKEN);    //vlozenie prvku pred aktivny prvok
void TDLLCopy (TDLList *, TOKEN *);       //vracia hodnotu aktivneho prvku
void TDLLActualize (TDLList *, TOKEN);    //prepisanie dat aktivneho prvku
void TDLLSucc (TDLList *);              //posun aktivity doprava
void TDLLPred (TDLList *);              //posun aktivity dolava
int TDLLActive (TDLList *);             //zistuje aktivitu zoznamu


#endif /* tokenList_h */
