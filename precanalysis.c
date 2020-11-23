#include "scanner.h"
#include "psa_lib.h"
#include "symstack.h"
#include <string.h>
#include "tokenList.h"


#define PTABLE_ROWS 14
#define PTABLE_COLUMNS 14
//extern int rule_array[120][40];

int SYM1, SYM2, SYM3;

tsym_stack_symbol *KONTROLA;
tsym_stack syms_Stack;
tsym_stack_symbol* checked_symbol;
TOKEN exp_start, exp_end;
TOKEN help_tmp;
int count;
bool red_found;
psa_rules rule;

//int rule_array[120][40];

int psa_prec_table[PTABLE_ROWS][PTABLE_COLUMNS] = {
        /*           0   1   2   3   4   5     6   7    8    9   10  11  12  13
        /*           |+| |-| |*| |/| |(| |)| |==| |!=| |>=| |<=| |<| |>| |i| |$| */
        /* 0 +  */ {  2 , 2 , 1 , 1 , 1 , 2 , 2 ,  2 ,  2 ,  2 ,  2 , 2 , 1 , 2  },
        /* 1 -  */ {  2 , 2 , 1 , 1 , 1 , 2 , 2 ,  2 ,  2 ,  2 ,  2 , 2 , 1 , 2  },
        /* 2 *  */ {  2 , 2 , 2 , 2 , 1 , 2 , 2 ,  2 ,  2 ,  2 ,  2 , 2 , 1 , 2  },
        /* 3 /  */ {  2 , 2 , 2 , 2 , 1 , 2 , 2 ,  2 ,  2 ,  2 ,  2 , 2 , 1 , 2  },
        /* 4 (  */ {  1 , 1 , 1 , 1 , 1 , 3 , 1 ,  1 ,  1 ,  1 ,  1 , 1 , 1 , 4  },
        /* 5 )  */ {  2 , 2 , 2 , 2 , 4 , 2 , 2 ,  2 ,  2 ,  2 ,  2 , 2 , 4 , 2  },
        /* 6 == */ {  1 , 1 , 1 , 1 , 1 , 2 , 2 ,  2 ,  2 ,  2 ,  2 , 2 , 1 , 2  },
        /* 7 != */ {  1 , 1 , 1 , 1 , 1 , 2 , 2 ,  2 ,  2 ,  2 ,  2 , 2 , 1 , 2  },
        /* 8 >= */ {  1 , 1 , 1 , 1 , 1 , 2 , 2 ,  2 ,  2 ,  2 ,  2 , 2 , 1 , 2  },
        /* 9 <= */ {  1 , 1 , 1 , 1 , 1 , 2 , 2 ,  2 ,  2 ,  2 ,  2 , 2 , 1 , 2  },
        /* 10 <  */{  1 , 1 , 1 , 1 , 1 , 2 , 2 ,  2 ,  2 ,  2 ,  2 , 2 , 1 , 2  },
        /* 11 >  */{  1 , 1 , 1 , 1 , 1 , 2 , 2 ,  2 ,  2 ,  2 ,  2 , 2 , 1 , 2  },
        /* 12 i  */{  2 , 2 , 2 , 2 , 4 , 2 , 2 ,  2 ,  2 ,  2 ,  2 , 2 , 4 , 2  },
        /* 13 $  */{  1 , 1 , 1 , 1 , 1 , 4 , 1 ,  1 ,  1 ,  1 ,  1 , 1 , 1 , 4  }
};

psa_symbols psa_tokenToSymbol(TOKEN* token) {

    if (token->type == t_PLUS)
        return T_PLUS;
    else if (token->type == t_MINUS)
        return T_MINUS;
    else if (token->type == t_MULTIPLY)
        return T_MUL;
    else if (token->type == t_DIVIDE)
        return T_DIV;
    else if (token->type == t_RIGHT_BRACKET)
        return T_RIGHT_PARENTHESIS;
    else if (token->type == t_LEFT_BRACKET)
        return T_LEFT_PARENTHESIS;
    else if (token->type == t_LESS)
        return T_LESS;
    else if (token->type == t_GREATER)
        return T_MORE;
    else if (token->type == t_LESSOREQUAL)
        return T_LOE;
    else if (token->type == t_GREATEROREQUAL)
        return T_MOE;
    else if (token->type == t_IDENTIFIER)
        return T_ID;
    else if (token->type == t_INT_NON_ZERO)
        return T_ID;
    else if (token->type == t_INT_ZERO)
        return T_ID;
    else if (token->type == t_FLOAT)
        return T_ID;
    else if (token->type == t_STRING)
        return T_ID;
    else if (token->type == t_EQUAL)
        return T_EQUAL;
    else if (token->type == t_NOT_EQUAL)
        return T_NEQUAL;
    else if (token->type == t_DOLLAR){
        return T_DOLLAR;
    }
}

psa_rules psa_rule_application(int pocet, tsym_stack_symbol* sym1, tsym_stack_symbol* sym2, tsym_stack_symbol* sym3) {

    if (pocet == 1) {

        if (sym1->symbol == T_ID) {

            return R_OP;
        } else {

            return R_NOTDEFINED;
        }
    }

        // NON_TERM sa bude pushovat pocas reduce pravidla namiesto popnutych symbolov
    else if (pocet == 3) {
        if (sym1->symbol == T_LEFT_PARENTHESIS && sym2->symbol == T_NON_TERM && sym3->symbol == T_RIGHT_PARENTHESIS) {
            return R_EBRACES;
        }

        if (sym1->symbol == T_NON_TERM && sym3->symbol == T_NON_TERM) {

            if(sym2->symbol == T_PLUS) {
                return R_PLUS;
            }

            else if (sym2->symbol == T_MINUS)
                return R_MINUS;

            else if (sym2->symbol == T_MUL)
                return R_MUL ;

            else if (sym2->symbol == T_DIV)
                return R_DIV;

            else if (sym2->symbol == T_EQUAL )
                return R_EQUAL;

            else if (sym2->symbol == T_NEQUAL )
                return R_NEQUAL;

            else if (sym2->symbol == T_MOE)
                return R_MOE;

            else if (sym2->symbol == T_LOE )
                return R_LOE;

            else if (sym2->symbol == T_MORE ) {
                return R_MORE;
            }
            else if (sym2->symbol == T_LESS )
                return R_LESS;

        } else {
            return R_NOTDEFINED;
        }
        return R_NOTDEFINED;

    }

    return R_NOTDEFINED;
}

void evaluation(TDLList* psa_list) {
    TOKEN dollar;

    alloc(3, &dollar);
    strcpy(dollar.lex, "$");
    dollar.type = t_DOLLAR;



// int rule_counter = 0;
//static int id_counter = 0;

    extern psa_rules rule;

//TOKENY zaciatok a start
/* exp_start.lex = id_counter;
 exp_end.lex = NULL;

 exp_start.type = EXP_START;
 exp_end.type = EXP_END;*/

//TDLLDeleteFirst(psa_list);
    TDLLFirst(psa_list);
//TDLLInsertFirst(psa_list, exp_start);
    TDLLInsertLast(psa_list, dollar);
    symstack_init(&syms_Stack);

    symstack_push(&syms_Stack, T_DOLLAR); //DOLLAR NA SPODKU STACKU

    psa_symbols input_sym;
    tsym_stack_symbol* top_terminal;


    bool koniec = false;

    do {

        input_sym = psa_tokenToSymbol(&psa_list->Act->tdata);
        top_terminal = symstack_top_t(&syms_Stack);

        int terminal = top_terminal->symbol;
        int vstup = input_sym;

//int test = psa_prec_table[1][1];

//printf("Na terminale je %d, na vstupe je %d\n", terminal, vstup);

        int SPLIT = psa_prec_table[terminal/*top_terminal->symbol*/][vstup/*input_sym*/];
        printf("SPLIT %d\n", SPLIT);

/* SHIFT */
        if (SPLIT == 1) {
//printf("som v <\n");

            symstack_push_red(&syms_Stack, T_RED);
            KONTROLA = syms_Stack.top;
            printf("PUSH %d\n", KONTROLA->symbol);

            symstack_push(&syms_Stack, input_sym);

            KONTROLA = syms_Stack.top;
            printf("PUSH %d\n", KONTROLA->symbol);


            psa_list->Act = psa_list->Act->rptr;

            printf("\n\nKONIEC\n\n");

        }

/* REDUKCIA */
        else if (SPLIT == 2) {
//printf("som v >\n");
            tsym_stack_symbol* checked_symbol;
///////////////////////////////////////////////////////////////////

///////////////// TU POZOR /////////////////////////
            checked_symbol = syms_Stack.top;
/////////////////////////////////////////////////


            while (checked_symbol != NULL)
            {
// printf("Na vrchu je %d\n", checked_symbol->symbol);
                if (checked_symbol->symbol != T_RED)
                {
                    red_found = false;
                    count++;
                }
                else if (checked_symbol->symbol == T_RED)
                {
                    red_found = true;
                    break;
                }

                checked_symbol = checked_symbol->next;
            }
////////////////////////////////////////////////////////////////////////////////////////////////////

            if (count != 3 && count != 1) {

                /*KONTROLA = syms_Stack.top;
                printf("POP %d\n", KONTROLA->symbol);
                symstack_pop(&syms_Stack);

                KONTROLA = syms_Stack.top;
                printf("POP %d\n", KONTROLA->symbol);
                symstack_pop(&syms_Stack);

                KONTROLA = syms_Stack.top;
                printf("POP %d\n", KONTROLA->symbol);
                symstack_pop(&syms_Stack);

                KONTROLA = syms_Stack.top;
                printf("POP %d\n", KONTROLA->symbol);
                symstack_pop(&syms_Stack);*/

                fprintf(stderr, "Syntax Error, zly pocet symbolov\n");
                exit(2);
            }

            if (count == 1 && red_found) {

                //1 symbolx potrebne pre redukciu
                tsym_stack_symbol* red1;

                red1 = syms_Stack.top;


                rule = psa_rule_application(count, red1, NULL, NULL);

                KONTROLA = syms_Stack.top;
                printf("POP %d\n", KONTROLA->symbol);
                symstack_pop(&syms_Stack);

                KONTROLA = syms_Stack.top;
                printf("POP %d\n", KONTROLA->symbol);
                symstack_pop(&syms_Stack); //popuje sa RED symbol


                symstack_push(&syms_Stack, T_NON_TERM);

                KONTROLA = syms_Stack.top;
                printf("PUSH %d\n", KONTROLA->symbol);



            }   else if (count == 3 && red_found) {

                //3 symbolx potrebne pre redukciu
                tsym_stack_symbol* red1;
                tsym_stack_symbol* red2;
                tsym_stack_symbol* red3;

                red3 = syms_Stack.top;
                red2 = red3->next;
                red1 = red3->next->next;

                rule = psa_rule_application(count, red1, red2, red3);

                KONTROLA = syms_Stack.top;
                printf("POP %d\n", KONTROLA->symbol);
                symstack_pop(&syms_Stack);

                KONTROLA = syms_Stack.top;
                printf("POP %d\n", KONTROLA->symbol);
                symstack_pop(&syms_Stack);

                KONTROLA = syms_Stack.top;
                printf("POP %d\n", KONTROLA->symbol);
                symstack_pop(&syms_Stack);

                KONTROLA = syms_Stack.top;
                printf("POP %d\n", KONTROLA->symbol);
                symstack_pop(&syms_Stack); //popuje sa RED symbol

                symstack_push(&syms_Stack, T_NON_TERM);
                KONTROLA = syms_Stack.top;
                printf("PUSH %d\n", KONTROLA->symbol);
            }

            if (rule == R_NOTDEFINED) {
                fprintf(stderr, "PSA - Syntax error\n");
                exit(2);
            } else {
                count = 0;
            }

            printf("\n\nKONIEC\n\n");
        }

        /* EQUAL */
        else if (SPLIT == 3) {
        // printf("som v =\n");

            symstack_push(&syms_Stack, input_sym);
            KONTROLA = syms_Stack.top;
            printf("PUSH %d\n", KONTROLA->symbol);

            psa_list->Act = psa_list->Act->rptr;

            printf("\n\nKONIEC\n\n");
        }

        else if (SPLIT == 4) {
        //printf("som v x\n");

            if((input_sym == T_DOLLAR && top_terminal->symbol == T_DOLLAR) != true) {

                fprintf(stderr, "PSA SYNTAX ERROR, EXIT (2)\n");

                exit(2); //situacia nemoze nastat
            } else
                koniec = true;
        }

    } while (koniec != true);

    TDLLDeleteLast(psa_list); //removnem dollar na konci listu

        //TDLLInsertLast(psa_list, exp_end);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    symstack_free(&syms_Stack);

}
