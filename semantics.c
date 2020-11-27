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
//tato localna symtable je iba pre potreby testovania
tBSTNodePtrLocal local;
//TODO pri kazdom volani erroru uvolnit symtables

/*
 kontrolna funkcia pre potreby debuggingu
 vytlaci parametre funkcie vkladanej symtable
 */
void printFunction(char *id, functionData data) {
    printf("Spustam printFunction");
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
 pomocna funkcia
 tato funkcia vracia v premennej type typ literalu
 ACT je nastavena na literal
 moze byt volana iba ked viem ze sa jedna o literal
 nechava act prvok ZA literalom, ak TRUE
 */
bool isLiteral(TDLList *L, int *type) {
    printf("Spustam isLiteral\n");
    if ((L->Act->tdata.type == t_INT_ZERO) || (L->Act->tdata.type == t_INT_NON_ZERO) ||
        (L->Act->tdata.type == t_STRING) || (L->Act->tdata.type == t_FLOAT)) {
        switch (L->Act->tdata.type) {
            case t_INT_ZERO:
                *type = t_INT_ID;
                printf("isLiteral:TRUE!, t_INT_ID\n");
                TDLLSucc(L);
                return true;
                break;
                
            case t_INT_NON_ZERO:
                *type = t_INT_ID;
                printf("isLiteral:TRUE!, t_INT_NON_ZERO\n");
                TDLLSucc(L);
                return true;
                break;
            
            case t_STRING:
                *type = t_STRING_ID;
                printf("isLiteral:TRUE!, t_STRING_ID\n");
                TDLLSucc(L);
                return true;
                break;
                
            case t_FLOAT:
                *type = t_FLOAT64;
                printf("isLiteral:TRUE!, t_FLOAT64\n");
                TDLLSucc(L);
                return true;
                break;

            default:
                break;
        }
    } else {
        printf("isLiteral: FALSE!\n\n");
        return false;
    }
    printf("isLiteral:FALSE!\n\n");
    return false;
}

/*
 tato funkcia vracia true, ak sa jedna o vyraz
 datovy typ vyrazu vracia v premennej "type", v pripade za sa datovy typy nezhoduju, vola error
!!! funkcia ocakava aktivny prvok na prvom pripadnom identifikatore/literali/zatvorke vyrazu !!!
 kontroluje rovnost datovych typov
 
 @param L zoznam tokenov, aktivny prvok ocakova na spravnom mieste, vid. popis ^
 @param type premenna typu int, vracia sa v nej datovy typ
 */
bool isExpression(TDLList *L, int *type, tBSTNodePtrLocal *node) {
    printf("isExpression: Aktivny a nasledujuci prvok po spusteni isExpression: %s %d\n%s %d\n", L->Act->tdata.lex, L->Act->tdata.type,L->Act->rptr->tdata.lex, L->Act->rptr->tdata.type);
    //tu sa ulozi typ prveho literalu/id
    int typ = 0;
    //tu sa ulozi typ dalsich literalov/id
    int typ2 = 0;
    char *unneeded = NULL;
    
    //za prvym literalom je eol alebo ';' alebo ','
    if (L->Act->rptr->tdata.type == t_EOL || L->Act->rptr->tdata.type == T_SEMICOLON || L->Act->rptr->tdata.type == t_COMMA) {
        printf("isExpression: FALSE\n");
        return false;
    }
    //ulozim typ prveho literalu/id
    while (true) {
        //printf("ZAMOTANY SOM V DRUHOM IFE\n");
        //nasli sme prvy id
        if (L->Act->tdata.type == t_IDENTIFIER) {
            //overim ci je v symtable, ulozime jeho typ do typ
            if (BSTSearchLocal(*node, L->Act->tdata.lex, &typ, unneeded)) {
                printf("ID %s \"JE\" V SYMTABLE\n\n", L->Act->tdata.lex);
                break;
            } else {
                //neni v symtable, volaj error
                printf("isExpression: LEXEM : %s\n", L->Act->tdata.lex);
                printf("isExpression: id neni v symtable, volaj error\n");
                error_call(ERR_SEM_UNDEF, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
        }
        //ak najdeme literal ulozim jeho typ do premennej typ
        if (L->Act->tdata.type == t_INT_ZERO) {
            typ = t_INT_ID;
            break;
        }
        if (L->Act->tdata.type == t_INT_NON_ZERO) {
            typ = t_INT_ID;
            break;
        }
        if (L->Act->tdata.type == t_STRING) {
            typ = t_STRING_ID;
            break;
        }
        if (L->Act->tdata.type == t_FLOAT) {
            typ = t_FLOAT64;
            break;
        }
        //ak narazime na eol, daco je zle
        if (L->Act->rptr->tdata.type == t_EOL) {
            printf("isExpression ma problem...\n");
            break;
        }
        TDLLSucc(L);
    }
    
    //printf("Act PO PRVOM WHILE: %s\n\n", L->Act->tdata.lex);
    //return false;
    //prehladam dalsie literaly/id
    //ak narazime na eol alebo ';', ukonci cyklus
    while (L->Act->tdata.type != t_EOL) {
        //printf("Act V DRUHOM WHILE: %s\n\n", L->Act->tdata.lex);
        //printf("ZAMOTANY SOM V DRUHOM IFE\n");
        if (L->Act->tdata.type == T_SEMICOLON) {
            break;
        }
        if (L->Act->tdata.type == t_COMMA) {
            break;
        }
        //nasli sme id
        if (L->Act->tdata.type == t_IDENTIFIER) {
            //nchadza sa v tabulke, do typ sa ulozi jeho typ
            if (BSTSearchLocal(*node, L->Act->tdata.lex, &typ2, unneeded)) {
                printf("ID %s \"JE\" V SYMTABLE\n\n", L->Act->tdata.lex);
                //typ prveho a n-teho id sa nerovnaju
                if (typ != typ2) {
                    printf("isExpression: nekompatibilne typy 1, volaj error\n");
                    error_call(ERR_SEM_EXCOMPAT, L);
                    BSTDisposeGlobal(&functions);
                    BSTDisposeLocal(node);
                }
                TDLLSucc(L);
                continue;
            } else {
            printf("isExpression: passing undeclared id: \"%s\", volam error\n", L->Act->tdata.lex);
            error_call(ERR_SEM_EXCOMPAT, L);
            BSTDisposeGlobal(&functions);
            BSTDisposeLocal(node);
                
            }
            
        }
        //nasli sme literal t_INT_ZERO
        if (L->Act->tdata.type == t_INT_ZERO) {
            //porovname typy
            if (typ != t_INT_ID) {
                printf("isExpression: nekompatibilne typy 2, volaj error\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
            TDLLSucc(L);
            continue;
        }
        if (L->Act->tdata.type == t_INT_NON_ZERO) {
            //porovname typy
            if (typ != t_INT_ID) {
                printf("isExpression: nekompatibilne typy 3, volaj error\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
            TDLLSucc(L);
            continue;
        }
        if (L->Act->tdata.type == t_STRING) {
            //porovname typy
            if (typ != t_STRING_ID) {
                printf("isExpression: nekompatibilne typy 4, volaj error\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
            TDLLSucc(L);
            continue;
        }if (L->Act->tdata.type == t_FLOAT) {
            //porovname typy
            if (typ != t_FLOAT64) {
                printf("isExpression: nekompatibilne typy 5, volaj error\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
            TDLLSucc(L);
            continue;
        }
        TDLLSucc(L);
    }
    //vsetko v poho, vratim true a datovy typ
    //printf("moj typ: mal by byt 24: %d\n", typ);
    *type = typ;
    printf("Aktivny prvok po vykonani celeho isExpression: %s %d\n", L->Act->tdata.lex, L->Act->tdata.type);
    printf("isExpression: TRUE\n\n");
    return true;
}

/*
 tato funkcia kontroluje spravny pocet aj typ parametrov v return statemente
 Act prvok je na returne
 */
void checkReturnStatement(TDLList *L, char *id, tBSTNodePtrLocal *node) {
    printf("Spustam checkReturnStatement\n");
    printf("\ncheckReturnStatement Act: %s id: %s\n\n", L->Act->tdata.lex, id);
    //sem sa vlozia informacie o funkcii
    functionData data;
   
    char *unneeded = NULL;
    //vkladanie, tkato fcia uz musi byt v symtable
    BSTSearchGlobal(functions, id, &data);
    printf("pocet params: %d\npocet returns: %d\n", data.numOfParams, data.numOfReturns);
    
    //sem sa ulozi typ returnovaneho literalu/identifiktora/vyrazu
    int typ = 0;
    //int numOfReturnVals = 0;
    //ziadne navratove typy
    //TODO tento prvy if tu mozno nemusi byt, takuto situaciu riesi parser
    if (data.numOfReturns == 0) {
        printf("CheckReturnStatement: pocet navratovych hodnot = 0\n");
        if (L->Act->rptr->tdata.type == t_EOL) {
            printf("tu vsetko v poriadku\n");
            return;
            //v pravo od returnu nieco je, error
        } else {
            printf("CheckReturnStatement: nespravny pocet vyrazov v return statemente, volaj error\n");
            error_call(ERR_SEM_RETURN, L);
            BSTDisposeGlobal(&functions);
            BSTDisposeLocal(node);
        }
    }
    //jediny navratovy typ
    /*if (data.numOfReturns == 1) {
        printf("CheckReturnStatement: pocet navratovych hodnot = 1\n");
        //returnovany je literal int
        if (L->Act->rptr->tdata.type == t_INT_NON_ZERO) {
            //nezhoda
            if (data.paramsType[0] != t_INT_ID) {
                printf("CheckReturnStatement: nekompatibilne return 1, volaj error\n");
                error_call(ERR_SEM_RETURN, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(&local);
            } else {return;}
        }
        if (L->Act->rptr->tdata.type == t_INT_ZERO) {
            //nezhoda
            if (data.paramsType[0] != t_INT_ID) {
                printf("CheckReturnStatement: nekompatibilne return 2, volaj error\n");
                error_call(ERR_SEM_RETURN, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(&local);
            } else {return;}
        }
        if (L->Act->rptr->tdata.type == t_FLOAT) {
            //nezhoda
            if (data.paramsType[0] != t_FLOAT64) {
                printf("CheckReturnStatement: nekompatibilne return 3, volaj error\n");
                error_call(ERR_SEM_RETURN, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(&local);
            } else {return;}
        }
        if (L->Act->rptr->tdata.type == t_STRING) {
            //nezhoda
            if (data.paramsType[0] != t_STRING_ID) {
                printf("CheckReturnStatement: nekompatibilne return 4, volaj error\n");
                error_call(ERR_SEM_RETURN, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(&local);
            } else {return;}
        }
    }*/
    if (data.numOfReturns > 0) {
        printf("CheckReturnStatement: pocet navratovych hodnot > 0\n");
        for (int i = 0; i < data.numOfReturns; i++) {
            TDLLSucc(L);
            if (!strcmp(L->Act->tdata.lex, ",")) {
                TDLLSucc(L);
            }
            //vracana hodnota je vyraz
            if (isExpression(L, &typ, node)) {
                printf("CheckReturnStatement: %d-ty prvok je vyraz\n", i);
                //typ vyrazu je spravny
                if (data.returns[i] == typ) {
                    printf("CheckReturnStatement: %d-ty typ sedi, kontrolujem dalej...\n", i);
                    continue;
                }
                printf("CheckReturnStatement: nekompatibilne return 5, volaj error\n");
                error_call(ERR_SEM_RETURN, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
            //vracana hodnota je literal
            if (isLiteral(L, &typ)) {
                printf("CheckReturnStatement: %d-ty prvok je literal\n", i);
                if (data.returns[i] == typ) {
                    printf("CheckReturnStatement: %d-ty typ sedi, kontrolujem dalej...\n", i);
                    continue;
                }
                printf("CheckReturnStatement: nekompatibilne return 6, volaj error\n");
                error_call(ERR_SEM_RETURN, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
            //vracana hodnota je samotny id
            if (L->Act->tdata.type == t_IDENTIFIER) {
                printf("CheckReturnStatement: %d-ty prvok je id\n", i);
                if (BSTSearchLocal(*node, L->Act->tdata.lex, &typ, unneeded)) {
                    printf("CheckReturnStatement: %d-ty prvok je id a nachadza sa v symtable\n", i);
                    if (data.returns[i] == typ) {
                        printf("CheckReturnStatement: %d-ty typ sedi, kontrolujem dalej...\n", i);
                        printf("VYPISUJEM ALC->RPTR : %s\n", L->Act->rptr->tdata.lex);
                        continue;
                    }
                    printf("CheckReturnStatement: nekompatibilne return 7, volaj error\n");
                    error_call(ERR_SEM_RETURN, L);
                    BSTDisposeGlobal(&functions);
                    BSTDisposeLocal(node);
                }
            }
            //TDLLSucc(L);
        }
        //printf("act : PODSTATNE: %s\n", L->Act->tdata.lex);
    }
    //treba este skontrolovat, ci nie su returnvals navyse
    if (data.numOfReturns == 1) {
        TDLLSucc(L);
    }
    
    printf("act po skoneceni checkReturnStatement: %s\n", L->Act->tdata.lex);
    if (L->Act->tdata.type != t_EOL) {
        printf("CheckReturnStatement: too many return vals 1, volam error\n");
        printf("checkReturnStatement: lexem: %s     typ %d\n\n", L->Act->tdata.lex, L->Act->tdata.type);
        error_call(ERR_SEM_RETURN, L);
        BSTDisposeGlobal(&functions);
        BSTDisposeLocal(node);
    }
    
    printf("RETURNED SUCCESFULY!\n\n");
    printf("current act: %s %d\n", L->Act->tdata.lex, L->Act->tdata.type);
}


/*
 tato funkcia kontroluje spravne zadane parametre pri volani funkcie
 bool BSTSearchLocal (tBSTNodePtrLocal RootPtr, char* name, int *Type, char *Data);
 */
void checkCallFunction(TDLList *L, char *id, tBSTNodePtrLocal *node) {
    printf("Som v check call function pre id: %s\n", id);
    functionData data;
    //sem bude vracat hodnotu BSTSearchLoacal
    int type = 0;
    //TODO upravid BSTSearchLocal aby tento string nebol potrebny
    char *unneeded = NULL;
    
    //ak sa vola print, treba overit iba identifikatory v symtables
    if (!strcmp(id, "print")) {
        printf("Volana funkcia je print...\n");
        //ak najdeme pravu zatvorku, sme na konci volania
        while (L->Act->tdata.type != t_RIGHT_BRACKET) {
            TDLLSucc(L);
            printf("LEXEM: %s\n\n", L->Act->tdata.lex);
            //nasli sme id treba overit v symtable
            if (L->Act->tdata.type == t_IDENTIFIER) {
                if (BSTSearchLocal(*node, L->Act->tdata.lex, &type, unneeded)) {
                    printf("TRUE\n");
                    return;
                } else {
                    printf("passing undeclared id, volaj error\n");
                    error_call(ERR_SEM_RETURN, L);
                    BSTDisposeGlobal(&functions);
                    BSTDisposeLocal(node);
                }
            }
            
        }
        return;
    }
    //printf("som v check call function\nid: %s\naktivny: %s\n", id, L->Act->tdata.lex);
    //sem bude vracat data BSTSearchGlobal
    if (BSTSearchGlobal(functions, id, &data)) {
        for (int i = 0; i < data.numOfParams; i++) {
            //printf("tlacim num of params, tolkotokrat podje for: %d\n", data.numOfParams);
            //printf("tlacim hodnotu i: %d\n", i);
            //posun z '(' -> 'parameter' alebo ',' -> 'parameter'
            TDLLSucc(L);
            TDLLSucc(L);
            //ak je parameter id
            //printf("TLACIM LEXEM: %s\n", L->Act->tdata.lex);
            if (L->Act->tdata.type == t_IDENTIFIER) {
                printf("som v tomto ife...\n");
                //musi byt v lokalnej/nadlokalnej symtable || 'local' zatial iba pre potreby prekladu
                if (BSTSearchLocal(*node, L->Act->tdata.lex, &type, unneeded)) {
                    //jeho typ musi byt rovnaky ako typ parametru na pozicii i vo funkcii
                    if (type == data.paramsType[i]) {
                        //vsetko sedi, kontroluj dalsi parameter
                        continue;
                    } else {
                        printf("zadany parameter ma nekompatibilny typ, volaj error\n");
                        error_call(ERR_SEM_RETURN, L);
                        BSTDisposeGlobal(&functions);
                        BSTDisposeLocal(node);
                    }
                } else {
                    printf("passing undefined identifier, volaj error\n");
                    error_call(ERR_SEM_RETURN, L);
                    BSTDisposeGlobal(&functions);
                    BSTDisposeLocal(node);
                }
            } else {
                //parameter nie je identifikator, je napr konstanta, retazcovy literal...
                //TODO POZOR, OPERATOR | MOZNO ROBI ZLOBU, OTESTOVAT
                switch (L->Act->tdata.type) {
                    case t_INT_ZERO | t_INT_NON_ZERO:
                        if (data.paramsType[i] == t_INT_ID) {
                            //parameter v poriadku, kontroluj dalsi parameter
                            continue;
                        } else {
                            printf("passing wrong type 1, volaj error\n");
                            error_call(ERR_SEM_RETURN, L);
                            BSTDisposeGlobal(&functions);
                            BSTDisposeLocal(node);
                        }
                        break;
                    case t_STRING:
                        //printf("tlacim type: %d\n", L->Act->tdata.type);
                        printf("tlacim paramtype: %d\n", data.paramsType[i]);
                        if (data.paramsType[i] == t_STRING_ID) {
                            //parameter v poriadku, kontroluj dalsi parameter
                            printf("parameter v poho\n");
                            continue;
                        } else {
                            printf("passing wrong type 2, volaj error\n");
                            error_call(ERR_SEM_RETURN, L);
                            BSTDisposeGlobal(&functions);
                            BSTDisposeLocal(node);
                        }
                        break;
                    case t_FLOAT:
                        if (data.paramsType[i] == t_FLOAT64) {
                            //parameter v poriadku, kontroluj dalsi parameter
                            continue;
                        } else {
                            printf("passing wrong type 3, volaj error\n");
                            error_call(ERR_SEM_RETURN, L);
                            BSTDisposeGlobal(&functions);
                            BSTDisposeLocal(node);
                        }
                        break;
                        
                    default:
                        break;
                }
            }
        }
        //aktivny prvok by mal byt na poslednom predavanom parametry
        //ak vpravo od neho neni '(' znamena to nespravny pocet parametrov
        if (strcmp(L->Act->rptr->tdata.lex, ")")) {
          //  printf("nespravny pocet argumentov 4, volaj error\n");
            error_call(ERR_SEM_RETURN, L);
        }
    } else {
        printf("calling undeclared function, volaj error\n");
        error_call(ERR_SEM_UNDEF, L);
        BSTDisposeGlobal(&functions);
        BSTDisposeLocal(node);
    }
}


/*
* Zisti ci su rovnake datove typy
* Kontroluje ci pouziva len relacne operatory
* L: Zoznam lexemov
* node: Lokalny strom premennych
*/
void if_control(TDLList *L, tBSTNodePtrLocal *node, functionData params){
//    printf("\n\n SOM VO FUKNCII IFCONTROL\n\n");
  //  printf("%s a typ %d\n", L->Act->tdata.lex, L->Act->tdata.type);


    
//    printf("premenna v ifcontrole %s\n", L->Act->tdata.lex);
    // Ak su oba identifikatory
    if(L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->rptr->tdata.type == t_IDENTIFIER){
        int typ1;
        int typ2;
     //    printf("%s a typ %d\n", L->Act->tdata.lex, L->Act->tdata.type);
        // je prvy v strome?
        if(BSTSearchLocal(*node, L->Act->tdata.lex, &typ1, "null")){
      //      printf("%s, %d\n",L->Act->tdata.lex, typ1 );
        }else{
             error_call(ERR_SEM_UNDEF, L);
             BSTDisposeLocal(node);
        }
        
        // Je druhy v strome?
        if(BSTSearchLocal(*node, L->Act->rptr->rptr->tdata.lex, &typ2, "null")){
     //      printf("%s, %d\n",L->Act->rptr->rptr->tdata.lex, typ2 );
        }else{
             error_call(ERR_SEM_UNDEF, L);
              BSTDisposeLocal(node);
        }
        
        // Maju rovnake typy?
        if(!(typ1 == typ2)) error_call(ERR_SEM_EXCOMPAT, L);

    // ak je lavy operator identifikator
    }else if(L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->rptr->tdata.type != t_IDENTIFIER){
        int typ1;
        int typ2;
    //    printf("POROVNAVAM ID A CISLO\n");
        // Je prvy id v strome?
        if(BSTSearchLocal(*node, L->Act->tdata.lex, &typ1, "null")){
     //         printf("Nasiel som: %s s typom %d\n", L->Act->tdata.lex, typ1);
        }else{
            error_call(ERR_SEM_UNDEF, L);
            BSTDisposeLocal(node);
        }

        // Ma id rovnaky typ ako cislo?
        if(L->Act->rptr->rptr->tdata.type == t_INT_NON_ZERO || L->Act->rptr->rptr->tdata.type == t_INT_ZERO){
      //      printf("ide o integer\n");
            typ2 = t_INT_ID;
    //        printf("Typ id: %d, typ hodnoty: %d\n", typ1, typ2);
            if(!(typ1 == typ2)) error_call(ERR_SEM_EXCOMPAT, L);
        // ma id rovnaky typ ako float?
        }else if(L->Act->rptr->rptr->tdata.type == t_FLOAT64 || L->Act->rptr->rptr->tdata.type == t_FLOAT){
            typ2 = t_FLOAT64;
    //        printf("Typ id: %d, typ hodnoty: %d\n", typ1, typ2);
            if(!(typ1 == typ2)) error_call(ERR_SEM_EXCOMPAT, L);
        // ma id rovnaky typ ako string?
        }else if(L->Act->rptr->rptr->tdata.type == t_STRING){
            typ2 = t_STRING_ID;
            if(!(typ1 == typ2)) error_call(ERR_SEM_EXCOMPAT, L);
        }
        
    // ak je pravy operator identifikator
    }else if(L->Act->tdata.type != t_IDENTIFIER && L->Act->rptr->rptr->tdata.type == t_IDENTIFIER){
        int typ1;
        int typ2;
        // Je v strome?
        if(BSTSearchLocal(*node, L->Act->rptr->rptr->tdata.lex, &typ2, "null")){
            
        }else error_call(ERR_SEM_UNDEF, L);

        // Ma id rovnaky typ ako cislo?
        if(L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ZERO){
            typ1 = t_INT_ID;
        //    printf("Typ id: %d, typ hodnoty: %d\n", typ1, typ2);
            if(!(typ1 == typ2)){
                 error_call(ERR_SEM_EXCOMPAT, L);
                   BSTDisposeLocal(node);
            }

        // ma id rovnaky typ ako float?
        }else if(L->Act->tdata.type == t_FLOAT64 || L->Act->tdata.type == t_FLOAT){
            typ1 = t_FLOAT64;
        //    printf("Typ id: %d, typ hodnoty: %d\n", typ1, typ2);
            if(!(typ1 == typ2)){
                   error_call(ERR_SEM_EXCOMPAT, L);
                   BSTDisposeLocal(node);
            }

        // ma id rovnaky typ ako string?
        }else if(L->Act->tdata.type == t_STRING){
            typ1 = t_STRING_ID;
            if(!(typ1 == typ2)){
                   error_call(ERR_SEM_EXCOMPAT, L);
                   BSTDisposeLocal(node);
            }
        }

    // Ak ani jeden nie je identifikator
    }else{
        // Ak ide o integre maju rovnake typy?
        if(L->Act->tdata.type == t_INT_ZERO || L->Act->tdata.type == t_INT_NON_ZERO){
            int typson1 = t_INT_ID;
            if(L->Act->rptr->rptr->tdata.type == t_INT_NON_ZERO || L->Act->rptr->rptr->tdata.type == t_INT_ZERO){
                int typson2 = t_INT_ID;
                if(!(typson1 == typson2)){
                    error_call(ERR_SEM_EXCOMPAT, L);
                   BSTDisposeLocal(node);
                }

            }
        // Ak ide o floaty maju rovnake typy?
        }else if(L->Act->tdata.type == t_FLOAT64 || L->Act->tdata.type == t_FLOAT){
            int typson1 = t_FLOAT64;
            if(L->Act->rptr->rptr->tdata.type == t_FLOAT64 || L->Act->rptr->rptr->tdata.type == t_FLOAT){
                int typson2 = t_FLOAT64;
                if(!(typson1 == typson2)){
                    error_call(ERR_SEM_EXCOMPAT, L);
                    BSTDisposeLocal(node);
                }

            }
        // Ak ide o stringy maju rovnake typy?
        }if(L->Act->tdata.type == t_STRING){
            int typson1 = t_STRING_ID;
            if(L->Act->rptr->rptr->tdata.type == t_STRING){
                int typson2 = t_STRING_ID;
                if(!(typson1 == typson2)){
                    error_call(ERR_SEM_EXCOMPAT, L);
                    BSTDisposeLocal(node);
                }

            }
        }
    }

    TDLLSucc(L);
  //  printf("Idem skontrolovat relacne operatory\n");
    int real_op = 0;
   

        
    while(L->Act->tdata.type != t_BRACES_L){
     //   printf("Relacony operator: %s\n", L->Act->tdata.lex);
        if(L->Act->tdata.type == T_SEMICOLON){
       //     printf("Breakujem z ifcontrol\n");
            break;
        }
        if(L->Act->tdata.type == t_GREATER){
            printf("Je vacsie\n");
            real_op++;
        }else if(L->Act->tdata.type == t_GREATEROREQUAL){
             printf("Je vacsie\n");
            real_op++;
        }else if(L->Act->tdata.type == t_LESS){
             printf("Je vacsie\n");
            real_op++;
        }else if(L->Act->tdata.type == t_LESSOREQUAL){
             printf("Je vacsie\n");
            real_op++;
        }else if(L->Act->tdata.type == t_EQUAL){
             printf("Je vacsie\n");
            real_op++;
        }else if(L->Act->tdata.type == t_NOT_EQUAL){
             printf("Je vacsie\n");
            real_op++;
        }else {
            if(L->Act->tdata.type == t_PLUS){
                error_call(ERR_SEM_OTHER, L);
                BSTDisposeLocal(node);
            }
             
            if(L->Act->tdata.type == t_MINUS){
                error_call(ERR_SEM_OTHER, L);
                BSTDisposeLocal(node);
            }
                
            if(L->Act->tdata.type == t_MULTIPLY){
                error_call(ERR_SEM_OTHER, L);
                BSTDisposeLocal(node);
            }
              
            if(L->Act->tdata.type == t_DIVIDE){
                error_call(ERR_SEM_OTHER, L);
                BSTDisposeLocal(node);
            }
               
        }

        TDLLSucc(L);
    }

     
    TDLLPred(L);
    if(real_op != 1){
        error_call(ERR_SEM_OTHER, L);
        BSTDisposeLocal(node);
    }
    //printf("Vsetko preslo v poriadku\n");
}

/*
* Kontroluje vyrazy v hlavicke foru
* Prvy vyraz moze byt vynechany
* Druhy vyraz(Podmienka) nemoze byt vynechana
* Treti vyraz(zvacsenie iteracnej premennej) moze byt vynechany
* L: Zoznam lexemov
* node: Lokalny strom v ktorom su ulozene premenne
*/
void for_control(TDLList *L, tBSTNodePtrLocal *node, functionData params){
//    printf("\n\n SOM VO FUNKCII FORCONTROL\n\n");
   
    TDLLSucc(L);
  //  printf("Aktivny prvok vo fore %s %d\n", L->Act->tdata.lex, L->Act->tdata.type);
    // ak je prvy lexem bodkociarka
    if(L->Act->tdata.type == T_SEMICOLON){
   //     printf("nasiel som bodkociarku");
        //TDLLSucc(L); // chod na bodkociarku
        TDLLSucc(L); // chod za bodkociarku (ma byt identifikator alebo vyraz)

        // Skontroluj podmienku
        if_control(L, node, params);
        // Prejdi za podmienku
        //TDLLSucc(L);
        //TDLLSucc(L);
        TDLLSucc(L);
        // ak chyba bodkociarka chyba
    //    printf("premenna pred semicilon %s", L->Act->tdata.lex);
        if(L->Act->tdata.type != T_SEMICOLON){
            error_call(ERR_SEM_OTHER, L);
                BSTDisposeLocal(node);
        }
        TDLLSucc(L);
        // Ak nechyba vyraz skontroluj ho

   //     printf("LEXEm vo fore: %s\n", L->Act->tdata.lex);
//   printf("premenna do assign %s", L->Act->tdata.lex);
        if(!(L->Act->tdata.type == t_BRACES_L)) {assign_vals_control(L, node, params);}
        TDLLPred(L);
    // Ak prvy lexem nie je bodkociarka
    }else if(L->Act->tdata.type == t_IDENTIFIER){
        // chod na lexem za for
        //TDLLSucc(L);
        // deklaruj identifikator
     //   printf("nasiel som identifikator\n");
        dec_var_control(L, node, params);
        // preskoc bodkociarku
        //TDLLSucc(L);
       
     //   printf("\n\n LEXEm: %s", L->Act->tdata.lex);
        TDLLSucc(L);
     //   printf("\n\n LEXEm: %s", L->Act->tdata.lex);
        // skontroluj podmienku
    //    printf("Volam ifcontrol\n");
        if_control(L, node, params);
        // Chod za podmienku
        //TDLLSucc(L);
        //TDLLSucc(L);
        TDLLSucc(L);
   //     printf(" sme pred assign control hodnota %s\n", L->Act->tdata.lex);
        // ak chyba bodkociarka chyba
        if(L->Act->tdata.type != T_SEMICOLON){
            error_call(ERR_SEM_OTHER, L);
                BSTDisposeLocal(node);
        }
        TDLLSucc(L);
        // ak nechyba vyraz skontroluj ho
       // printf("Assigncontrol volam s lexemom: %s\n", L->Act->tdata.lex);
        if(!(L->Act->tdata.type == t_BRACES_L)) assign_vals_control(L, node, params);
        TDLLPred(L);
    // ak chyba bodkociarka aj deklaracia
    }else
    {
    
        error_call(ERR_SEM_OTHER, L);
         BSTDisposeLocal(node);
    }
}

/*
* Kontroluje deklaraciu premennych
* L: Zoznam lexemov
* node: Lokalny strom na ukladanie premennych
*/
void dec_var_control(TDLList *L, tBSTNodePtrLocal *node, functionData params){


    /*for(int i = 0; i < params.numOfParams; i++){
        BSTInsertLocal(node, params.params[i], params.paramsType[i], "null");
    }*/

	//printf("\n\n SOM V DECVARCONTROL\n\n");
	// Nazov premennej na deklarovanie
	char *id = L->Act->tdata.lex;
	// Na ulozenie datoveho typu
	int type;
	// Zbytocna premenna
	char *data = "null";

	// pocet stringov
	int string_count = 0;
	// pocet cisel vo vyraze
	int int_count = 0;
	// pocet floatov vo vyraze
	int float_count = 0;
	// pocet identifikatorov
	//int identifier = 0;

    if(L->Act->lptr->tdata.type != t_EOL) error_call(ERR_SEM_OTHER, L);
    // ak je premenna ktoru chceme definovat uz definovana
    if(BSTSearchLocal(*node, id, &type, data)) error_call(ERR_SEM_OTHER,L);
	
	// Chod na dalsi prvok teda  ->  :=
	TDLLSucc(L);
  //  printf("pred %s za %s", L->Act->lptr->tdata.lex, L->Act->tdata.lex);
	// Chod na koniec riadku
	while(L->Act->tdata.type != t_EOL){
		
		// Ak ide o integer
		if(L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ZERO){
			// Uloz jeho typ
			type = t_INT_ID;
			// Zapocitaj najdenie integeru
			int_count++;

		// Ak ide o float
		}else if(L->Act->tdata.type == t_FLOAT64 || L->Act->tdata.type == t_FLOAT){
			// Uloz jeho typ
			type = t_FLOAT64;
			// zapocitaj najdenie floatu
			float_count++;

		// Ak ide o string
		}else if(L->Act->tdata.type == t_STRING){
			// uloz jeho typ
			type = t_STRING_ID;
			// zapocitaj najdenie stringu
			string_count++;

		// Ak ide o identifikator
		}else if(L->Act->tdata.type == t_IDENTIFIER){
			// Premenna na ulozenie typu identifikatoru v strome
			int datovytyp;
			// Nazov najdeneho identifikatoru
			char *name = L->Act->tdata.lex;
            functionData returned_data;
            if(!(strcmp(L->Act->tdata.lex, "inputs"))){  
                error_call(ERR_SEM_OTHER, L);
                   
            }else if(!(strcmp(L->Act->tdata.lex, "inputi"))){
                error_call(ERR_SEM_OTHER, L);
               
            }else if(!(strcmp(L->Act->tdata.lex, "inputf"))){
                error_call(ERR_SEM_OTHER, L);
                
            }else if(!(strcmp(L->Act->tdata.lex, "int2float"))){
                type = t_FLOAT64;
                BSTInsertLocal(node, id,&type, data);
             
            }else if(!(strcmp(L->Act->tdata.lex, "float2int"))){
                type = t_INT_ID;
                BSTInsertLocal(node, id,&type, data);
               
            }else if(!(strcmp(L->Act->tdata.lex, "len"))){
                type = t_INT_ID;
                BSTInsertLocal(node, id,&type, data);
             
            }else if(!(strcmp(L->Act->tdata.lex, "substr"))){
                error_call(ERR_SEM_OTHER, L);
             
            }else if(!(strcmp(L->Act->tdata.lex, "ord"))){
                error_call(ERR_SEM_OTHER, L);
 
            }else if(!(strcmp(L->Act->tdata.lex, "chr"))){
                error_call(ERR_SEM_OTHER, L);

            }else if(!(strcmp(L->Act->tdata.lex, "print"))){
                error_call(ERR_SEM_EXCOMPAT, L);

            }else if(BSTSearchGlobal(functions, L->Act->tdata.lex, &returned_data)){
                if(returned_data.numOfReturns != 1) error_call(ERR_SEM_OTHER, L);
                else{
                    type = returned_data.returns[0];
                    BSTInsertLocal(node, id,&type, data);
                }
            }else if (BSTSearchLocal(*node, name, &datovytyp, data)) {
				    type = datovytyp;
                if(datovytyp == t_INT_ID){
                    int_count++;
                }else if(datovytyp == t_STRING_ID){
                    string_count++;
                }else if(datovytyp == t_FLOAT64){
                    float_count++;
                }
    		}
		}
		// Ak prideme vo vyraze na bodkociarku zastav, kvoli kontrole deklaracie vo for-e
		if(L->Act->tdata.type == T_SEMICOLON) break;
		// Ak najdes ciarku tzv, chce sa deklarovat viac premennych naraz chyba
		if(L->Act->tdata.type == t_COMMA){
			error_call(ERR_SEM_OTHER, L);
		}
		TDLLSucc(L);
	}

	// Ak boli na priradenie iba string
	if(string_count != 0 && int_count == 0 && float_count == 0){
		// Uloz uzol so stringom do stromu
		BSTInsertLocal(node, id,&type, data);

	// ak boli na priradenie vo vyraze iba integre 
	}else if(string_count == 0 && int_count != 0 && float_count == 0){
		// uloz uzol s integerom do stromu
		BSTInsertLocal(node, id, &type, data);

	// ak boli na priradenie vo vyraze iba floaty
	}else if(string_count == 0 && int_count == 0 && float_count != 0){
		// tak uloz uzol s typom float do stromu
		BSTInsertLocal(node, id, &type, data);
	}else{
		error_call(ERR_SEM_DATATYPE, L);
	}
}


/* kontroluje priradnie pri viacerych premennych a vyrazoch
* L: Zoznam s lexemami
* node: Lokalny strom pre ukladanie premennych
*/
void assign_vals_control(TDLList *L, tBSTNodePtrLocal *node, functionData params){

    /*for(int i = 0; i < params.numOfParams; i++){
        BSTInsertLocal(node, params.params[i], params.paramsType[i], "null");
    }*/
    
//    printf("\n\n SOM V ASSIGNVALSCONTROL\n\n");
    // Na ukladanie premennych
    char *id[256];
    // na ukladanie typov
    int type[256];
    int types_of_defined_vars[256];
    // zbytocna premenna
    char *data = "null";
    // pocet stringov
    int string_count = 0;
    // pocet cisel
    int int_count = 0;
    // pocet floatov
    int float_count = 0;
    // pocet identifikatorov
    int identifier = 0;
    // id na ukladanie premennych v spravnom poradi
    int id_insert = 0;
    int is_function = 0;

    // Pocet premennych nalavo od =
    // po ukonceni cyklu bude L->act ukazovat na =
    while(L->Act->tdata.type != t_ASSIGN){
        if(L->Act->tdata.type == t_IDENTIFIER){
            id[identifier] = L->Act->tdata.lex;
            if(BSTSearchLocal(*node, id[identifier], &types_of_defined_vars[identifier],data)){
              //  printf("NAzvy premennych %s a ich typy %d, pozicia %d\n", id[identifier], types_of_defined_vars[identifier], identifier);
            }else{
                error_call(ERR_SEM_UNDEF, L);
            }
            identifier++;
        }
        TDLLSucc(L);
    }
 //   printf("\nPRESIEL SOM ZA WHILE LOOP\n");


    // pocet identifikatorov napravo od =
    //int identif_count = 0;
    // pocet ciarok napravo od =
    int comma_count = 0;

 //   printf("\n SME NA LEXEME: %s\n", L->Act->tdata.lex);
    // prechadzaj pokym neprejdeme na koniec riadku
    while(L->Act->tdata.type!= t_EOL){
            // ak ide o cislo
            if(L->Act->tdata.type == t_LEFT_BRACKET) break;
            if(L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ZERO){
            //    printf("Piradujem integer\n");
                type[id_insert] = t_INT_ID;
                int_count++;
                id_insert++;
            // ak ide o float
            }else if(L->Act->tdata.type == t_FLOAT64 || L->Act->tdata.type == t_FLOAT){
                printf("\n\n NASIEL SOM FLOAT\n\n");
                type[id_insert] = t_FLOAT64;
                float_count++;
                id_insert++;
            // ak ide o string
            }else if(L->Act->tdata.type == t_STRING){
                type[id_insert] = t_STRING_ID;
                string_count++;
                id_insert++;
            // ak ide o premennu
            }else if(L->Act->tdata.type == t_IDENTIFIER){
                int datovytyp;
                char *name = L->Act->tdata.lex;
                functionData returned_data;
             //   printf("\nMENO PREMENNEJ NALAVO OD = %s\n\n", name);
           
                if(!(strcmp(L->Act->tdata.lex, "inputs"))){
                  //  printf("Ide o inputs\n");
                    if(identifier != 2) error_call(ERR_SEM_OTHER, L);
                   // printf("Kontrolujem typy premennych\n");
                   if(types_of_defined_vars[0] != t_STRING_ID || types_of_defined_vars[1] != t_INT_ID){
                       error_call(ERR_SEM_EXCOMPAT, L);
                   }
                   is_function++;
                }else if(!(strcmp(L->Act->tdata.lex, "inputi"))){
                     if(identifier != 2) error_call(ERR_SEM_OTHER, L);
                    if(types_of_defined_vars[0] != t_INT_ID || types_of_defined_vars[1] != t_INT_ID){
                        error_call(ERR_SEM_EXCOMPAT, L);
                    }
                    is_function++;
                }else if(!(strcmp(L->Act->tdata.lex, "inputf"))){
                     if(identifier != 2) error_call(ERR_SEM_OTHER, L);
                    if(types_of_defined_vars[0] != t_FLOAT64 || types_of_defined_vars[1] != t_INT_ID){
                        error_call(ERR_SEM_EXCOMPAT, L);
                    }
                    is_function++;
                }else if(!(strcmp(L->Act->tdata.lex, "int2float"))){
                     if(identifier != 1) error_call(ERR_SEM_OTHER, L);
                    if(types_of_defined_vars[0] != t_FLOAT64){
                        error_call(ERR_SEM_EXCOMPAT, L);
                    }
                  //  printf("kontrola prebehla v poriadku\n");
                    is_function++;
                }else if(!(strcmp(L->Act->tdata.lex, "float2int"))){
                     if(identifier != 1) error_call(ERR_SEM_OTHER, L);
                    if(types_of_defined_vars[0] != t_INT_ID){
                        error_call(ERR_SEM_EXCOMPAT, L);
                    }
                    is_function++;
                }else if(!(strcmp(L->Act->tdata.lex, "len"))){
                     if(identifier != 1) error_call(ERR_SEM_OTHER, L);
                    if(types_of_defined_vars[0] != t_INT_ID){
                        error_call(ERR_SEM_EXCOMPAT, L);
                    }
                    is_function++;
                }else if(!(strcmp(L->Act->tdata.lex, "substr"))){
                     if(identifier != 2) error_call(ERR_SEM_OTHER, L);
                    if(types_of_defined_vars[0] != t_STRING_ID || types_of_defined_vars[1] != t_INT_ID){
                        error_call(ERR_SEM_EXCOMPAT, L);
                    }
                    is_function++;
                }else if(!(strcmp(L->Act->tdata.lex, "ord"))){
                     if(identifier != 2) error_call(ERR_SEM_OTHER, L);
                    if(types_of_defined_vars[0] != t_INT_ID || types_of_defined_vars[1] != t_INT_ID){
                        error_call(ERR_SEM_EXCOMPAT, L);
                    }
                    is_function++;
                }else if(!(strcmp(L->Act->tdata.lex, "chr"))){
                     if(identifier != 2) error_call(ERR_SEM_OTHER, L);
                    if(types_of_defined_vars[0] != t_STRING_ID || types_of_defined_vars[1] != t_INT_ID){
                        error_call(ERR_SEM_EXCOMPAT, L);
                    }
                    is_function++;
                }else if(!(strcmp(L->Act->tdata.lex, "print"))){
                    error_call(ERR_SEM_EXCOMPAT, L);

                }else if(BSTSearchGlobal(functions, L->Act->tdata.lex, &returned_data)){
                    if(identifier != returned_data.numOfReturns) error_call(ERR_SEM_OTHER, L);
                    else{
                       //printf("\n\n NAZOV PREMENNEJ %s\n\n\n", L->Act->tdata.lex);
                        int i = 0;
                        while(returned_data.numOfReturns != i){
                            type[i] = returned_data.returns[i];
                            printf("\n\n typonn : %d\n\n", type[i]);
                            i++;
                        }

                        for(int i = 0; i < identifier; i++){
                            //printf("\n\npremenna napravo: %d, premena nalavo: %d\n\n", types_of_defined_vars[i], type[i]);
                            if(types_of_defined_vars[i] != type[i]){
                                printf("\n\nVOlam error");
                                error_call(ERR_SEM_EXCOMPAT, L);
                            }else{
                                is_function++;
                                break;
                            }
                        }
                    }
                }else if(BSTSearchLocal(*node, name, &datovytyp, data)) {
                    //printf("NASIEL SOM DATOVY TYP PREMENNEJ %s\n\n", L->Act->tdata.lex);
                    type[id_insert] = datovytyp;
                    if(datovytyp == t_INT_ID){
                       printf("Ide o integer\n");
                        int_count++;
                        id_insert++;
                    }else if(datovytyp == t_FLOAT64){
                        float_count++;
                        id_insert++;
                    }else if(datovytyp == t_STRING_ID){
                   //     printf("Ide o string\n");
                        string_count++;
                        id_insert++;
                    }
                }else{
                    error_call(ERR_SEM_UNDEF, L);
                }
            // ak nejde o cislo, string, float alebo premennu
            }

            if(L->Act->tdata.type == T_SEMICOLON) break;
            // Ak najdes ciarku tzv, chce sa deklarovat viac premennych naraz chyba
              
            // ak sme prisli na ciarku alebo koniec riadku
            if(L->Act->tdata.type == t_COMMA || L->Act->rptr->tdata.type == t_EOL || L->Act->tdata.type == t_BRACES_L){

             //   printf("\n\n IDEM PRIRADIT HODNOTU\n\n");
                // ak vo vyraze boli len stringy
                if(string_count != 0 && int_count == 0 && float_count == 0){
                    int zahod;
                    // je premenna do ktorej chceme ulozit hodnotu deklarovana?
              //      printf("Hlada node zo stringu\n\n");
                    if(!(BSTSearchLocal(*node, id[comma_count], &zahod, data))) error_call(ERR_SEM_UNDEF, L);
                    if(types_of_defined_vars[comma_count] == t_STRING){
                        BSTInsertLocal(node, id[comma_count], type, data);
                        id_insert++;
                        string_count = 0;
                        int_count = 0;
                        float_count = 0;
                    }else{
                        error_call(ERR_SEM_EXCOMPAT, L);
                    }
                
                    // ak vo vyraze boli len cisla
                }else if(string_count == 0 && int_count != 0 && float_count == 0){
                    int zahod;
                //    printf("Hladam node z integer\n\n");
                    // je premenna do ktorej chceme ulozit hodnotu deklarovana?
                    if(!(BSTSearchLocal(*node, id[comma_count], &zahod, data))) error_call(ERR_SEM_UNDEF, L);

                //    printf("POROVNAMAVAM %d a %d\n\n", types_of_defined_vars[comma_count], t_INT_ID);
                    if(types_of_defined_vars[comma_count] == t_INT_ID){
                        //BSTInsertLocal(node, id[comma_count], types_of_defined_vars[comma_count], data);
                        id_insert++;
                        string_count = 0;
                        int_count = 0;
                        float_count = 0;
                    }else{
                      //  printf("\n\n TYP PREMENNEJ SA NEROVNA S INTEGROM\n");
                        error_call(ERR_SEM_EXCOMPAT, L);
                    }

                // ak vo vyraze boli len floaty
                }else if(string_count == 0 && int_count == 0 && float_count != 0){
                    int zahod;
                  //  printf("Hladam node z float\n\n");
                    // je premenna do ktorej chceme ulozit hodnotu deklarovana?
                    if(!(BSTSearchLocal(*node, id[comma_count], &zahod, data))) error_call(ERR_SEM_UNDEF, L);
                    if(types_of_defined_vars[comma_count] == t_FLOAT64){
                        BSTInsertLocal(node, id[comma_count], type, data);
                        id_insert++;
                        string_count = 0;
                        int_count = 0;
                        float_count = 0;
                    }else{
                        error_call(ERR_SEM_EXCOMPAT, L);
                    }
                }else if(string_count == 0 && int_count == 0 && float_count == 0){
                    
                }else{
                    error_call(ERR_SEM_EXCOMPAT, L);
                }
                comma_count++;
            }
        //printf("Lexem: %s\n", L->Act->tdata.lex);
        TDLLSucc(L);
    }
    // Ak je pocet premennych do ktorych sa ma priradovat ako hodnot rozdielny
    if(is_function == 0){
        if(identifier != comma_count) error_call(ERR_SEM_DATATYPE, L);
    }else{

    }
    //TDLLPred(L);
}



/*
 tato funkcia bude prechadzet telom funkcie, a kontrolovat statements, deklaracie a priradenia
 bude potrebne okamzite vytvorit lokalny strom, ulozit donho parametre danej fciee z globalneho stromu
 a taktiez uz bude treba inicializovat zasobniky stromov
 @param L ukazatel na list tokenov, ukazatel Act je na zaciatku tela funkcie, teda '{'
 @param id identifikator funkcie, not sure if needed
 */

void enterFuncBody(TDLList *L, char *id,  MainStack mainstack){
    printf("Entering function body: %s\n", id);
    
    
    //ked vojdeme do funkcie, Act je na id, posunieme ho za prvu '{'
    while (strcmp(L->Act->tdata.lex, "{")) {
        TDLLSucc(L);
        //printf("WHILE LEX %s\n", L->Act->tdata.lex);
    }
    TDLLSucc(L);
    //printf("POST WHILE LEX %s\n", L->Act->tdata.lex);
    
    
    int localBraceCount = 1;
    //TODO tu musi byt implmentovany stack symtables, ukazatel na neho sa musi predavat ako parameter kazdej funkcie
    //vlozime parametre funkcie id do lokalnej symtable
    //insertInLocalSymtable()
    //budeme prechadzat telom funckie kym z neho nevyjdeme
    tBSTNodePtrLocal node;
	BSTInitLocal(&node);
    int typ = t_INT_ID;
    BSTInsertLocal(&node, "_", &typ, "null");
    
    functionData params;
    BSTSearchGlobal(functions, id, &params);
    
    for(int i = 0; i < params.numOfParams; i++){
        BSTInsertLocal(&node, params.params[i], &params.paramsType[i], "null");
    }

    
    while (localBraceCount != 0) {
        printf("Lexem v enterfuncbody: %s\n", L->Act->tdata.lex);
        

        if (!strcmp(L->Act->tdata.lex, "}")) {
            localBraceCount--;
            printf("Nasiel som '}'\n");
            printf("\n\n LOCAL BRACE COUNT: %d \n\n", localBraceCount);
        }
        if (!strcmp(L->Act->tdata.lex, "{")) {
            localBraceCount++;
            printf("Nasiel som '{'\n");
            printf("\n\n LOCAL BRACE COUNT: %d \n\n", localBraceCount);
        }
        //printf("\n\n LOCAL BRACE COUNT: %d \n\n", localBraceCount);
        //printf("enterfunctionBody aktivny prvok: %d %s\n",L->Act->tdata.type, L->Act->tdata.lex);
        //nasli sme volanie funkcie
        if ((L->Act->tdata.type == t_IDENTIFIER) && (L->Act->lptr->tdata.type != t_FUNC) && (L->Act->rptr->tdata.type == t_LEFT_BRACKET)) {
            printf("EnterFBody: Nasiel som volanie funkcie %s\n", L->Act->tdata.lex);
            checkCallFunction(L, L->Act->tdata.lex, &node);
            //printf("kde ostane aktivny prvok? : %s\n", L->Act->tdata.lex);
        }
        //nasli sme assignment
        if(L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->tdata.type == t_ASSIGN){
			TDLLElemPtr temp_node = L->Act;
			while(temp_node->tdata.type != t_EOL){
				if(temp_node->lptr->tdata.type == t_EOL){
                    //printf("\n\n HODNOTA LEXEMU: %s\n\n", temp_node->tdata.lex);
					assign_vals_control(L, &node, params);
					break;
				}
				TDLLPred(L);
				temp_node = temp_node->lptr;
			}
		}
        if (L->Act->tdata.type == t_DEFINITION_2) {
            TDLLPred(L);
			dec_var_control(L, &node, params);
        }
        if (L->Act->tdata.type == t_FOR) {
            //tBSTNodePtrLocal for_node;
            //BSTInitLocal(&for_node);
            //PushTreeMain(&mainstack, for_node);
            //printf("Volam funkciu for control\n");
            for_control(L, &node, params);
            //for_node = PopTreeMain(&mainstack);
           
            TDLLPred(L);
            TDLLPred(L);
            //TDLLPred(L);
        }
        if (L->Act->tdata.type == t_IF) {
            TDLLSucc(L);
            tBSTNodePtrLocal if_node;
            BSTInitLocal(&if_node);
            PushTreeMain(&mainstack, if_node);
			if_control(L, &node, params);
            if_node = PopTreeMain(&mainstack);
        }
        
        
        if (L->Act->tdata.type == t_RETURN) {
           // printf("EnterFBody: Nasiel som return statement\n");
            checkReturnStatement(L, id, &node);
        }
        

       
        //posun v zozname doprava
        TDLLSucc(L);
    }
}




/*
 tato funkcia bude druhy krat prechadzat cez zoznam tokenov, pricom vsetky deklracie funkcii
 su uz syntakticky okontrolovane a vlozene do globalneho stromu funkcii
 opat teda najde klucove slovo func, a zavola funkciu enterFunctionBody ktorej preda id funkcie, aby vedela v ktorej sa nachadza
 #param L zoznam tokenov
 */
void sencondRun(TDLList *L) {
    
   
    printf("Druhy beh spusteny\n\n");
    //aktivita musi ist na zaciatok
    TDLLFirst(L);
    //prehladavaj kym nenajdes funkciu
    while (L->Act != NULL) {
        //printf("som vo while\n");
        if (!strcmp(L->Act->tdata.lex, "func")) {
            //printf("vstupim do ifu\n");
             // -------PSEUDO KOD----------
            tBSTNodePtrLocal rootPtr;
            BSTInitLocal(&rootPtr);

            MainStack mainstack;
            mainstack.a[0] = rootPtr;

            InitMainStack(&mainstack);
         
            PushTreeMain(&mainstack, rootPtr);
            enterFuncBody(L, L->Act->rptr->tdata.lex, mainstack);
            PopTreeMain(&mainstack);
        }


        TDLLSucc(L);
    }
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
    ord.numOfReturns =2;
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
    //printf("pocet parametrov ktore kontrolujem: %d\n", data.numOfParams);
    //printf("sem: som v paramsRedefinitionCheck\n");
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
            data.paramsType[paramsCt] = L->Act->rptr->tdata.type;
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
    } else {
        //skontroluj ci identifikator uz neni v symtable
        if (BSTSearchGlobal(functions, id, &data)) {
            printf("Semantika: pokus o redefiniciu, volam error\n");
            error_call(ERR_SEM_UNDEF, L);
        }
        checkFunctionParams(L, id);
    }
}

/*
 TATA FUNKCIA SPUSTA SEMANTICKU ANALYZU
 rozumej ju ako main semantickej analyzy
 v prvej faze prechadza zoznamom tokenov, hlada keyword func, a analyzuje deklaracie funkcii, pricom ich insertuje do stromu
 v druhej faze opat hlada keyword func, pricom uz funkciam vstupuje do tela a vykonava tam semanticku kontrolu
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
    printf("Spustam druhy beh...\n");
    
    //TDLLPrintAllTokens(L);
    TDLLFirst(L);
    
    BSTInitLocal(&local);
    sencondRun(L);
    /* TEST BSTSearch - nechat tak
    //tu som inicializoval
    BSTInitLocal(&local);
    //tieto testovacie data vlozim do stromu
    char *id = "xy";
    int typ = 10;
    
    //vlozim data
    BSTInsertLocal(&local,id ,&typ, data);

    //tieto premenne si musim vytvorit, aby mal BSTSearch kam vracat data
    int datovytyp;
    char *content = NULL;
    
    //searching
    if (BSTSearchLocal(local, id, &datovytyp, content)) {
        printf("datovy typ: %d\n", datovytyp);
    }
    */
    //uvolnime pouzivane struktury
    TDLLDisposeList(L);
    BSTDisposeGlobal(&functions);

    printf("Semantika: USPESNY KONIEC\n");
} // koniec goThroughList


