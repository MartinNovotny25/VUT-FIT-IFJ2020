#include <stdio.h>
#include <stdio.h>

#include "scanner.h"
#include "error.h"
#include "parser.h"

//V stat je ELSE ako break, dalej to mozno bude robit problemy, v buducnosti sa na to este pozriet!!

TOKEN token;
TOKEN last_token;
TOKEN help;
bool no_id_in_params_flag = false; //pri id, id nemoze byt return type pri def_func -- ID je možnost v RULE_TYPE, ak by nebol tento case,
                                    // prítomnost identifikátor medzi NÁVRATOVÝMI TYPMI funckie by bola akceptovaná
                                    // Ak je flag == true, tak ak príde identifikator medzi návratovými typmi, ERROR
bool is_return = false; // flag pre vynutenie return statementu
bool was_return = false; // flag pre zaznacenie ci funkcia obsahovala return

// toto mozte ignorovat, nic to asi nerobi
int global_brace_count = 0; //pocitadlo mnozinovych zatvoriek

void rule_eol();

void def_func();

void rule_params();

void rule_params_n();

void rule_func_retlist_body();

void rule_func_body();

void rule_type();

//PRAVIDLO RETURN_TYPE nie je implementované, bol by to zbytočný medzikrok -- asi by som to mal upraviť v gramatike
void rule_return_type_n();

void rule_return();

void rule_exp_n();

void rule_stat();

void rule_func_assign();

void rule_id_n();

void rule_init_def();

void rule_func_params();

void rule_func_params_n();

void rule_for_def();

void rule_for_assign();

void rule_id_n_or_call_func();

// Zaciatok vykonavania syntaktickej a lexikalnej analýzy
int main() {

    //načitanie prvého tokenu
    token = get_next_token(stdin);

    // eol(y) na zaciatku -- nepovinne
    if (token.type == t_EOL) {
        rule_eol();
    }

    // Test package main
    if (token.type != t_PACKAGE_MAIN) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }


    // Token EOL - Evidentne netreba
    /*if (token.type != t_EOL) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }*/

    // Neterminal <eol> -- nepovinny eol
    if (token.type == t_EOL) {
        rule_eol();
    }

    // Neterminal <def_func> -- Zacne definicia funkcie
    if (token.type != t_FUNC) { error_call(ERR_SYN); }
    else {
        token = get_next_token(stdin);
        def_func();
    }

    //Nemusime vynucovat eol na tomto mieste
    /*if (token.type == t_EOL) {
        printf("som v poslednom rule eol\n");
        rule_eol();
    }*/

    //printf("%d\n", global_brace_count);
    //if (global_brace_count != 0) { error_call(ERR_SYN); }

    printf("USPESNY KONIEC\n");
    return 0;
}

// funkcia pre neterminal <def_func> -- Pravidlo pre správne definovanie funkcie
void def_func() {
    // terminal ID
    if (token.type != t_IDENTIFIER) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }

    // token (
    if (token.type != t_LEFT_BRACKET) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }

    // neterminal <params> -- Vstupne parametre funkcie
    // Zoznam parametrov moze byt prazdny, preto može ist za lavou zatvorkou hned pravá
    if (token.type != t_IDENTIFIER && token.type != t_RIGHT_BRACKET) { error_call(ERR_SYN); }
    else {
        //token = get_next_token(stdin);
        rule_params();
    }

    // Nasleduje typy návratového listu a za ním telo -- preto RETLIST_BODY
    if (token.type != t_LEFT_BRACKET && token.type != t_BRACES_L) { error_call(ERR_SYN); }
    else {
        rule_func_retlist_body();
        // printf("vysiel som z retlist body\n");
    }

    //printf("is_return je %d, was_return je %d\n", is_return, was_return);

    // Ak funckia mala mat return, ale v kode sa nenachadza -- ERROR
    if (was_return == false && is_return == true) {error_call(ERR_SYN);}

    // nastavime flagy na povodnu hodnotu pre dalsie mozne def_func
    was_return = false;
    is_return = false;

    global_brace_count--;

    // Nepovinne EOLy
    if (token.type == t_EOL)
    {
        //printf("som v rule eol na konci\n");
        rule_eol();
    }

    // Ak sa za def func nenachaza ukoncovacia mnozinova zatvorka
    if (token.type != t_BRACES_R) {error_call(ERR_SYN);}
    else {token = get_next_token(stdin);}

    //Nepovinne eoly medzi blokmi
    if (token.type == t_EOL)
    {
        rule_eol();
    }

    printf("som von\n");
    //Ak dôjde k dalsej definicii, znova def_func
    if (token.type == t_FUNC)
    {
        //printf("som v druhom def_func\n");
        token = get_next_token(stdin);
        def_func();
    }

}

// Čo môže nastať
// case t_LEFT_BRACKET - Príde ľavá zátvorka -- skočíme do rule_func_body, kde zistíme, či: 1.) Dôjde ku špecifikácií návratových typov -- return sa vynucuje
//                                                                                          2.) Ďalší token bude ukončovacia zátvorka -- zoznam návratových typov
//                                                                                              bude prázdny, return sa nevynucuje.
//
// case t_BRACES_L - Rovno po zozname vstupnych parametrov príde otváracia množinová zátvorka -- Nemáme návratové typy, return sa nevynucuje
// pokracujeme rovno na telo funckie
//
//


void rule_func_retlist_body() {
    switch (token.type) {
        //REQUIRED_RETURN
        case t_LEFT_BRACKET:
            //printf("Tu pojdem do FUNC_BODY\n");
            token = get_next_token(stdin);

            rule_func_body();

            break;

            //OPTIONAL_RETURN
        case t_BRACES_L:
            global_brace_count++;
            token = get_next_token(stdin);
            if (token.type != t_EOL) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            // Neterminál stat (STATEMENT) -- Vyjadruje možné príkazy v tele funkcie
            rule_stat();

    }
}
// Funkcia pre neterminál rule_func_body
// case t_RIGHT_BRACKET -- Príde ukončovacia zátvorka, zoznam návratových typov je prázdny, return sa nevynucuje
// zvyšné cases -- špecifikácie návratových typov, return sa vynucuje
// v tomto prípade pokracuje neterminál RETURN_TYPE_N pre dalsie možné návratové typy


void rule_func_body() {
    switch (token.type) {
        case t_RIGHT_BRACKET:
            //token = get_next_token(stdin);
            break;

        case t_INT_ID:
            is_return = true;
            token = get_next_token(stdin);
            rule_return_type_n();
            break;
        case t_FLOAT64:
            is_return = true;
            token = get_next_token(stdin);
            rule_return_type_n();
            break;
        case t_STRING_ID:
            is_return = true;
            token = get_next_token(stdin);
            rule_return_type_n();
            break;

    }

    //V prípade, že nám nepríde ukončovacia zátvorka zoznamu návratových typov, ERROR.

    if (token.type != t_RIGHT_BRACKET) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }

    // Token otváracej množinovej zátvorky pre začiatok tela funckie

    if (token.type != t_BRACES_L) { error_call(ERR_SYN); }
    else {
        global_brace_count++;
        token = get_next_token(stdin);
    }

    // Nutný EOL po otváracej množinovej zátvorke
    if (token.type != t_EOL) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }

    //printf("tu pojde do STAT a OPT RET\n");

    // Neterminál EOL pre možné príkazy v tele funckie
    rule_stat();

}

// funkcia pre neterminal stat (STATEMENT) -- Možné príkazy v tele funkcie
void rule_stat() {
   // printf("vosiel som do stat s tokenom %d\n", token.type);


    //switch podla typu tokenu
    switch (token.type) {
        // <stat> -> id ID_N/CALL_FUNC -- ak pride id
        // ak príde ID, môže sa jednat buď o volanie funkcie bez návratových hodnôt (napr. PRINT), alebo môže ísť o priradenie/definíciu
        // voláme rule_id_n_or_call_func na rozhonutie
        case t_IDENTIFIER:
            token = get_next_token(stdin);
            rule_id_n_or_call_func();

            break;

            // ak príde IF
        case t_IF:
            //printf("som v if\n");
            token = get_next_token(stdin);

            //sem pride psa, nacitava znaky vyrazu do zatvorky
            if (token.type == t_IDENTIFIER
                || token.type == t_INT_ZERO
                || token.type == t_INT_NON_ZERO
                || token.type == t_FLOAT64) {
                token = get_next_token(stdin);
            } else { error_call(ERR_SYN); }

            // Otváracia množinová zátvorka pre telo IFu
            if (token.type != t_BRACES_L) {
                //printf("zle braces_L\n");
                error_call(ERR_SYN);
            }
            else {
                global_brace_count++;
                token = get_next_token(stdin);
            }

            //Vynútenie eolu po otváracej množinovej zátvorke
            if (token.type != t_EOL) {
                //printf("NENI EOL\n");
                error_call(ERR_SYN);
            }
            else { token = get_next_token(stdin); }

            // Dalsie optional EOLy
            if (token.type == t_EOL) {
                rule_eol();
            }

            //rekurzivne volanie stat v tele stat if -- dalsi token sa nacita v rule_eol

            rule_stat();

            // Dalsie optional EOLy
            if (token.type == t_EOL) {
                rule_eol();
            }

            // Ukoncovacia množinová zátvorka tela IFu
            if (token.type != t_BRACES_R) {
                //printf("zle braces_R\n");
                error_call(ERR_SYN);
            }
            else {
                global_brace_count--;
                token = get_next_token(stdin);
            }

            //ELSE statement

            // token ELSE
            if (token.type != t_ELSE) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }


            //Otváracia množinová zátvorka tela bloku ELSE
            if (token.type != t_BRACES_L) {
                //printf("zle braces_L\n");
                error_call(ERR_SYN);
            }
            else {
                global_brace_count++;
                token = get_next_token(stdin);
            }

            //Vynútenie EOlu po otváracej zátvorke
            if (token.type != t_EOL) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            // Dalsie optional EOLy
            if (token.type == t_EOL) {
                rule_eol();
            }

            //rekurzivne volanie stat v else vetve
            rule_stat();

            // Dalsie optional EOLy
            if (token.type == t_EOL) {
                rule_eol();
            }

            // ukoncovacia mnozinová zátvorka na konci tela bloku ELSE
            if (token.type != t_BRACES_R) { error_call(ERR_SYN); }
            else {
                global_brace_count--;
                token = get_next_token(stdin);
            }

            // Vynútenie EOLu za zatvorkou
            if (token.type != t_EOL) {
                //printf("NENI EOL ZA ELSE--token je %d\n", token.type);
                error_call(ERR_SYN);
            }
            else { token = get_next_token(stdin); }

            // Dalsie optional EOLy
            if (token.type == t_EOL) {
                rule_eol();
            }

            //volanie stat po skonceni if  pre dalsie možné príkazy v tele funkie
            rule_stat();
            break;

         // token FOR
        case t_FOR:
            token = get_next_token(stdin);


            //rule for_def, ktoré skontroluje definíciu LOKALNEJ ITERACNEJ PREMENNEJ
            //Bodkociarka sa kontroluje v pravidle
            rule_for_def();

           // printf("som von z rule_def\n");

            // Výraz for cyklu
            // SEM PRIDE PCA na vyhodnotenie for vyrazu
            if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
                token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            // bodkociarka pre oddelenie vyrazov v hlavicke for
            if (token.type != T_SEMICOLON) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            // Pravidlo for_assign pre priradenie hodnoty premennej po vykonaní FOR cyklu
            //bodkociarka sa kontroluje v pravidle
            rule_for_assign();

            // token otváracej množinovej zátvorky tela for cyklu
            if (token.type != t_BRACES_L) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            // Vynútenie eolu po zatvorke
            if (token.type != t_EOL) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            // Dalsie optional EOLy
            if (token.type == t_EOL) {
                rule_eol();
            }

            //rekurzivne volanie stat v tele for cyklu

            rule_stat();

            // Dalsie optional EOLy
            if (token.type == t_EOL) {
                rule_eol();
            }

            // token ukončovacej zátvorky tela for cyklu
            if (token.type != t_BRACES_R) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            // Vynútenie EOLu po ukončovacej zátvorke
            if (token.type != t_EOL) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            // Dalsie optional EOLy
            if (token.type == t_EOL) {
                rule_eol();
            }

            // Volanie pravidla stat po vykonaní príkazu FOR
            rule_stat();
            break;

        // Prípad, kedy stat príde EOL -- nepovinný eol medzi príkazmy (STATEMENTAMI)
        case t_EOL:
            //printf("vosiel som do stat_eol\n");
            token = get_next_token(stdin);
            rule_stat();
            break;

            //ak po stat nebude nič a skonči sa blok
        case t_BRACES_R:
            //printf("SOM V BRACES\n");
            break;

        // Ak príde return -- pokracujeme do rule_return a za tym rule_stat
        // TU POZOR -- Syntakticky môže byt return hocikde v kóde, nie je to chyba, avšak parser bude dalej parsovat kód.
        // Generator by ASI mal vediet jak s tym naložiť
        case t_RETURN:
            //token = get_next_token(stdin);
            rule_return();
            //printf("Vysiel som z rule return\n");
            rule_stat();
            break;
         // Pride hocico ine - ERROR
        default:
            error_call(ERR_SYN);
    }
}
//Neterminal ID_N/CALL_FUNC
// case t_COMMA -- Ak príde čiarka, neni to volanie funckie, ale príkaz priradenia do viacerých premenných
// case t_DEFINITION_2 -- Príkaz definicie do jednej premennej
// case t_ASSIGN -- Príkaz priradenia do jednej premennej
// case t_LEFT_BRACKET -- Príde ľavá zátvorka a teda volanie funkcie bez návratového typu
// default - hocico ide, ERROR


void rule_id_n_or_call_func()
{
    switch (token.type)
    {
        case t_COMMA:
            //Kontrola dalších
            rule_id_n();
            //Či príde := alebo =
            rule_init_def();
            // či s bude priradzovan pomocou výrazu alebo volaním funckie
            rule_func_assign();
            rule_stat();
            break;
        case t_DEFINITION_2:
            //Kontrola dalších ID, vieme že neprídu, ale musí to tam byť, nič to nemení
            rule_id_n();
            //Či príde := alebo =, netreba, ale treba to tam pre správne fungovanie
            rule_init_def();
            // či s bude priradzovan pomocou výrazu alebo volaním funckie
            rule_func_assign();
            rule_stat();
            break;

        case t_ASSIGN:
            //Kontrola dalších ID, vieme že neprídu, ale musí to tam byť, nič to nemení
            rule_id_n();
            //Či príde := alebo =, netreba, ale treba to tam pre správne fungovanie
            rule_init_def();
            // či s bude priradzovan pomocou výrazu alebo volaním funckie
            rule_func_assign();
            rule_stat();
            break;

        case t_LEFT_BRACKET:
            token = get_next_token(stdin);
            // Testovanie parametrov funkcie
            rule_func_params();
            // Nepovinné eoly
            rule_eol();
            // Další statement
            rule_stat();
            break;
        default:
            error_call(ERR_SYN);
    }


}

//// TU SA DOPLNI PRECEDENCNA
// funckia pre neterminal func_assign
// Rozhoduje, ci sa priradzuje výrazom alebo volaním funkcie
void rule_func_assign() {
    //V tomto pripade sa pozriem na token dopredu, ak bude lava zatvorka tak funckia, ak nieco ine, vyraz
    //Nesmie byt odriadkovanie
    if (token.type == t_EOL) {error_call(ERR_SYN);}

    // Ak príde identifikátor, neviem si to je výraz alebo volanie funckie, volam dalsi token
    if (token.type == t_IDENTIFIER) {
        token = get_next_token(stdin);

        // Lava zatvorka -- volanie funckie
        if (token.type == t_LEFT_BRACKET) {
            token = get_next_token(stdin);

            // po vyjdeni budem mat nacitany dalsi token
            rule_func_params();

            rule_eol();

            rule_stat();
        }

            // ciarka - dalsi vyraz
        else if (token.type == t_COMMA) {
            token = get_next_token(stdin);
            // Ci za jednym vyrazom pokracuje dalsi -- exp_NEXT
            rule_exp_n();
        }

        // TODO
        // Sem pojde precedencna ak za identifikatorom pojde aritmeticke znamienko
    }

        // ak mi nepride id -- nemusis riesit, ide vyraz, ale nejde lebo ho este netreba
        //TODO
        //tiez dorobit volanie precedencnej

    else if (token.type == t_FLOAT || token.type == t_INT_NON_ZERO || token.type == t_INT_ZERO ||
             token.type == t_STRING) {
        token = get_next_token(stdin);

        // Ci za jednym vyrazom pokracuje dalsi
        rule_exp_n();

        rule_eol();
        rule_stat();
    }
}

// funkcia pre neterminal params
void rule_params() {
    // nastavenie flagu na true
    no_id_in_params_flag = true;
    switch (token.type) {
        case t_IDENTIFIER:
            token = get_next_token(stdin);
            //kontrola, ci je správny typ
            rule_type();
            //Ak nasleduju dalsie parametre
            rule_params_n();
            break;

        // koniec zoznamu ukoncovacou zatvorkou
        case t_RIGHT_BRACKET:
            token = get_next_token(stdin);
            break;
    }
    no_id_in_params_flag = false;

    //naviac
    //token = get_next_token(stdin);
}

// funckia pre netemrinal params_n
void rule_params_n() {
    //ak pride zatvorka -- koniec
    if (token.type == t_RIGHT_BRACKET) {
        token = get_next_token(stdin);
        return;
    }

    // prva musi prist ciarka ako oddelovac dalsieho parametra
    if (token.type != t_COMMA /*&& token.type != t_RIGHT_BRACKET*/) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }

    // kontrola, ci po ciarke pride id
    if (token.type != t_IDENTIFIER) { error_call(ERR_SYN); }
        // ak pride id, pokracujeme typom a nasledne volame params_n znova
    else {
        token = get_next_token(stdin);
        //kontrola, ci je typ premennej správny
        rule_type();
        // rekurzivne volanie params_n
        rule_params_n();
    }
}

//funkcia pre neterminal rule_func_params -- MUSI BYT EXP
// Kontrola návratových typov
void rule_func_params() {
   // printf("FUNC_PARAMS\n");
   //Token pravá zátvorka, koniec
    if (token.type == t_RIGHT_BRACKET) {
        token = get_next_token(stdin);
        return;
    }
    else {
        // Možné typy vo vstupnych parametroch funckie
        if (token.type != t_IDENTIFIER && token.type != t_FLOAT64 && token.type != t_STRING &&
            token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN); }
        token = get_next_token(stdin);
        //rekurzivne func_params_NEXT, bud budu dalsie typy alebo koniec
        rule_func_params_n();
    }
}

void rule_func_params_n() {
    //printf("FUNC_PARAMS_N\n");
    if (token.type == t_RIGHT_BRACKET) { return; }

    else {
        // Za prechádzajúcim typom musí prísť čiarka napr. (10 , ...)
        if (token.type != t_COMMA) { error_call(ERR_SYN); }
        token = get_next_token(stdin);

        if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
            token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN); }
        token = get_next_token(stdin);
        //rekurzivne volanie, bud dalsie typy alebo koniec
        rule_func_params_n();

    }
}

//funckia pre neterminal id_n
// funckia, ktorá skontroluje postupnost identifikátorov, do ktorých sa budú priradzovať hodnoty
void rule_id_n() {
    switch (token.type) {
        // za predchádzajúcim identifikátorom musí prísť čiarka
        case t_COMMA:
            token = get_next_token(stdin);
            //token ID
            if (token.type != t_IDENTIFIER) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            //rekurzivne volanie pre dalsie mozne IDs, ak nepridu, koniec
            rule_id_n();
            break;
            //ak pride =, koniec
        case t_ASSIGN:
            break;
            //ak pride :=, koniec, NEPODPORUJEME, ROZSIRENIE MULTI_VAL
        case t_DEFINITION_2:
            break;
            // hocico ine, ERROR
        default:
            error_call(ERR_SYN);
    }
}

//Pravidlo pre definiciu ITERACNEJ PREMENNEJ v hlavicke for cyklu
void rule_for_def() {
    switch (token.type) {
        case t_IDENTIFIER:
            token = get_next_token(stdin);
            //kontrola, ci pride inicializacia alebo definicia -- Neviem ci je IBA priradenie hodnoty možné, ak áno, iteračná premenná musí byť definovaná dopredu
            rule_init_def();

            //TU PRIDE PSA
            if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
                token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }
            if (token.type != T_SEMICOLON) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            break;
            // Definicia je vynechaná
        case T_SEMICOLON:
            token = get_next_token(stdin);
            break;
        default:
            //hocico ine -- ERROR
            error_call(ERR_SYN);
    }

}
// pravidlo pre PRÍKAZ PRIRADENIA  v hlavičke for cyklu
void rule_for_assign() {
    switch (token.type) {
        case t_IDENTIFIER:
            token = get_next_token(stdin);

            //môže príst iba token assign =
            if (token.type != t_ASSIGN) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            //sem pride psa
            if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
                token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            break;

         // otváracia množinová zátvorka -- začiatok bloku tela for
        case t_BRACES_L:
            break;
        // hocico ine - break
        default:
            error_call(ERR_SYN);
    }
}

//Kontrola, či príde = alebo :=
void rule_init_def() {
    switch (token.type) {
        case t_ASSIGN:
            token = get_next_token(stdin);
            break;
        case t_DEFINITION_2:
            token = get_next_token(stdin);
            break;
        default:
            error_call(ERR_SYN);
    }
}

// funkcia preneterminal <type>
void rule_type() {
    //printf("som v TYPE\n");
    // Priradenie datoveho typu -
    switch (token.type) {
        case t_FLOAT64:
            token = get_next_token(stdin);
            break;
        case t_INT_ID:
            token = get_next_token(stdin);
            break;
        case t_INT_ZERO:
            token = get_next_token(stdin);
            break;
        case t_STRING_ID:
            token = get_next_token(stdin);
            break;
        case t_IDENTIFIER:
            if (no_id_in_params_flag == true) { error_call(ERR_SYN); }
            token = get_next_token(stdin);
            break;
        default:
            error_call(ERR_SYN);
    }
}

// pravidlo pre neterminal optinal return -- VOLAT TOKEN PRED ZAVOLANIM FUNKCIE
void rule_return() {
    //printf("som v rule_return s tokenom %d\n", token.type);
    switch (token.type) {
        //ak príde token RETURN
        case t_RETURN:
            was_return = true; // nastavíme premennú výskytu RETURN na true
            token = get_next_token(stdin);

            // Ak je prítomnost RETURN nutná -- definicia obsahuje návratové typy
            if (is_return == true) {
                //pokial za returnom nenasledujú návratové hodnoty -- ERROR -- kontrolovat ci sú správne robí GENERATOR
                if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
                    token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN); }
                else { token = get_next_token(stdin); }

                // Ak za returnom nasledujú dalsie return výrazy
                rule_exp_n();

                /*if (token.type != t_EOL) { error_call(ERR_SYN); }
                else { token = get_next_token(stdin); }*/

                //mozné EOLy sa returnom
                if (token.type == t_EOL) {
                    rule_eol();
                }

                /*if (token.type != t_BRACES_R) { error_call(ERR_SYN); }
                else { token = get_next_token(stdin); }*/


                // Ak je prítomnosť RETURN nepovinná
            } else if (is_return == false) {
                // Nic nevraciame, cize za RETURNOM musí ísť eol
                if (token.type != t_EOL) { error_call(ERR_SYN); }
                else { token = get_next_token(stdin); }

                // Možné eoly na konci
                if (token.type == t_EOL) {
                    rule_eol();
                }
            }
            break;

            // Ak prídu ukoncovacie zátvorky -- NEMUSI TU BYT ASI, NEVIEM PRECO TO TU JE TBH
        /*case t_BRACES_R:
            // ak treba RETURN toto nemože prisť
            if (is_return == true) {
               // printf("CHYBA RETURN\n");
                error_call(ERR_SYN);
            }
            token = get_next_token(stdin);
            break;*/

        /*case t_EOL: -- ANI TOTO TU ASI NEMUSI BYT, S TAKYM SA SEM ANI NEDOSTANEM
            if (is_return == true) {
               // printf("CHYBA RETURN\n");
                error_call(ERR_SYN);
            }
            token = get_next_token(stdin);
            break;*/

        default:
            error_call(ERR_SYN);

    }
}

// funckia pre neterminal return_type_n
void rule_return_type_n() {
    no_id_in_params_flag = true;
    switch (token.type) {
        case t_COMMA:
            // Musí prís čiarka za predchadzajúcim návratovým typom
            token = get_next_token(stdin);
            rule_type();
            rule_return_type_n();
            break;

            //prava zatvorka, koniec
        case t_RIGHT_BRACKET:
            //token = get_next_token(stdin);
            break;

        default:
            error_call(ERR_SYN);
    }
    no_id_in_params_flag = false;
}

//predtym pride ciarka a zavola sa rule_exp_n a nacita sa token
void rule_exp_n() {
   // printf("EXP_N s tokenom %d\n",token.type);

    //token = get_next_token(stdin);

    if (token.type == t_EOL)
    {
        return;

    } else if (token.type != t_COMMA) {error_call(ERR_SYN);}
    else {
        token = get_next_token(stdin);

        if (token.type == t_EOL) {error_call(ERR_SYN);}

        if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
            token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) {error_call(ERR_SYN); }
        else {token = get_next_token(stdin);}

        rule_exp_n();
    }
}

// funkcie pre neterminal <eol>
void rule_eol() {

    //Zaplata pre PRAVE ZATVORKY
    if (token.type == t_BRACES_R) { return; }

    //printf("som v rule_EOL s tokenom %d\n", token.type);
    token = get_next_token(stdin);

    switch (token.type) {
        case t_EOL:
            rule_eol();
            break;

        default:
            break;
    }
}
