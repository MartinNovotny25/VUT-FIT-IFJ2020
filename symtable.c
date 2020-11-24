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
 


void BSTInsertLocal (tBSTNodePtrLocal* RootPtr, char * Name, char *Type, char *Data)	{	

				

	if (*RootPtr == NULL)
	{
		*RootPtr = malloc(sizeof (struct tBSTNodeLocal));	
		(*RootPtr)->Name = Name;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL;	
		(*RootPtr)->Type = Type;
		(*RootPtr)->Data = Data;
	}

	else if (strcmp(Name, (*RootPtr)->Name) == 0)
	{		
		// this state should never be called since parser should check if local var is in tree
		// before trying to insert it again.
		// TODO :  when parser is done this should be deleted becase it is a dead code !!!								
		printf("THIS CODE SHOULD NEVER BE REACHED!\n"); 
		
	}
	else if (strcmp(Name, (*RootPtr)->Name) < 0)     	
	{
		BSTInsertLocal(&(*RootPtr)->LPtr,Name, Type, Data);
	}
	else if (strcmp(Name, (*RootPtr)->Name) > 0)
	{
		BSTInsertLocal(&(*RootPtr)->RPtr,Name, Type, Data);
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



