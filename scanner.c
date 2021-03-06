/*
*  Predmet: Formalne jazyky a prekladace (IFJ) - FIT VUT v Brne
*  Zdrojovy subor lexikalnej analyzy pre projekt IFJ2020
*  Vytvoril: Simon Fenko - xfenko01
*
*  Datum: 10/2020
*/
#include "scanner.h"

bool First_token = false;
bool non_zero_int = false;
bool zero_int = false;
bool floating_point = false;
char current_token[100];
int current_token_position = 0;
char current_char;
char current_char2;
char current_char3;
char current_char4;
char current_char5;
/* Vymazavanie current_tokenu (napr. chodia medzery-vtedy ich vyhodime a
pokracujeme dalej)*/

/*
int main()
{
    TOKEN a;
    while (1)
    {
        //TOKEN a;
        a = get_next_token(stdin);
        //printf("%d\n", a.type );
        if (a.type == t_EOF)
        {
            break;
        }
        
    }
    return 0;
}
*/

//Funkcia na odstranovanie znaku '_'
void remove_(void)
{
    current_token[current_token_position] = 0x00;
    current_token_position--;
    current_token[current_token_position] = 0x00;
}

void delete_string(void)
{
    current_token[0] = 0x00;
    current_token_position = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*najprv vytiahne znak zo suboru a hodi ho do current_char a prilepi ho
 na koniec stringu, do ktoreho sa uklada sučasny načitavany token, zvyši
 poziciu stringu (dlžku stringu a zakonči ho 0)
*/
void load_c(FILE* text)
{
    current_char = getc(text);
    current_token[current_token_position] = current_char;
    current_token_position++;
    current_token[current_token_position] = 0x00;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* zavola sa unload a zaroven spravi ungetc - hladačik sa posunie o jedno
 dozadu a odreze posledny character zo stringu c current_token */
void unload_c(FILE *text)
{
    ungetc(current_char,text);
    current_token_position--;
    current_token[current_token_position] = 0x00;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* alokovanie miesta pre string v tokene */

void alloc(int length, TOKEN* token)
{
    token->lex = (char*) malloc(length * sizeof(char));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* najprv alokujeme miesto pre textoy obsah tokenu, potom ho tam skopirujeme
 z current_token, potom skopirujeme type a vymazeme current_token*/

void end_token(int type, TOKEN* token)
{
    alloc(current_token_position+1, token);
    strcpy(token->lex, current_token);
    token->type = type;
    delete_string();
}

TOKEN get_next_token(FILE* text)
{
    TOKEN token;
    static int state;
    if(state != IND_DED)
    {
        state = START;
    }
    
    while (true)
    {
        //nacitavame znaky a v pripade prázdneho súboru nastane EOF
        load_c(text);
        if (current_char == EOF && strlen(current_token) == 1 )
        {
            //TOKEN EOF
            token.type = t_EOF;
            return token;
        }
        
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        switch (state)
        {
            case START:
                //V pripade whitespace
                if (current_char == ' ')
                {
                    state = START;
                    delete_string();
                }
                //V pripade EOL (TAB)
                else if (current_char == 9)
                {
                    state = START;
                    delete_string();
                }
                else if (current_char == '<')
                {
                    //Posuvame sa do medzistavu, pretoze moze z toho byt token <=
                    state = t_LESS;
                }
                else if (current_char == '>')
                {
                    //Posuvame sa do medzistavu, pretoze moze z toho byt token >=
                    state = t_GREATER;
                }
                else if (current_char == '=')
                {
                    //Posume sa do medzistavu, pretoze moze z toho byt token ==
                    state = t_ASSIGN;
                }
                else if (current_char == '!')
                {
                    //Posuvame sa do medzistavu, pretoze moze z toho byt token !=
                    state = EXLAMATION;
                }
                else if (current_char == ',')
                {
                    //Koncime tokenom COMMA
                    end_token(t_COMMA, &token);
                    return token;
                    break;
                }
                else if (current_char == '{')
                {
                    //Koncime tokenom {
                    end_token(t_BRACES_L, &token);
                    return token;
                    break;
                }
                else if (current_char == '}')
                {
                    //Koncime tokenom }
                    end_token(t_BRACES_R, &token);
                    return token;
                    break;
                }
                else if (current_char == '(')
                {
                    //Koncime tokenom (
                    end_token(t_LEFT_BRACKET, &token);
                    return token;
                    break;
                }
                else if (current_char == ')')
                {
                    //koncime tokenom )
                    end_token(t_RIGHT_BRACKET, &token);
                    return token;
                    break;
                }
                else if (current_char == '/')
                {
                    //Posuvame sa do medzistavu, pretoze moze to byt aj token //one line comment
                    state = ONE_LINE_COMMENT_1;
                }
                else if (current_char == '*')
                {
                    //Koncime tokenom *
                    end_token(t_MULTIPLY, &token);
                    return token;
                    break;
                }
                else if (current_char == '+')
                {
                    //Koncime token +
                    end_token(t_PLUS, &token);
                    return token;
                    break;
                }
                else if (current_char == ';')
                {
                    //Koncime tokenom ;
                    end_token(T_SEMICOLON, &token);
                    return token;
                    break;
                }
                else if (current_char == '-')
                {
                    //Koncime tokenom -
                    end_token(t_MINUS, &token);
                    return token;
                    break;
                }
                else if (current_char == ':')
                {
                    //Posuvame sa do medzistavu, pretoze moze nastat token :=
                    state = t_DEFINITION_1;
                }
                else if (current_char == '0')
                {
                    //Posuvame sa do medzistavu, pretoze po cisle 0 moze prist aj desatinna cast
                    state = t_INT_ZERO;
                }
                else if ((isdigit(current_char)) && (current_char != '0'))
                {
                    //Posuvame sa do medzistavu, pretoze moze byt viacciferne cislo alebo desatinne
                    state = t_INT_NON_ZERO;
                }
                else if (current_char == '"')
                {
                    //Posuvame sa do medzistavu zaciatok stringu
                    state = STRING_START;
                }
                else if ((isalpha(current_char)) || (current_char == '_'))
                {
                    //Posuvame sa do medzistavu identifikator, pretoze moze by viacznakovy identifikator
                    state = t_IDENTIFIER;
                }
                else if (current_char == '\n')
                {
                    //Koncime tokenom EOL
                    end_token(t_EOL, &token);
                    First_token = true;
                    return token;
                }
                else
                {
                    //Nepovolene znaky koncia lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                break;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            case t_LESS:
                if (current_char == '=')
                {
                    //Koncime tokenom <=
                    end_token(t_LESSOREQUAL, &token);
                    return token;
                }
                else
                {
                    //V pripade nepovoleneho znaku, koncime tokenom <
                    unload_c(text);
                    end_token(t_LESS, &token);
                    return token;
                }
                break;

            case t_GREATER:
                if (current_char == '=')
                {
                    //Koncime tokenom >=
                    end_token(t_GREATEROREQUAL, &token);
                    return token;
                }
                else
                {
                    //V pripade nepovoleneho znaku, koncime tokenom >
                    unload_c(text);
                    end_token(t_GREATER, &token);
                    return token;
                }
                break;
            
            case t_ASSIGN:
                if (current_char == '=')
                {
                    //Koncime tokenom ==
                    end_token(t_EQUAL, &token);
                    return token;
                }
                else
                {
                    //V pripade nepovoleneho znaku, koncime tokenom =
                    unload_c(text);
                    end_token(t_ASSIGN, &token);
                    return token;
                }
                break;
            
            case EXLAMATION:
                if (current_char == '=')
                {
                    //Koncime tokenom !=
                    end_token(t_NOT_EQUAL, &token);
                    return token;
                }
                else if(current_char != EOF)
                {
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    unload_c(text);
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                break;
            
            case t_DEFINITION_1:
                if (current_char == '=')
            {
                //Koncime tokenom :=
                end_token(t_DEFINITION_2, &token);
                return token;
            }
            else
            {
                //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                fprintf(stderr , "Lexical error.\n");
                exit(1);
            }
            break;
            
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
            //Stav identifikatoru
            case t_IDENTIFIER:
                if (isalpha(current_char) || isdigit(current_char) || current_char == '_')
                {
                    //Ostavame v stave identifikatoru
                    state = t_IDENTIFIER;
                }
                else
                {
                    if(!strcmp(current_token, "package "))
                    {
                        state = t_PACKAGE_CONTROLL;
                    }
                    else{
                        unload_c(text);
                        if(!strcmp(current_token, "if"))
                        {
                            end_token(t_IF, &token);
                        }
                        else if(!strcmp(current_token, "for"))
                        {
                            end_token(t_FOR, &token);
                        }
                        else if(!strcmp(current_token, "string"))
                        {
                            end_token(t_STRING_ID, &token);
                        }
                        else if(!strcmp(current_token, "else"))
                        {
                            end_token(t_ELSE, &token);
                        }
                        else if(!strcmp(current_token, "return"))
                        {
                            end_token(t_RETURN, &token);
                        }
                        else if(!strcmp(current_token, "float64"))
                        {
                            end_token(t_FLOAT64, &token);
                        }
                        else if(!strcmp(current_token, "int"))
                        {
                            end_token(t_INT_ID, &token);
                        }
                        else if(!strcmp(current_token, "package"))
                        {
                            end_token(t_PACKAGE, &token);
                        }
                        else if(!strcmp(current_token, "func"))
                        {
                            end_token(t_FUNC, &token);
                        }
                        else{
                            end_token(t_IDENTIFIER, &token);
                        }
                        return token;
                    }
                }
                break;
            
            //Osetrenie Package main
            case t_PACKAGE_CONTROLL:
                if (current_char == 'm')
                {
                    //Ked pride znak 'm' za stringom "package " ideme do medzistavu, aby sme skontrolovali ci nasledujuci bude 'a'
                    state = t_PACKAGE_CONTROLL1;
                }
                else
                {
                    //V pripade, že nepride znak 'm', tak unloadneme prichadzajuci znak a vratime token t_PACKAGE a riesime dalej prichadzajuci znak
                    unload_c(text);
                    ungetc(' ',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    end_token(t_PACKAGE, &token);
                    return token;
                }
                break;
            
            case t_PACKAGE_CONTROLL1:
                if (current_char == 'a')
                {
                    //Ked pride znak 'a' za stringom "package " ideme do medzistavu, aby sme skontrolovali ci nasledujuci bude 'i'
                    current_char2 = current_char;
                    state = t_PACKAGE_CONTROLL2;
                }
                else
                {
                    //V pripade, že nepride znak 'a', tak unloadneme prichadzajuce znaky a vratime token t_PACKAGE a riesime dalej prichadzajuce znaky
                    unload_c(text);
                    ungetc('m',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    ungetc(' ',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    end_token(t_PACKAGE, &token);
                    return token;
                }
                break;
                
            case t_PACKAGE_CONTROLL2:
                if (current_char == 'i')
                {
                    //Ked pride znak 'i' za stringom "package " ideme do medzistavu, aby sme skontrolovali ci nasledujuci bude 'n'
                    current_char3 = current_char;
                    state = t_PACKAGE_CONTROLL3;
                }
                else
                {
                    //V pripade, že nepride znak 'i', tak unloadneme prichadzajuce znaky a vratime token t_PACKAGE a riesime dalej prichadzajuce znaky
                    unload_c(text);
                    ungetc('a',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    ungetc('m',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    ungetc(' ',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    end_token(t_PACKAGE, &token);
                    return token;
                }
                break;
                
            case t_PACKAGE_CONTROLL3:
                if (current_char == 'n')
                {
                    //Ked pride znak 'n' za stringom "package " ideme do medzistavu, aby sme skontrolovali ci nasledujuci nahodou nebude cislo pismeno alebo '_'
                    current_char4 = current_char;
                    state = t_PACKAGE_CONTROLL4;
                }
                else
                {
                    //V pripade, že nepride znak 'n', tak unloadneme prichadzajuce znaky a vratime token t_PACKAGE a riesime dalej prichadzajuce znaky
                    unload_c(text);
                    ungetc('i',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    ungetc('a',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    ungetc('m',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    ungetc(' ',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    end_token(t_PACKAGE, &token);
                    return token;
                }
                break;
                
            case t_PACKAGE_CONTROLL4:
                if (isalpha(current_char) || isdigit(current_char) || current_char == '_')
                {
                    //V pripade ak je nasledujuci znak pismeno/cislica alebo '_', tak unloadneme znaky a vratime token t_PACKAGE a riesime unloadnute znaky
                    unload_c(text);
                    ungetc('n',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    ungetc('i',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    ungetc('a',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    ungetc('m',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    ungetc(' ',text);
                    current_token_position--;
                    current_token[current_token_position] = 0x00;
                    end_token(t_PACKAGE, &token);
                    return token;
                }
                else
                {
                    //V pripade ak nepride cislica/pismeno ani '_', tak vraciame token t_PACKAGE_MAIN, pretoze sme dosiahli, ze nam prislo "package main"
                    unload_c(text);
                    end_token(t_PACKAGE_MAIN, &token);
                    return token;
                }
                break;
                
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
            //ONE LINE COMMENT WITH //
            case ONE_LINE_COMMENT_1:
                if (current_char == '/')
                {
                    //Presuvame sa do stavu //One line comment
                    state = ONE_LINE_COMMENT_2;
                }
                else if (current_char == '*')
                {
                    //Presuvame sa do stavu /* multiline comment zaciatok
                    state = MULTILINE_1;
                }
                else
                {
                    //V pripade nepovoleneho znaku, koncime tokenom /
                    unload_c(text);
                    end_token(t_DIVIDE, &token);
                    return token;
                }
                break;
                
            case ONE_LINE_COMMENT_2:
                if (current_char == '\n')
                {
                    //Koncime tokenom EOL (ONE LINE COMMENT)
                    First_token = true;
                    //Zmazeme komentar
                    delete_string();
                    //A vraciame sa na pociatocny stav
                    state = START;
                    //Koncime cyklus
                    break;
                }
                else if (current_char == EOF)
                {
                    //Koncime tokenom EOL (ONE LINE COMMENT)
                    unload_c(text);
                    //Zmazeme komentar
                    delete_string();
                    //A vraciame sa na pociatocny stav
                    state = START;
                    //Koncime cyklus
                    break;
                }
                if (current_char == '\r')
                {
                    //Koncime tokenom EOL (ONE LINE COMMENT)
                    First_token = true;
                    //Zmazeme komentar
                    delete_string();
                    //A vraciame sa na pociatocny stav
                    state = START;
                    //Koncime cyklus
                    break;
                }
                else
                {
                    //Ostavame v stave //One line comment
                    state = ONE_LINE_COMMENT_2;
                }
                break;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
           
            // MULTILINE COMMENT WITH /*
            case MULTILINE_1:
                if (current_char == '*')
                {
                    //Ostavame v stave /* multiline comment *
                    state = MULTILINE_2;
                }
                else
                {
                    //Ostavame v stave multiline1 /*
                    state = MULTILINE_1;
                }
                break;
            case MULTILINE_2:
                if (current_char == '/')
                {
                    //Koncime tokenom Multiline Comment /* */
                    First_token = true;
                    //Zmazeme komentar
                    delete_string();
                    //A vraciame sa na pociatocny stav
                    state = START;
                    //Koncime cyklus
                    break;
                }
                else
                {
                    //Ostavame v stave multiline commentu /* *
                    state = MULTILINE_1;
                }
                break;
            
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
            // okrem nuly
            case t_INT_NON_ZERO:
                non_zero_int = true;
                if (current_char == '.')
                {
                    //Presuvame sa do medzistavu desatinnej bodky, pretoze za desatinnou bodkou musia nasledovat cisla
                    state = DOT;
                }
                else if ((current_char == 'e') || (current_char == 'E'))
                {
                    //Presuvame sa do medzistavu exponenta, pretoze za znakom e/E musi nasledovat cislo alebo znak +/-
                    current_char2 = current_char;
                    state = EXPONENT;
                }
                else if (isdigit(current_char))
                {
                    //Ostavame v stave INT_NON_ZERO, pretoze token moze byt viacciferny
                    state = t_INT_NON_ZERO;
                }
                else if (current_char == '_')
                {
                    //Presunieme sa do pomocneho stavu, v ktorom zistime ci je v podtrzitko povolene
                    state = UNDERSCORE;
                }
                else
                {
                    //V pripade nepovoleneho znaku koncime tokenom nenuloveho cisla a nepovoleny znak skenujeme dalej
                    unload_c(text);
                    end_token(t_INT_NON_ZERO, &token);
                    non_zero_int = false;
                    return token;
                }
                break;
            
            // zacina nulou
            case t_INT_ZERO:
                zero_int = true;
                if ((current_char == 'e') || (current_char == 'E'))
                {
                    //Presuvame sa do medzistavu exponent, pretoze za znakom e/E musi nasledovat cislo alebo znak +/-
                    current_char2 = current_char;
                    state = EXPONENT;
                }
                else if (current_char == '.')
                {
                    //Presuvame sa do medzistavu desatinnej bodky, pretoze za nou musi nasledovat cislo
                    state = DOT;
                }
                else if ((current_char == 'b') || (current_char == 'B'))
                {
                    //Presuvame sa do medzistavu binarnej sustavy, pretoze za znakmi 0b/0B moze nasledovať znak 0 alebo znak 1
                    state = BINARY;
                }
                else if ((current_char == 'o') || (current_char == 'O'))
                {
                    //Presuvame sa do medzistavu osmickovej sustavy, pretoze za znakmi 0o/0O mozu nasledovat len cifry
                    state = OCTAL;
                }
                else if ((current_char == 'x') || (current_char == 'X'))
                {
                    //Presuvame sa do medzistavu sestnastkovej sustavy, pretoze po znakoch 0x/0X mozu nasledovat len cifry 0-1 alebo pismena A-F,a-f
                    state = HEXADECIMAL;
                }
                else if (current_char == '_')
                {
                    //Presunieme sa do pomocneho stavu, v ktorom zistime ci je v podtrzitko povolene
                    state = UNDERSCORE;
                }
                else if (current_char == '0')
                {
                    //V pripade nepovoleneho znaku (cislom), koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else if (isdigit(current_char))
                {
                    //V pripade nepovoleneho znaku (cislom), koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else
                {
                    //V pripade nepovoleneho znaku koncime tokenom INT_ZERO a skenujeme nepovoleny znak
                    unload_c(text);
                    end_token(t_INT_ZERO, &token);
                    zero_int = false;
                    return token;
                }
                break;
                
            case UNDERSCORE:
                if (isdigit(current_char))
                {
                    //V pripade, ze pride cislo tak unloadneme
                    unload_c(text);
                    //odstranime podtrzitko
                    remove_();
                    //Ked je hodnota nastavena na true, vraciame sa do nuloveho stavu
                    if (zero_int)
                    {
                        state = t_INT_ZERO;
                    }
                    //Inak sa presuvame do nenuloveho stavu
                    else
                    {
                        state = t_INT_NON_ZERO;
                    }
                    
                }
                else
                {
                    //V pripade nepovoleneho znaku (cislom), koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                
            case DOT:
                if (isdigit(current_char))
                {
                    //Presuvame sa do medzistavu FLOAT, pretoze desatinna cast moze byt viacciferna
                    zero_int = false;
                    non_zero_int = false;
                    state = t_FLOAT;
                }
                else if (current_char == '_')
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                    //Odstranime znak '_', pretoze pri cislach znak '_' ignorujeme
                    //remove_();
                }
                else if(current_char != EOF)
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    unload_c(text);
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                
                break;
                
            case t_FLOAT:
                floating_point = true;
                if (current_char == 'e' || current_char == 'E')
                {
                    //presuvame sa do medzistavu exponent, pretoze za znakmi e/E musi nasledovat cislo alebo znak +/-
                    current_char2 = current_char;
                    state = EXPONENT;
                }
                else if (isdigit(current_char))
                {
                    //ostavame v stave FLOAT, pretoze desatinna cast cisla moze byt viacciferna
                    state = t_FLOAT;
                }
                else if (current_char == '_')
                {
                    //odstranime znak '_', pretoze pri cislach znak '_' ignorujeme
                    remove_();
                }
                else
                {
                    //V pripade nepovoleneho znaku koncime tokenom FLOAT a skenujeme nepovoleny znak
                    unload_c(text);
                    end_token(t_FLOAT, &token);
                    floating_point = false;
                    return token;
                }
                break;
                
            //exponent
            case EXPONENT:
                
                if (isdigit(current_char))
                {
                    //Ostavame v stave exponent, pretoze za znakom e/E moze byt viac cifier
                    state = EXPONENT2;
                }
                else if (current_char == '+' || current_char == '-'  )
                {
                    //Presuvame sa do medzistavu PLUS_MINUS_EXPONENT, pretoze za znakmi +/- musi nasledovat cislo
                    current_char3 = current_char;
                    state = PLUS_MINUS_EXPONENT;
                }
                else
                {
                    unload_c(text);
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                break;

            //Exponent so znamienkom plus alebo minus
            case PLUS_MINUS_EXPONENT:
                if(isdigit(current_char))
                {
                    //Presuvame sa do medzistavu exponent2, pretoze za znakom +/- moze byt viac cifier
                    state = EXPONENT2;
                }
                else
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                break;
             
            //pokracovanie exponenta so znamienkom
            case EXPONENT2:
                if (isdigit(current_char))
                {
                    //Ostavame v stave exponent2, pretoze za znakom +/- moze byt viac cifier
                    state = EXPONENT2;
                }
                else
                {
                    //V pripade nepovoleneho znaku koncime tokenom FLOAT a skenujeme nepovoleny znak
                    unload_c(text);
                    end_token(t_FLOAT, &token);
                    return token;
                }
                break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///BASE ROZSIRENIE
                
            case BINARY:
                if (current_char == '0')
                {
                    //Presuvame sa do medzistavu, v ktorom zistime ci binarne cislo nie je 0
                    state = BINARY_ZERO;
                }
                else if (current_char == '1')
                {
                    //Binarne cislo uz nemoze byt 0, preto nastavime hodnotu false a ostavame v stave Binary
                    zero_int = false;
                    state = BINARY;
                }
                else if (current_char == '_')
                {
                    //odstranime znak '_', pretoze pri cislach znak '_' ignorujeme
                    remove_();
                }
                else if ((isdigit(current_char)) && (current_char != '0') && (current_char != '1'))
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else if (isalpha(current_char))
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else
                {
                    //V pripade nepovoleneho znaku zistime ci je zero_int false, ak je tak cislo nie je 0
                    if(zero_int==false)
                    {
                        //V pripade nepovoleneho znaku koncime tokenom INT_NON_ZERO a skenujeme nepovoleny znak
                        unload_c(text);
                        end_token(t_INT_NON_ZERO, &token);
                        return token;
                    }
                }
                break;
                
            case BINARY_ZERO:
                if (current_char == '0')
                {
                    //V pripade ze pride 0, ostavame v stave BINARY_ZERO, pretoze binarne cislo moze byt stale 0
                    state = BINARY_ZERO;
                }
                else if (current_char == '1')
                {
                    //V pripade ze prisla 1, nastavime hodnotu false, pretoze binarne cislo uz nemoze byt 0, kedze obsahuje 1
                    zero_int = false;
                    //Vraciame sa do stavu BINARY
                    state = BINARY;
                }
                else if ((isdigit(current_char)) && (current_char != '0') && (current_char != '1'))
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else if (isalpha(current_char))
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else if (current_char == '_')
                {
                    //odstranime znak '_', pretoze pri cislach znak '_' ignorujeme
                    remove_();
                }
                else
                {
                    //V pripade, ze je hodnota true, vieme ze binarne cislo je 0, lebo neobsahuje 1
                    if(zero_int)
                    {
                        //V pripade nepovoleneho znaku koncime tokenom INT_ZERO a skenujeme nepovoleny znak
                        unload_c(text);
                        end_token(t_INT_ZERO, &token);
                        return token;
                    }
                    //V pripade, ze je hodnota false, vieme ze binarne cislo je nenulove, lebo obsahuje 1
                    else if (zero_int == false)
                    {
                        //V pripade nepovoleneho znaku koncime tokenom INT_NON_ZERO a skenujeme nepovoleny znak
                        unload_c(text);
                        end_token(t_INT_NON_ZERO, &token);
                        return token;
                    }
                }
                break;
                
            case OCTAL:
                if ((isdigit(current_char)) && (current_char!='0') && (current_char!='8') && (current_char!='9'))
                {
                    //V pripade ze dostaneme nenulove cislo, nastavime hodnotu false, pretoze vieme ze nebude uz 0
                    zero_int = false;
                    //Ostavame v stave OCTAL, pretoze nemusi byt len jeden znak
                    state = OCTAL;
                }
                else if (current_char == '0')
                {
                    //V pripade ze pride 0, sa presuvame do stavu OCTAL_ZERO, pretoze moze byt nulove cislo
                    state = OCTAL_ZERO;
                }
                else if ((current_char == '8') || (current_char == '9'))
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else if (isalpha(current_char))
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else if (current_char == '_')
                {
                    //odstranime znak '_', pretoze pri cislach znak '_' ignorujeme
                    remove_();
                }
                else
                {
                    //V pripade ze je hodnota false, vieme ze cislo je nenulove, lebo obsahuje iny znak ako 0
                    if(zero_int==false)
                    {
                        //V pripade nepovoleneho znaku koncime tokenom INT_NON_ZERO a skenujeme nepovoleny znak
                        unload_c(text);
                        end_token(t_INT_NON_ZERO, &token);
                        return token;
                    }
                }
                break;
                
            case OCTAL_ZERO:
                if (current_char == '0')
                {
                    //V pripade, ze pride cislica 0, ostavame v stave OCTAL_ZERO, pretoze to stale moze byt 0 cislo
                    state = OCTAL_ZERO;
                }
                else if ((isdigit(current_char)) && (current_char!='0') && (current_char!='8') && (current_char!='9'))
                {
                    //V pripade ze nepride cislica 0, nastavime hodnotu false, pretoze vieme, ze to uz nebude nulove cislo
                    zero_int = false;
                    //Vraciame sa teda do stavu OCTAL
                    state = OCTAL;
                }
                else if ((current_char == '8') || (current_char == '9'))
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else if (isalpha(current_char))
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else if (current_char == '_')
                {
                    //odstranime znak '_', pretoze pri cislach znak '_' ignorujeme
                    remove_();
                }
                else
                {
                    //V pripade, ze je hodnota true, vieme ze cislo je nulove
                    if(zero_int)
                    {
                        //V pripade nepovoleneho znaku koncime tokenom INT_ZERO a skenujeme nepovoleny znak
                        unload_c(text);
                        end_token(t_INT_ZERO, &token);
                        return token;
                    }
                    //V pripade, ze je hodnota false, vieme ze cislo nie je nulove
                    else if (zero_int == false)
                    {
                        //V pripade nepovoleneho znaku koncime tokenom INT_NON_ZERO a skenujeme nepovoleny znak
                        unload_c(text);
                        end_token(t_INT_NON_ZERO, &token);
                        return token;
                    }
                }
                break;
                
            case HEXADECIMAL:
                if (((isdigit(current_char)) || (current_char == 'A') || (current_char == 'a') || (current_char == 'B') || (current_char == 'b') || (current_char == 'C') || (current_char == 'c') || (current_char == 'D') || (current_char == 'd') || (current_char == 'E') || (current_char == 'e') || (current_char == 'F') || (current_char == 'f')) && (current_char!='0'))
                {
                    //V pripade ak pride povoleny znak a nie je to 0, tak nastavime hodnotu false, lebo vieme ze to uz nebude nulove cislo
                    zero_int = false;
                    //Ostavame teda v stave HEXADECIMAL
                    state = HEXADECIMAL;
                }
                else if (current_char == '0')
                {
                    //V pripade, ze pride cislica 0, presuvame sa do stavu HEXA_ZERO, lebo cislo moze byt nulove
                    state = HEXA_ZERO;
                }
                else if ((current_char >='g' && current_char <= 'z') || (current_char >= 'G' && current_char <= 'Z'))
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else if (current_char == '_')
                {
                    //odstranime znak '_', pretoze pri cislach znak '_' ignorujeme
                    remove_();
                }
                else
                {
                    //V pripade cez je hodnota false, vieme ze cislo je nenulove
                    if(zero_int==false)
                    {
                        //V pripade nepovoleneho znaku koncime tokenom INT_NON_ZERO a skenujeme nepovoleny znak
                        unload_c(text);
                        end_token(t_INT_NON_ZERO, &token);
                        return token;
                    }
                }
                break;
             
            case HEXA_ZERO:
                if (current_char == '0')
                {
                    //V pripade ze pride 0, ostavame v stave HEXA_ZERO, lebo cislo moze byt nulove
                    state = HEXA_ZERO;
                }
                else if (((isdigit(current_char)) || (current_char == 'A') || (current_char == 'a') || (current_char == 'B') || (current_char == 'b') || (current_char == 'C') || (current_char == 'c') || (current_char == 'D') || (current_char == 'd') || (current_char == 'E') || (current_char == 'e') || (current_char == 'F') || (current_char == 'f')) && (current_char!='0'))
                {
                    //V pripade ze dostaneme znak ktory neni 0, nastavime hodnotu false, lebo vieme ze cislo nebude nulove
                    zero_int = false;
                    //Presuvame sa do stavu HEXADECIMAL
                    state = HEXADECIMAL;
                }
                else if ((current_char >='g' && current_char <= 'z') || (current_char >= 'G' && current_char <= 'Z'))
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                else if (current_char == '_')
                {
                    //odstranime znak '_', pretoze pri cislach znak '_' ignorujeme
                    remove_();
                }
                else
                {
                    //V pripade ze je hodnota nastavena na true, vieme ze cislo je nulove
                    if(zero_int)
                    {
                        //V pripade nepovoleneho znaku koncime tokenom INT_ZERO a skenujeme nepovoleny znak
                        unload_c(text);
                        end_token(t_INT_ZERO, &token);
                        return token;
                    }
                    //V pripade ze je hodnota nastavena na false, vieme ze cislo je nenulove
                    else if (zero_int == false)
                    {
                        //V pripade nepovoleneho znaku koncime tokenom INT_NON_ZERO a skenujeme nepovoleny znak
                        unload_c(text);
                        end_token(t_INT_NON_ZERO, &token);
                        return token;
                    }
                }
                break;
                    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                
            case STRING_START:
                if (current_char == '"' )
                {
                    //Koncime tokenom STRING ""
                    end_token(t_STRING, &token);
                    return token;
                }
                else if (current_char == 92)
                {
                    //Presuvame sa do medzistavu ES_STRING, pretoze mozu nasledovat vyhradene znaky, potrebne na ES
                    state = ES_STRING;
                }
                else if (current_char == EOF)
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                    
                else if (current_char > 31 && current_char != 39 && current_char != 92)
                {
                    //Ostavame v stave STRING_START, pretoze string moze byt viacznakovy
                    state = STRING_START;
                }
                break;
                
            case ES_STRING:
                if (current_char == 'x')
                {
                    state = STRING_HEXA1;
                }
                else if ((current_char == 'n') || (current_char == 't') || (current_char == 34) || (current_char == 92))
                {
                    state = STRING_START;
                }
                else
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                break;
                
            case STRING_HEXA1:
                if ((current_char >= '0' && current_char <= '9') || (current_char >= 'a' && current_char <= 'f') || (current_char >= 'A' && current_char <= 'F') )
                {
                    state = STRING_HEXA2;
                }
                else
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                break;
                
            case STRING_HEXA2:
                if ((current_char >= '0' && current_char <= '9') || (current_char >= 'a' && current_char <= 'f') || (current_char >= 'A' && current_char <= 'F') )
                {
                    state = STRING_START;
                }
                else
                {
                    //V pripade nepovoleneho znaku, koncime lexikalnou chybou
                    fprintf(stderr , "Lexical error.\n");
                    exit(1);
                }
                break;
        }
    }
}
