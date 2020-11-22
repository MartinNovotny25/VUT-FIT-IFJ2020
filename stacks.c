#include "symtable.h"

void InitMainStack (MainStack *S)
{
	S->top = 0;
}

// Vlozi Globalny strom s info alebo lokalny strom s info(infosky su ulozene v strome)
void PushTreeMain (MainStack *S, tBSTNodePtrGlobal ptrGlobal, tBSTNodePtrLocal ptrLocal)
{
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK)
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {
		S->top++;
		S->a[S->top]=ptrLocal;
        S->b[S->top]=ptrGlobal;
	}
}

// Vyhodi vrchny strom
// Asi bude treba upravit
void PopTreeMain (MainStack *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
	(S->a[S->top--]);
    (S->b[S->top--]);
}

// Vycisti cely hlavny zasobnik
bool EmptyMainStack (MainStack *S)
{
  return(S->top==0);
}
////////////////////////////////////////////////////////////////////////////////////////

// Inicializuje pomocny zasobnik
void InitHelpStack (HelpStack *S)
{
	S->top = 0;
}

// Uklada ukazatel na strom(ramec)
// Aby sme vedeli kde konci aj ked je to nepotrebne 1 strom je 1 ramec
void PushPtrTreeHelp (HelpStack *S, tBSTNodePtrGlobal *ptrGlobal, tBSTNodePtrLocal *ptrLocal)
{
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK)
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {
		S->top++;
		S->a[S->top]=ptrLocal;
        S->b[S->top]=ptrGlobal;
	}
}

// Po praci so stromom(ramcom) bude treba uvolnit dany strom
void PopPtrTreeHelp (HelpStack *S)
{
	(S->a[S->top--]);
    (S->b[S->top--]);
}

// Zavola sa na konci semantiky
bool EmptyHelpStack (HelpStack *S)
{
  return(S->top==0);
}

