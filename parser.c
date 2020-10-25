#include <stdio.h>
#include <stdio.h>

#include "scanner.h"

int def_func();

int main() {
    TOKEN token;
    token = get_next_token(stdin);
    int def_return;
    //TODO
    /* <prog> -> package main EOL <def_func> EOF */

    /*if(token.type != 27) {fprintf(stderr, "Chybny vstup"); return 2;}
        else {token = get_next_token(stdin);}*/
    if (token.type != t_FUNC) { fprintf(stderr, "Chybny vstup"); return 2;}
        else
            {
               def_return = def_func(token);
            }

    if (def_return != 0) { fprintf(stderr, "CHYBA"); return 2;}
    else {fprintf(stdout, "Dobre"); return 0;}

}

int def_func(TOKEN data)
{


}
