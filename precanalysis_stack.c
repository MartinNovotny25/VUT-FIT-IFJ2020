#include "symstack.h"


void symstack_init(tsym_stack* sym_stack){
    sym_stack->top = NULL;
}

bool symstack_push(tsym_stack* sym_stack, psa_symbols symbol){
    tsym_stack_symbol* pushed_sym = (tsym_stack_symbol*)malloc(sizeof(tsym_stack_symbol));

    if (pushed_sym == NULL) {
        return false;
    }
    pushed_sym->next = sym_stack->top;
    pushed_sym->symbol = symbol;

    sym_stack->top = pushed_sym;

    return true;
}

bool symstack_pop(tsym_stack* sym_stack) {
    if (sym_stack->top != NULL)
    {

        tsym_stack_symbol* stack_sym = sym_stack->top;
        sym_stack->top = stack_sym->next;
        free(stack_sym);
        return true;
    }
    return false;
}

tsym_stack_symbol* symstack_top_t(tsym_stack* sym_stack)
{

    tsym_stack_symbol* stack_sym = sym_stack->top;
    while (stack_sym != NULL) {

        if (stack_sym->symbol != T_RED && stack_sym->symbol != T_NON_TERM) {
            return stack_sym;
        }

        stack_sym = stack_sym->next;
    }
    return NULL;
}

void symstack_push_red(tsym_stack* sym_stack, psa_symbols symbol)
{
    tsym_stack_symbol* left = NULL;
    tsym_stack_symbol* stack_sym = sym_stack->top;

    while (stack_sym != NULL)
    {
        if (stack_sym->symbol < T_RED /*&& stack_sym->symbol != T_NON_TERM*/){

            tsym_stack_symbol* pushed_sym = (tsym_stack_symbol*)malloc(sizeof(tsym_stack_symbol));
            pushed_sym->symbol = symbol;

            if (left == NULL){
                pushed_sym->next = sym_stack->top;
                sym_stack->top = pushed_sym;
            }
            else {
                pushed_sym->next = left->next;
                left->next = pushed_sym;
            }
            return;

        }

        left = stack_sym;
        stack_sym = stack_sym->next;
    }

    return;

}

tsym_stack_symbol* sym_stack_top(tsym_stack* sym_stack){

    return sym_stack->top;
}



void symstack_free(tsym_stack* sym_stack){
    while (symstack_pop(sym_stack));
}
