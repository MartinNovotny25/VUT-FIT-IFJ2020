/*
*  Predmet: Formalne jazyky a prekladace (IFJ) - FIT VUT v Brne
*  Obojsmerny zoznam tokenov pre projekt IFJ2020
*  Vytvoril: Andrej Hyros - xhyros00
*  Datum: 10/2020
*/
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tokenList.h"
#include "scanner.h"


/*
- tato funckia vytlaci cely obsah zoznamu
@param L ukazatel na zoznam
*/
void TDLLPrintAllTokens(TDLList *L) {
    printf("tokenList: Printing all tokens from DL list...\n\n");
    TDLLFirst(L);
    
    while (L->Act != NULL) {                          //while prechadza listom
        printf("typ   %d\n", L->Act->tdata.type);
        if (L->Act->tdata.type == t_EOF) {
            printf("lexem EOF\n\n");
        } else {
            if (L->Act->tdata.type == t_EOL) {
                printf("lexem EOL\n\n");
            } else {
                printf("lexem %s\n\n", L->Act->tdata.lex);
            }
        }
        
        TDLLSucc(L);                                //presun aktivity na nasledujuci token
    }
    printf("tokenList: Printing Ended Succesfully...\n");
    TDLLFirst(L);                                      //aktivita sa vrati na zaciatok
}

/*
- inicializuje zoznam L pred prvym pouzitim
@param L ukazatel na zoznam
*/
void TDLLInitList (TDLList *L) {

    L->First = NULL;        //vsetky ukazetele zoznamu nastavi na NULL
    L->Last = NULL;
    L->Act = NULL;
}


/*
- odstrani vsetky prvky zoznamu a korektne uvolni pamat
- zanecha zoznam v stave, v akom sa nachadzal po inicializacii
@param L ukazatel na zoznam
*/
void TDLLDisposeList (TDLList *L) {

    TDLLElemPtr tmp = L->First;             //tmp ukazuje na prvy prvok
    while (L->First != NULL) {              //zoznam nie je prazdny
        L->First = L->First->rptr;          //cyklus posuva ukazatel First do prava a uvolnuje pamat prvku pred nim
        free(tmp);
        tmp = L->First;
    }
    L->First = NULL;                        //ukazatele zoznamu nastavi na NULL
    L->Last = NULL;
    L->Act = NULL;
}

/*
 - vlozi novy prvok na zaciatok zoznamu
 @param L ukazatel na zoznam
 @param token token ktory bude ulozeny do zoznamu
 */
void TDLLInsertFirst (TDLList *L, TOKEN token) {

    TDLLElemPtr tmp = (TDLLElemPtr) malloc(sizeof(struct TDLLElem));       //alokacia pamate
    if (tmp == NULL) {                                 //overenie uspesnosti alokacie
        return;
    }
    tmp->tdata = token;                                    //nakopirovanie dat
    tmp->rptr = L->First;                               //prepojenie ukazatelov
    tmp->lptr = NULL;
    if (L->First == NULL) {                             //zoznam je prazdny
        L->Last = tmp;                                  //posledny prvok bude tmp
    }
    else {                                              //zoznam nie je prazdny
        L->First->lptr = tmp;                           //lavy ukazate prveho bude ukazovat na novy prvok
    }
    L->First = tmp;                                     //prvy prvok bude tmp
}


/*
- vlozi novy prvok na koniec zoznamu
@param L ukazatel na zoznam
@param token token ktory bude ulozeny do zoznamu
*/
void TDLLInsertLast(TDLList *L, TOKEN token) {

    TDLLElemPtr tmp = (TDLLElemPtr) malloc(sizeof(struct TDLLElem));       //alokacia pamate
    if (tmp == NULL) {
        return;
    }
    tmp->tdata = token;                                    //nakopirovanie dat
    tmp->rptr = NULL;                                   //pravy ukazatel noveho prvku musi byt NULL
    tmp->lptr = L->Last;                                //lavy ukazatl noveho ukazuje na Last
    if (L->Last == NULL) {                              //zoznam je prazdny
        L->First = tmp;
    }
    else {                                              //zoznam nieje prazdny
        L->Last->rptr = tmp;
    }
    L->Last = tmp;
}

/*
 - nastavi aktivitu na prvy prvok zoznamu
 @param L ukazatel na zoznam
 */
void TDLLFirst (TDLList *L) {

    L->Act = L->First;                  //nastavenie aktivity na prvy prvok
}


/*
- nastavi aktivitu na posledny prvok zoznamu
@param L ukazatel na zoznam
*/
void TDLLLast (TDLList *L) {

    L->Act = L->Last;                  //nastavenie aktivity na posledny prvok
}

/*
 - vracia obsah prveho prvku premennou token
 @param L ukazatel na zoznam
 @param token v tejto premennej sa vrati obsah prvku
 */
void TDLLCopyFirst (TDLList *L, TOKEN *token) {

    if (L->First == NULL) {             //zoznam je prazdny
        return;
    }
    *token = L->First->tdata;              //kopirovanie dat prveho prvku
}

/*
- vracia obsah posledneho prvku premennou token
@param L ukazatel na zoznam
@param token v tejto premennej sa vrati obsah prvku
*/
void TDLLCopyLast (TDLList *L, TOKEN *token) {

    if (L->First == NULL) {             //zoznam je prazdny
           return;
       }
       *token = L->Last->tdata;            //kopirovanie dat z posledneho prvku
}

/*
 - zrusi prvy prvok zoznamu L
 @param L ukazatel na zoznam
 */
void TDLLDeleteFirst (TDLList *L) {

    if (L->First != NULL) {                         //zoznam nie je prazdny
        TDLLElemPtr tmp = L->First;
        if (L->First == L->Act) {
            L->Act = NULL;
        }
        if (L->First == L->Last) {                  //zoznam ma jediny prvok
            tmp = L->First;
            L->First = NULL;
            L->Last = NULL;
        }
        else {                                      //zoznam ma viac prvkov
            tmp = L->First;
            L->First = L->First->rptr;
            L->First->lptr = NULL;
        }
        free(tmp);
    }
}

/*
- zrusi posledny prvok zoznamu L
@param L ukazatel na zoznam
*/
void TDLLDeleteLast (TDLList *L) {

    if (L->First != NULL) {                      //zoznam nie je prazdny
        TDLLElemPtr tmp = L->First;
        if (L->First == L->Act) {
            L->Act = NULL;
        }
        if (L->First == L->Last) {               //zoznam ma jediny prvok
            tmp = L->Last;
            L->First = NULL;
            L->Last = NULL;
        }
        else {                                   //zoznam ma viac prvkov
            tmp = L->Last;
            L->Last = L->Last->lptr;
            L->Last->rptr = NULL;
        }
        free(tmp);
    }
}

/*
- zrusi prvok zoznamu L za aktivnym prvkom
@param L ukazatel na zoznam
*/
void TDLLPostDelete (TDLList *L) {

    if (L->Act && L->Act != L->Last) {              //zoznam musi byt aktivny inde ako na posledom prvku
        TDLLElemPtr tmp = L->Act->rptr;              //tmp bude ukazovat na odstranovany prvok
        if (tmp == L->Last) {                       //mazany prvok je posledny
            L->Act->rptr = NULL;                    //pravy ukazatel aktivneho prvku bude NULL
            L->Last = L->Act;                       //aktivny prvok sa stava poslednym
        }
        else {                                      //mazany prvok nieje posledny
            L->Act->rptr = tmp->rptr;               //pravy ukazatel aktivneho ukazuje vpravo od mazaneho
            tmp->rptr->lptr = L->Act;               //lavy ukazetel vpravo od mazaneho bude ukazovat na aktivny
        }
        free(tmp);                                  //uvolnenie pamati mazaneho prvku
    }
}


/*
- zrusi prvok zoznamu L pred aktivnym prvkom
@param L ukazatel na zoznam
*/
void TDLLPreDelete (TDLList *L) {

    if (L->Act && L->Act != L->First) {             //zoznam musi byt aktivny inde ako na prvom prvku
        TDLLElemPtr tmp = L->Act->lptr;              //tmp bude ukazovat na odstranovany prvok
        if (tmp == L->First) {                      //odstranovan prvok je prvy
            L->Act->lptr = NULL;                    //lavy ukazatel aktivneho prvku bude NULL
            L->First = L->Act;                      //aktivny prvok sa stava prvym
        }
        else {                                      //odstranovany prvok nie je prvy
            L->Act->lptr = tmp->lptr;               //lavy ukazatel aktivneho musi ukazovat vlavo od mazaneho
            tmp->lptr->rptr = L->Act;               //pravy ukazatel vpravo od mazaneho musi ukazovat na aktivny
        }
        free(tmp);                                  //uvolnenie pamati mazaneho prvku
    }
}

/*
- vlozi prvok do zoznamu L za aktivny prvok
@param L ukazatel na zoznam
@token vkladane data
*/
void TDLLPostInsert (TDLList *L, TOKEN token) {

    if (L->Act) {                                   //zoznam je aktivny
        TDLLElemPtr tmp = (TDLLElemPtr) malloc(sizeof(struct TDLLElem));   //alokacia pamati
        tmp->tdata = token;                            //kopirovanie dat
        if (L->Act == L->Last) {                    //aktivny prvok je posledny
            L->Act->rptr = tmp;
            tmp->lptr = L->Act;
            tmp->rptr = NULL;
            L->Last = tmp;
        }
        else {                                      //aktivny prvok nie je posledny
            tmp->rptr = L->Act->rptr;               //pravy ukazatel tmp ukazuje vpravo od Act
            tmp->rptr->lptr = tmp;                  //lavy ukazatel prvku vpravo od tmp ukazuje na tmp
            tmp->lptr = L->Act;                     //lavy ukazatel tmp ukazuje na Act
            L->Act->rptr = tmp;                     //pravy ukazatel act ukazuje na tmp
        }
    }
}


/*
- vlozi prvok do zoznamu L pred aktivny prvok
@param L ukazatel na zoznam
@token vkladane data
*/
void TDLLPreInsert (TDLList *L, TOKEN token) {

    if (L->Act) {                                   //zoznam je aktivny
        TDLLElemPtr tmp = (TDLLElemPtr) malloc(sizeof(struct TDLLElem));   //alokacia pamati
        tmp->tdata = token;                            //kopirovanie dat
        if (L->Act == L->First) {                    //aktivny prvok je prvy
            tmp->rptr = L->Act;                      //pravy ukazatel tmp ukazuje na act
            tmp->lptr = NULL;                        //lavy ukazatel tmp je NULL
            L->Act->lptr = tmp;                      //lavy ukazatel act ukazuje na tmp
            L->First = tmp;                          //tmp je prvy prvok v zozname
        }
        else {                                      //aktivny prvok nie je prvy
            tmp->rptr = L->Act;                      //pravy ukazatel tmp ukazuje na act
            tmp->lptr = L->Act->lptr;                //lavy ukazatel tmp ukazuje na prvok pred act
            tmp->lptr->rptr = tmp;                   //pravy ukazatel prvku pred act ukazuje na tmp
            L->Act->lptr = tmp;                      //lavy ukazatel act ukazuje na tmp
        }
    }
}

/*
 - vracia obsah aktivneho prvku prostrednictvom premennej token
 @param L ukazatel na zoznam
 @token sem sa ulozi obsah aktivneho prvku
 */
void TDLLCopy (TDLList *L, TOKEN *token) {

     if (L->Act == NULL) {                              //zoznam nie je aktivny
         return;
    }
    *token = L->Act->tdata;                         //kopirovanie dat aktivneho prvku
}


/*
 - prepise obsah aktivneho prvku
 @param L ukazatel na zoznam
 @token novy obsah
 */
void TDLLActualize (TDLList *L, TOKEN token) {
    if (L->Act) {
        L->Act->tdata = token;                                 //kopirovanie dat z aktivneho prvku
    }
}

/*
 - posunie aktivny prvok doprava
 @param L ukazatel na zoznam
 */
void TDLLSucc (TDLList *L) {

    if (L->Act) {
        L->Act = L->Act->rptr;                               //posunutie aktivity do prava
    }
}

/*
- posunie aktivny prvok do lava
@param L ukazatel na zoznam
*/
void TDLLPred (TDLList *L) {

    if (L->Act) {
        L->Act = L->Act->lptr;                               //posunutie aktivity do lava
    }
}

/*
 - vracia nenulovu hodnotu, ak je zoznam aktivny
 @param L ukazatel na zoznam
 */
int TDLLActive (TDLList *L) {

    return L->Act != NULL;
}

