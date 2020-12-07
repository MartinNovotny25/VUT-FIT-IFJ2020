/*
*  Predmet: Formalne jazyky a prekladace (IFJ) - FIT VUT v Brne
*  Hlavickovy subor semantickej analyzy pre projekt IFJ2020
*  Vytvoril: Peter Cellar - xcella00
*            Andrej Hyros - xhyros00
*  Datum: 10/2020
*/
#ifndef semantika_h
#define semantika_h

#include <stdio.h>
#include "tokenList.h"
#include "symtable.h"





/*
void assign_vals_control(TDLList *L, tBSTNodePtrLocal *node);
void dec_var_control(TDLList *L, tBSTNodePtrLocal *node);
void for_control(TDLList *L, tBSTNodePtrLocal *node);
void if_control(TDLList *L, tBSTNodePtrLocal *node);


*/
void for_control(TDLList *L, tBSTNodePtrLocal *node, functionData params);
void enter_for_body(TDLList *L, tBSTNodePtrLocal *fornode, functionData params, char *id);
void enter_else_body(TDLList *L, tBSTNodePtrLocal *elsenode, functionData params, char *id);
void enter_if_body(TDLList *L, tBSTNodePtrLocal *funcnode, functionData params, char *id);
void assign_vals_control(TDLList *L, tBSTNodePtrLocal *node, functionData params);
void dec_var_control(TDLList *L, tBSTNodePtrLocal *node, functionData params);
void printFunction(char *id, functionData data);


void checkReturnStatement(TDLList *L, char *id, tBSTNodePtrLocal *node);
void checkCallFunction(TDLList *L, char *id, tBSTNodePtrLocal *node);
void secondRun(TDLList *L);
void insertBuiltInFunction();
void paramsRedefinitionCheck(functionData data, TDLList *L);
void enterFunctionBody(TDLList *L, char *id, MainStack mainstack);
void checkFunctionParams(TDLList *L, char *id);
void checkFunction(TDLList *L);
void goThroughList(TDLList *L);
#endif

/*
 #define ERR_LEX 1
 #define ERR_SYN 2
 #define ERR_SEM_UNDEF 3
 #define ERR_SEM_DATATYPE 4
 #define ERR_SEM_EXCOMPAT 5
 #define ERR_SEM_RETURN 6
 #define ERR_SEM_OTHER 7
 #define ERR_SEM_ZERODIV 9
 #define ERR_INTERN 99
 
 t_INT_ID 24
 t_FLOAT64 23
 t_STRING_ID 20
 
 #define START 0
 #define t_LESS 1
 #define t_GREATER 2
 #define t_ASSIGN 3
 #define EXLAMATION 4
 #define t_COMMA 5
 #define t_LEFT_BRACKET 6
 #define t_RIGHT_BRACKET 7
 #define t_DIVIDE 8
 #define t_MULTIPLY 9
 #define t_PLUS 10
 #define t_MINUS 11
 #define ONE_LINE_COMMENT_1 12
 #define ONE_LINE_COMMENT_2 13
 #define IND_DED 14
 #define t_INT_ZERO 16
 #define t_INT_NON_ZERO 17
 #define STRING_START 18
 #define t_IDENTIFIER 19
 #define t_STRING_ID 20
 #define t_ELSE 21
 #define t_IF 22
 #define t_FLOAT64 23
 #define t_INT_ID 24
 #define t_RETURN 25
 #define t_FOR 26
 #define t_PACKAGE_MAIN 27
 #define t_FUNC 28
 #define t_EOF 29
 #define t_LESSOREQUAL 30
 #define t_GREATEROREQUAL 31
 #define t_EQUAL 32
 #define t_NOT_EQUAL 33
 #define COMMENT1 34
 #define COMMENT2 35
 #define COMMENT3 36
 #define MULTILINE_1 37
 #define MULTILINE_2 38
 #define DOT 39
 #define EXPONENT 40
 #define t_FLOAT 41
 #define ES_STRING 42
 #define t_STRING 43
 #define STRING_HEXA1 44
 #define STRING_HEXA2 45
 #define PLUS_MINUS_EXPONENT 46
 #define t_EOL 47
 #define EXPONENT2 48
 #define t_DEFINITION_1 49
 #define t_DEFINITION_2 50
 #define t_BRACES_R 51
 #define t_BRACES_L 52
 #define EXP_START 53
 #define EXP_END 54
 #define HELP 55
 #define T_SEMICOLON 56
 #define ZERO_EXPONENT 57
 #define BINARY 58
 #define OCTAL 59
 #define HEXADECIMAL 60
 */
