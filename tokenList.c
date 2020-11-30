/*
**    Part of IFJ Project 2020/2021
**    Author: Andrej Hyros xhyros00
*/

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tokenList.h"
#include "scanner.h"

/*
 void TDLLInitList (TDLList *);          //inicializacia zoznamu
 void TDLLDisposeList (TDLList *);       //zrusenie vsetkych prvkov
 void TDLLInsertFirst (TDLList *, int);  //vlozenie prvku na zaciatok zoznmu
 void TDLLInsertLast(TDLList *, int);    //vlozenie prvku na koniec zoznamu
 void TDLLFirst (TDLList *);             //nastavenie aktivity na prvy prvok
 void TDLLLast (TDLList *);              //nastavenie aktivity na posedny prvok
 void TDLLCopyFirst (TDLList *, int *);  //vracia hodnotu prveho prvku
 void TDLLCopyLast (TDLList *, int *);   //vracia hodnotu posledneho prvku
 void TDLLDeleteFirst (TDLList *);       //odstranenie prveho prvku
 void TDLLDeleteLast (TDLList *);        //odstranenie posledneho prvku
 void TDLLPostDelete (TDLList *);        //odstranenie prvku za aktivnym prvkom
 void TDLLPreDelete (TDLList *);         //odstranenie prvku pred aktivnym prvkom
 void TDLLPostInsert (TDLList *, int);   //vlozenie prvku za aktivny prok
 void TDLLPreInsert (TDLList *, int);    //vlozenie prvku pred aktivny prvok
 void TDLLCopy (TDLList *, int *);       //vracia hodnotu aktivneho prvku
 void TDLLActualize (TDLList *, int);    //prepisanie dat aktivneho prvku
 void TDLLSucc (TDLList *);              //posun aktivity doprava
 void TDLLPred (TDLList *);              //posun aktivity dolava
 int TDLLActive (TDLList *);             //zistuje aktivitu zoznamu
 */

/*
int main()                          //tmp main - iba na testiky
{
    printf("entered main\n");
    int ascii = 65;
    
    TDLList tmpL;
    TDLLInitList(&tmpL);
    
    TOKEN tmpTOKEN[5];
    
    for (int i = 0; i < 5; i++) {
        printf("entered for\n");
        tmpTOKEN[i].lex = "ascii";
        tmpTOKEN[i].type = i;
        printf("%d  %s\n", tmpTOKEN[i].type,  tmpTOKEN[i].lex);
        ascii++;
        TDLLInsertLast(&tmpL, tmpTOKEN[i]);
        TDLLFirst(&tmpL);
    }
    printf("for ^\n     Print all tokens \\/\n");
    //while (tmpL.Act != NULL) {
    //    printf("entered while\n");
     //   printf("%d  %s\n", tmpL.Act->tdata.type,  tmpL.Act->tdata.lex);
      //  TDLLSucc(&tmpL);
    }
    TDLLPrintAllTokens(&tmpL);
    TDLLDisposeList(&tmpL);
    printf("return iminent\n");
    return 0;
}
*/

void DLError() {
//nema prakticke vyuzite v projekte
//TODO overit ci vymazanie nic nerozbije a vymazat
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf ("tokenList: *ERROR* The program has performed an illegal operation.\n");
    //errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}




void TDLLPrintAllTokens(TDLList *L) {
    printf("tokenList: Printing all tokens from DL list...\n\n");
    TDLLFirst(L);
    
    while (L->Act != NULL) {                          //while prechadza listom
        printf("token %d\n", L->Act->tdata.type);
        if (L->Act->tdata.type == 29) {
            printf("lexem EOF\n\n");
        } else {
        printf("lexem %s\n\n", L->Act->tdata.lex);
        }
        TDLLSucc(L);                                //presun aktivity na nasledujuci token
    }
    printf("tokenList: Printing Ended Succesfully...\n");
    TDLLFirst(L);                                      //aktivita sa vrati na zaciatok
}

void TDLLInitList (TDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    //printf("som v InitList\n");
    L->First = NULL;        //vsetky ukazetele zoznamu nastavi na NULL
    L->Last = NULL;
    L->Act = NULL;
    //printf("koncim InitList\n");
}

void TDLLDisposeList (TDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free.
**/
    printf("tokenList: Disposing all Elements...\n");
    TDLLElemPtr tmp = L->First;              //tmp ukazuje na prvy prvok
    while (L->First != NULL) {              //zoznam nie je prazdny
        L->First = L->First->rptr;          //cyklus posuva ukazatel First do prava a uvolnuje pamat prvku pred nim
        free(tmp);
        tmp = L->First;
    }
    L->First = NULL;                        //ukazatele zoznamu nastavi na NULL
    L->Last = NULL;
    L->Act = NULL;
}

void TDLLInsertFirst (TDLList *L, TOKEN token) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    TDLLElemPtr tmp = (TDLLElemPtr) malloc(sizeof(struct TDLLElem));       //alokacia pamate
    if (tmp == NULL) {                                 //overenie uspesnosti alokacie
        DLError();
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

void TDLLInsertLast(TDLList *L, TOKEN token) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    
    //printf("som v InsertLast a idem alokovat\n");
    TDLLElemPtr tmp = (TDLLElemPtr) malloc(sizeof(struct TDLLElem));       //alokacia pamate
    if (tmp == NULL) {
        DLError();
        return;
    }
    //printf("som v InsertLast a po alokovani\n\n");
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

void TDLLFirst (TDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->First;                  //nastavenie aktivity na prvy prvok
}

void TDLLLast (TDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->Last;                  //nastavenie aktivity na posledny prvok
}

void TDLLCopyFirst (TDLList *L, TOKEN *token) {
/*
** Prostřednictvím parametru token vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if (L->First == NULL) {             //zoznam je prazdny
        DLError();
        return;
    }
    *token = L->First->tdata;              //kopirovanie dat prveho prvku
}

void TDLLCopyLast (TDLList *L, TOKEN *token) {
/*
** Prostřednictvím parametru token vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if (L->First == NULL) {             //zoznam je prazdny
           DLError();
           return;
       }
       *token = L->Last->tdata;            //kopirovanie dat z posledneho prvku
}

void TDLLDeleteFirst (TDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
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

void TDLLDeleteLast (TDLList *L) {
/*
** Zruší poslední prvek seznamu L.
** Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/
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

void TDLLPostDelete (TDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
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

void TDLLPreDelete (TDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
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

void TDLLPostInsert (TDLList *L, TOKEN token) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
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

void TDLLPreInsert (TDLList *L, TOKEN token) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
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

void TDLLCopy (TDLList *L, TOKEN *token) {
/*
** Prostřednictvím parametru token vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
     if (L->Act == NULL) {                              //zoznam nie je aktivny
         DLError();
         return;
    }
    *token = L->Act->tdata;                         //kopirovanie dat aktivneho prvku
}

void TDLLActualize (TDLList *L, TOKEN token) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
    if (L->Act) {
        L->Act->tdata = token;                                 //kopirovanie dat z aktivneho prvku
    }
}

void TDLLSucc (TDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
    if (L->Act) {
        L->Act = L->Act->rptr;                               //posunutie aktivity do prava
    }
}


void TDLLPred (TDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
    if (L->Act) {
        L->Act = L->Act->lptr;                               //posunutie aktivity do lava
    }
}

int TDLLActive (TDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
    return L->Act != NULL;
}

