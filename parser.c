#include <stdio.h>
#include <stdio.h>

#include "scanner.h"
#include "error.h"

TOKEN token;
bool params_flag = false; //pri id, id nemoze byt return type pri def_func

void rule_eol();
void def_func();
void rule_params();
void rule_params_n();
void rule_func_retlist_body();

int main() {
    token = get_next_token(stdin);

    //TODO
    /* <prog> -> package main EOL <def_func> EOF */

    // eol(y) na zaciatku
    if (token.type == t_EOL)
    {
        token = get_next_token(stdin);
        rule_eol();
    }

    // Test package main
    if (token.type != t_PACKAGE_MAIN) {error_call(ERR_SYN); }
        else {token = get_next_token(stdin);}

    // Token EOL
    if (token.type != t_EOL) {error_call(ERR_SYN);}
        else {token = get_next_token(stdin);}

    // Neterminal <eol>
    if (token.type == t_EOL)
    {
        token = get_next_token(stdin);
        rule_eol();
    }
    // Neterminal <def_func>
    if (token.type != t_FUNC) {error_call(ERR_SYN);}
        else {
            token = get_next_token(stdin);
            def_func();
        }
    return 0;
}

// funkcia pre neterminal <def_func>
void def_func()
{
    // terminal ID
    if (token.type != t_IDENTIFIER) {error_call(ERR_SYN);}
        else {token = get_next_token(stdin);}

    // token (
    if (token.type != t_LEFT_BRACKET) {error_call(ERR_SYN);}
        else {token = get_next_token(stdin);}

    // neterminal <params>
    if (token.type != t_IDENTIFIER) {error_call(ERR_SYN);}
        else {
            //token = get_next_token(stdin);
            rule_params();
        }
    if (token.type != t_RIGHT_BRACKET && token.type != t_BRACES_R) {error_call(ERR_SYN);}
        else {
        rule_func_retlist_body();
    }


    //VYPIS NEIMPLEMENTOVANYCH TERMINALOV
   // while (token.type != )

    /*token = get_next_token(stdin);
    token = get_next_token(stdin);
    token = get_next_token(stdin);*/
}

// funkcie pre neterminal <eol>
void rule_eol()
{
    //printf("som v EOL\n");
    switch (token.type)
    {
        case t_EOL:
            token = get_next_token(stdin);
            rule_eol();
            break;

        default:
            break;
    }
}

// funkcia preneterminal <type>
void rule_type()
{
    //printf("som v TYPE\n");
    // Priradenie datoveho typu -
    switch (token.type)
    {
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
            if (params_flag == true) {error_call(ERR_SYN);}
            token = get_next_token(stdin);
            break;
        default:
            error_call(ERR_SYN);
    }
}

// funkcia pre neterminal params
void rule_params()
{
    params_flag = true;
    switch (token.type)
    {
        case t_IDENTIFIER:
            token = get_next_token(stdin);
            rule_type();
            rule_params_n();
            break;
        case t_RIGHT_BRACKET:
            token = get_next_token(stdin);
            break;
    }
    params_flag = false;
}
// funckia pre netemrinal params_n
void rule_params_n()
{
    //ak pride zatvorka -- koniec (OTESTOVAT CI POTREBNE)
    if (token.type == t_RIGHT_BRACKET) { return; }

    // prva musi prist ciarka ako oddelovac dalsieho parametra
    if (token.type != t_COMMA /*&& token.type != t_RIGHT_BRACKET*/) {error_call(ERR_SYN);}
        else {token = get_next_token(stdin);}

    // kontrola, ci po ciarke pride id

    if (token.type != t_IDENTIFIER) {error_call(ERR_SYN);}
        // ak pride id, pokracujeme typom a nasledne volame params_n znova
        else {
                token = get_next_token(stdin);
                rule_type();
                rule_params_n();
            }
}

void rule_func_retlist_body()
{
    switch (token.type) {
        case t_RIGHT_BRACKET:
            printf("Tu pojdem do FUNC_BODY\n");
            token = get_next_token(stdin);

            //PREROBIT NA IF AZ BUDE FUNC BODY DONE
            while (token.type != t_BRACES_R)
               {token = get_next_token(stdin); printf("som vo while\n");}
            break;

        case t_BRACES_L:
            token = get_next_token(stdin);
            if (token.type != t_EOL) {error_call(ERR_SYN);}
                else {token = get_next_token(stdin);}

            printf("tu budem pokracovat do STAT a OPTIONAL RET\n");

            if (token.type != t_BRACES_R) {error_call(ERR_SYN);}
                else {token = get_next_token(stdin);}
    }
}




