/*
*  Predmet: Formalne jazyky a prekladace (IFJ) - FIT VUT v Brne
*  Zdrojovy subor tabulky symbolov(sucast semantickej analyzy) pre projekt IFJ2020
*  Vytvoril: Peter Cellar - xcella01
*
*  Datum: 10/2020
*/
#include "symtable.h"


// ========================================= GLOBALNY STROM ============================================== //

/*
 - tato funkcia inicializuje globalny strom
 @param RootPtr ukazatel na uzol globalneho stromu
 */
void BSTInitGlobal (tBSTNodePtrGlobal *RootPtr) {

	*(RootPtr) = NULL;

}	

/*
 - tato funkcia prehladava globalny strom a hlada zadany kluc == name
 - ak je uzol s takym klucom najdeny, vracia sa TRUE a vracia obsah uzlu v premennej Content
 - ak uzol s danym klucom nie je najdeny, vracia FALSE a obsah premennej Content neni definovany
 @param RootPtr ukazatel na uzol globalneho stromu
 @param name hladany kluc(retazec)
 @param Content vracia data v pripade najdenia kluca
 */
bool BSTSearchGlobal (tBSTNodePtrGlobal RootPtr, char* name,functionData *Content)	{

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

/*
 - tato funkcia vlozi uzol s danym klucom a datamy do globalneho stromu
 @param RootPtr ukazatel na uzol globalneho stromu
 @param name kluc vkladaneho uzla
 @param Content data uzla
 */
void BSTInsertGlobal (tBSTNodePtrGlobal* RootPtr, char* name, functionData Content)	{

    
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
		(*RootPtr)->content = Content;

	}
	else if (strcmp(name, (*RootPtr)->Name) < 0)     	
	{
		BSTInsertGlobal(&(*RootPtr)->LPtr,name,Content);
	}
	else if (strcmp(name, (*RootPtr)->Name) > 0)
	{
		BSTInsertGlobal(&(*RootPtr)->RPtr,name,Content);
	}
}

/*
 - tato funkcia rekurzivne vymaze cely globalny strom a korektne uvolni pamat
 @param RootPtr ukazatel na uzol globalneho stromu
*/
void BSTDisposeGlobal (tBSTNodePtrGlobal *RootPtr) {

	if (*RootPtr != NULL)					
	{
		BSTDisposeGlobal(&(*RootPtr)->LPtr);
		BSTDisposeGlobal(&(*RootPtr)->RPtr);
		free(*RootPtr);
		*RootPtr = NULL;
	}
}

// ========================================= LOKALNY STROM ============================================== //

/*
- tato funkcia inicializuje lokalny strom
@param RootPtr ukazatel na uzol lokalneho stromu
*/
void BSTInitLocal (tBSTNodePtrLocal *RootPtr) {
	
	*(RootPtr) = NULL;

}	

/*
- tato funkcia prehladava lokalny strom a hlada zadany kluc == name
- ak je uzol s takym klucom najdeny, vracia sa TRUE a vracia data v premennych Content a Type
- ak uzol s danym klucom nie je najdeny, vracia FALSE a obsah premennej Content a Type neni definovany
@param RootPtr ukazatel na uzol lokalneho stromu
@param name hladany kluc(retazec)
@param Type vracani typ ulozeneho identifikatora
@param Content vracane data v pripade najdenia kluca
*/
bool BSTSearchLocal (tBSTNodePtrLocal RootPtr, char* name, int *Type, char *Data)    {


    if (RootPtr != NULL && RootPtr->Name != NULL)
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
 

/*
- tato funkcia vlozi uzol s danym klucom a datamy do lokalneho stromu
@param RootPtr ukazatel na uzol lokalneho stromu
@param name kluc vkladaneho uzla
@param Type typ ukladaneho identifikatora
@param Content data uzla
*/
void BSTInsertLocal (tBSTNodePtrLocal* RootPtr, char * Name, int *Type, char *Data)	{


	if (*RootPtr == NULL){
		*RootPtr = malloc(sizeof (struct tBSTNodeLocal));	
		(*RootPtr)->Name = Name;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL;	
		(*RootPtr)->Type = *Type;
		(*RootPtr)->Data = Data;
	
	}else if (strcmp(Name, (*RootPtr)->Name) == 0)
	{
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


/*
 - pomocna funkcia na vyhladanie, presun a uvolnenie najpravejsieho uzlu
 @param PtrReplaced ukazatel na uzol, do ktoreho bude presunuta hodnota najpravejsieho uzlu v postrome
 @param RootPtr ukazatel na podstrom(uzol), kde bude hladany najpravejsi uzol
 */
void ReplaceByRightmost (tBSTNodePtrLocal PtrReplaced, tBSTNodePtrLocal *RootPtr) {

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
/*
 - tato funkcia zrusi uzol stromu, ktory obsahuje kluc K
 @param RootPtr ukazatel na uzol lokalneho stromu
 @param K hladany kluc(retazec)
 */
void BSTDelete (tBSTNodePtrLocal *RootPtr, char *K) {

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

/*
- tato funkcia rekurzivne vymaze cely lokalny strom a korektne uvolni pamat
@param RootPtr ukazatel na uzol lokalneho stromu
*/
void BSTDisposeLocal (tBSTNodePtrLocal *RootPtr) {	


	if (*RootPtr != NULL)					
	{
		BSTDisposeLocal(&(*RootPtr)->LPtr);
		BSTDisposeLocal(&(*RootPtr)->RPtr);
		free(*RootPtr);
		*RootPtr = NULL;
	}
	


}

// ========================================= ZASOBNIK STROMOV ============================================== //


/*
 - tato funkcia inicializuje zasobnik ukazatelov na lokalne stromy
 */
void InitMainStack (MainStack *S)
{
	S->top = 0;
}


/*
 - tato funkcia vlozi ukazatel na lokalny strom do zasobnika
 @param S zasobnik ukazatelov na lokalne stromy
 @param ptrLocal vkladany ukazatel
 */
void PushTreeMain (MainStack *S, tBSTNodePtrLocal *ptrLocal)
{

    
  if (S->top==MAXSTACK)
   printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {
		S->top++;
		S->a[S->top]=ptrLocal;
		printf("top %d\n", S->top);
	}
}

/*
 - tato funkcia odstrani vrchny ukazatel zo zasobnika
 @param S zasobnik ukazatelov na lokalne stromy
 */
tBSTNodePtrLocal PopTreeMain (MainStack *S)
{
	tBSTNodePtrLocal temp;
	temp = *(S->a[S->top]);
	S->top--;
	printf("top %d\n", S->top);
	return temp;
}

/*
 - tato funkcia odstrani cely obsah zasobnika
 @param S zasobnik ukazatelov na lokalne stromy
 */
bool EmptyMainStack (MainStack *S)
{
  return(S->top==0);
}

/* ==========================================================================================
 - tato funkcia prehlada zasobnik stromov, vracia true ak najde zhodu a v premennej type vracia typ
 @param stack zasobnik, ktory sa prehlada
 @param id identifikator hladanej premennej
 @param type tymto ukazatelom sa vracia datovy typ
 */
bool MainStackSearch(MainStack stack, char* id, int *type) {
    
    //pocet stromov v zasobniku
    int i = stack.top;
    //na ulozenie typu
    int typ = 0;
    char* unneeded = NULL;
    
    while (i != 0) {
        if (BSTSearchLocal((*stack.a[i]), id, &typ, unneeded)) {
            *type = typ;
            return true;
        }
        i--;
    }
    return false;
}



////////////////////////////////////////////////////////////////////////////////////////

