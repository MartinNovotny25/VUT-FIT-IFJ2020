#include "scanner.h"
#include "psa_lib.h"
#include "precanalysis_stack.h"
#include <string.h>
#include "error.h"


#define PTABLE_ROWS 14
#define PTABLE_COLUMNS 14

//tsym_stack_symbol *KONTROLA;
tsym_stack syms_Stack; // Zásobník tokenov (symbolov)
int count; //pocet symbolov v REDUKCII
bool red_found; // symbol redukcie bol najdeny
psa_rules rule; //hodnota pravidla


// Precedencna tabulka, rozhoduje o výbere operácie podla tokenu na vstupe a tokenu na zásobníku
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

// Prevedie tokeny na symboly, s ktorym pracuje precedencna
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

// Funckia aplikuje pravidlo pre redukciu, viz psa_lib.h
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
        // napr E+E -> 1. a 3. symbol je NON_TERM
        if (sym1->symbol == T_NON_TERM && sym3->symbol == T_NON_TERM) {

            if(sym2->symbol == T_PLUS)
                return R_PLUS;

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

//Samotna riadiaca funckia
void evaluation(TDLList* psa_list, TDLList* global_tokens) {
    // TOKEN dollar, zaradim na koniec vyrazu a posledny znak
    TOKEN dollar;
    alloc(3, &dollar);
    strcpy(dollar.lex, "$");
    dollar.type = t_DOLLAR;

    TDLLFirst(psa_list);
    TDLLInsertLast(psa_list, dollar);

    //Inicializacia stacku
    symstack_init(&syms_Stack);

    //Pushnutie dolara na spodok stacku ako koniec stacku
    symstack_push(&syms_Stack, T_DOLLAR); //DOLLAR NA SPODKU STACKU

    psa_symbols input_sym; //Symbol na vstupe -- VYRAZ
    tsym_stack_symbol* top_terminal; //Symbol na vrchole terminalu

    bool koniec = false; // PREMENNA pre koniec vyhodnocovania vyrazu

    do {

        input_sym = psa_tokenToSymbol(&psa_list->Act->tdata); //do input sym sa umiestni aktualny znak DLL
        top_terminal = symstack_top_t(&syms_Stack); //top terminal je vrchol stacku

        int terminal = top_terminal->symbol;
        int vstup = input_sym;

        //int test = psa_prec_table[1][1];
        //printf("Na terminale je %d, na vstupe je %d\n", terminal, vstup);

        //SPLIT premenna symbolizuje vyber pravidla
        // 1 = SHIFT
        // 2 = REDUKCIA
        // 3 = EQUAL
        // 4 = X, NEMOZNE // V PRIPADE  $ a $ USPESNY KONIEC

        int SPLIT = psa_prec_table[terminal][vstup];
        //printf("SPLIT %d\n", SPLIT);

        /* SHIFT */
        if (SPLIT == 1) {

            // Ak pride shift, pred najvrchnejsi termnal sa pushne znak pre redukciu
            symstack_push_red(&syms_Stack, T_RED);
            //Na vrch sa pusne aktualny znak na vstupe
            symstack_push(&syms_Stack, input_sym);

            //aktivita sa nastaví na dalsi znak
            psa_list->Act = psa_list->Act->rptr;

        }

        /* REDUKCIA */
        else if (SPLIT == 2) {

            //Premenna pre cyklenie cez znaky, ktore s budu nasledne popovať zo stacku po redukcii
            tsym_stack_symbol* checked_symbol;
        ///////////////// TU POZOR /////////////////////////
            //nastavenie na vrchol zasobniku
            checked_symbol = syms_Stack.top;
        /////////////////////////////////////////////////

        //cyklus
            while (checked_symbol != NULL)
            {
                // pokial sa nenajde symbol redukcie, zvysi sa count
                if (checked_symbol->symbol != T_RED)
                {
                    red_found = false;
                    count++;
                }
                // ak sa najde, cyklus sa breakne
                else if (checked_symbol->symbol == T_RED)
                {
                    red_found = true;
                    break;
                }

                checked_symbol = checked_symbol->next;
            }
////////////////////////////////////////////////////////////////////////////////////////////////////

            // Moze sa redukovat iba ak su 3 alebo 1 znak
            // napr E+E -> E, (E) -> E, i -> E
            if (count != 3 && count != 1) {
                symstack_free(&syms_Stack);
                //fprintf(stderr, "PSA - Syntax error\n");
                error_call(ERR_SYN, *(&global_tokens));
            }

            // Jeden znak
            if (count == 1 && red_found) {

                //1 symbol potrebny pre redukciu
                tsym_stack_symbol* red1;
                red1 = syms_Stack.top;

                //Aplikacia pravidla
                rule = psa_rule_application(count, red1, NULL, NULL);

                // popnutie vrchneho symbolu
                symstack_pop(&syms_Stack);
                symstack_pop(&syms_Stack); //popuje sa RED symbol


                //Pushnutie nontermu -> E
                symstack_push(&syms_Stack, T_NON_TERM);

            }   else if (count == 3 && red_found) {

                //3 symboly potrebne pre redukciu
                tsym_stack_symbol* red1;
                tsym_stack_symbol* red2;
                tsym_stack_symbol* red3;

                red3 = syms_Stack.top;
                red2 = red3->next;
                red1 = red3->next->next;

                // Apliakcia pravidla
                rule = psa_rule_application(count, red1, red2, red3);

                //Popnutie 3 symbolob, ktore sa redukovali
                symstack_pop(&syms_Stack);
                symstack_pop(&syms_Stack);
                symstack_pop(&syms_Stack);
                symstack_pop(&syms_Stack); //popuje sa RED symbol

                // push nonterm
                symstack_push(&syms_Stack, T_NON_TERM);

            }

            // Ak nebolo uplatnene ziadne pravidlo -- Error
            if (rule == R_NOTDEFINED) {
                symstack_free(&syms_Stack);
                //fprintf(stderr, "PSA - Syntax error\n");
                error_call(ERR_SYN, *(&global_tokens));
                // Ak chill , nastavi sa count spat na 0
            } else {
                count = 0;
            }


        }
        /* EQUAL */
        else if (SPLIT == 3) {
        // printf("som v =\n");

            // Aktualny symbol sa pushne na stack, aktivita sa posunie
            symstack_push(&syms_Stack, input_sym);
            psa_list->Act = psa_list->Act->rptr;
        }

        else if (SPLIT == 4) {
        //printf("som v x\n");

            // Ak je na vstupe Dollar a na zasobniku Dollar, VYRAZ JE VALIDNY
            if((input_sym == T_DOLLAR && top_terminal->symbol == T_DOLLAR) != true) {
                symstack_free(&syms_Stack);
                //fprintf(stderr, "PSA SYNTAX ERROR, EXIT (2)\n");
                error_call(ERR_SYN, *(&global_tokens));
            } else
                koniec = true;
        }

    } while (koniec != true);

  //  TDLLDeleteLast(psa_list); //removnem dollar na konci listu
  // Uvolnenie stacku
    symstack_free(&syms_Stack);

}
