//
// Created by novot on 11/23/2020.
//

#ifndef PARSER_C_PSA_LIB_H
#define PARSER_C_PSA_LIB_H
typedef enum PSA_RULES {
    R_PLUS, // E -> E + E 0
    R_MINUS, // E -> E - E 1
    R_MUL, // E -> E * E 2
    R_DIV, // E -> E / E 3
    R_EQUAL, // E -> E = E 5
    R_NEQUAL, // E -> E != E 6
    R_MOE, // E -> E >= E  7
    R_LOE, // E -> E <= E 8
    R_LESS, // E -> E < E 9
    R_MORE, // E -> E > E 10
    R_EBRACES, // E -> (E) 11
    R_OP, // E -> i  12
    R_NOTDEFINED // Nedefinovane pravidlo 13
} psa_rules;


typedef enum PSA_SYMBOLS {

    T_PLUS, T_MINUS, // + -
    T_MUL, T_DIV, // * / //
    T_LEFT_PARENTHESIS, T_RIGHT_PARENTHESIS, // ( )
    T_EQUAL, T_NEQUAL, T_MOE, T_LOE, T_LESS, T_MORE, // = != >= <= < >
    T_ID, // identifikator
    T_DOLLAR, T_RED, T_NON_TERM // $ , neterminal, znak redukcie
} psa_symbols;


#endif //PARSER_C_PSA_LIB_H
