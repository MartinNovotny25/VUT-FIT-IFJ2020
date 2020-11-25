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

//TODO pri kazdom volani erroru uvolnit symtables
//TODO vlozit so symtable vstavane funkcie

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
 tato funkcia vlozi do globalnej symtable funkcii vstavane funckie
 func inputs() (string,int)
 func inputi() (int,int)
 func inputf() (float64,int)
 func int2float(i int) (float64)
 func float2int(f float64) (int)
 func len(s string) (int)
 func substr(s string, i int, n int) (string, int)
 func ord(s string, i int) (int, int)
 func chr(i int) (string, int)
 func print ( term1 , term2 , ..., termm )
 */
void insertBuiltInFunction() {
    functionData inputs;
    inputs.numOfParams = 0;
    inputs.numOfReturns = 2;
    inputs.returns[0] = t_STRING_ID;
    inputs.returns[1] = t_INT_ID;
    
    functionData inputi;
    inputi.numOfParams = 0;
    inputi.numOfReturns = 2;
    inputi.returns[0] = t_INT_ID;
    inputi.returns[1] = t_INT_ID;
    
    functionData inputf;
    inputf.numOfParams = 0;
    inputf.numOfReturns = 2;
    inputf.returns[0] = t_FLOAT64;
    inputf.returns[1] = t_INT_ID;
    
    functionData int2float;
    int2float.numOfParams = 1;
    int2float.numOfReturns = 1;
    int2float.returns[0] = t_FLOAT64;
    int2float.paramsType[0] = t_INT_ID;
    int2float.params[0] = "i";
    
    functionData float2int;
    float2int.numOfParams = 1;
    float2int.numOfReturns = 1;
    float2int.returns[0] = t_INT_ID;
    float2int.paramsType[0] = t_FLOAT64;
    float2int.params[0] = "f";
    
    functionData len;
    len.numOfParams = 1;
    len.numOfReturns = 1;
    len.returns[0] = t_INT_ID;
    len.paramsType[0] = t_STRING_ID;
    len.params[0] = "s";
    
    functionData substr;
    substr.numOfParams = 3;
    substr.numOfReturns =2;
    substr.returns[0] = t_STRING_ID;
    substr.returns[1] = t_INT_ID;
    substr.paramsType[0] = t_STRING_ID;
    substr.paramsType[1] = t_INT_ID;
    substr.paramsType[2] = t_INT_ID;
    substr.params[0] = "s";
    substr.params[1] = "i";
    substr.params[2] = "n";
    
    functionData ord;
    ord.numOfParams = 2;
    ord.numOfReturns =21;
    ord.returns[0] = t_INT_ID;
    ord.returns[1] = t_INT_ID;
    ord.paramsType[0] = t_STRING_ID;
    ord.paramsType[1] = t_INT_ID;
    ord.params[0] = "s";
    ord.params[0] = "i";
    
    functionData chr;
    chr.numOfParams = 1;
    chr.numOfReturns = 2;
    chr.returns[0] = t_STRING_ID;
    chr.returns[1] = t_INT_ID;
    chr.paramsType[0] = t_INT_ID;
    chr.params[0] = "i";
    
    functionData print;
    print.numOfParams = MAXPARAMS;
    print.numOfReturns = 0;
    
    BSTInsertGlobal(&functions, "inputs", inputs);
    BSTInsertGlobal(&functions, "inputi", inputi);
    BSTInsertGlobal(&functions, "inputf", inputf);
    BSTInsertGlobal(&functions, "int2float", int2float);
    BSTInsertGlobal(&functions, "float2int", float2int);
    BSTInsertGlobal(&functions, "len", len);
    BSTInsertGlobal(&functions, "substr", substr);
    BSTInsertGlobal(&functions, "ord", ord);
    BSTInsertGlobal(&functions, "chr", chr);
    BSTInsertGlobal(&functions, "print", print);
}

/*
 tato funkcia kontroluje nazvy parametrov vo funkcii, nemozu sa volat rovnako
 TODO parametre sa budu musiet vlozit aj do lokalnej symtable
 @param data struktura obsahujuca pocet a typy parametrov
 @param L ukazatel na zoznam tokenov, potrebny pre potreby uvolnenia
 */
void paramsRedefinitionCheck(functionData data, TDLList *L) {
    printf("pocet parametrov ktore kontrolujem: %d\n", data.numOfParams);
    printf("sem: som v paramsRedefinitionCheck\n");
    if (data.numOfParams == 0) {
        return;
    }
    
    for (int i = 0; i < data.numOfParams; i++) {
        for (int j = 0; j < data.numOfParams; j++) {
            //porovname parameter sameho so sebou, tento krok treba preskocit
            if (i == j) {
                continue;
            }
            //id parametrov sa rovnaju, volaj error
            if (!strcmp(data.params[i], data.params[j])) {
                BSTDisposeGlobal(&functions);
                printf("Semantika: redefinicia parametrov v deklaracii funkcie, error\n");
                error_call(ERR_SEM_UNDEF, L);
                return;
            }
        }
    }
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
    //sem sa ulozia udaje o funkcii, numOfParams a Returns treba nastavit na nulu, inak robi problemy
    functionData data;
    data.numOfParams = paramsCt;
    data.numOfReturns = retCt;
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
   
    //overime ci sa parametre funkcie navzajom neredifinuju
    paramsRedefinitionCheck(data, L);
    //vlozenie funkcie do symtable
    BSTInsertGlobal(&functions, id, data);
    printFunction(id, data);
}










/*
 funkcia sa spusta pri kazdom najdeni klucoveho slova "func"
 v podstate sa sklada z velkeho ifu, ktory vysetruje identifikator "main", ak je iny identifikator, spusta sa analyza danej funkcie
 @param L ukazatel na list tokenov
 */
void checkFunction(TDLList *L) {
    //pomocna premenna, BSTSearchGlobal() sem vravcia data
    functionData data;
    //pomocny string, ulozime sem identifikator fcie
    char *id = L->Act->rptr->tdata.lex;
    //identifikator funkcie je "main"
    if (!strcmp(id, "main")) {
        //ak uz main bol raz najdeny, volaj error
        if (mainFound) {
            printf("Semantika: pokus o redefiniciu mainu, volam error\n");
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
            printf("chybny main function, volam error\n");
            error_call(ERR_SEM_UNDEF, L);
            return;
        }
        //printf("Semantika: Main checked succesfully\n");
        
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
 rozumej ju ako main semantickej analyzy
 prechadza zoznamom tokenov - klucove slovo "func" spusta seriu funkcii zameranych na semanticku kontrolu danej funkcie
 kedze sa cely kod sklada z funkcii, pokryje cely kod
 @param L kompletny zoznam tokenov
 */
void goThroughList(TDLList *L) {
    
    //inicializacia tabulky funkcii
    BSTInitGlobal(&functions);
    insertBuiltInFunction();
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
    //main nebol najdeny, volaj error
    if (!mainFound) {
        printf("Semantika:main not found, volam error\n");
        error_call(ERR_SEM_UNDEF, L);
        return;
        
    }
    //uvolnime pouzivane struktury
    TDLLDisposeList(L);
    BSTDisposeGlobal(&functions);

    printf("Semantika: USPESNY KONIEC\n");
} // koniec goThroughList
