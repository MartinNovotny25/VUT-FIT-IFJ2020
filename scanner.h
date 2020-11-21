/*
**    Part of IFJ Project 2020/2021
**    Author: Simon Fenko xfenko01
*/

#ifndef scanner_h
#define scanner_h

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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


typedef struct TOKEN
{
    char* lex;
    int type;
}TOKEN;

//mazanie current_tokenu
void delete_string(void);

/*najprv vytiahne znak zo suboru a hodi ho do current_char a prilepi ho
 na koniec stringu, do ktoreho sa uklada sučasny načitavany token, zvyši
 poziciu stringu (dlžku stringu a zakonči ho 0 )
*/
void load_c(FILE* text);

/* zavola sa unload a zaroven spravi ungetc - hladačik sa posunie o jedno
dozadu a odreze posledny character zo stringu c current_token */
void unload_c(FILE *text);

/* alokovanie miesta pre string v tokene */
void alloc(int length, TOKEN* token);

/* najprv alokujeme miesto pre textoy obsah tokenu, potom ho tam skopirujeme
 z current_token, potom skopirujeme type a vymazeme current_token */
void end_token(int type, TOKEN* token);

TOKEN get_next_token(FILE* text);


#endif /* scanner_h  */
