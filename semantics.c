//
//  semantics.c
//  
//
//  Created by Andrej Hyros on 23/11/2020.
//
#include <stdio.h>
#include <string.h>
#include "semantics.h"
#include "tokenList.h"
#include "error.h"
#include "symtable.h"
#include "scanner.h"

bool mainFound = false;
int globalBraceCount = 0;


/*
 tato funkcia zavedie do symtable: identifikator funkcie, pocet a typ parametrov, pocet a typ vystupnych typov,
 spusta sa ked sa najde funkcia ina ako main(), semantika deklaracie by mala byt spravna, iba ukladame udaje o tejto fcii
 @param L ukazatel na list tokenov
 @param id identifikator funkcie
 */
//priklad funkcie
//func swap(x string, y string) (string, string) {
void checkFunctionParams(TDLList *L, char *id) {
    //pocitadla parametrov a navratovych typov
    int paramsCt = 0;
    int retCt = 0;
    //posun aktivity na prvu zatvorku ( z klucoveho slova "func"
    
    //printf("cFp first print: lexem: %s\n\n", L->Act->tdata.lex);
    TDLLSucc(L);
    TDLLSucc(L);
    //printf("cFp: lexem: %s\n\n", L->Act->tdata.lex);
    //cita tokeny kym nenajde ")" / citame parametre
    while (strcmp(L->Act->tdata.lex, ")")) {
        //printf("cFp 1st while: lexem %s\n\n", L->Act->tdata.lex);
        //nasleduje identifikator
        if (L->Act->tdata.type == 19) {
            paramsCt++;
            //TODO insert() - vloz do strom tento id aj jeho typ
            
            //posun aktivny prvok na id->typ->','
            TDLLSucc(L);
            TDLLSucc(L);
            //keby tu nebolo continue, hrozi preskocenie zatovrky, teda by sa while zastavil nespravne
            continue;
            
        }
        //posun '(' -> id alebo ',' -> id
        TDLLSucc(L);
    }
    //posun ')' -> '(' alebo ')' -> '{'
    TDLLSucc(L);
    //printf("cFp interwhile: lexem %s\n\n", L->Act->tdata.lex);
    
    //cita tokeny kym nenajde ")" / citame navratove typy
    while (strcmp(L->Act->tdata.lex, ")")) {
        //navratove typy nemusia byt deklarovane, v tom pripade { ukonci cyklus
        if (!strcmp(L->Act->tdata.lex, "{")) {
            break;
        }
        //printf("cFp 2nd while: lexem %s\n\n", L->Act->tdata.lex);
        //ak najdeme navratove typy, vlozime do symtable
        if ((L->Act->rptr->tdata.type == t_INT_ID) ||
            (L->Act->rptr->tdata.type == t_STRING_ID) ||
            (L->Act->rptr->tdata.type == t_FLOAT64))
            {
            //TODO navratove typy a ich pocet sa musi ulozit do symtable
            retCt++;
        }
        TDLLSucc(L);
    }
    printf("Semantika: cFp: kontrolny vypis parametrov (viacriadkovy)\nid: %s\nparametrov: %d\nnavratovych typov: %d\n\n", id, paramsCt, retCt);
}
/*
 funkcia sa spusta pri kazdom najdeni klucoveho slova "func"
 v podstate sa sklada z velkeho ifu, ktory vysetruje identifikator "main", ak je iny identifikator, spusta sa analyza danej funkcie
 @param L ukazatel na list tokenov
 */
void checkFunction(TDLList *L) {
    //pomocny string, ulozime sem identifikator fcie
    char *id = L->Act->rptr->tdata.lex;
    //identifikator funkcie je "main"
    if (!strcmp(id, "main")) {
        //ak uz main bol raz najdeny, volaj error
        if (mainFound) {
            error_call(3, L);
            return;
        }
        //pomocny pointer, mal by ukazovat vpravo od lexemu main, teda (
        struct TDLLElem *tmp = L->Act->rptr->rptr;
        //overime lexemi vpravo od identifikatoru main, ak su (){, tak sme nasli main
        if ((!strcmp(tmp->tdata.lex, "(")) && (!strcmp(tmp->rptr->tdata.lex, ")")) && (!strcmp(tmp->rptr->rptr->tdata.lex, "{"))) {
            mainFound = true;
        }
        else {
            //main nema spravne parametre, volaj error
            error_call(3, L);
            return;
        }
        printf("Semantika: Main checked succesfully\n");
        
            //identifikator funkcie != main \/
    } else {
        //printf("Semantika: Printing id: %s \n", L->Act->rptr->tdata.lex);
        //skontroluj ci identifikator uz neni v symtable
        //if (isInSymtable(id)) {                                   +
            //pokus o redefiniciu, volaj error                      +
        //   error_call(3, L);                                      +
        //    return;                                               +
        //}
        checkFunctionParams(L, id);
        
        //InsertInGlobalSymtable(id, tu asi este budu ine parametre?);  +pseudoF
    }
    //checkFuncBody();                                              +pseudoF
}


//prechadza DL zoznamom tokenov
void goThroughList(TDLList *L) {
    TDLLFirst(L);
    printf("Semantika: Prechadzam zoznamom tokenov...\n\n");
    int i = 0, j = 0;      //pre potreby vypiskov
    while (L->Act != NULL) {
        if (!strcmp(L->Act->tdata.lex, "func")) {
            j++;
            printf("Semantika: %d. function found...\n", j);
            printf("Semantika: Function id - %s\n", L->Act->rptr->tdata.lex);
            checkFunction(L);
            
        }
        //printf("Sematika: Loop number %d\n", i);
        i++;
        TDLLSucc(L);
    }
    if (!mainFound) {
        error_call(3, L);
        return;
    }
    
    
} // koniec goThroughList
