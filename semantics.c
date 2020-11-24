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
//zatial nic nerobi
int globalBraceCount = 0;

//globalna symtable funkcii
tBSTNodePtrGlobal functions;

//TODO pri kontrole parametrov funkcii treba este overit, ci nemaju parametre rovnake identifikatory...


/*
 kontrolna funkcia pre potreby debuggingu
 vytlaci parametre funkcie vkladanej do symtable
 */
void printFunction(char *id, functionData data) {
    printf("Function id: %s\n", id);
    printf("Number of params: %d\n", data.numOfParams);
    printf("Number of return vals: %d\n", data.numOfReturns);
    printf("Params: \n");
    for (int i = 0; i < data.numOfParams; i++) {
        //printf("we even here?\n");
        switch (data.paramsType[i]) {
            case t_INT_ID:
                printf("int ");
                break;
            case t_STRING_ID:
                printf("string ");
                break;
            case t_FLOAT64:
                printf("float64 ");
                break;
                
            default:
                break;
        }
        printf("%s\n", data.params[i]);
    }
    printf("Return types:\n");
    for (int i = 0; i < data.numOfReturns; i++) {
        //printf("or here?\n");
        switch (data.returns[i]) {
            case t_INT_ID:
                printf("int ");
                break;
            case t_STRING_ID:
                printf("string ");
                break;
            case t_FLOAT64:
                printf("float64 ");
                break;
                
            default:
                break;
        }
    }
    printf("\n\n\n");
}








/*
tato funkcia bude prechadzet telom funkcie, a kontrolovat statements, deklaracie a priradenia
spusta sa ked sa najde funkcia ina ako main(), semantika deklaracie by mala byt spravna, iba ukladame udaje o tejto fcii
@param L ukazatel na list tokenov, ukazatel Act je na zaciatku tela funkcie, teda '{'
@param id identifikator funkcie, not sure if needed
*/
void enterFunctionBody(TDLList *L, char *id) {
    int localBraceCount = 1;
    //budeme prechadzat telom funckie kym z neho nevyjdeme
    while (localBraceCount != 0) {
        //TODO - tu sa budu hladat statements, deklaracie, priradenia a volania funkcii, a budu sa nalezite volat ich respektivne funkcie
    }
}











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
    //sem sa ulozia udaje o funkcii
    functionData data;
    //posun aktivity na prvu zatvorku ( z klucoveho slova "func"
    TDLLSucc(L);
    TDLLSucc(L);
    //cita tokeny kym nenajde ")" / citame parametre
    while (strcmp(L->Act->tdata.lex, ")")) {
        //nasleduje identifikator
        if (L->Act->tdata.type == 19) {
            //ulozime potrebne udaje(nazvy, typy a pocet parametrov)(cyklicky ich pridavam)
            data.params[paramsCt] = L->Act->tdata.lex;
            data.paramsType[paramsCt] = L->Act->tdata.type;
            paramsCt++;
            data.numOfParams = paramsCt;
            //posun aktivny prvok na id->typ->','
            TDLLSucc(L);
            TDLLSucc(L);
            //keby tu nebolo continue, hrozi preskocenie zatvorky, teda by sa while zastavil nespravne
            continue;
            
        }
        //posun '(' -> id alebo ',' -> id
        TDLLSucc(L);
    }
    //posun ')' -> '(' alebo ')' -> '{'
    TDLLSucc(L);
    
    //cita tokeny kym nenajde ")" / citame navratove typy
    while (strcmp(L->Act->tdata.lex, ")")) {
        //navratove typy nemusia byt deklarovane, v tom pripade { ukonci cyklus
        if (!strcmp(L->Act->tdata.lex, "{")) {
            break;
        }
        //ak najdeme navratove typy, vlozime do symtable
        if ((L->Act->rptr->tdata.type == t_INT_ID) ||
            (L->Act->rptr->tdata.type == t_STRING_ID) ||
            (L->Act->rptr->tdata.type == t_FLOAT64)) {
            //ulozim potrebne udaje(typy a pocet navratovych hodnot)
            data.returns[retCt] = L->Act->rptr->tdata.type;
            retCt++;
            data.numOfReturns = retCt;
        }
        TDLLSucc(L);
    }
    BSTInsertGlobal(&functions, id, data);
    printFunction(id, data);
}










/*
 funkcia sa spusta pri kazdom najdeni klucoveho slova "func"
 v podstate sa sklada z velkeho ifu, ktory vysetruje identifikator "main", ak je iny identifikator, spusta sa analyza danej funkcie
 @param L ukazatel na list tokenov
 */
void checkFunction(TDLList *L) {
    //pomocna premenna, BSTSearchGobal() sem vravcia data
    functionData data;
    //pomocny string, ulozime sem identifikator fcie
    char *id = L->Act->rptr->tdata.lex;
    //identifikator funkcie je "main"
    if (!strcmp(id, "main")) {
        //ak uz main bol raz najdeny, volaj error
        if (mainFound) {
            error_call(ERR_SEM_UNDEF, L);
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
            error_call(ERR_SEM_UNDEF, L);
            return;
        }
        printf("Semantika: Main checked succesfully\n");
        
    //identifikator funkcie != main \/
    } else {
        //skontroluj ci identifikator uz neni v symtable
        if (BSTSearchGlobal(functions, id, &data)) {
            printf("Semantika: pokus o redefiniciu, volam error\n");
            error_call(ERR_SEM_UNDEF, L);
        }
        checkFunctionParams(L, id);
    }
    //checkFuncBody();                                              +pseudoF
}

/*
 TATA FUNKCIA SPUSTA SEMANTICKU ANALYZU
 rozumej ju ako main semntickej analyzy
 prechadza zoznamom tokenov - klucove slovo "func" spusta seriu funkcii zameranych na semanticku kontrolu danej funkcie
 kedze sa cely kod sklada s funkcii, pokryje cely kod
 @param L kompletny zoznam tokenov
 */
void goThroughList(TDLList *L) {
    
    //inicializacia tabulky funkcii
    BSTInitGlobal(&functions);
    
    TDLLFirst(L);
    printf("Semantika: Prechadzam zoznamom tokenov...\n\n");
    int i = 0, j = 0;      //pre potreby vypiskov
    while (L->Act != NULL) {
        if (!strcmp(L->Act->tdata.lex, "func")) {
            j++;
            //printf("Semantika: %d. function found...\n", j);
            //printf("Semantika: Function id - %s\n", L->Act->rptr->tdata.lex);
            checkFunction(L);
            
        }
         
        i++;
        //printf("Sematika: Loop number %d\n", i);
        TDLLSucc(L);
    }
    if (!mainFound) {
        error_call(ERR_SEM_UNDEF, L);
        return;
        
    }
    //uvolnime pouzivane struktury
    TDLLDisposeList(L);
    BSTDisposeGlobal(&functions);

    printf("Semantika: USPESNY KONIEC\n");
} // koniec goThroughList
