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

    printf("Symtable: Disposing all nodes...\n");
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


    //printf("SOM V SEARCHLOCAL\n");
	printf("Hladam uzol s nazvom: %s\n", name);
    if (RootPtr != NULL && RootPtr->Name != NULL)
    {
        if ((strcmp(name, RootPtr->Name) < 0))
        {
            printf("SOM V 1st if\n");
            return BSTSearchLocal(RootPtr->LPtr,name, Type, Data);
                
        }
        else if (strcmp(name, RootPtr->Name) > 0)
        {
            printf("SOM V 2nd if\n");
            return BSTSearchLocal(RootPtr->RPtr,name, Type, Data);
        }
        else
        {
            printf("SOM V ELSE - TU CHCEM BYT\n");
            *Type = RootPtr->Type;
            //printf("SOM V ELSE - 1\n");
            //*Data = *RootPtr->Data;
           // printf("SOM V ELSE - 2\n");
            return true;
        }
    }
    else
    {
		printf("Nenasiel som hodnotu\n");
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
	//	printf("Vlozil som prvy uzol\n");
	}else if (strcmp(Name, (*RootPtr)->Name) == 0)
	{		
		// this state should never be called since parser should check if local var is in tree
		// before trying to insert it again.
		// TODO :  when parser is done this should be deleted becase it is a dead code !!!								
	//	printf("THIS CODE SHOULD NEVER BE REACHED!\n"); 
		
	}else if (strcmp(Name, (*RootPtr)->Name) < 0)     	
	{
	//	printf("Vlkadam dolava\n");
		BSTInsertLocal(&(*RootPtr)->LPtr,Name, Type, Data);
	}else if (strcmp(Name, (*RootPtr)->Name) > 0)
	{
	//	printf("Vkladam doprava\n");
		BSTInsertLocal(&(*RootPtr)->RPtr,Name, Type, Data);
	}
}

void ReplaceByRightmost (tBSTNodePtrLocal PtrReplaced, tBSTNodePtrLocal *RootPtr) {
/*   ------------------
** Pomocná funkce pro vyhledání, přesun a uvolnění nejpravějšího uzlu.
**
** Ukazatel PtrReplaced ukazuje na uzel, do kterého bude přesunuta hodnota
** nejpravějšího uzlu v podstromu, který je určen ukazatelem RootPtr.
** Předpokládá se, že hodnota ukazatele RootPtr nebude NULL (zajistěte to
** testováním před volání této funkce). Tuto funkci implementujte rekurzivně.
**
** Tato pomocná funkce bude použita dále. Než ji začnete implementovat,
** přečtěte si komentář k funkci BSTDelete().
**/
	

	if(*RootPtr != NULL && PtrReplaced != NULL){
		if((*RootPtr)->RPtr != NULL){ 
			// najde najpravejsi uzol
			ReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);
		}else{
			PtrReplaced->Name = (*RootPtr)->Name;
			PtrReplaced->Type = (*RootPtr)->Type;
			PtrReplaced->Data = (*RootPtr)->Data;
			BSTDelete(RootPtr, (*RootPtr)->Name);
		}
	}else return;
}

void BSTDelete (tBSTNodePtrLocal *RootPtr, char *K) {
/*   ---------
** Zruší uzel stromu, který obsahuje klíč K.
**
** Pokud uzel se zadaným klíčem neexistuje, nedělá funkce nic.
** Pokud má rušený uzel jen jeden podstrom, pak jej zdědí otec rušeného uzlu.
** Pokud má rušený uzel oba podstromy, pak je rušený uzel nahrazen nejpravějším
** uzlem levého podstromu. Pozor! Nejpravější uzel nemusí být listem.
**
** Tuto funkci implementujte rekurzivně s využitím dříve deklarované
** pomocné funkce ReplaceByRightmost.
**/

	if(*RootPtr != NULL){ // nemame prazdny uzol
		if((*RootPtr)->Name > K){ // kluc je vlavo 
			BSTDelete(&(*RootPtr)->LPtr, K);
		}else if((*RootPtr)->Name < K){ // kluc je vpravo
			BSTDelete(&(*RootPtr)->RPtr, K); 
		}else if((*RootPtr)->Name == K){ // ak sme nasli uzol

			if((*RootPtr)->LPtr != NULL && (*RootPtr)->RPtr != NULL){ // 2 podstromy
					ReplaceByRightmost(*RootPtr, &(*RootPtr)->LPtr);
			}else if((*RootPtr)->LPtr != NULL && (*RootPtr)->RPtr == NULL){ // lavy 
				tBSTNodePtrLocal temp_node;//funguje
				temp_node = (*RootPtr)->LPtr; // ulozime odstranovany uzol
				free(*RootPtr);
				*RootPtr = temp_node;

			}else if((*RootPtr)->RPtr != NULL && (*RootPtr)->LPtr == NULL){ // pravy
				tBSTNodePtrLocal temp_node; // funguje
				temp_node = (*RootPtr)->RPtr;
				free(*RootPtr);
				*RootPtr = temp_node;

			}else{ // ziadny podstrom funguje
				free(*RootPtr);
				*RootPtr = NULL;
			}
		}
	}else{
		return;
	}
}

void BSTDisposeLocal (tBSTNodePtrLocal *RootPtr) {	


	printf("Dispose Local\n");
	if (*RootPtr != NULL)					
	{
        printf("rootprt != NULL\n");
		BSTDisposeLocal(&(*RootPtr)->LPtr);
		BSTDisposeLocal(&(*RootPtr)->RPtr);
		free(*RootPtr);
		*RootPtr = NULL;
	}
    printf("RootPtr == NULL, koniec tejto rekurzie\n");
	


}

void InitMainStack (MainStack *S)
{
	S->top = 0;
}

// Vlozi Globalny strom s info alebo lokalny strom s info(infosky su ulozene v strome)
void PushTreeMain (MainStack *S, tBSTNodePtrLocal *ptrLocal)
{
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK)
   printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {
		S->top++;
		S->a[S->top]=ptrLocal;
		printf("top %d\n", S->top);
	}
}

// Vyhodi vrchny strom
// Asi bude treba upravit
tBSTNodePtrLocal PopTreeMain (MainStack *S)
{
	tBSTNodePtrLocal temp;
	temp = *(S->a[S->top]);
	S->top--;
	printf("top %d\n", S->top);
	return temp;
}

// Vycisti cely hlavny zasobnik
bool EmptyMainStack (MainStack *S)
{
  return(S->top==0);
}
////////////////////////////////////////////////////////////////////////////////////////

