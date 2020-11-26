#ifndef semantika_h
#define semantika_h


#include "tokenList.h"
#include "symtable.h"

void goThroughList(TDLList *L);
void assign_vals_control(TDLList *L, tBSTNodePtrLocal *node);
void dec_var_control(TDLList *L, tBSTNodePtrLocal *node);
void if_control(TDLList *L, tBSTNodePtrLocal *node);
void for_control(TDLList *L, tBSTNodePtrLocal *node);

#endif