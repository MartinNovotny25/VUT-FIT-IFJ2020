/*
*  Predmet: Formalne jazyky a prekladace (IFJ) - FIT VUT v Brne
*  Hlavickovy subor zasobniku precedencnej syntaktickej analyzy pre projekt IFJ2020
*  Vytvoril: Martin Novotny Mlinarcsik - xnovot1r
*
*  Datum: 10/2020
*/

#ifndef SYMSTACK_H
#define SYMSTACK_H

#include "precanalysis.h"
#include <stdbool.h>
#include <stdlib.h>
#include "psa_lib.h"


typedef struct stack_symbol
{
    psa_symbols symbol;
    struct stack_symbol *next;
} tsym_stack_symbol;


typedef struct
{
    tsym_stack_symbol *top; /// Pointer to stack item on top of stack.
} tsym_stack;


void symstack_init(tsym_stack* stack);
void symstack_push(tsym_stack* stack, psa_symbols symbol);
bool symstack_pop(tsym_stack* stack);
tsym_stack_symbol* symstack_top_t(tsym_stack* stack);
void symstack_push_red(tsym_stack* stack, psa_symbols symbol);
void symstack_free(tsym_stack* stack);

#endif