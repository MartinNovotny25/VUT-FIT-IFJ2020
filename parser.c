/*
*  Predmet: Formalne jazyky a prekladace (IFJ) - FIT VUT v Brne
*  Zdrojovy subor syntaktickej analyzy pre projekt IFJ2020
*  Vytvoril: Martin Novotny Mlinarcsik - xnovot1r
*
*  Datum: 10/2020
*/


#include <stdio.h>

#include "tokenList.h"
#include "error.h"
#include "parser.h"
#include "precanalysis.h"
#include "semantics.h"


TDLList tokens;
TDLList psa_list;
TOKEN token;
TOKEN help;
bool no_id_in_params_flag = false; //pri id, id nemoze byt return type pri def_func -- ID je možnost v RULE_TYPE, ak by nebol tento case,
                                    // prítomnost identifikátor medzi NÁVRATOVÝMI TYPMI funckie by bola akceptovaná
                                    // Ak je flag == true, tak ak príde identifikator medzi návratovými typmi, ERROR
bool is_return = false; // flag pre vynutenie return statementu
bool was_return = false; // flag pre zaznacenie ci funkcia obsahovala return
bool between_def = false; //premenna, ktora udava ci sa nachadzame medzi blokmi kodu
int global_brace_count = 0; //pocitadlo mnozinovych zatvoriek




// Zaciatok vykonavania syntaktickej a lexikalnej analýzy
int main() {

    //inicilizacia zoznamu tokenov
    TDLLInitList(&tokens);

    //načitanie prvého tokenu
    token = get_next_token(stdin);
    //nacitanie tokenu do zoznamu tokenov, bude sa volast s kazdym get_next_token
    TDLLInsertLast(&tokens, token);
    // eol(y) na zaciatku -- nepovinne
    if (token.type == t_EOL) {
        rule_eol();
    }

    // Test package main
    if (token.type != t_PACKAGE_MAIN) { error_call(ERR_SYN, &tokens); }
    else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

    // Neterminal <eol> -- nepovinny eol
    if (token.type == t_EOL) {
        rule_eol();
    }

    // Neterminal <def_func> -- Zacne definicia funkcie
    if (token.type != t_FUNC) { error_call(ERR_SYN, &tokens); }
    else {
        token = get_next_token(stdin);
        TDLLInsertLast(&tokens, token);
        def_func();
    }
    goThroughList(&tokens);

    return 0;
}

// funkcia pre neterminal <def_func> -- Pravidlo pre správne definovanie funkcie
void def_func() {
    // terminal ID
    if (token.type != t_IDENTIFIER) { error_call(ERR_SYN, &tokens); }
    else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

    // token (
    if (token.type != t_LEFT_BRACKET) { error_call(ERR_SYN, &tokens); }
    else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

    // neterminal <params> -- Vstupne parametre funkcie
    // Zoznam parametrov moze byt prazdny, preto može ist za lavou zatvorkou hned pravá
    if (token.type != t_IDENTIFIER && token.type != t_RIGHT_BRACKET) { error_call(ERR_SYN, &tokens); }
    else {
        //token = get_next_token(stdin);
        //TDLLInsertLast(&tokens, token);
        rule_params();
    }

    // Nasleduje typy návratového listu a za ním telo -- preto RETLIST_BODY
    if (token.type != t_LEFT_BRACKET && token.type != t_BRACES_L) { error_call(ERR_SYN, &tokens); }
    else {
        rule_func_retlist_body();
    }


    // Ak funckia mala mat return, ale v kode sa nenachadza -- ERROR
    if (was_return == false && is_return == true) {error_call(ERR_SEM_RETURN, &tokens);}

    // nastavime flagy na povodnu hodnotu pre dalsie mozne def_func
    was_return = false;
    is_return = false;

    global_brace_count--;

    // Nepovinne EOLy
    if (token.type == t_EOL)
    {
        rule_eol();
    }

    // Ak sa za def func nenachaza ukoncovacia mnozinova zatvorka
    if (token.type != t_BRACES_R) {error_call(ERR_SYN, &tokens);}
    else {token = get_next_token(stdin); TDLLInsertLast(&tokens, token);}

    //Nepovinne eoly medzi blokmi
    if (token.type == t_EOL)
    {
        between_def = true;
        rule_eol();
        between_def = false;
    }

    //Ak dôjde k dalsej definicii, znova def_func
    if (token.type == t_FUNC)
    {
        token = get_next_token(stdin);
        TDLLInsertLast(&tokens, token);
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
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);

            rule_func_body();

            break;

            //OPTIONAL_RETURN
        case t_BRACES_L:
            global_brace_count++;
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            if (token.type != t_EOL) { error_call(ERR_SYN, &tokens); }
            else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

            // Neterminál stat (STATEMENT) -- Vyjadruje možné príkazy v tele funkcie
            rule_stat();

    }
}

void rule_return_type() {
    switch (token.type) {
        case t_RIGHT_BRACKET:
            //token = get_next_token(stdin);
            //TDLLInsertLast(&tokens, token);
            break;

        case t_INT_ID:
            is_return = true;
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            rule_return_type_n();
            break;
        case t_FLOAT64:
            is_return = true;
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            rule_return_type_n();
            break;
        case t_STRING_ID:
            is_return = true;
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            rule_return_type_n();
            break;
    }

}

// Funkcia pre neterminál rule_func_body
// case t_RIGHT_BRACKET -- Príde ukončovacia zátvorka, zoznam návratových typov je prázdny, return sa nevynucuje
// zvyšné cases -- špecifikácie návratových typov, return sa vynucuje
// v tomto prípade pokracuje neterminál RETURN_TYPE_N pre dalsie možné návratové typy


void rule_func_body() {
    //pravidlo return_type
    rule_return_type();

    //V prípade, že nám nepríde ukončovacia zátvorka zoznamu návratových typov, ERROR.

    if (token.type != t_RIGHT_BRACKET) { error_call(ERR_SYN, &tokens); }
    else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

    // Token otváracej množinovej zátvorky pre začiatok tela funckie

    if (token.type != t_BRACES_L) { error_call(ERR_SYN, &tokens); }
    else {
        global_brace_count++;
        token = get_next_token(stdin);
        TDLLInsertLast(&tokens, token);
    }

    // Nutný EOL po otváracej množinovej zátvorke
    if (token.type != t_EOL) { error_call(ERR_SYN, &tokens); }
    else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }


    // Neterminál EOL pre možné príkazy v tele funckie
    rule_stat();

}

// funkcia pre neterminal stat (STATEMENT) -- Možné príkazy v tele funkcie
void rule_stat() {
    //switch podla typu tokenu
    switch (token.type) {
        // <stat> -> id ID_N/CALL_FUNC -- ak pride id
        // ak príde ID, môže sa jednat buď o volanie funkcie bez návratových hodnôt (napr. PRINT), alebo môže ísť o priradenie/definíciu
        // voláme rule_id_n_or_call_func na rozhonutie
        case t_IDENTIFIER:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            rule_id_n_or_call_func();

            break;

            // ak príde IF
        case t_IF:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);

            // If bez vyrazu
            if (token.type == t_BRACES_L) {error_call(ERR_SYN, &tokens);}

            //Nacitavanie tokenov vyrazu do listu -- PSA
            TDLLInitList(&psa_list);

             while (token.type != t_BRACES_L)
            {
                if ((token.type == t_PLUS)
                    || (token.type == t_MINUS)
                    || (token.type == t_DIVIDE)
                    || (token.type == t_MULTIPLY)
                    || (token.type == t_RIGHT_BRACKET)
                    || (token.type == t_LEFT_BRACKET)
                    || (token.type == t_LESS)
                    || (token.type == t_LESSOREQUAL)
                    || (token.type == t_GREATER)
                    || (token.type == t_GREATEROREQUAL)
                    || (token.type == t_EQUAL)
                    || (token.type == t_NOT_EQUAL)
                    || (token.type == t_IDENTIFIER)
                    || (token.type == t_FLOAT)
                    || (token.type == t_INT_NON_ZERO)
                    || (token.type == t_INT_ZERO)
                    || (token.type == t_STRING)) {

                    TDLLInsertLast(&psa_list, token);
                    token = get_next_token(stdin);
                    TDLLInsertLast(&tokens, token);

                    
                } else {error_call(ERR_SYN, &tokens);}


            }
            // TU SA ZAVOLA PRECEDENCNA S NAPLNENYM LISTOM
            evaluation(&psa_list, &tokens);
             TDLLDisposeList(&psa_list);

            // Otváracia množinová zátvorka pre telo IFu
            if (token.type != t_BRACES_L) {
                error_call(ERR_SYN, &tokens);
            }
            else {
                global_brace_count++;
                token = get_next_token(stdin);
                TDLLInsertLast(&tokens, token);
            }

            //Vynútenie eolu po otváracej množinovej zátvorke
            if (token.type != t_EOL) {
                error_call(ERR_SYN, &tokens);
            }
            else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

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
                error_call(ERR_SYN, &tokens);
            }
            else {
                global_brace_count--;
                token = get_next_token(stdin);
                TDLLInsertLast(&tokens, token);
            }

            //ELSE statement

            // token ELSE
            if (token.type != t_ELSE) { error_call(ERR_SYN, &tokens); }
            else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }


            //Otváracia množinová zátvorka tela bloku ELSE
            if (token.type != t_BRACES_L) {
                error_call(ERR_SYN, &tokens);
            }
            else {
                global_brace_count++;
                token = get_next_token(stdin);
                TDLLInsertLast(&tokens, token);
            }

            //Vynútenie EOlu po otváracej zátvorke
            if (token.type != t_EOL) { error_call(ERR_SYN, &tokens); }
            else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

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
            if (token.type != t_BRACES_R) { error_call(ERR_SYN, &tokens); }
            else {
                global_brace_count--;
                token = get_next_token(stdin);
                TDLLInsertLast(&tokens, token);
            }

            // Vynútenie EOLu za zatvorkou
            if (token.type != t_EOL) {
                error_call(ERR_SYN, &tokens);
            }
            else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

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
            TDLLInsertLast(&tokens, token);


            //rule for_def, ktoré skontroluje definíciu LOKALNEJ ITERACNEJ PREMENNEJ
            //Bodkociarka sa kontroluje v pravidle
            rule_for_def();

            TDLLInitList(&psa_list);

            //for bez výrazu
            if (token.type == T_SEMICOLON) {error_call(ERR_SYN, &tokens);}

            while (token.type != T_SEMICOLON)
            {
                if ((token.type == t_PLUS)
                    || (token.type == t_MINUS)
                    || (token.type == t_DIVIDE)
                    || (token.type == t_MULTIPLY)
                    || (token.type == t_RIGHT_BRACKET)
                    || (token.type == t_LEFT_BRACKET)
                    || (token.type == t_LESS)
                    || (token.type == t_LESSOREQUAL)
                    || (token.type == t_GREATER)
                    || (token.type == t_GREATEROREQUAL)
                    || (token.type == t_EQUAL)
                    || (token.type == t_NOT_EQUAL)
                    || (token.type == t_IDENTIFIER)
                    || (token.type == t_FLOAT)
                    || (token.type == t_INT_NON_ZERO)
                    || (token.type == t_INT_ZERO)
                    || (token.type == t_STRING)) {

                    TDLLInsertLast(&psa_list, token);
                    token = get_next_token(stdin); TDLLInsertLast(&tokens, token);

                } else {error_call(ERR_SYN, &tokens);}


            }
            evaluation(&psa_list, &tokens);
            TDLLDisposeList(&psa_list);
            token = get_next_token(stdin); TDLLInsertLast(&tokens, token);



            // Pravidlo for_assign pre priradenie hodnoty premennej po vykonaní FOR cyklu
            //bodkociarka sa kontroluje v pravidle
            rule_for_assign();

            // token otváracej množinovej zátvorky tela for cyklu
            if (token.type != t_BRACES_L) { error_call(ERR_SYN, &tokens); }
            else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

            // Vynútenie eolu po zatvorke
            if (token.type != t_EOL) { error_call(ERR_SYN, &tokens); }
            else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

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
            if (token.type != t_BRACES_R) { error_call(ERR_SYN, &tokens); }
            else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

            // Vynútenie EOLu po ukončovacej zátvorke
            if (token.type != t_EOL) { error_call(ERR_SYN, &tokens); }
            else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

            // Dalsie optional EOLy
            if (token.type == t_EOL) {
                rule_eol();
            }

            // Volanie pravidla stat po vykonaní príkazu FOR
            rule_stat();
            break;

        // Prípad, kedy stat príde EOL -- nepovinný eol medzi príkazmy (STATEMENTAMI)
        case t_EOL:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            rule_stat();
            break;

            //ak po stat nebude nič a skonči sa blok
        case t_BRACES_R:
            break;

        // Ak príde return -- pokracujeme do rule_return a za tym rule_stat
        // TU POZOR -- Syntakticky môže byt return hocikde v kóde, nie je to chyba, avšak parser bude dalej parsovat kód.
        // Generator by ASI mal vediet jak s tym naložiť
        case t_RETURN:
            rule_return();
            rule_stat();
            break;
         // Pride hocico ine - ERROR
        default:
            error_call(ERR_SYN, &tokens);
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
            TDLLInsertLast(&tokens, token);
            // Testovanie parametrov funkcie
            rule_func_params();
            // Nepovinné eoly
            rule_eol();
            // Další statement
            rule_stat();
            break;
        default:
            error_call(ERR_SYN, &tokens);
    }


}

// funckia pre neterminal func_assign
// Rozhoduje, ci sa priradzuje výrazom alebo volaním funkcie
void rule_func_assign() {
    //V tomto pripade sa pozriem na token dopredu, ak bude lava zatvorka tak funckia, ak nieco ine, vyraz
    //Nesmie byt odriadkovanie
    if (token.type == t_EOL) { error_call(ERR_SYN, &tokens); }

    // Ak príde identifikátor, neviem si to je výraz alebo volanie funckie, volam dalsi token
    if (token.type == t_IDENTIFIER) {
        //PARSER -- tu si ulozim token cisto pre vkladanie do psa_listu
        help = token;

        token = get_next_token(stdin);
        TDLLInsertLast(&tokens, token);

        // Lava zatvorka -- volanie funckie
        if (token.type == t_LEFT_BRACKET) {
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);

            // po vyjdeni budem mat nacitany dalsi token
            rule_func_params();

            rule_eol();

            rule_stat();
        }

            // ciarka - dalsi vyraz
        else if (token.type == t_COMMA) {
            // Ci za jednym vyrazom pokracuje dalsi -- exp_NEXT
            rule_exp_n();

            rule_eol();
            rule_stat();
        }


            // Sem pojde precedencna ak za identifikatorom pojde aritmeticke znamienko
        else if ((token.type == t_PLUS)
                 || (token.type == t_MINUS)
                 || (token.type == t_DIVIDE)
                 || (token.type == t_MULTIPLY)) {
            TDLLInitList(&psa_list);
            // v help mam ulozeny identifikator, kedze je sucastou vyrazu a v token uz je nacitane znamienko
            TDLLInsertLast(&psa_list, help);
            TDLLInsertLast(&psa_list, token);
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);

            //Nacitanie dalsich znakov, koncim ked mi pride ciarka alebo EOL

            while (token.type != t_COMMA && token.type != t_EOL) {
                if ((token.type == t_PLUS)
                    || (token.type == t_MINUS)
                    || (token.type == t_DIVIDE)
                    || (token.type == t_MULTIPLY)
                    || (token.type == t_RIGHT_BRACKET)
                    || (token.type == t_LEFT_BRACKET)
                    || (token.type == t_LESS)
                    || (token.type == t_LESSOREQUAL)
                    || (token.type == t_GREATER)
                    || (token.type == t_GREATEROREQUAL)
                    || (token.type == t_EQUAL)
                    || (token.type == t_NOT_EQUAL)
                    || (token.type == t_IDENTIFIER)
                    || (token.type == t_FLOAT)
                    || (token.type == t_INT_NON_ZERO)
                    || (token.type == t_INT_ZERO)
                    || (token.type == t_STRING)) {

                    TDLLInsertLast(&psa_list, token);
                    token = get_next_token(stdin);
                    TDLLInsertLast(&tokens, token);

                } else { error_call(ERR_SYN, &tokens); }

            }


            //Ak precedencna analyza vrati 1 -- ERROR
            evaluation(&psa_list, &tokens);
            TDLLDisposeList(&psa_list);

            // Pride mi ciarka alebo eol, musim zavolat exp_n ci za tym nieco ide
            rule_exp_n();
            rule_eol();
            rule_stat();

        }
    }

        // ak mi nepride id -- nemusis riesit, ide vyraz
    else if (token.type == t_FLOAT || token.type == t_INT_NON_ZERO || token.type == t_INT_ZERO ||
             token.type == t_STRING || token.type == t_LEFT_BRACKET) {

        TDLLInitList(&psa_list);
        TDLLInsertLast(&psa_list, token);

        token = get_next_token(stdin);
        TDLLInsertLast(&tokens, token);

        //nacitavam, kym nepride ciarka alebo eol -- koneic vyrazu
        while (token.type != t_COMMA && token.type != t_EOL) {
            if ((token.type == t_PLUS)
                || (token.type == t_MINUS)
                || (token.type == t_DIVIDE)
                || (token.type == t_MULTIPLY)
                || (token.type == t_RIGHT_BRACKET)
                || (token.type == t_LEFT_BRACKET)
                || (token.type == t_LESS)
                || (token.type == t_LESSOREQUAL)
                || (token.type == t_GREATER)
                || (token.type == t_GREATEROREQUAL)
                || (token.type == t_EQUAL)
                || (token.type == t_NOT_EQUAL)
                || (token.type == t_IDENTIFIER)
                || (token.type == t_FLOAT)
                || (token.type == t_INT_NON_ZERO)
                || (token.type == t_INT_ZERO)
                || (token.type == t_STRING)) {

                TDLLInsertLast(&psa_list, token);
                token = get_next_token(stdin);
                TDLLInsertLast(&tokens, token);

            } else {
                error_call(ERR_SYN, &tokens);
            }
        }

        evaluation(&psa_list, &tokens);
        TDLLDisposeList(&psa_list);

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
            TDLLInsertLast(&tokens, token);
            //kontrola, ci je správny typ
            rule_type();
            //Ak nasleduju dalsie parametre
            rule_params_n();
            break;

        // koniec zoznamu ukoncovacou zatvorkou
        case t_RIGHT_BRACKET:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            break;
    }
    no_id_in_params_flag = false;
}

// funckia pre netemrinal params_n
void rule_params_n() {
    //ak pride zatvorka -- koniec
    if (token.type == t_RIGHT_BRACKET) {
        token = get_next_token(stdin);
        TDLLInsertLast(&tokens, token);
        return;
    }

    // prva musi prist ciarka ako oddelovac dalsieho parametra
    if (token.type != t_COMMA) { error_call(ERR_SYN, &tokens); }
    else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

    // kontrola, ci po ciarke pride id
    if (token.type != t_IDENTIFIER) { error_call(ERR_SYN, &tokens); }
        // ak pride id, pokracujeme typom a nasledne volame params_n znova
    else {
        token = get_next_token(stdin);
        TDLLInsertLast(&tokens, token);
        //kontrola, ci je typ premennej správny
        rule_type();
        // rekurzivne volanie params_n
        rule_params_n();
    }
}

//funkcia pre neterminal rule_func_params -- MUSI BYT EXP
// Kontrola návratových typov
void rule_func_params() {
   //Token pravá zátvorka, koniec
    if (token.type == t_RIGHT_BRACKET) {
        token = get_next_token(stdin);
        TDLLInsertLast(&tokens, token);
        return;
    }
    else {
        // Možné typy vo vstupnych parametroch funckie
        if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
            token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN, &tokens); }
        token = get_next_token(stdin);
        TDLLInsertLast(&tokens, token);
        //rekurzivne func_params_NEXT, bud budu dalsie typy alebo koniec
        rule_func_params_n();
    }
}

void rule_func_params_n() {
    if (token.type == t_RIGHT_BRACKET) { return; }

    else {
        // Za prechádzajúcim typom musí prísť čiarka napr. (10 , ...)
        if (token.type != t_COMMA) { error_call(ERR_SYN, &tokens); }
        token = get_next_token(stdin);
        TDLLInsertLast(&tokens, token);

        if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
            token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN, &tokens); }
        token = get_next_token(stdin);
        TDLLInsertLast(&tokens, token);
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
            TDLLInsertLast(&tokens, token);
            //token ID
            if (token.type != t_IDENTIFIER) { error_call(ERR_SYN, &tokens); }
            else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }


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
            error_call(ERR_SYN, &tokens);
    }
}

//Pravidlo pre definiciu ITERACNEJ PREMENNEJ v hlavicke for cyklu
void rule_for_def() {
    switch (token.type) {
        case t_IDENTIFIER:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            //kontrola, ci pride inicializacia alebo definicia -- Neviem ci je IBA priradenie hodnoty možné, ak áno, iteračná premenná musí byť definovaná dopredu
            if (token.type != t_DEFINITION_2) {error_call(ERR_SYN, &tokens);}
            else {token = get_next_token(stdin);  TDLLInsertLast(&tokens, token);}

            TDLLInitList(&psa_list);

            while (token.type != T_SEMICOLON) {
                if ((token.type == t_PLUS)
                    || (token.type == t_MINUS)
                    || (token.type == t_DIVIDE)
                    || (token.type == t_MULTIPLY)
                    || (token.type == t_RIGHT_BRACKET)
                    || (token.type == t_LEFT_BRACKET)
                    || (token.type == t_LESS)
                    || (token.type == t_LESSOREQUAL)
                    || (token.type == t_GREATER)
                    || (token.type == t_GREATEROREQUAL)
                    || (token.type == t_EQUAL)
                    || (token.type == t_NOT_EQUAL)
                    || (token.type == t_IDENTIFIER)
                    || (token.type == t_FLOAT)
                    || (token.type == t_INT_NON_ZERO)
                    || (token.type == t_INT_ZERO)
                    || (token.type == t_STRING)) {

                    TDLLInsertLast(&psa_list, token);
                    token = get_next_token(stdin); TDLLInsertLast(&tokens, token);
                    

                } else { error_call(ERR_SYN, &tokens); }
            }

            evaluation(&psa_list, &tokens);
            TDLLDisposeList(&psa_list);
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            break;
            // Definicia je vynechaná
        case T_SEMICOLON:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            break;
        default:
            //hocico ine -- ERROR
            error_call(ERR_SYN, &tokens);
    }

}
// pravidlo pre PRÍKAZ PRIRADENIA  v hlavičke for cyklu
void rule_for_assign() {
    switch (token.type) {
        case t_IDENTIFIER:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);

            rule_id_n();

            //môže príst iba token assign =
            if (token.type != t_ASSIGN) { error_call(ERR_SYN, &tokens); }
            else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

            TDLLInitList(&psa_list);

            if (token.type == t_BRACES_L) {error_call(ERR_SYN, &tokens);}

            while (token.type != t_BRACES_L && token.type != t_COMMA)
            {
                if ((token.type == t_PLUS)
                    || (token.type == t_MINUS)
                    || (token.type == t_DIVIDE)
                    || (token.type == t_MULTIPLY)
                    || (token.type == t_RIGHT_BRACKET)
                    || (token.type == t_LEFT_BRACKET)
                    || (token.type == t_LESS)
                    || (token.type == t_LESSOREQUAL)
                    || (token.type == t_GREATER)
                    || (token.type == t_GREATEROREQUAL)
                    || (token.type == t_EQUAL)
                    || (token.type == t_NOT_EQUAL)
                    || (token.type == t_IDENTIFIER)
                    || (token.type == t_FLOAT)
                    || (token.type == t_INT_NON_ZERO)
                    || (token.type == t_INT_ZERO)
                    || (token.type == t_STRING)) {

                    TDLLInsertLast(&psa_list, token);
                    token = get_next_token(stdin); TDLLInsertLast(&tokens, token);

                } else {error_call(ERR_SYN, &tokens);}


            }
            evaluation(&psa_list, &tokens);
            TDLLDisposeList(&psa_list);

            rule_exp_n();
            break;

         // otváracia množinová zátvorka -- začiatok bloku tela for
        case t_BRACES_L:
            break;
        // hocico ine - break
        default:
            error_call(ERR_SYN, &tokens);
    }
}

//Kontrola, či príde = alebo :=
void rule_init_def() {
    switch (token.type) {
        case t_ASSIGN:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            break;
        case t_DEFINITION_2:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            break;
        default:
            error_call(ERR_SYN, &tokens);
    }
}

// funkcia preneterminal <type>
void rule_type() {
    // Priradenie datoveho typu -
    switch (token.type) {
        case t_FLOAT64:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            break;
        case t_INT_ID:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            break;
        case t_INT_ZERO:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            break;
        case t_STRING_ID:
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            break;
        case t_IDENTIFIER:
            if (no_id_in_params_flag == true) { error_call(ERR_SYN, &tokens); }
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            break;
        default:
            error_call(ERR_SYN, &tokens);
    }
}

// pravidlo pre neterminal optinal return -- VOLAT TOKEN PRED ZAVOLANIM FUNKCIE
void rule_return() {
    switch (token.type) {
        //ak príde token RETURN
        case t_RETURN:
            was_return = true; // nastavíme premennú výskytu RETURN na true
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);

            // Ak je prítomnost RETURN nutná -- definicia obsahuje návratové typy // PRAVIDLO REQUIRED RETURN
            if (is_return == true) {
                required_return();
                // Ak je prítomnosť RETURN nepovinná // PRAVIDLO OPTIONAL RETURN
            } else if (is_return == false) {
                optional_return();
            }
            break;

        default:
            error_call(ERR_SYN, &tokens);

    }
}

// funckia pre neterminal return_type_n
void rule_return_type_n() {
    no_id_in_params_flag = true;
    switch (token.type) {
        case t_COMMA:
            // Musí prís čiarka za predchadzajúcim návratovým typom
            token = get_next_token(stdin);
            TDLLInsertLast(&tokens, token);
            rule_type();
            rule_return_type_n();
            break;

            //prava zatvorka, koniec
        case t_RIGHT_BRACKET:
            break;

        default:
            error_call(ERR_SYN, &tokens);
    }
    no_id_in_params_flag = false;
}

//predtym pride ciarka a zavola sa rule_exp_n a nacita sa token
void rule_exp_n() {

    if (token.type == t_EOL)
    {
        return;

    } else if (token.type == t_BRACES_L) {
        return;
    } else if (token.type != t_COMMA) {error_call(ERR_SYN, &tokens);}
    else {
        token = get_next_token(stdin);
        TDLLInsertLast(&tokens, token);

        TDLLInitList(&psa_list);

        if (token.type == t_EOL) {error_call(ERR_SYN, &tokens);}
        else {

            while (token.type != t_COMMA && token.type != t_EOL && token.type != t_BRACES_L) {
                if ((token.type == t_PLUS)
                    || (token.type == t_MINUS)
                    || (token.type == t_DIVIDE)
                    || (token.type == t_MULTIPLY)
                    || (token.type == t_RIGHT_BRACKET)
                    || (token.type == t_LEFT_BRACKET)
                    || (token.type == t_LESS)
                    || (token.type == t_LESSOREQUAL)
                    || (token.type == t_GREATER)
                    || (token.type == t_GREATEROREQUAL)
                    || (token.type == t_EQUAL)
                    || (token.type == t_NOT_EQUAL)
                    || (token.type == t_IDENTIFIER)
                    || (token.type == t_FLOAT)
                    || (token.type == t_INT_NON_ZERO)
                    || (token.type == t_INT_ZERO)
                    || (token.type == t_STRING)) {

                    TDLLInsertLast(&psa_list, token);
                    token = get_next_token(stdin); TDLLInsertLast(&tokens, token);

                } else { error_call(ERR_SYN, &tokens); }
            }

            evaluation(&psa_list, &tokens);
            TDLLDisposeList(&psa_list);


            rule_exp_n();
        }
    }
}

// funkcie pre neterminal <eol>
void rule_eol() {

    //Zaplata pre PRAVE ZATVORKY
    if (token.type == t_BRACES_R) { return; }

    token = get_next_token(stdin);
    TDLLInsertLast(&tokens, token);

    switch (token.type) {
        case t_EOL:
            rule_eol();
            break;

        default:
            if (between_def == true) {
                if (token.type != t_FUNC && token.type != t_EOF) {
                    error_call(ERR_SYN, &tokens);
                }
            }
            break;
    }
}

void optional_return(){

    // Nic nevraciame, cize za RETURNOM musí ísť eol
    if (token.type != t_EOL) { error_call(ERR_SEM_RETURN, &tokens); }
    else { token = get_next_token(stdin); TDLLInsertLast(&tokens, token); }

    // Možné eoly na konci
    if (token.type == t_EOL) {
        rule_eol();
    }
}



void required_return() {
    TDLLInitList(&psa_list);

    if (token.type == t_EOL) {error_call(ERR_SEM_RETURN, &tokens);}
    else {

        while (token.type != t_COMMA && token.type != t_EOL) {
            if ((token.type == t_PLUS)
                || (token.type == t_MINUS)
                || (token.type == t_DIVIDE)
                || (token.type == t_MULTIPLY)
                || (token.type == t_RIGHT_BRACKET)
                || (token.type == t_LEFT_BRACKET)
                || (token.type == t_LESS)
                || (token.type == t_LESSOREQUAL)
                || (token.type == t_GREATER)
                || (token.type == t_GREATEROREQUAL)
                || (token.type == t_EQUAL)
                || (token.type == t_NOT_EQUAL)
                || (token.type == t_IDENTIFIER)
                || (token.type == t_FLOAT)
                || (token.type == t_INT_NON_ZERO)
                || (token.type == t_INT_ZERO)
                || (token.type == t_STRING)){

                TDLLInsertLast(&psa_list, token);
                token = get_next_token(stdin);
                TDLLInsertLast(&tokens, token);

            } else { error_call(ERR_SYN, &tokens); }
        }

        evaluation(&psa_list, &tokens);
        TDLLDisposeList(&psa_list);

    }


    // Ak za returnom nasledujú dalsie return výrazy
    rule_exp_n();



    //mozné EOLy sa returnom
    if (token.type == t_EOL) {
        rule_eol();
    }

}
