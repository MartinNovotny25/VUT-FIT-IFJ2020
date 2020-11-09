#include <stdio.h>
#include <stdio.h>

#include "scanner.h"
#include "error.h"

//V stat je ELSE ako break, dalej to mozno bude robit problemy, v buducnosti sa na to este pozriet!!

TOKEN token;
TOKEN last_token;
TOKEN help_token;
bool no_id_in_params_flag = false; //pri id, id nemoze byt return type pri def_func
bool is_return = false; // flag pre vynutenie return statementu
bool return_happened = false; // pomocny bool pre potrebu testovania RETURNOV vymazat az bude stat

int global_brace_count = 0; //pocitadlo mnozinovych zatvoriek

void rule_eol();

void def_func();

void rule_params();

void rule_params_n();

void rule_func_retlist_body();

void rule_func_body();

void rule_type();

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

int main() {

    token = get_next_token(stdin);

    // eol(y) na zaciatku
    if (token.type == t_EOL) {
        rule_eol();
    }

    // Test package main
    if (token.type != t_PACKAGE_MAIN) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }


    // Token EOL
    if (token.type != t_EOL) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }

    // Neterminal <eol>
    if (token.type == t_EOL) {
        rule_eol();
    }

    // Neterminal <def_func>
    if (token.type != t_FUNC) { error_call(ERR_SYN); }
    else {
        token = get_next_token(stdin);
        def_func();
    }

    //printf("%d\n", global_brace_count);
    if (global_brace_count != 0) { error_call(ERR_SYN); }

    printf("USPESNY KONIEC\n");
    return 0;
}

// funkcia pre neterminal <def_func>
void def_func() {
    // terminal ID
    if (token.type != t_IDENTIFIER) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }

    // token (
    if (token.type != t_LEFT_BRACKET) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }

    // neterminal <params>
    if (token.type != t_IDENTIFIER && token.type != t_RIGHT_BRACKET) { error_call(ERR_SYN); }
    else {
        //token = get_next_token(stdin);
        rule_params();
    }

    if (token.type != t_LEFT_BRACKET && token.type != t_BRACES_L) { error_call(ERR_SYN); }
    else {
        rule_func_retlist_body();
        // printf("vysiel som z retlist body\n");
    }


    //VYPIS NEIMPLEMENTOVANYCH TERMINALOV
    /*while (token.type != t_BRACES_R && token.type != t_EOF) {token = get_next_token(stdin);}*/
    global_brace_count--;
}

void rule_func_retlist_body() {
    switch (token.type) {
        //REQUIRED_RETURN
        case t_LEFT_BRACKET:
            //printf("Tu pojdem do FUNC_BODY\n");
            token = get_next_token(stdin);

            //PREROBIT NA IF AZ BUDE FUNC BODY DONE
            rule_func_body();

            break;

            //OPTIONAL_RETURN
        case t_BRACES_L:
            global_brace_count++;
            token = get_next_token(stdin);
            if (token.type != t_EOL) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

           // printf("tu budem pokracovat do STAT a OPTIONAL RETkokot\n");

            rule_stat();
           // printf("SOM PRED RULE_RETURN\n");
            rule_return();

            //printf("vysiel som zo stat\n");

            //ODKOMENTOVAT az dokoncim stat a opt ret
            /*if (token.type != t_BRACES_R) {error_call(ERR_SYN);}
                else {token = get_next_token(stdin);}*/

    }
}

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


    if (token.type != t_RIGHT_BRACKET) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }

    if (token.type != t_BRACES_L) { error_call(ERR_SYN); }
    else {
        global_brace_count++;
        token = get_next_token(stdin);
    }

    if (token.type != t_EOL) { error_call(ERR_SYN); }
    else { token = get_next_token(stdin); }

    //printf("tu pojde do STAT a OPT RET\n");

    rule_stat();

    //printf("SOM PRED RULE_RETURN\n");
    rule_return();
}

// funkcia pre neterminal stat
void rule_stat() {
   // printf("vosiel som do stat s tokenom %d\n", token.type);
    //switch podla typu tokenu
    switch (token.type) {
        // <stat> -> id CALL_FUNC/ASSIGN -- ak pride id
        case t_IDENTIFIER:
            token = get_next_token(stdin);
            rule_id_n_or_call_func();

            break;

            // <stat> ->
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

            if (token.type != t_BRACES_L) {
                //printf("zle braces_L\n");
                error_call(ERR_SYN);
            }
            else {
                global_brace_count++;
                token = get_next_token(stdin);
            }


            if (token.type != t_EOL) {
                //printf("NENI EOL\n");
                error_call(ERR_SYN);
            }
            else { token = get_next_token(stdin); }


            if (token.type == t_EOL) {
                rule_eol();
            }

            //rekurzivne volanie stat v tele stat if -- dalsi token sa nacita v rule_eol

            rule_stat();

            if (token.type == t_EOL) {
                rule_eol();
            }

            if (token.type != t_BRACES_R) {
                //printf("zle braces_R\n");
                error_call(ERR_SYN);
            }
            else {
                global_brace_count--;
                token = get_next_token(stdin);
            }

            //ELSE


            if (token.type != t_ELSE) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            if (token.type != t_BRACES_L) {
                printf("zle braces_L\n");
                error_call(ERR_SYN);
            }
            else {
                global_brace_count++;
                token = get_next_token(stdin);
            }


            if (token.type != t_EOL) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            if (token.type == t_EOL) {
                rule_eol();
            }

            //rekurzivnevolanie stat v else vetve
            rule_stat();


            if (token.type == t_EOL) {
                rule_eol();
            }

            if (token.type != t_BRACES_R) { error_call(ERR_SYN); }
            else {
                global_brace_count--;
                token = get_next_token(stdin);
            }

            if (token.type != t_EOL) {
                //printf("NENI EOL ZA ELSE--token je %d\n", token.type);
                error_call(ERR_SYN);
            }
            else { token = get_next_token(stdin); }

            if (token.type == t_EOL) {
                rule_eol();
            }

            //volanie stat po skonceni if
            rule_stat();
            break;

        case t_FOR:
            token = get_next_token(stdin);

            rule_for_def();

           // printf("som von z rule_def\n");

            // SEM PRIDE PCA na vyhodnotenie for vyrazu
            if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
                token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            if (token.type != T_SEMICOLON) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            rule_for_assign();

            if (token.type != t_BRACES_L) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            if (token.type != t_EOL) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            if (token.type == t_EOL) {
                rule_eol();
            }

            //rekurzivne volanie stat v tele for cyklu

            rule_stat();

            if (token.type == t_EOL) {
                rule_eol();
            }

            if (token.type != t_BRACES_R) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            if (token.type != t_EOL) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            if (token.type == t_EOL) {
                rule_eol();
            }

            rule_stat();
            break;

        case t_EOL:
            //printf("vosiel som do stat_eol\n");
            token = get_next_token(stdin);
            rule_stat();
            break;

            //ak po stat nebude nič a skonči sa blok
        case t_BRACES_R:
            //printf("SOM V BRACES\n");
            break;
        case t_RETURN:
            //printf("SOM V ELSE\n");
            //token = get_next_token(stdin);
            break;

        default:
            error_call(ERR_SYN);
    }
}

void rule_id_n_or_call_func()
{
    switch (token.type)
    {
        case t_COMMA:
            rule_id_n();
            rule_init_def();
            rule_func_assign();
            rule_stat();
            break;
        case t_DEFINITION_2:
            rule_id_n();
            rule_init_def();
            rule_func_assign();
            rule_stat();
            break;

        case t_ASSIGN:
            rule_id_n();
            rule_init_def();
            rule_func_assign();
            rule_stat();
            break;

        case t_LEFT_BRACKET:
            token = get_next_token(stdin);
            rule_func_params();
            rule_eol();
            rule_stat();
            break;
        default:
            error_call(ERR_SYN);
    }


}


// funckia pre neterminal func_assign spolu s neterminalom OPT_ID
void rule_func_assign() {
    //printf("FUNC_ASSIGN\n");
    //V tomto pripade sa pozriem na token dopredu, ak bude lava zatvorka tak funckia, ak nieco ine, vyraz
    // Just in case do help_token ulozin ID
    if (token.type == t_IDENTIFIER) {
        token = get_next_token(stdin);

        if (token.type == t_LEFT_BRACKET) {
            token = get_next_token(stdin);
            // po vyjdeni budem mat nacitany dalsi token
            rule_func_params();

            //printf("TOKEN PRED RULE EOL JE %d\n", token.type);

            rule_eol();

            rule_stat();
        }

            // ciarka - dalsi vyraz
        else if (token.type == t_COMMA) {
            token = get_next_token(stdin);
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

        rule_exp_n();

        rule_eol();
        rule_stat();
    }
}

// funkcia pre neterminal params
void rule_params() {
    no_id_in_params_flag = true;
    switch (token.type) {
        case t_IDENTIFIER:
            token = get_next_token(stdin);
            rule_type();
            rule_params_n();
            break;
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
    //ak pride zatvorka -- koniec (OTESTOVAT CI POTREBNE)
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
        rule_type();
        rule_params_n();
    }
}

//funkcia pre neterminal rule_func_params -- MUSI BYT EXP
void rule_func_params() {
   // printf("FUNC_PARAMS\n");
    if (token.type == t_RIGHT_BRACKET) {
        token = get_next_token(stdin);
        return;
    }
    else {
        if (token.type != t_IDENTIFIER && token.type != t_FLOAT64 && token.type != t_STRING &&
            token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN); }
        token = get_next_token(stdin);
        rule_func_params_n();
    }
}

void rule_func_params_n() {
    //printf("FUNC_PARAMS_N\n");
    if (token.type == t_RIGHT_BRACKET) { return; }

    else {
        if (token.type != t_COMMA) { error_call(ERR_SYN); }
        token = get_next_token(stdin);
        if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
            token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN); }
        token = get_next_token(stdin);
        rule_func_params_n();

    }
}

//funckia pre neterminal id_n
void rule_id_n() {
    switch (token.type) {
        case t_COMMA:
            token = get_next_token(stdin);
            if (token.type != t_IDENTIFIER) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            rule_id_n();
            break;
        case t_ASSIGN:
            break;
        case t_DEFINITION_2:
            break;
        default:
            error_call(ERR_SYN);
    }
}

void rule_for_def() {
    switch (token.type) {
        case t_IDENTIFIER:
            token = get_next_token(stdin);
            rule_init_def();

            //TU PRIDE PSA
            if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
                token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }
            if (token.type != T_SEMICOLON) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            break;
        case T_SEMICOLON:
            token = get_next_token(stdin);
            break;
        default:
            error_call(ERR_SYN);
    }

}

void rule_for_assign() {
    switch (token.type) {
        case t_IDENTIFIER:
            token = get_next_token(stdin);

            if (token.type != t_ASSIGN) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
                token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN); }
            else { token = get_next_token(stdin); }

            break;

        case t_BRACES_L:
            break;
        default:
            error_call(ERR_SYN);
    }
}

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
        case t_STRING:
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
   // printf("som v rule_return s tokenom %d\n", token.type);
    switch (token.type) {
        case t_RETURN:
            token = get_next_token(stdin);

            if (is_return == 1) {
                if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
                    token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) { error_call(ERR_SYN); }
                else { token = get_next_token(stdin); }

                rule_exp_n();

                /*if (token.type != t_EOL) { error_call(ERR_SYN); }
                else { token = get_next_token(stdin); }*/

                if (token.type == t_EOL) {
                    rule_eol();
                }

                if (token.type != t_BRACES_R) { error_call(ERR_SYN); }
                else { token = get_next_token(stdin); }


            } else if (is_return == 0) {
                if (token.type != t_EOL) { error_call(ERR_SYN); }
                else { token = get_next_token(stdin); }

                if (token.type == t_EOL) {
                    rule_eol();
                }
            }
            break;

        case t_BRACES_R:
            if (is_return == true) {
               // printf("CHYBA RETURN\n");
                error_call(ERR_SYN);
            }
            token = get_next_token(stdin);
            break;

        case t_EOL:
            if (is_return == true) {
               // printf("CHYBA RETURN\n");
                error_call(ERR_SYN);
            }
            token = get_next_token(stdin);
            break;

        default:
            error_call(ERR_SYN);

    }
}


void rule_return_type_n() {
    no_id_in_params_flag = true;
    switch (token.type) {
        case t_COMMA:
            token = get_next_token(stdin);
            rule_type();
            rule_return_type_n();
            break;

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

    } else if (token.type != t_COMMA) {printf("1\n");error_call(ERR_SYN);}
    else {
        token = get_next_token(stdin);

        if (token.type != t_IDENTIFIER && token.type != t_FLOAT && token.type != t_STRING &&
            token.type != t_INT_NON_ZERO && token.type != t_INT_ZERO) {printf("2\n");error_call(ERR_SYN); }
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