#include <stdio.h>
#include <stdio.h>

#include "scanner.h"
#include "error.h"

TOKEN token;
bool params_flag = false;

void rule_eol();
void def_func();
void rule_params();
void rule_params_n();

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
    if (token.type != t_PACKAGE_MAIN) { error_call(ERR_SYN); }
        else {token = get_next_token(stdin);}

    // Token EOL
    if (token.type != t_EOL) { error_call(ERR_SYN);}
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

// <def_func>
void def_func()
{
    //printf("som v DEF\n");
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

void rule_params()
{
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

}

void rule_params_n()
{
    if (token.type != t_COMMA && token.type != t_RIGHT_BRACKET) {error_call(ERR_SYN);}
        else {token = get_next_token(stdin);}

    switch (token.type)
    {
        case t_IDENTIFIER:
            token = get_next_token(stdin);
            rule_type();
            rule_params_n();
            break;
        case t_RIGHT_BRACKET:
            printf("som v right bracket/n");
            token = get_next_token(stdin);
            break;
    }

}




