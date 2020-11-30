/* 
**	Part of IFJ Project 2019/2020
**	Author: Samuel Stuchly xstuch06
*/

#include "symtable.h"


// TODO : fix documentation

tBSTNodePtrGlobal GlobalBody ;
tBSTNodePtrLocal LocalVariables;
tBSTNodePtrLocal  GFDefinedVarsTree;
tBSTNodePtrLocal  LFDefinedVarsTree;


void BSTInitGlobal (tBSTNodePtrGlobal *RootPtr) {
/*   -------
** Function inicializes tree before its first use.
**/
	*(RootPtr) = NULL;

}	

bool BSTSearchGlobal (tBSTNodePtrGlobal RootPtr, char* name,functionData *Content)	{
/*  ---------
** Function searches for a node with key == name.
**
** Pokud je takový nalezen, vrací funkce hodnotu TRUE a v proměnné Content se
** vrací obsah příslušného uzlu.´Pokud příslušný uzel není nalezen, vrací funkce
** hodnotu FALSE a obsah proměnné Content není definován (nic do ní proto
** nepřiřazujte).
**/


	if (RootPtr != NULL)
	{
		if ((strcmp(name, RootPtr->Name) < 0))
		{
			return BSTSearchGlobal(RootPtr->LPtr,name,Content);
				
		}
		else if (strcmp(name, RootPtr->Name) > 0)
		{
			return BSTSearchGlobal(RootPtr->RPtr,name,Content);
		}
		else
		{
			*(Content) = RootPtr->content;
			return true;								
		}
	}
	else 
	{

		return false;
	}
} 


void BSTInsertGlobal (tBSTNodePtrGlobal* RootPtr, char* name, functionData Content)	{

    //printf("Symtable: Inserting function\n");
	if (*RootPtr == NULL)
	{
		*RootPtr = malloc(sizeof (struct tBSTNodeGlobal));	
		(*RootPtr)->Name = name;
		(*RootPtr)->content = Content;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL;	
	}

	else if (strcmp(name, (*RootPtr)->Name) == 0)
	{														
		(*RootPtr)->content = Content;					//if found , only data changes 

	}
	else if (strcmp(name, (*RootPtr)->Name) < 0)     	
	{
		BSTInsertGlobal(&(*RootPtr)->LPtr,name,Content);
	}
	else if (strcmp(name, (*RootPtr)->Name) > 0)
	{
		BSTInsertGlobal(&(*RootPtr)->RPtr,name,Content);
	}
	
	//printf("Symtable: Function inserted succesfully\n");
}


void BSTDisposeGlobal (tBSTNodePtrGlobal *RootPtr) {	
/*   
** Deletes entire binary tree and correctly frees allocated memory.
**/	


	if (*RootPtr != NULL)					
	{
		BSTDisposeGlobal(&(*RootPtr)->LPtr);
		BSTDisposeGlobal(&(*RootPtr)->RPtr);
		free(*RootPtr);
		*RootPtr = NULL;
	}
}


/*void BSTFunctionDataInit(functionData *data) {
    data->numOfParams = 0;
    data->numOfReturns = 0;
    data->params[MAXPARAMS] = {NULL};
    data->returns[MAXPARAMS] = {0};
    data->paramsType[MAXPARAMS] = {0};
}*/

// ========================================= LOCAL TREE ==================================================== //

void BSTInitLocal (tBSTNodePtrLocal *RootPtr) {
	
	printf("Inicializoval som lokalny strom\n");
	*(RootPtr) = NULL;

}	

bool BSTSearchLocal (tBSTNodePtrLocal RootPtr, char* name, int *Type, char *Data)    {

    if (RootPtr != NULL && RootPtr->Name!= NULL)
    {
        if ((strcmp(name, RootPtr->Name) < 0))
        {
            return BSTSearchLocal(RootPtr->LPtr,name, Type, Data);
        }
        else if (strcmp(name, RootPtr->Name) > 0)
        {
            return BSTSearchLocal(RootPtr->RPtr,name, Type, Data);
        }
        else
        {
           
            *Type = RootPtr->Type;
           
            //*Data = *RootPtr->Data;
            
            return true;
        }
    }
    else
    {
		
        return false;
    }
}
 


void BSTInsertLocal (tBSTNodePtrLocal* RootPtr, char * Name, int *Type, char *Data)	{

	printf("NAZOV: %s,TYP: %d, HODNOTA: %s\n", Name, *Type, Data);

	if (*RootPtr == NULL){
		*RootPtr = malloc(sizeof (struct tBSTNodeLocal));	
		(*RootPtr)->Name = Name;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL;	
		(*RootPtr)->Type = *Type;
		(*RootPtr)->Data = Data;
		printf("Vlozil som prvy uzol\n");
	}else if (strcmp(Name, (*RootPtr)->Name) == 0)
	{		
		// this state should never be called since parser should check if local var is in tree
		// before trying to insert it again.
		// TODO :  when parser is done this should be deleted becase it is a dead code !!!								
		printf("THIS CODE SHOULD NEVER BE REACHED!\n"); 
		
	}else if (strcmp(Name, (*RootPtr)->Name) < 0)     	
	{
		printf("Vlkadam dolava\n");
		BSTInsertLocal(&(*RootPtr)->LPtr,Name, Type, Data);
	}else if (strcmp(Name, (*RootPtr)->Name) > 0)
	{
		printf("Vkladam doprava\n");
		BSTInsertLocal(&(*RootPtr)->RPtr,Name, Type, Data);
	}
	
	
	
}


void BSTDisposeLocal (tBSTNodePtrLocal *RootPtr) {	


	printf("Dispose Local\n");
	if (*RootPtr != NULL)					
	{
		BSTDisposeLocal(&(*RootPtr)->LPtr);
		BSTDisposeLocal(&(*RootPtr)->RPtr);
		free(*RootPtr);
		*RootPtr = NULL;
	}
	


}

void InitMainStack (MainStack *S)
{
	S->top = 0;
}

// Vlozi Globalny strom s info alebo lokalny strom s info(infosky su ulozene v strome)
void PushTreeMain (MainStack *S, tBSTNodePtrLocal ptrLocal)
{
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK)
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {
		S->top++;
		S->a[S->top]=ptrLocal;
	}
}

// Vyhodi vrchny strom
// Asi bude treba upravit
tBSTNodePtrLocal PopTreeMain (MainStack *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
	return (S->a[S->top--]);
}

// Vycisti cely hlavny zasobnik
bool EmptyMainStack (MainStack *S)
{
  return(S->top==0);
}
////////////////////////////////////////////////////////////////////////////////////////
/*
// Inicializuje pomocny zasobnik
void InitHelpStack (HelpStack *S)
{
	S->top = 0;
}

// Uklada ukazatel na strom(ramec)
// Aby sme vedeli kde konci aj ked je to nepotrebne 1 strom je 1 ramec
void PushPtrTreeHelp (HelpStack *S, tBSTNodePtrLocal *ptrLocal)
{
//Při implementaci v poli může dojít k přetečení zásobníku.
  if (S->top==MAXSTACK)
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {
		S->top++;
		S->a[S->top]=ptrLocal;
	}
}

// Po praci so stromom(ramcom) bude treba uvolnit dany strom
void PopPtrTreeHelp (HelpStack *S)
{
	(S->a[S->top--]);
}

// Zavola sa na konci semantiky
bool EmptyHelpStack (HelpStack *S)
{
  return(S->top==0);
}
*/



