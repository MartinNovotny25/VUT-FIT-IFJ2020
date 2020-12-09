/*
*  Predmet: Formalne jazyky a prekladace (IFJ) - FIT VUT v Brne
*  Zdrojovy subor error kniznice pre projekt IFJ2020
*  Vytvoril: Martin Novotny - xnovot1r
*
*  Datum: 10/2020
*/

#include "error.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"
#include "tokenList.h"

void error_call(int err, TDLList *L)
{
    
    TDLLDisposeList(L);        //v pripade chyby je potrebne uvolnit DLlist
    
    int exit_err;
    switch (err)
    {
        case ERR_LEX:
            fprintf(stderr, "ERR 1 - Lexikalna chyba\n");
            exit_err = 1;
            break;

        case ERR_SYN:
            fprintf(stderr, "ERR 2 - Syntakticka chyba\n");
            exit_err = 2;
            break;

        case ERR_SEM_UNDEF:
            fprintf(stderr, "ERR 3 - Semanticka chyba - nedefinovana funkcia\n");
            exit_err = 3;
            break;

        case ERR_SEM_DATATYPE:
            fprintf(stderr, "ERR 4 - Semanticka chyba - Odvodenie datoveho typu\n");
            exit_err = 4;
            break;

        case ERR_SEM_EXCOMPAT:
            fprintf(stderr, "ERR 5 - Semanticka chyba - Typova kompatibilita\n");
            exit_err = 5;
            break;

        case ERR_SEM_RETURN:
            fprintf(stderr, "ERR 6 - Semanticka chyba - Nespravny pocet parametrov/navratovych hodnot\n");
            exit_err = 6;
            break;

        case ERR_SEM_OTHER:
            fprintf(stderr, "ERR 7 - Semanticka chyba - Ostatne chyby\n");
            exit_err = 7;
            break;

        case ERR_SEM_ZERODIV:
            fprintf(stderr, "ERR 9 - Semanticka chyba - Delenie nulou\n" );
            exit_err = 9;
            break;

        case ERR_INTERN:
            fprintf(stderr, "ERR 99 - Vnutorna chyba prekladaca\n" );
            exit_err = 99;
            break;
    }
    fprintf(stderr, "UKONCUJEM\n");
    exit(exit_err);
}
