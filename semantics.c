/*
*  Predmet: Formalne jazyky a prekladace (IFJ) - FIT VUT v Brne
*  Zdrojovy subor semantickej analyzy pre projekt IFJ2020
*  Vytvoril: Peter Cellar - xcella00
*            Andrej Hyros - xhyros00
*  Datum: 10/2020
*/
#include <stdio.h>
#include <string.h>
#include "semantics.h"
#include "tokenList.h"
#include "error.h"
#include "symtable.h"
#include "scanner.h"

bool mainFound = false;
//globalna symtable funkcii
tBSTNodePtrGlobal functions;
//tato localna symtable je iba pre potreby testovania
MainStack mainstack;
        
/* ==========================================================================================
 - kontrolna funkcia pre potreby debuggingu
 - vytlaci parametre funkcie vkladanej symtable
 @param id identifikator(retazec) funkcie
 @param data struktura ukladajuca informacie o danej funkcii
 */
void printFunction(char *id, functionData data) {
    printf("Spustam printFunction\n");
    printf("Function id: %s\n", id);
    printf("Number of params: %d\n", data.numOfParams);
    printf("Number of return vals: %d\n", data.numOfReturns);
    printf("Params: \n");
    for (int i = 0; i < data.numOfParams; i++) {
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
    printf("\nprintFunction Ended\n\n");
}   //koniec printFunction


/* ==========================================================================================
 - tato funkcia vracia true, ak najde literal, a v premennej type vrati jeho typ
 - aktivny prvok je ocakavany na tokene daneho literalu
 - ak sa nasiel literal, aktivny prvok ostava na prvom tokene za danym literalom
 @param L obojstranne viazany zoznam tokenov
 @param type na tuto adresu sa ulozi typ literalu
 */
bool isLiteral(TDLList *L, int *type) {
    if ((L->Act->tdata.type == t_INT_ZERO) || (L->Act->tdata.type == t_INT_NON_ZERO) ||
        (L->Act->tdata.type == t_STRING) || (L->Act->tdata.type == t_FLOAT)) {
        switch (L->Act->tdata.type) {
            case t_INT_ZERO:
                *type = t_INT_ID;
                TDLLSucc(L);
                return true;
                break;
                
            case t_INT_NON_ZERO:
                *type = t_INT_ID;
                TDLLSucc(L);
                return true;
                break;
            
            case t_STRING:
                *type = t_STRING_ID;
                TDLLSucc(L);
                return true;
                break;
                
            case t_FLOAT:
                *type = t_FLOAT64;
                TDLLSucc(L);
                return true;
                break;

            default:
                break;
        }
    } else {
        return false;
    }
    return false;
}   //koniec isLiteral

/* ==========================================================================================
 - tato funkcia vracia true, ak najde vyraz, a v premennej type vrati jeho typ
 - ak sa datove typy jednotlivych casti vyrazu nezhoduju, vola error
 - aktivny prvok je ocakavany na prvom tokene vyrazu(id, literal, operator...)
 - aktivny prvok ostava na prvom tokene za danym vyrazom
 @param L obojstranne viazany zoznam tokenov
 @param type na tuto adresu sa ulozi typ literalu
 */
bool isExpression(TDLList *L, int *type, tBSTNodePtrLocal *node) {
    //tu sa ulozi typ prveho literalu/id
    int typ = 0;
    //tu sa ulozi typ dalsich literalov/id
    int typ2 = 0;
    bool isString = false;
    
    //za prvym literalom je eol alebo ';' alebo ','
    if (L->Act->rptr->tdata.type == t_EOL || L->Act->rptr->tdata.type == T_SEMICOLON || L->Act->rptr->tdata.type == t_COMMA) {
        return false;
    }
    //ulozim typ prveho literalu/id
    while (true) {
        //nasli sme prvy id
        if (L->Act->tdata.type == t_IDENTIFIER) {
            //overim ci je v symtable, ulozime jeho typ do typ
            if (MainStackSearch(mainstack, L->Act->tdata.lex, &typ)) {
                //nemoze to byt '_'
                if (!strcmp(L->Act->tdata.lex, "_")) {
                    error_call(ERR_SEM_UNDEF, L);
                    BSTDisposeGlobal(&functions);
                    BSTDisposeLocal(node);
                }
                break;
            } else {
                //neni v symtable, volaj error
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
            break;
        }
        TDLLSucc(L);
    }
    //ak najdeme string, nastavime flag isString
    if (typ == t_STRING_ID) {
        isString = true;
    }
    
    
    //prehladam dalsie literaly/id
    //ak narazime na eol alebo ';', ukonci cyklus
    while (L->Act->tdata.type != t_EOL) {
        if (L->Act->tdata.type == T_SEMICOLON) {
            break;
        }
        if (L->Act->tdata.type == t_COMMA) {
            break;
        }
        //ak najdeme operator iny ako '+' a sme v retazcovom vyraze, je to chyba
        if (isString) {
            if (L->Act->tdata.type == t_MINUS ||
                L->Act->tdata.type == t_DIVIDE ||
                L->Act->tdata.type == t_MULTIPLY) {
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
        }
        //najdeme relacny operator
        if (L->Act->tdata.type == t_LESS ||
            L->Act->tdata.type == t_GREATER ||
            L->Act->tdata.type == t_LESSOREQUAL ||
            L->Act->tdata.type == t_GREATEROREQUAL ||
            L->Act->tdata.type == t_EQUAL) {
            error_call(ERR_SEM_EXCOMPAT, L);
            BSTDisposeGlobal(&functions);
            BSTDisposeLocal(node);
        }
        //nasli sme id
        if (L->Act->tdata.type == t_IDENTIFIER) {
            //nchadza sa v tabulke, do typ sa ulozi jeho typ
            if (MainStackSearch(mainstack, L->Act->tdata.lex, &typ2)) {
                //nesmie to byt '_'
                if (!strcmp(L->Act->tdata.lex, "_")) {
                    error_call(ERR_SEM_UNDEF, L);
                    BSTDisposeGlobal(&functions);
                    BSTDisposeLocal(node);
                }
                //typ prveho a n-teho id sa nerovnaju
                if (typ != typ2) {
                    error_call(ERR_SEM_EXCOMPAT, L);
                    BSTDisposeGlobal(&functions);
                    BSTDisposeLocal(node);
                }
                TDLLSucc(L);
                continue;
            } else {
            error_call(ERR_SEM_EXCOMPAT, L);
            BSTDisposeGlobal(&functions);
            BSTDisposeLocal(node);
                
            }
            
        }
        //nasli sme literal t_INT_ZERO
        if (L->Act->tdata.type == t_INT_ZERO) {
            //porovname typy
            if (typ != t_INT_ID) {
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
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
            TDLLSucc(L);
            continue;
        }if (L->Act->tdata.type == t_FLOAT) {
            //porovname typy
            if (typ != t_FLOAT64) {
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
            TDLLSucc(L);
            continue;
        }
        TDLLSucc(L);
    }
    //vsetko v poriadku, vratim true a datovy typ
    *type = typ;
    //printf("Aktivny prvok po vykonani celeho isExpression: %s %d\n", L->Act->tdata.lex, L->Act->tdata.type);
    return true;
}   //koniec isExpression

/* ==========================================================================================
 - tato funckia kontroluje spravne parametre return statementu(pocet a typy)
 - aktivny prvok sa ocakava na klucovom slove return
 @param L obojstranne viazany zoznam tokenov
 @param id identifikator(retazec) danej funkcie
 */
void checkReturnStatement(TDLList *L, char *id, tBSTNodePtrLocal *node) {
    //sem sa vlozia informacie o funkcii
    functionData data;
   
    //vkladanie, tkato fcia uz musi byt v symtable
    BSTSearchGlobal(functions, id, &data);
    //sem sa ulozi typ returnovaneho literalu/identifiktora/vyrazu
    int typ = 0;
    
    //returnujeme z mainu
    if (!strcmp("main", id)) {
        if (L->Act->rptr->tdata.type == t_EOL) {
            return;
        } else {
            error_call(ERR_SEM_RETURN, L);
            BSTDisposeGlobal(&functions);
            BSTDisposeLocal(node);
        }
    }
    
    
    //ziadne navratove typy
    if (data.numOfReturns == 0) {
        if (L->Act->rptr->tdata.type == t_EOL) {
            return;
            //v pravo od returnu nieco je, error
        } else {
            error_call(ERR_SEM_RETURN, L);
            BSTDisposeGlobal(&functions);
            BSTDisposeLocal(node);
        }
    }
    
    if (data.numOfReturns > 0) {
        for (int i = 0; i < data.numOfReturns; i++) {
            TDLLSucc(L);
            if (!strcmp(L->Act->tdata.lex, ",")) {
                TDLLSucc(L);
            }
            //vracana hodnota je vyraz
            if (isExpression(L, &typ, node)) {
                //typ vyrazu je spravny
                if (data.returns[i] == typ) {
                    continue;
                }
                error_call(ERR_SEM_RETURN, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
            //vracana hodnota je literal
            if (isLiteral(L, &typ)) {
                if (data.returns[i] == typ) {
                    continue;
                }
                error_call(ERR_SEM_RETURN, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
            //vracana hodnota je samotny id
            if (L->Act->tdata.type == t_IDENTIFIER) {
                //musi sa nachadzat v symtable a nesmie to byt '_'
                if (MainStackSearch(mainstack, L->Act->tdata.lex, &typ) && (strcmp(L->Act->tdata.lex, "_"))) {
                //if (BSTSearchLocal(*node, L->Act->tdata.lex, &typ, unneeded)) {
                    if (data.returns[i] == typ) {
                        //po kazdom overeni musi aktivny prvok ostat za id/exp/literalom
                        TDLLSucc(L);
                        //printf("VYPISUJEM ALC->RPTR : %s\n", L->Act->rptr->tdata.lex);
                        continue;
                    }
                    error_call(ERR_SEM_RETURN, L);
                    BSTDisposeGlobal(&functions);
                    BSTDisposeLocal(node);
                }
            }
        }
    }
    //kontrola prebytocnyh parametrov, pre =1
    if (data.numOfReturns == 1) {
        if (L->Act->tdata.type != t_EOL) {
                   error_call(ERR_SEM_RETURN, L);
                   BSTDisposeGlobal(&functions);
                   BSTDisposeLocal(node);
        }
        return;
    }
        
    //kontrola prebytocnych parametrov, pre >1
    if (L->Act->tdata.type != t_EOL) {
        error_call(ERR_SEM_RETURN, L);
        BSTDisposeGlobal(&functions);
        BSTDisposeLocal(node);
    }
}   //koniec checkReturnStatement



/* ==========================================================================================
 - tato funkcia kontroluje zadane parametre pri volani funkcie
 - ak najde nezhodu(nespravny typ/pocet parametrov), vola error
 @param L obojstranne viazany zoznam tokenov
 @param id identifikator(retazec) danej funkcie
 */
void checkCallFunction(TDLList *L, char *id, tBSTNodePtrLocal *node) {
    
    functionData data;
    //sem bude vracat hodnotu BSTSearchLoacal
    int type = 0;
    
    //volanu funkciu nieje mozne volat, lebo je prekryta identifikatorom premennej
    if (MainStackSearch(mainstack, id, &type)) {
        error_call(ERR_SEM_OTHER, L);
        BSTDisposeGlobal(&functions);
        BSTDisposeLocal(node);
    }
    
    //ak sa vola print, treba overit iba identifikatory v symtables
    if (!strcmp(id, "print")) {
        //ak najdeme pravu zatvorku, sme na konci volania
        while (L->Act->tdata.type != t_RIGHT_BRACKET) {
            TDLLSucc(L);
            //nasli sme id, treba overit v symtable
            if (L->Act->tdata.type == t_IDENTIFIER) {
                
                //if (BSTSearchLocal(*node, L->Act->tdata.lex, &type, unneeded)) {
                if (MainStackSearch(mainstack, L->Act->tdata.lex, &type)) {
                    return;
                } else {
                    error_call(ERR_SEM_RETURN, L);
                    BSTDisposeGlobal(&functions);
                    BSTDisposeLocal(node);
                }
            }
            
        }
        return;
    }
    
    //sem bude vracat data BSTSearchGlobal
    if (BSTSearchGlobal(functions, id, &data)) {
        //v priprade existencie navratovych typov sa nesmie volat mimo priradenia
        if (data.numOfReturns > 0) {
            if (L->Act->lptr->tdata.type == t_EOL) {
                error_call(ERR_SEM_RETURN, L);
                BSTDisposeGlobal(&functions);
                BSTDisposeLocal(node);
            }
        }
        
        for (int i = 0; i < data.numOfParams; i++) {
            //posun z '(' -> 'parameter' alebo ',' -> 'parameter'
            TDLLSucc(L);
            TDLLSucc(L);
            //ak je parameter id
            if (L->Act->tdata.type == t_IDENTIFIER) {
                //musi byt v lokalnej/nadlokalnej symtable
                //nesmie to byt '_'
                if (MainStackSearch(mainstack, L->Act->tdata.lex, &type) && (strcmp(L->Act->tdata.lex, "_"))) {
                    //jeho typ musi byt rovnaky ako typ parametru na pozicii i vo funkcii
                    if (type == data.paramsType[i]) {
                        //vsetko sedi, kontroluj dalsi parameter
                        continue;
                    } else {
                        error_call(ERR_SEM_RETURN, L);
                        BSTDisposeGlobal(&functions);
                        BSTDisposeLocal(node);
                    }
                } else {
                    error_call(ERR_SEM_RETURN, L);
                    BSTDisposeGlobal(&functions);
                    BSTDisposeLocal(node);
                }
            } else {
                //parameter nie je identifikator, je napr konstanta, retazcovy literal...
                switch (L->Act->tdata.type) {
                    case t_INT_ZERO | t_INT_NON_ZERO:
                        if (data.paramsType[i] == t_INT_ID) {
                            //parameter v poriadku, kontroluj dalsi parameter
                            continue;
                        } else {
                            error_call(ERR_SEM_RETURN, L);
                            BSTDisposeGlobal(&functions);
                            BSTDisposeLocal(node);
                        }
                        break;
                    case t_STRING:
                        if (data.paramsType[i] == t_STRING_ID) {
                            //parameter v poriadku, kontroluj dalsi parameter
                            continue;
                        } else {
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
            //fix pre pripad nula parametrov
            if (L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->rptr->tdata.type == t_RIGHT_BRACKET) {
                return;
            }
            error_call(ERR_SEM_RETURN, L);
        }
    }else{
        error_call(ERR_SEM_UNDEF, L);
        BSTDisposeGlobal(&functions);
        BSTDisposeLocal(node);
    }
}   //koniec checkCallFunction


/* ==========================================================================================
 - Tato funkcia kontroluje podmienku v hlavicke ifu
 - Ak je v hlavicke viac relacnych operatorov alebo iny operator ako relac. --> chyba
 @param L obojstranne viazany zoznam tokenov
 @param node lokalny strom ramca nad ifom
 @param params informacie o funkcie v ktorej sme
 */
void if_control(TDLList *L, tBSTNodePtrLocal *node, functionData params){

    if(!(strcmp(L->Act->tdata.lex, "("))){
        TDLLSucc(L);
    }
    // Ak su oba identifikatory
    if(L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->rptr->tdata.type == t_IDENTIFIER){
        int typ1;
        int typ2;
        // je prvy v strome?
        if(MainStackSearch(mainstack, L->Act->tdata.lex, &typ1)){
            if(!(strcmp(L->Act->tdata.lex, "_"))){
                //printf("1\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }

        }else{
             error_call(ERR_SEM_UNDEF, L);
             BSTDisposeLocal(node);
        }
        
        
        if(MainStackSearch(mainstack, L->Act->rptr->rptr->tdata.lex, &typ2)){
            if(!(strcmp(L->Act->rptr->rptr->tdata.lex, "_"))){
                //printf("2\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }

        }else{
            error_call(ERR_SEM_UNDEF, L);
            BSTDisposeLocal(node);
        }

        // Maju rovnake typy?
        if(!(typ1 == typ2)){
            //printf("3\n");
            error_call(ERR_SEM_EXCOMPAT, L);
            BSTDisposeLocal(node);
        }

    // ak je lavy operator identifikator
    }else if(L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->rptr->tdata.type != t_IDENTIFIER){
        int typ1;
        int typ2;
        // Je prvy id v strome?
        if(MainStackSearch(mainstack, L->Act->tdata.lex, &typ1)){
            if(!(strcmp(L->Act->tdata.lex, "_"))){
                //printf("4\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }

        }else{
            error_call(ERR_SEM_UNDEF, L);
            BSTDisposeLocal(node);
        }
    
        // Ma id rovnaky typ ako cislo?
        if(L->Act->rptr->rptr->tdata.type == t_INT_NON_ZERO || L->Act->rptr->rptr->tdata.type == t_INT_ZERO){
            typ2 = t_INT_ID;
            if(!(typ1 == typ2)){
                //printf("5\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }
        // ma id rovnaky typ ako float?
        }else if(L->Act->rptr->rptr->tdata.type == t_FLOAT64 || L->Act->rptr->rptr->tdata.type == t_FLOAT){
            typ2 = t_FLOAT64;
            if(!(typ1 == typ2)){
                //printf("6\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }
        // ma id rovnaky typ ako string?
        }else if(L->Act->rptr->rptr->tdata.type == t_STRING){
            typ2 = t_STRING_ID;
            if(!(typ1 == typ2)){
                //printf("7\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }
        }
        
    // ak je pravy operator identifikator
    }else if(L->Act->tdata.type != t_IDENTIFIER && L->Act->rptr->rptr->tdata.type == t_IDENTIFIER){
        int typ1;
        int typ2;
        // Je v strome?
        if(MainStackSearch(mainstack, L->Act->rptr->rptr->tdata.lex, &typ2)){
            if(!(strcmp(L->Act->rptr->rptr->tdata.lex, "_"))){
                //printf("8\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                
                BSTDisposeLocal(node);
            }
        }else{
            error_call(ERR_SEM_UNDEF, L);
            BSTDisposeLocal(node);
        }

        // Ma id rovnaky typ ako cislo?
        if(L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ZERO){
            typ1 = t_INT_ID;
            if(!(typ1 == typ2)){
                //printf("1\n");
                 error_call(ERR_SEM_EXCOMPAT, L);
                   BSTDisposeLocal(node);
            }

        // ma id rovnaky typ ako float?
        }else if(L->Act->tdata.type == t_FLOAT64 || L->Act->tdata.type == t_FLOAT){
            typ1 = t_FLOAT64;
            if(!(typ1 == typ2)){
                //printf("1\n");
                   error_call(ERR_SEM_EXCOMPAT, L);
                   BSTDisposeLocal(node);
            }

        // ma id rovnaky typ ako string?
        }else if(L->Act->tdata.type == t_STRING){
            typ1 = t_STRING_ID;
            if(!(typ1 == typ2)){
                //printf("1\n");
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
                if(typson1 != typson2){
                    //printf("1\n");
                    error_call(ERR_SEM_EXCOMPAT, L);
                   BSTDisposeLocal(node);
                }

            }else{
                //printf("1\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }
        // Ak ide o floaty maju rovnake typy?
        }else if(L->Act->tdata.type == t_FLOAT64 || L->Act->tdata.type == t_FLOAT){
            int typson1 = t_FLOAT64;
            if(L->Act->rptr->rptr->tdata.type == t_FLOAT64 || L->Act->rptr->rptr->tdata.type == t_FLOAT){
                int typson2 = t_FLOAT64;
                if(typson1 != typson2){
                    error_call(ERR_SEM_EXCOMPAT, L);
                    BSTDisposeLocal(node);
                }

            }else{
                //printf("1\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }
        // Ak ide o stringy maju rovnake typy?
        }if(L->Act->tdata.type == t_STRING){
            int typson1 = t_STRING_ID;
            if(L->Act->rptr->rptr->tdata.type == t_STRING){
                int typson2 = t_STRING_ID;
                if(typson1 != typson2){
                    //printf("1\n");
                    error_call(ERR_SEM_EXCOMPAT, L);
                    BSTDisposeLocal(node);
                }

            }else{
                //printf("1\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }
        }
    }

    TDLLSucc(L);
    int real_op = 0;
   

        
    while(L->Act->tdata.type != t_BRACES_L){
        if(L->Act->tdata.type == T_SEMICOLON){
            break;
        }
        if(L->Act->tdata.type == t_GREATER){
            real_op++;
        }else if(L->Act->tdata.type == t_GREATEROREQUAL){
            real_op++;
        }else if(L->Act->tdata.type == t_LESS){
            real_op++;
        }else if(L->Act->tdata.type == t_LESSOREQUAL){
            real_op++;
        }else if(L->Act->tdata.type == t_EQUAL){
            real_op++;
        }else if(L->Act->tdata.type == t_NOT_EQUAL){
            real_op++;
        }else {
            if(L->Act->tdata.type == t_PLUS){
                //printf("1\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }
             
            if(L->Act->tdata.type == t_MINUS){
                //printf("1\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }
                
            if(L->Act->tdata.type == t_MULTIPLY){
                //printf("1\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }
              
            if(L->Act->tdata.type == t_DIVIDE){
                //printf("1\n");
                error_call(ERR_SEM_EXCOMPAT, L);
                BSTDisposeLocal(node);
            }
               
        }

        TDLLSucc(L);
    }
     
    if(real_op != 1){
        //printf("1\n");
        error_call(ERR_SEM_EXCOMPAT, L);
        BSTDisposeLocal(node);
    }
}   //koniec if_control

//==========================================================================================
/* ==========================================================================================
 - Tato funkcia kontroluje telo else ramca
 @param L obojstranne viazany zoznam tokenov
 @param elsenode lokalny strom ramca else tela
 @param params informacie o funkcie v ktorej sme
 @param id nazov funkcie v ktorej sme
 */
void enter_else_body(TDLList *L, tBSTNodePtrLocal *elsenode, functionData params, char *id){
    // preskoc else
    TDLLSucc(L);
    // prescko {
    TDLLSucc(L);

    while(L->Act->tdata.type != t_BRACES_R){
        if ((L->Act->tdata.type == t_IDENTIFIER) && (L->Act->lptr->tdata.type != t_FUNC) && (L->Act->rptr->tdata.type == t_LEFT_BRACKET)) {
            checkCallFunction(L, L->Act->tdata.lex, elsenode);
        }
        if(L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->tdata.type == t_ASSIGN){
			TDLLElemPtr temp_node = L->Act;
			while(temp_node->tdata.type != t_EOL){
				if(temp_node->lptr->tdata.type == t_EOL){
					assign_vals_control(L, elsenode, params);
					break;
				}
				TDLLPred(L);
				temp_node = temp_node->lptr;
			}
		}
        
        if (L->Act->tdata.type == t_DEFINITION_2) {
            TDLLPred(L);
			dec_var_control(L, elsenode, params);
        }
        if (L->Act->tdata.type == t_FOR) {           
            enter_for_body(L, elsenode, params, id);  
        }
        if (L->Act->tdata.type == t_IF) {
            TDLLSucc(L);          
			enter_if_body(L, elsenode, params, id);
        }
        if (L->Act->tdata.type == t_RETURN) {
            checkReturnStatement(L, id, elsenode);
        }
        
        TDLLSucc(L);
    }
}   //koniec enter_else_body

/* ==========================================================================================
 - Tato funkcia kontroluje telo if ramca
 @param L obojstranne viazany zoznam tokenov
 @param funcnode lokalny strom ramca nad ifom(pre kontrolu podmienky v if hlavicke)
 @param params informacie o funkcie v ktorej sme
 @param id nazov funkcie v ktorej sme
 */
void enter_if_body(TDLList *L, tBSTNodePtrLocal *funcnode, functionData params, char *id){
   
    if_control(L, funcnode, params);
    // lokalny strom na vyuzivanie v ife
    // pri kazdom ife sa vytvori novy strom
    tBSTNodePtrLocal if_node;
    BSTInitLocal(&if_node);
    // pri kazdom ide sa ulozie ifnode na zasobnik
    PushTreeMain(&mainstack, &if_node);
    // chod za { ifu
    // kvoli podmienke while-u
    TDLLSucc(L);
    
    // chod az za else
    // v else nie je ziadnka podmienka tzv. co je pred { nas nemusi trapit
    while(L->Act->tdata.type != t_BRACES_L){        
        if ((L->Act->tdata.type == t_IDENTIFIER) && (L->Act->lptr->tdata.type != t_FUNC) && (L->Act->rptr->tdata.type == t_LEFT_BRACKET)) {
            checkCallFunction(L, L->Act->tdata.lex, &if_node);
        }
        if(L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->tdata.type == t_ASSIGN){
			TDLLElemPtr temp_node = L->Act;
			while(temp_node->tdata.type != t_EOL){
				if(temp_node->lptr->tdata.type == t_EOL){
					assign_vals_control(L, &if_node, params);
					break;
				}
				TDLLPred(L);
				temp_node = temp_node->lptr;
			}
		}
        
        if (L->Act->tdata.type == t_DEFINITION_2) {
            TDLLPred(L);
			dec_var_control(L, &if_node, params);
        }
        if (L->Act->tdata.type == t_FOR) {           
            enter_for_body(L, &if_node, params, id);
        }
        if (L->Act->tdata.type == t_IF) {
            TDLLSucc(L);        
			enter_if_body(L, &if_node, params, id);
        }
        if (L->Act->tdata.type == t_RETURN) {
            checkReturnStatement(L, id, &if_node);
        }
        if(L->Act->tdata.type == t_ELSE){
            if_node = PopTreeMain(&mainstack);
            // uvolnenie celeho lokalneho stromu ifu
            BSTDisposeLocal(&if_node);
            // lokalny strom na vyuzivanie v ife
            // pri kazdom ife sa vytvori novy strom
            tBSTNodePtrLocal else_node;
            BSTInitLocal(&else_node);
            // pri kazdom else sa ulozie elsenode na zasobnik
            PushTreeMain(&mainstack, &else_node);
            enter_else_body(L, &else_node, params, id);

            else_node = PopTreeMain(&mainstack);
            // uvolnenie celeho lokalneho stromu else
            BSTDisposeLocal(&else_node);
            // po else sa vrati }
            // break aby nedoslo k seg fault
            // while by pokracoval pokym nenajde { ale ten po else uz nie je
            // pokracoval by teda donekonecna alebo po seg fault
            if(L->Act->tdata.type == t_BRACES_R) break;
        }
        TDLLSucc(L);
    }   
    TDLLSucc(L);
}   //koniec enter_if_body

/* ==========================================================================================
 - Tato funkcia kontroluje hlavicku foru
 @param L obojstranne viazany zoznam tokenov
 @param node lokalny strom ramca nad forom 
 @param params informacie o funkcie v ktorej sme
 */
void for_control(TDLList *L, tBSTNodePtrLocal *node, functionData params){   
    TDLLSucc(L);
	// ak je prvy lexem bodkociarka
	if(L->Act->tdata.type == T_SEMICOLON){
		TDLLSucc(L); // chod za bodkociarku (ma byt identifikator alebo vyraz)
		// Skontroluj podmienku
		if_control(L, node, params);

		if(L->Act->tdata.type != T_SEMICOLON){
            error_call(ERR_SEM_OTHER, L);
            BSTDisposeLocal(node);
        } 
        TDLLSucc(L);
        if(!(L->Act->tdata.type == t_BRACES_L)) {assign_vals_control(L, node, params);}
        TDLLPred(L);
	// Ak prvy lexem nie je bodkociarka
	}else if(L->Act->tdata.type == t_IDENTIFIER){
	
		dec_var_control(L, node, params);
	
        TDLLSucc(L);

		if_control(L, node, params);
		
		if(L->Act->tdata.type != T_SEMICOLON){
            error_call(ERR_SEM_OTHER, L);
                BSTDisposeLocal(node);
        } 
		TDLLSucc(L);
		// ak nechyba vyraz skontroluj ho
		if(!(L->Act->tdata.type == t_BRACES_L)) assign_vals_control(L, node, params);
        TDLLPred(L);
	// ak chyba bodkociarka aj deklaracia
	}else
	{
        error_call(ERR_SEM_OTHER, L);
        BSTDisposeLocal(node);
	}
}   //koniec for_control

/* ==========================================================================================
 - Tato funkcia kontroluje telo foru
 @param L obojstranne viazany zoznam tokenov
 @param fornode lokalny strom ramca nad forom
 @param params informacie o funkcie v ktorej sme
 @param id nazov funkcie v ktorej sme
 */
void enter_for_body(TDLList *L, tBSTNodePtrLocal *fornode, functionData params, char *id){

    tBSTNodePtrLocal for_node;
    BSTInitLocal(&for_node);
    char *nametodelete;
    PushTreeMain(&mainstack, &for_node);
    if(L->Act->rptr->tdata.type != T_SEMICOLON){
        // uloz premennu resp. uzol stromu do tempnode
        nametodelete = L->Act->rptr->tdata.lex;
    } else {
        nametodelete = "none";
    }
    
    for_control(L, fornode, params);
   
    while(L->Act->tdata.type != t_BRACES_R){        
        if ((L->Act->tdata.type == t_IDENTIFIER) && (L->Act->lptr->tdata.type != t_FUNC) && (L->Act->rptr->tdata.type == t_LEFT_BRACKET)) {
            checkCallFunction(L, L->Act->tdata.lex, &for_node);
        }
        if(L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->tdata.type == t_ASSIGN){
			TDLLElemPtr temp_node = L->Act;
			while(temp_node->tdata.type != t_EOL){
				if(temp_node->lptr->tdata.type == t_EOL){                   
					assign_vals_control(L, &for_node, params);
					break;
				}
				TDLLPred(L);
				temp_node = temp_node->lptr;
			}
		}
        
        if (L->Act->tdata.type == t_DEFINITION_2) {
            TDLLPred(L);
			dec_var_control(L, &for_node, params);
        }
        if (L->Act->tdata.type == t_FOR) {          
            enter_for_body(L, &for_node, params, id);   
        }
        if (L->Act->tdata.type == t_IF) {
            TDLLSucc(L);
			enter_if_body(L, &for_node, params, id);
        }
        if (L->Act->tdata.type == t_RETURN) {
            checkReturnStatement(L, id, fornode);
        }
        TDLLSucc(L);
    }

    // chod za }
    // v enterfuncbody sa nezapocitalo {
    // tak sa nemoze ani }
    // inac ide o chybne pocitanie
    TDLLSucc(L);
    
    if(!(strcmp(nametodelete, "none"))){

    }else{
        BSTDelete(fornode, nametodelete);
    }
    for_node = PopTreeMain(&mainstack);
    BSTDisposeLocal(&for_node);
}   //koniec enter_for_body

/* ==========================================================================================
 - Tato funkcia kontroluje deklaraciu
 @param L obojstranne viazany zoznam tokenov
 @param node lokalny strom 
 @param params informacie o funkcie v ktorej sme
 */
void dec_var_control(TDLList *L, tBSTNodePtrLocal *node, functionData params){

     if(L->Act->rptr->tdata.type == t_EOL){
        error_call(ERR_SEM_OTHER, L);
    }
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
    if(L->Act->lptr->tdata.type != t_FOR){
        if(L->Act->lptr->tdata.type != t_EOL) error_call(ERR_SEM_OTHER, L);
    }
    // ak je premenna ktoru chceme definovat uz definovana
    if(BSTSearchLocal(*node, id, &type, data)){
        error_call(ERR_SEM_UNDEF,L);
        BSTDisposeLocal(node);
    }
    // Chod na dalsi prvok teda  ->  :=
    //TDLLSucc(L);
    // Chod na koniec riadku
    while(L->Act->tdata.type != t_EOL){
        if(L->Act->tdata.type == t_GREATER){
            BSTDisposeLocal(node);
            printf("1\n");
            error_call(ERR_SEM_EXCOMPAT, L);
        }else if(L->Act->tdata.type == t_GREATEROREQUAL){
            BSTDisposeLocal(node);
            printf("1\n");
            error_call(ERR_SEM_EXCOMPAT, L);
        }else if(L->Act->tdata.type == t_LESS){
            BSTDisposeLocal(node);
            printf("1\n");
            error_call(ERR_SEM_EXCOMPAT, L);
        }else if(L->Act->tdata.type == t_LESSOREQUAL){
            BSTDisposeLocal(node);
            printf("1\n");
            error_call(ERR_SEM_EXCOMPAT, L);
        }else if(L->Act->tdata.type == t_EQUAL){
            BSTDisposeLocal(node);
            printf("1\n");
            error_call(ERR_SEM_EXCOMPAT, L);
        }else if(L->Act->tdata.type == t_NOT_EQUAL){
            BSTDisposeLocal(node);
            printf("1\n");
            error_call(ERR_SEM_EXCOMPAT, L);
        }

        if(!(strcmp(L->Act->tdata.lex, "/"))){
            if(L->Act->rptr->tdata.type == t_INT_ZERO){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_ZERODIV, L);
            }
        }

        if(!(strcmp(L->Act->tdata.lex, "_"))){
            BSTDisposeLocal(node);
            error_call(ERR_SEM_UNDEF, L);
        }
        
        // Ak ide o integer
        if(L->Act->tdata.type == t_INT_ZERO){
            // Uloz jeho typ
            type = t_INT_ID;
            // Zapocitaj najdenie integeru
            int_count++;

        // Ak ide o float
        }else if(L->Act->tdata.type == t_INT_NON_ZERO){
            type = t_INT_ID;
            // Zapocitaj najdenie integeru
            data = "0";
            int_count++;

        }else if(L->Act->tdata.type == t_FLOAT64 || L->Act->tdata.type == t_FLOAT){

            if(!(strcmp(L->Act->tdata.lex, "0.0"))){
                if(!(strcmp(L->Act->lptr->tdata.lex, "/"))){
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_ZERODIV, L);
                }
            }
            // Uloz jeho typ
            type = t_FLOAT64;
            // zapocitaj najdenie floatu
            float_count++;

        // Ak ide o string
        }else if(L->Act->tdata.type == t_STRING){
            if(L->Act->rptr->tdata.type == t_MINUS || L->Act->rptr->tdata.type == t_DIVIDE || L->Act->rptr->tdata.type == t_MULTIPLY){
                BSTDisposeLocal(node);
                printf("1\n");
                error_call(ERR_SEM_EXCOMPAT, L);
            }
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

            if(!(strcmp(L->Act->tdata.lex, "_"))){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_UNDEF, L);
            }

            if(!(strcmp(L->Act->tdata.lex, "inputs"))){
                error_call(ERR_SEM_OTHER, L);
                   
            }else if(!(strcmp(L->Act->tdata.lex, "inputi"))){
                error_call(ERR_SEM_OTHER, L);
               
            }else if(!(strcmp(L->Act->tdata.lex, "inputf"))){
                error_call(ERR_SEM_OTHER, L);
                
            }else if(!(strcmp(L->Act->tdata.lex, "int2float"))){
                assign_func_params_control(L,node, "int2float");
                type = t_FLOAT64;
                BSTInsertLocal(node, id,&type, data);
             
            }else if(!(strcmp(L->Act->tdata.lex, "float2int"))){
                assign_func_params_control(L,node, "float2int");
                type = t_INT_ID;
                BSTInsertLocal(node, id,&type, data);
               
            }else if(!(strcmp(L->Act->tdata.lex, "len"))){
                assign_func_params_control(L,node, "len");
                type = t_INT_ID;
                BSTInsertLocal(node, id,&type, data);
             
            }else if(!(strcmp(L->Act->tdata.lex, "substr"))){
                error_call(ERR_SEM_OTHER, L);
             
            }else if(!(strcmp(L->Act->tdata.lex, "ord"))){
                error_call(ERR_SEM_OTHER, L);
 
            }else if(!(strcmp(L->Act->tdata.lex, "chr"))){
                error_call(ERR_SEM_OTHER, L);

            }else if(!(strcmp(L->Act->tdata.lex, "print"))){
                printf("1\n");
                error_call(ERR_SEM_EXCOMPAT, L);

            }else if(BSTSearchGlobal(functions, L->Act->tdata.lex, &returned_data)){
                if(L->Act->rptr->tdata.type == t_LEFT_BRACKET){
                    checkCallFunction(L, L->Act->tdata.lex, node);
                
                    if(returned_data.numOfReturns != 1) error_call(ERR_SEM_RETURN, L);
                    else{
                        type = returned_data.returns[0];
                        BSTInsertLocal(node, id,&type, data);
                    }
                }
            }else if (MainStackSearch(mainstack, name, &datovytyp)) {
                    type = datovytyp;
                if(datovytyp == t_INT_ID){
                    int_count++;
                }else if(datovytyp == t_STRING_ID){
                    if(L->Act->rptr->tdata.type == t_MINUS || L->Act->rptr->tdata.type == t_DIVIDE || L->Act->rptr->tdata.type == t_MULTIPLY){
                    BSTDisposeLocal(node);
                        printf("1\n");
                    error_call(ERR_SEM_EXCOMPAT, L);
                    }
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
}   //koniec dec_var_control

/* ==========================================================================================
 - Tato funkcia kontroluje parametre vstavanych funkcii
 @param L obojstranne viazany zoznam tokenov
 @param node lokalny strom ramca 
 @param func_name Nazov najdenej vstavanej funkcie
 */
void assign_func_params_control(TDLList *L,tBSTNodePtrLocal *node, char *func_name){
    int type;
    if(!(strcmp(func_name, "inputs"))){
        TDLLSucc(L);
        TDLLSucc(L);
        if(L->Act->tdata.type != t_RIGHT_BRACKET){
            BSTDisposeLocal(node);
            error_call(ERR_SEM_DATATYPE, L);
        } 
    }else if(!(strcmp(func_name, "inputi"))){
        TDLLSucc(L);
        TDLLSucc(L);
        if(L->Act->tdata.type != t_RIGHT_BRACKET){
            BSTDisposeLocal(node);
            error_call(ERR_SEM_DATATYPE, L);
        } 
    }else if(!(strcmp(func_name, "inputf"))){
        TDLLSucc(L);
        TDLLSucc(L);
        if(L->Act->tdata.type != t_RIGHT_BRACKET){
            BSTDisposeLocal(node);
            error_call(ERR_SEM_DATATYPE, L);
        } 
    }else if(!(strcmp(func_name, "int2float"))){
        TDLLSucc(L);
        TDLLSucc(L);
        if(L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ZERO || L->Act->tdata.type == t_INT_ID){

        }else if(L->Act->tdata.type == t_IDENTIFIER){
            if(!(MainStackSearch(mainstack, L->Act->tdata.lex, &type))){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_UNDEF,L);
            }else{
                if(type == t_INT_ID){}
                else{
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_DATATYPE, L);
                }
            }
        }else{
            BSTDisposeLocal(node);
            error_call(ERR_SEM_DATATYPE, L);
        }
    }else if(!(strcmp(func_name, "float2int"))){
        TDLLSucc(L);
        TDLLSucc(L);
        if(L->Act->tdata.type == t_FLOAT64 || L->Act->tdata.type == t_FLOAT){

        }else if(L->Act->tdata.type == t_IDENTIFIER){
            if(!(MainStackSearch(mainstack, L->Act->tdata.lex, &type))){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_UNDEF, L);
            }else{
                if(type == t_FLOAT64){}
                else{
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_DATATYPE, L);
                }
            }
        }else{
            BSTDisposeLocal(node);
            error_call(ERR_SEM_DATATYPE, L);
        }
    }else if(!(strcmp(func_name, "len"))){
        TDLLSucc(L);
        TDLLSucc(L);
        if(L->Act->tdata.type == t_STRING_ID || L->Act->tdata.type == t_STRING){

        }else if(L->Act->tdata.type == t_IDENTIFIER){
            if(!(MainStackSearch(mainstack, L->Act->tdata.lex, &type))){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_UNDEF, L);
            }else{
                if(type == t_STRING_ID){}
                else{
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_DATATYPE, L);
                }
            }
        }else{
            BSTDisposeLocal(node);
            error_call(ERR_SEM_DATATYPE, L);
        }
    }else if(!(strcmp(func_name, "substr"))){
        TDLLSucc(L);
        TDLLSucc(L);
        if(L->Act->tdata.type == t_STRING || L->Act->tdata.type == t_STRING_ID){
            TDLLSucc(L);
            if(L->Act->tdata.type != t_COMMA){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_DATATYPE, L);
            }
            TDLLSucc(L);
            if(L->Act->tdata.type == t_INT_ID || L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ZERO){
                TDLLSucc(L);
                if(L->Act->tdata.type != t_COMMA){
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_DATATYPE, L);
                }
                TDLLSucc(L);
                if(L->Act->tdata.type == t_INT_ID || L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ZERO){
                
                }else if(L->Act->tdata.type == t_IDENTIFIER){
                    if(!(MainStackSearch(mainstack, L->Act->tdata.lex, &type))){
                        BSTDisposeLocal(node);
                        error_call(ERR_SEM_UNDEF, L);
                    }else{
                        if(type == t_INT_ID){}
                        else{
                            BSTDisposeLocal(node);
                            error_call(ERR_SEM_DATATYPE, L);
                        }
                    }
                }else{
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_DATATYPE, L);
                }
            }else if(L->Act->tdata.type == t_IDENTIFIER){
                if(!(MainStackSearch(mainstack, L->Act->tdata.lex, &type))){
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_UNDEF, L);
                }else{
                    if(type == t_INT_ID){}
                    else{
                        BSTDisposeLocal(node);
                        error_call(ERR_SEM_DATATYPE, L);
                    }
                }
            }else{
                BSTDisposeLocal(node);
                error_call(ERR_SEM_DATATYPE, L);
            }
        }else if(L->Act->tdata.type == t_IDENTIFIER){
            if(!(MainStackSearch(mainstack, L->Act->tdata.lex, &type))){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_UNDEF, L);
            }else{
                if(type == t_STRING_ID){
                    TDLLSucc(L);
                    if(L->Act->tdata.type != t_COMMA){
                        BSTDisposeLocal(node);
                        error_call(ERR_SEM_DATATYPE, L);
                    }
                    TDLLSucc(L);
                    if(L->Act->tdata.type == t_INT_ID || L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ZERO){
                        TDLLSucc(L);
                        if(L->Act->tdata.type != t_COMMA){
                            BSTDisposeLocal(node);
                            error_call(ERR_SEM_DATATYPE, L);
                        }
                        TDLLSucc(L);
                        if(L->Act->tdata.type == t_INT_ID || L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ZERO){
                
                        }else if(L->Act->tdata.type == t_IDENTIFIER){
                            if(!(MainStackSearch(mainstack, L->Act->tdata.lex, &type))){
                                BSTDisposeLocal(node);
                                error_call(ERR_SEM_UNDEF, L);
                            }else{
                                if(type == t_INT_ID){}
                                else{
                                    BSTDisposeLocal(node);
                                    error_call(ERR_SEM_DATATYPE, L);
                                }
                            }
                        }else{
                            BSTDisposeLocal(node);
                            error_call(ERR_SEM_DATATYPE, L);
                        }
                    }else if(L->Act->tdata.type == t_IDENTIFIER){
                        if(!(MainStackSearch(mainstack, L->Act->tdata.lex, &type))){
                            BSTDisposeLocal(node);
                            error_call(ERR_SEM_UNDEF, L);
                        }else{
                            if(type == t_INT_ID){}
                            else{
                                BSTDisposeLocal(node);
                                error_call(ERR_SEM_DATATYPE, L);
                            }
                        }
                    }else{
                        BSTDisposeLocal(node);
                        error_call(ERR_SEM_DATATYPE, L);
                    }
                }
                else{
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_DATATYPE,L);
                }
            }
        }else{
            BSTDisposeLocal(node);
            error_call(ERR_SEM_DATATYPE, L);
        }
    }else if(!(strcmp(func_name, "ord"))){
        TDLLSucc(L);
        TDLLSucc(L);
        if(L->Act->tdata.type == t_STRING_ID || L->Act->tdata.type == t_STRING){
            TDLLSucc(L);
            if(L->Act->tdata.type != t_COMMA){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_DATATYPE, L);
            }
            TDLLSucc(L);
            if(L->Act->tdata.type == t_INT_ZERO || L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ID){

            }else if(L->Act->tdata.type == t_IDENTIFIER){
            if(!(MainStackSearch(mainstack, L->Act->tdata.lex, &type))){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_UNDEF, L);
            }else{
                if(type == t_INT_ID){}
                else{
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_DATATYPE, L);
                }
            }
        }else{
                BSTDisposeLocal(node);
                error_call(ERR_SEM_DATATYPE, L);
            }
        }else if(L->Act->tdata.type == t_IDENTIFIER){
            if(!(MainStackSearch(mainstack, L->Act->tdata.lex, &type))){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_UNDEF, L);
            }else{
                if(type == t_STRING_ID){}
                else{
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_DATATYPE, L);
                }
            }
        }else{
            BSTDisposeLocal(node);
            error_call(ERR_SEM_DATATYPE, L);
        }
    }else if(!(strcmp(func_name, "chr"))){
        TDLLSucc(L);
        TDLLSucc(L);
        if(L->Act->tdata.type == t_INT_ID || L->Act->tdata.type == t_INT_ZERO || L->Act->tdata.type == t_INT_NON_ZERO){

        }else if(L->Act->tdata.type == t_IDENTIFIER){
            if(!(MainStackSearch(mainstack, L->Act->tdata.lex, &type))){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_UNDEF, L);
            }else{
                if(type == t_INT_ID){}
                else{
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_DATATYPE, L);
                }
            }
        }else{
            BSTDisposeLocal(node);
            error_call(ERR_SEM_DATATYPE, L);
        }
    }
}   //koniec assign_func_params_control

/* ==========================================================================================
 - Tato funkcia kontroluje priradovanie
 @param L obojstranne viazany zoznam tokenov
 @param node lokalny strom ramca 
 @param params informacie o funkcie v ktorej sme
 */
void assign_vals_control(TDLList *L, tBSTNodePtrLocal *node, functionData params){

    if(L->Act->rptr->tdata.type == t_EOL){
        error_call(ERR_SEM_OTHER, L);
    }
  
    char *id[MAXPARAMS];
    // na ukladanie typov
    int type[MAXPARAMS];
    int types_of_defined_vars[MAXPARAMS];
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
    int underscore_found = 0;

    // Pocet premennych nalavo od =
    // po ukonceni cyklu bude L->act ukazovat na =
   
    while(L->Act->tdata.type != t_ASSIGN){
        if(L->Act->tdata.type == t_IDENTIFIER){

            id[identifier] = L->Act->tdata.lex;
            if(MainStackSearch(mainstack, id[identifier], &types_of_defined_vars[identifier])){
                //printf("Vo vyssom ramci: nazov: %s, hodnota %d\n\n", id[identifier], types_of_defined_vars[identifier]);
                    // premenna bola najdena v ramci a while bol hned zastaveny
                    // stack sa prehladava zhora dole
                if(!(strcmp(id[identifier], "_"))){
                    types_of_defined_vars[identifier] = 404;
                  //  printf("Premenna %s, typ %d, pozicia %d\n\n", id[identifier], types_of_defined_vars[identifier], identifier);
                    underscore_found = 1;
                }
                //identifier++;
                //break;
            }else{
                BSTDisposeLocal(node);
                error_call(ERR_SEM_UNDEF, L);
            }
            identifier++;
        }
        TDLLSucc(L);
    }
 //printf("\nPRESIEL SOM ZA WHILE LOOP\n");
   // printf("Pocet premennych nalavo od =: %d\n\n", identifier);


    // pocet identifikatorov napravo od =
    //int identif_count = 0;
    // pocet ciarok napravo od =
    int comma_count = 0;

 //   printf("\n SME NA LEXEME: %s\n", L->Act->tdata.lex);
    // prechadzaj pokym neprejdeme na koniec riadku
    while(L->Act->tdata.type!= t_EOL){
            if(L->Act->tdata.type == t_GREATER){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_EXCOMPAT, L);
            }else if(L->Act->tdata.type == t_GREATEROREQUAL){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_EXCOMPAT, L);
            }else if(L->Act->tdata.type == t_LESS){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_EXCOMPAT, L);
            }else if(L->Act->tdata.type == t_LESSOREQUAL){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_EXCOMPAT, L);
            }else if(L->Act->tdata.type == t_EQUAL){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_EXCOMPAT, L);
            }else if(L->Act->tdata.type == t_NOT_EQUAL){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_EXCOMPAT, L);
            }

            if(!(strcmp(L->Act->tdata.lex, "/"))){
                if(L->Act->rptr->tdata.type == t_INT_ZERO){
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_ZERODIV, L);
                }
            }

            if(!(strcmp(L->Act->tdata.lex, "_"))){
                BSTDisposeLocal(node);
                error_call(ERR_SEM_UNDEF, L);
            }
           

            if(L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ZERO){
              //  printf("Piradujem integer pozicia %d\n", id_insert);
                type[id_insert] = t_INT_ID;
                int_count++;
                id_insert++;
            // ak ide o float
            }else if(L->Act->tdata.type == t_FLOAT64 || L->Act->tdata.type == t_FLOAT){
             //   printf("\n\n NASIEL SOM FLOAT\n\n");
                if(!(strcmp(L->Act->tdata.lex, "0.0"))){
                    if(!(strcmp(L->Act->lptr->tdata.lex, "/"))){
                        BSTDisposeLocal(node);
                        error_call(ERR_SEM_ZERODIV, L);
                    }
                }
                type[id_insert] = t_FLOAT64;
                float_count++;
                id_insert++;
            // ak ide o string
            }else if(L->Act->tdata.type == t_STRING){
                if(L->Act->rptr->tdata.type == t_MINUS || L->Act->rptr->tdata.type == t_DIVIDE || L->Act->rptr->tdata.type == t_MULTIPLY){
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_EXCOMPAT, L);
                }
                
               // printf("Nasiel som string\n\n");
                type[id_insert] = t_STRING_ID;
                string_count++;
                id_insert++;
            // ak ide o premennu
            }else if(L->Act->tdata.type == t_IDENTIFIER){
                int datovytyp;
                char *name = L->Act->tdata.lex;
                functionData returned_data;
             //   printf("\nMENO PREMENNEJ NALAVO OD = %s\n\n", name);
                if(!(strcmp(L->Act->tdata.lex, "_"))){
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_UNDEF, L);
                }
           
                if(!(strcmp(L->Act->tdata.lex, "inputs"))){
                   // printf("Ide o inputs\n");
                    assign_func_params_control(L, node, "inputs");
                    if(identifier != 2) error_call(ERR_SEM_RETURN, L);
                   // printf("Kontrolujem typy premennych\n");
                   if(types_of_defined_vars[0] == t_STRING_ID && (types_of_defined_vars[1] == t_INT_ID || types_of_defined_vars[1] == 404)){
                       is_function++;
                   }else{
                       BSTDisposeLocal(node);
                       error_call(ERR_SEM_EXCOMPAT, L);
                   }
                }else if(!(strcmp(L->Act->tdata.lex, "inputi"))){
                    assign_func_params_control(L, node, "inputi");
                    if(identifier != 2) error_call(ERR_SEM_RETURN, L);
                    if(types_of_defined_vars[0] == t_INT_ID && (types_of_defined_vars[1] == t_INT_ID || types_of_defined_vars[1] == 404)){
                       is_function++;
                    }else{
                       BSTDisposeLocal(node);
                       error_call(ERR_SEM_EXCOMPAT, L);
                    }
                }else if(!(strcmp(L->Act->tdata.lex, "inputf"))){
                    assign_func_params_control(L, node, "inputf");
                    if(identifier != 2) error_call(ERR_SEM_RETURN, L);
                    if(types_of_defined_vars[0] == t_FLOAT64 && (types_of_defined_vars[1] == t_INT_ID || types_of_defined_vars[1] == 404)){
                        is_function++;
                    }else{
                       BSTDisposeLocal(node);
                       error_call(ERR_SEM_EXCOMPAT, L);
                    }
                }else if(!(strcmp(L->Act->tdata.lex, "int2float"))){
                    assign_func_params_control(L, node, "int2float");
                    if(identifier != 1) error_call(ERR_SEM_RETURN, L);
                    if(types_of_defined_vars[0] != t_FLOAT64){
                        if(types_of_defined_vars[1] != 404){
                            BSTDisposeLocal(node);
                            error_call(ERR_SEM_EXCOMPAT, L);
                        }
                    }
                  //  printf("kontrola prebehla v poriadku\n");
                    is_function++;
                }else if(!(strcmp(L->Act->tdata.lex, "float2int"))){
                    assign_func_params_control(L, node, "float2int");
                    if(identifier != 1) error_call(ERR_SEM_RETURN, L);
                    if(types_of_defined_vars[0] != t_INT_ID ){
                        if(types_of_defined_vars[1] != 404){
                            BSTDisposeLocal(node);
                            error_call(ERR_SEM_EXCOMPAT, L);
                        }
                    }
                    is_function++;
                }else if(!(strcmp(L->Act->tdata.lex, "len"))){
                    assign_func_params_control(L, node, "len");
                    if(identifier != 1) error_call(ERR_SEM_RETURN, L);
                    printf("Typ premennej: %d\n", types_of_defined_vars[0]);
                    if(types_of_defined_vars[0] != t_INT_ID ){
                        if( types_of_defined_vars[1] != 404){
                            BSTDisposeLocal(node);
                            error_call(ERR_SEM_EXCOMPAT, L);
                        }
                    }
                    is_function++;
                }else if(!(strcmp(L->Act->tdata.lex, "substr"))){
                    assign_func_params_control(L, node, "substr");
                    if(identifier != 2) error_call(ERR_SEM_RETURN, L);
                    if(types_of_defined_vars[0] == t_STRING_ID && (types_of_defined_vars[1] == t_INT_ID || types_of_defined_vars[1] == 404)){
                         is_function++;
                    }else{
                       BSTDisposeLocal(node);
                       error_call(ERR_SEM_EXCOMPAT, L);
                    }
                }else if(!(strcmp(L->Act->tdata.lex, "ord"))){
                    assign_func_params_control(L, node, "ord");
                    if(identifier != 2) error_call(ERR_SEM_RETURN, L);
                    if(types_of_defined_vars[0] == t_INT_ID && (types_of_defined_vars[1] == t_INT_ID || types_of_defined_vars[1] == 404)){
                        is_function++;
                    }else{
                       BSTDisposeLocal(node);
                       error_call(ERR_SEM_EXCOMPAT, L);
                    }
                }else if(!(strcmp(L->Act->tdata.lex, "chr"))){
                    assign_func_params_control(L, node, "chr");
                    if(identifier != 2) error_call(ERR_SEM_RETURN, L);
                    if(types_of_defined_vars[0] == t_STRING_ID && (types_of_defined_vars[1] == t_INT_ID || types_of_defined_vars[1] == 404)){
                        is_function++;
                    }else{
                       BSTDisposeLocal(node);
                       error_call(ERR_SEM_EXCOMPAT, L);
                    }
                }else if(!(strcmp(L->Act->tdata.lex, "print"))){
                    BSTDisposeLocal(node);
                    error_call(ERR_SEM_EXCOMPAT, L);

                }else if(BSTSearchGlobal(functions, L->Act->tdata.lex, &returned_data)){
                    if(L->Act->rptr->tdata.type == t_LEFT_BRACKET){
                    checkCallFunction(L, L->Act->tdata.lex, node);
                    if(identifier != returned_data.numOfReturns) error_call(ERR_SEM_RETURN, L);
                    else{
                     //  printf("\n\n NAZOV PREMENNEJ %s\n\n\n", L->Act->tdata.lex);
                        int i = 0;
                        while(returned_data.numOfReturns != i){
                            type[i] = returned_data.returns[i];
                        //    printf("\n\n typonn : %d\n\n", type[i]);
                            i++;
                        }

                        for(int i = 0; i < identifier; i++){
                         //   printf("\n\npremenna napravo: %d, premena nalavo: %d\n\n", types_of_defined_vars[i], type[i]);
                            if(types_of_defined_vars[i] != type[i]){
                                if(!(strcmp(id[i], "_"))){

                                }else{
                                    BSTDisposeLocal(node);
                                     error_call(ERR_SEM_EXCOMPAT, L);
                                }
                            }else{
                                is_function++;
                                break;
                            }
                        }
                    }
                    }
                }else if(MainStackSearch(mainstack, name, &datovytyp)) {
                     type[id_insert] = datovytyp;
                            if(datovytyp == t_INT_ID){
                              //  printf("Ide o integer\n");
                                int_count++;
                                id_insert++;
                            }else if(datovytyp == t_FLOAT64){
                                float_count++;
                                id_insert++;
                            }else if(datovytyp == t_STRING_ID){
                                if(L->Act->rptr->tdata.type == t_MINUS || L->Act->rptr->tdata.type == t_DIVIDE || L->Act->rptr->tdata.type == t_MULTIPLY){
                                    BSTDisposeLocal(node);
                                    error_call(ERR_SEM_EXCOMPAT, L);
                                }
                                string_count++;
                                id_insert++;
                            }
                        }else{
                            error_call(ERR_SEM_UNDEF, L);
                            BSTDisposeLocal(node);
                        }
                // ak premenna napravo od =
                // nebola najdena v glob.strome ani v lok.ramci najnizsej urovni
                // potom musime prehladat vyssie ramce
                }else{
                      
                }
            // ak nejde o cislo, string, float alebo premennu
            
            

            if(L->Act->tdata.type == T_SEMICOLON) break;
            // Ak najdes ciarku tzv, chce sa deklarovat viac premennych naraz chyba
              
            // ak sme prisli na ciarku alebo koniec riadku
            if(L->Act->tdata.type == t_COMMA || L->Act->rptr->tdata.type == t_EOL || L->Act->tdata.type == t_BRACES_L){

                //printf("\n\n IDEM PRIRADIT HODNOTU\n\n");
                // ak vo vyraze boli len stringy
                if(string_count != 0 && int_count == 0 && float_count == 0){
                    
                    // je premenna do ktorej chceme ulozit hodnotu deklarovana?
                    //printf("Hlada node zo stringu %s\n\n", id[comma_count]);
                    
                    //printf("Datovy typ napravo %d", types_of_defined_vars[comma_count]);
                    if(types_of_defined_vars[comma_count] == t_STRING_ID || types_of_defined_vars[comma_count] == 404){
                        BSTInsertLocal(node, id[comma_count], type, data);
                        id_insert++;
                        string_count = 0;
                        int_count = 0;
                        float_count = 0;
                    }else{
                        if(types_of_defined_vars[comma_count] == 0){
                            error_call(ERR_SEM_OTHER, L);
                        }else{
                            error_call(ERR_SEM_EXCOMPAT, L);
                        }
                    }
                
                    // ak vo vyraze boli len cisla
                }else if(string_count == 0 && int_count != 0 && float_count == 0){
                    
                   // printf("Hladam node z integer\n\n");
                    // je premenna do ktorej chceme ulozit hodnotu deklarovana?
                    

                   //printf("POROVNAAVAM %d a %d\n\n", types_of_defined_vars[comma_count], t_INT_ID);
                    if(types_of_defined_vars[comma_count] == t_INT_ID || types_of_defined_vars[comma_count] == 404){
                        //BSTInsertLocal(node, id[comma_count], types_of_defined_vars[comma_count], data);
                        id_insert++;
                        string_count = 0;
                        int_count = 0;
                        float_count = 0;
                    }else{
                        if(types_of_defined_vars[comma_count] == 0){
                            error_call(ERR_SEM_OTHER, L);
                        }else{
                            error_call(ERR_SEM_EXCOMPAT, L);
                        }
                    }

                // ak vo vyraze boli len floaty
                }else if(string_count == 0 && int_count == 0 && float_count != 0){
                    
                    // je premenna do ktorej chceme ulozit hodnotu deklarovana?
                    
                    if(types_of_defined_vars[comma_count] == t_FLOAT64 || types_of_defined_vars[comma_count] == 404){
                        BSTInsertLocal(node, id[comma_count], type, data);
                        id_insert++;
                        string_count = 0;
                        int_count = 0;
                        float_count = 0;
                    }else{
                        if(types_of_defined_vars[comma_count] == 0){
                            error_call(ERR_SEM_OTHER, L);
                        }else{
                            error_call(ERR_SEM_EXCOMPAT, L);
                        }
                    }
                }else if(underscore_found == 1){
                    id_insert++;
                    string_count = 0;
                    int_count = 0;
                    float_count = 0;
                }else if(string_count == 0 && int_count == 0 && float_count == 0){
                    
                }else{
                    error_call(ERR_SEM_EXCOMPAT, L);
                }
                comma_count++;
            }
        //printf("Lexem: %s\n", L->Act->tdata.lex);
        TDLLSucc(L);
    }
  //  printf("pocet id %d pocet ciarok %d\n\n", identifier, comma_count);
    // Ak je pocet premennych do ktorych sa ma priradovat ako hodnot rozdielny
    if(is_function == 0){
        if(identifier != comma_count){
            error_call(ERR_SEM_OTHER, L);
            BSTDisposeLocal(node);
        }
    }
 //   printf("uspesne som presiel za assign\n\n");
  //  printf("Posledny lexem %s\n\n", L->Act->tdata.lex);
    //TDLLPred(L);
}   //koniec assign_vals_control

/* ==========================================================================================
 - tato funckia prechadza telom funkcie a analyzuje ju
 - pri najdeni prikazov priradenia, deklaracie, cyklu atd vola funkcie pre kontrolu tychto prikazov
 @param L obojstranne viazany zoznam tokenov
 @param id identifikator(retazec) danej funkcie
 */

void enterFuncBody(TDLList *L, char *id){

    // Lokalny strom pre cely ramec funkcie
    tBSTNodePtrLocal func_node;
	BSTInitLocal(&func_node);
    // Ulozenie lokalneho ramca(lokalneho stromu) na zasobnik
    PushTreeMain(&mainstack, &func_node);
    // pre pracu s parametrami funkcie
    functionData params;
    BSTSearchGlobal(functions, id, &params);
    
    int typ = t_INT_ID;
    BSTInsertLocal(&func_node, "_", &typ, "null");
    
    //ked vojdeme do funkcie, Act je na id, posunieme ho za prvu '{'
    while (strcmp(L->Act->tdata.lex, "{")) {
        TDLLSucc(L);
    }
    TDLLSucc(L);
    int localBraceCount = 1;
    
    //vlozime parametre funkcie id do lokalnej symtable
    //budeme prechadzat telom funckie kym z neho nevyjdeme
    for(int i = 0; i < params.numOfParams; i++){
        //main ma vzdy nula parametrov, bez tohto ifu robilo problemy
        if (!strcmp(id, "main")) {
            break;
        }
        BSTInsertLocal(&func_node, params.params[i], &params.paramsType[i], "null");
    }
    
    while (localBraceCount != 0) {
        //zatvaracia }
        if (!strcmp(L->Act->tdata.lex, "}")) {
            localBraceCount--;
            if (localBraceCount == 0) {
                break;
            }
        }
        //otvaracia {
        if (!strcmp(L->Act->tdata.lex, "{")) {
            localBraceCount++;
        }
        //volanie funkcie
        if ((L->Act->tdata.type == t_IDENTIFIER) && (L->Act->lptr->tdata.type != t_FUNC) && (L->Act->rptr->tdata.type == t_LEFT_BRACKET)) {
            checkCallFunction(L, L->Act->tdata.lex, &func_node);
        }
        //priradenie
        if(L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->tdata.type == t_ASSIGN){
			TDLLElemPtr temp_node = L->Act;
			while(temp_node->tdata.type != t_EOL){
				if(temp_node->lptr->tdata.type == t_EOL){
					assign_vals_control(L, &func_node, params);
					break;
				}
				TDLLPred(L);
				temp_node = temp_node->lptr;
			}
		}
        //deklaracia
        if (L->Act->tdata.type == t_DEFINITION_2) {
            TDLLPred(L);
			dec_var_control(L, &func_node, params);
        }
        //cyklus for
        if (L->Act->tdata.type == t_FOR) {
            enter_for_body(L, &func_node, params, id);
        }
        //podmienka
        if (L->Act->tdata.type == t_IF) {
            TDLLSucc(L);
			enter_if_body(L, &func_node, params, id);
        }
        //return statement
        if (L->Act->tdata.type == t_RETURN) {
            checkReturnStatement(L, id, &func_node);
        }
        //posun v zozname doprava
        TDLLSucc(L);
    }
    // Uvolni lokalny ramec funkcie(lokalny strom) zo zasobniku
    func_node = PopTreeMain(&mainstack);
    BSTDisposeLocal(&func_node);
}   //koniec enterFuncBody




/* ==========================================================================================
 - tato funkcie predstavuje druhy beh cez zoznam tokenov
 - pri najdeni klucoveho slova "func" spusta funkciu pre kontrolu tela najdenej funkcie
 @param L obojsmerne viazany zoznam tokenov
 */
void secondRun(TDLList *L) {
    //aktivita musi ist na zaciatok
    TDLLFirst(L);
    //prehladavaj kym nenajdes funkciu
    while (L->Act->tdata.type != t_EOF) {
        //printf("som vo while\n");
        if (!strcmp(L->Act->tdata.lex, "func")) {
            // Stack pre pracu s ramcami vo funkcii
            // (Re)Inicializacia stacku
            InitMainStack(&mainstack);
            enterFuncBody(L, L->Act->rptr->tdata.lex);
        }
        TDLLSucc(L);
    }
}   //koniec secondRun



/* ==========================================================================================
 - tato funkcia vlozi do globalnej symtable funkcii vstavane funckie
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
}   //koniec insertBuiltInFunction

/* ==========================================================================================
 - tato funckia kontroluje redefiniciu parametrov funkcie
 - ak najde redefiniciu, vola error
 @param data struktura obsahujuca pocet,typy a idenrifikatory parametrov
 @param L obojstranne viazany zoznam tokenov
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
                error_call(ERR_SEM_UNDEF, L);
                return;
            }
        }
    }
}   //koniec paramsRedefinitionCheck


/* ==========================================================================================
 - tato funkcia zavedie do symtable: identifikator funkcie, pocet a typ parametrov, pocet a typ vystupnych typov
 - spusti sa pre najdeni deklaracie funkcie inej ako "main"
 @param L obojstranne viazany zoznam tokenov
 @param id identifikator funkcie
 */
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
    //printFunction(id, data);
}   //koniec checkFunctionParams


/*
 - tato funkcia sa spusta pri kazdom najdeni klucoveho slova "func"
 - kontroluje ci sa nejdena o main(alebo jeho redefiniciu), vtedy vola error
 - spusta analyzu parametrov najdenej funkcie
 @param L obojstranne viazany zoznam tokenov
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
            if ((!strcmp(tmp->tdata.lex, "(")) &&
                (!strcmp(tmp->rptr->tdata.lex, ")")) &&
                (!strcmp(tmp->rptr->rptr->tdata.lex, "(")) &&
                (!strcmp(tmp->rptr->rptr->rptr->tdata.lex, ")")) &&
                (!strcmp(tmp->rptr->rptr->rptr->rptr->tdata.lex, "{"))) {
                mainFound = true;
            } else {
                //main nema spravne parametre, volaj error
                error_call(ERR_SEM_RETURN, L);
                return;
            }
        }
    } else {
        //skontroluj ci identifikator uz neni v symtable
        if (BSTSearchGlobal(functions, id, &data)) {
            error_call(ERR_SEM_UNDEF, L);
        }
        checkFunctionParams(L, id);
    }
}  //koniec checkFunction


/*
 - TATA FUNKCIA SPUSTA SEMANTICKU ANALYZU
 - rozumej ju ako main semantickej analyzy
 - prvy prechod zoznamom tokenov
 -  hlada keyword func a analyzuje deklaracie funkcii
 v druhej faze opat hlada keyword func, pricom uz funkciam vstupuje do tela a vykonava tam semanticku kontrolu
 @param L kompletny zoznam tokenov
 */
void goThroughList(TDLList *L) {
    
    //inicializacia tabulky funkcii
    BSTInitGlobal(&functions);
    insertBuiltInFunction();
    TDLLFirst(L);
    int i = 0, j = 0;      //pre potreby vypiskov
    TDLLPrintAllTokens(L);
    while (L->Act->tdata.type != t_EOF) {
        if (!strcmp(L->Act->tdata.lex, "func")) {
            j++;
            checkFunction(L);
            
        }
        if (L->Act->rptr == NULL) {
            error_call(ERR_INTERN, L);
            return;
        }
        i++;
        TDLLSucc(L);
    }
    //main nebol najdeny, volaj error
    if (!mainFound) {
        error_call(ERR_SEM_UNDEF, L);
        return;
        
    }
    
    TDLLFirst(L);
    
    secondRun(L);
    TDLLPrintAllTokens(L);
    //uvolnime pouzivane struktury
    TDLLDisposeList(L);
    BSTDisposeGlobal(&functions);
    printf("USPESNY KONIEC SEMANTIKY\n");
} // koniec goThroughList
