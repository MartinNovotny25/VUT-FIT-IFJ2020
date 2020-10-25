/* 
**	Part of IFJ Project 2019/2020
**	Author: Samuel Stuchly xstuch06
*/

#include "symtable.h"


// TODO : fix documentation

tBSTNodePtr GlobalBody ;
tBSTNodePtrLocal LocalVariables;
tBSTNodePtrLocal  GFDefinedVarsTree;
tBSTNodePtrLocal  LFDefinedVarsTree;


void BSTInit (tBSTNodePtr *RootPtr) {
/*   -------
** Function inicializes tree before its first use.
**/
	*(RootPtr) = NULL;

}	

bool BSTSearch (tBSTNodePtr RootPtr, char* name, global_t *Content)	{
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
			return BSTSearch(RootPtr->LPtr,name,Content);
				
		}
		else if (strcmp(name, RootPtr->Name) > 0)
		{
			return BSTSearch(RootPtr->RPtr,name,Content);
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


void BSTInsert (tBSTNodePtr* RootPtr, char* name, global_t Content)	{	
/*   ---------
** Vloží do stromu RootPtr hodnotu Content s klíčem K.
**
** Pokud již uzel se zadaným klíčem ve stromu existuje, bude obsah uzlu
** s klíčem K nahrazen novou hodnotou. Pokud bude do stromu vložen nový
** uzel, bude vložen vždy jako list stromu.
**
** Funkci implementujte rekurzivně. Nedeklarujte žádnou pomocnou funkci.
**
** Rekurzivní implementace je méně efektivní, protože se při každém
** rekurzivním zanoření ukládá na zásobník obsah uzlu (zde integer).
** Nerekurzivní varianta by v tomto případě byla efektivnější jak z hlediska
** rychlosti, tak z hlediska paměťových nároků. Zde jde ale o školní
** příklad, na kterém si chceme ukázat eleganci rekurzivního zápisu.
**/
	
				


	if (*RootPtr == NULL)
	{
		*RootPtr = malloc(sizeof (struct tBSTNode));	
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
		BSTInsert(&(*RootPtr)->LPtr,name,Content);
	}
	else if (strcmp(name, (*RootPtr)->Name) > 0)
	{
		BSTInsert(&(*RootPtr)->RPtr,name,Content);
	}
	
	
}


void BSTDispose (tBSTNodePtr *RootPtr) {	
/*   
** Deletes entire binary tree and correctly frees allocated memory.
**/	


	if (*RootPtr != NULL)					
	{
		BSTDispose(&(*RootPtr)->LPtr);
		BSTDispose(&(*RootPtr)->RPtr);
		free(*RootPtr);
		*RootPtr = NULL;
	}
	


}


// ========================================= LOCAL TREE ==================================================== //

void BSTInitLocal (tBSTNodePtrLocal *RootPtr) {
	
	*(RootPtr) = NULL;

}	

bool BSTSearchLocal (tBSTNodePtrLocal RootPtr, char* name)	{



	if (RootPtr != NULL && RootPtr->Name!= NULL)
	{
		if ((strcmp(name, RootPtr->Name) < 0))
		{
			return BSTSearchLocal(RootPtr->LPtr,name);
				
		}
		else if (strcmp(name, RootPtr->Name) > 0)
		{
			return BSTSearchLocal(RootPtr->RPtr,name);
		}
		else
		{
			return true;								
		}
	}
	else 
	{
		return false;
	}
} 
 


void BSTInsertLocal (tBSTNodePtrLocal* RootPtr, char * name)	{	

				

	if (*RootPtr == NULL)
	{
		*RootPtr = malloc(sizeof (struct tBSTNodeLocal));	
		(*RootPtr)->Name = name;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL;	
	}

	else if (strcmp(name, (*RootPtr)->Name) == 0)
	{		
		// this state should never be called since parser should check if local var is in tree
		// before trying to insert it again.
		// TODO :  when parser is done this should be deleted becase it is a dead code !!!								
		printf("THIS CODE SHOULD NEVER BE REACHED!\n"); 
		
	}
	else if (strcmp(name, (*RootPtr)->Name) < 0)     	
	{
		BSTInsertLocal(&(*RootPtr)->LPtr,name);
	}
	else if (strcmp(name, (*RootPtr)->Name) > 0)
	{
		BSTInsertLocal(&(*RootPtr)->RPtr,name);
	}
	
	
	
}


void BSTDisposeLocal (tBSTNodePtrLocal *RootPtr) {	



	if (*RootPtr != NULL)					
	{
		BSTDisposeLocal(&(*RootPtr)->LPtr);
		BSTDisposeLocal(&(*RootPtr)->RPtr);
		free(*RootPtr);
		*RootPtr = NULL;
	}
	


}



