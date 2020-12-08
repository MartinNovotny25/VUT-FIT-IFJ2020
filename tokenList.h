/*
*  Predmet: Formalne jazyky a prekladace (IFJ) - FIT VUT v Brne
*  Obojsmerny zoznam tokenov pre projekt IFJ2020 (hlavickovy subor)
*  Vytvoril: Andrej Hyros - xhyros00
*  Datum: 10/2020
*/

#ifndef tokenList_h
#define tokenList_h

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "scanner.h"


typedef struct TDLLElem {                 //prvok dvojsmerneho zoznamu tokenov(TDLL)
        TOKEN tdata;                      //data prvku
        struct TDLLElem *lptr;            //ukazatel na predchadzajuci prvok
        struct TDLLElem *rptr;            //ukazatel na nasledujuci prvok
} *TDLLElemPtr;


typedef struct {                          //dvojsmerne viazany zoznam tokenov(TDLL)
    TDLLElemPtr First;                    //ukazatel na prvy prvok zoznamu
    TDLLElemPtr Act;                      //ukazatel na aktivny prvok zoznamu
    TDLLElemPtr Last;                     //ukazatel na posledny prvok zoznamu
} TDLList;

/* ---------------------PROTOTYPY FUNKCII NAD DL ZOZNAMOM TOKENOV--------------------- */

void TDLLPrintAllTokens(TDLList *);       //vytlaci vsetky tokeny v liste
void TDLLInitList (TDLList *);            //inicializacia zoznamu
void TDLLDisposeList (TDLList *);         //zrusenie vsetkych prvkov
void TDLLInsertFirst (TDLList *, TOKEN);  //vlozenie prvku na zaciatok zoznmu
void TDLLInsertLast(TDLList *, TOKEN);    //vlozenie prvku na koniec zoznamu
void TDLLFirst (TDLList *);               //nastavenie aktivity na prvy prvok
void TDLLLast (TDLList *);                //nastavenie aktivity na posedny prvok
void TDLLCopyFirst (TDLList *, TOKEN *);  //vracia hodnotu prveho prvku
void TDLLCopyLast (TDLList *, TOKEN *);   //vracia hodnotu posledneho prvku
void TDLLDeleteFirst (TDLList *);         //odstranenie prveho prvku
void TDLLDeleteLast (TDLList *);          //odstranenie posledneho prvku
void TDLLPostDelete (TDLList *);          //odstranenie prvku za aktivnym prvkom
void TDLLPreDelete (TDLList *);           //odstranenie prvku pred aktivnym prvkom
void TDLLPostInsert (TDLList *, TOKEN);   //vlozenie prvku za aktivny prok
void TDLLPreInsert (TDLList *, TOKEN);    //vlozenie prvku pred aktivny prvok
void TDLLCopy (TDLList *, TOKEN *);       //vracia hodnotu aktivneho prvku
void TDLLActualize (TDLList *, TOKEN);    //prepisanie dat aktivneho prvku
void TDLLSucc (TDLList *);                //posun aktivity doprava
void TDLLPred (TDLList *);                //posun aktivity dolava
int TDLLActive (TDLList *);               //zistuje aktivitu zoznamu


#endif /* tokenList_h */
