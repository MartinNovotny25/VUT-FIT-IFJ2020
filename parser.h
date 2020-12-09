/*
*  Predmet: Formalne jazyky a prekladace (IFJ) - FIT VUT v Brne
*  Hlavickovy subor syntaktickej analyzy pre projekt IFJ2020
*  Vytvoril: Martin Novotny - xnovot1r
*
*  Datum: 10/2020
*/

#ifndef PARSER_H
#define PARSER_H

void rule_eol();

void required_return();

void optional_return();

void def_func();

void rule_params();

void rule_params_n();

void rule_func_retlist_body();

void rule_func_body();

void rule_type();

void rule_return_type();

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


#endif
