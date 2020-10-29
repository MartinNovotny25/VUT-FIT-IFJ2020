    /*
   **    Part of IFJ Project 2020/2021
   **    Author: Simon Fenko xfenko01
   */


   #include "stack.h"
   #include "scanner.h"

   bool First_token = false;
   char current_token[100];
   int current_token_position = 0;
   char current_char;
   /* Vymazavanie current_tokenu (napr. chodia medzery-vtedy ich vyhodime a
   pokracujeme dalej)*/

   int main()
   {
       while (1)
       {
           TOKEN a;
           a = get_next_token(stdin);
           //printf("%d\n", a.type );
           if (a.type == t_EOF)
           {
               break;
           }
       }
       return 0;
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
       printf("token %d lex %s\n",token->type, token->lex);
       delete_string();
   }

   TOKEN get_next_token(FILE* text)
   {
       TOKEN token;
       static int state;
       if(state != IND_DED){
           state = START;
       }
       
       while (true)
       {
           load_c(text);
           if (current_char == EOF && strlen(current_token) == 1 )
           {
               token.type = t_EOF;
               return token;
           }
           
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
           switch (state)
           {
               case START:
                   if (current_char == ' ')
                   {
                       state = START;
                       delete_string();
                   }
                   else if (current_char == 9)
                   {
                       state = START;
                       delete_string();
                   }
                   else if (current_char == '<')
                   {
                       state = t_LESS;
                   }
                   else if (current_char == '>')
                   {
                       state = t_GREATER;
                   }
                   else if (current_char == '=')
                   {
                       state = t_ASSIGN;
                   }
                   else if (current_char == '!')
                   {
                       state = EXLAMATION;
                   }
                   else if (current_char == ',')
                   {
                       end_token(t_COMMA, &token);
                       return token;
                       break;
                   }
                   else if (current_char == '{')
                   {
                       end_token(t_BRACES_L, &token);
                       return token;
                       break;
                   }
                   else if (current_char == '}')
                   {
                       end_token(t_BRACES_R, &token);
                       return token;
                       break;
                   }
                   else if (current_char == '(')
                   {
                       end_token(t_LEFT_BRACKET, &token);
                       return token;
                       break;
                   }
                   else if (current_char == ')')
                   {
                       end_token(t_RIGHT_BRACKET, &token);
                       return token;
                       break;
                   }
                   else if (current_char == '/')
                   {
                       state = ONE_LINE_COMMENT_1;
                   }
                   else if (current_char == '*')
                   {
                       end_token(t_MULTIPLY, &token);
                       return token;
                       break;
                   }
                   else if (current_char == '+')
                   {
                       end_token(t_PLUS, &token);
                       return token;
                       break;
                   }
                   else if (current_char == '-')
                   {
                       end_token(t_MINUS, &token);
                       return token;
                       break;
                   }
                   else if (current_char == ':')
                   {
                       state = t_DEFINITION_1;
                   }
                   else if (current_char == '0')
                   {
                       state = t_INT_ZERO;
                   }
                   else if ((isdigit(current_char)) && (current_char != '0'))
                   {
                       state = t_INT_NON_ZERO;
                   }
                   else if (current_char == '"')
                   {
                       state = STRING_START;
                   }
                   else if ((isalpha(current_char)) || (current_char == '_'))
                   {
                       state = t_IDENTIFIER;
                   }
                   else if (current_char == '\n')
                   {
                       end_token(t_EOL, &token);
                       First_token = true;
                       return token;
                   }
                   
                   break;

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

               case t_LESS:
                   if (current_char == '=')
                   {
                       end_token(t_LESSOREQUAL, &token);
                       return token;
                   }
                   else
                   {
                       unload_c(text);
                       end_token(t_LESS, &token);
                       return token;
                   }
                   break;

               case t_GREATER:
                   if (current_char == '=')
                   {
                       end_token(t_GREATEROREQUAL, &token);
                       return token;
                   }
                   else
                   {
                       unload_c(text);
                       end_token(t_GREATER, &token);
                       return token;
                   }
                   break;
               
               case t_ASSIGN:
                   if (current_char == '=')
                   {
                       end_token(t_EQUAL, &token);
                       return token;
                   }
                   else
                   {
                       unload_c(text);
                       end_token(t_ASSIGN, &token);
                       return token;
                   }
                   break;
               
               case EXLAMATION:
                   if (current_char == '=')
                   {
                       end_token(t_NOT_EQUAL, &token);
                       return token;
                   }
                   else
                   {
                       fprintf(stderr , "Lexical error.\n");
                       exit(1);
                   }
                   break;
               
               case t_DEFINITION_1:
                   if (current_char == '=')
               {
                   end_token(t_DEFINITION_2, &token);
                   return token;
               }
               else
               {
                   fprintf(stderr , "Lexical error.\n");
                   exit(1);
               }
               break;
               
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                   
               case t_IDENTIFIER:
                   if (isalpha(current_char) || isdigit(current_char) || current_char == '_')
                   {
                       state = t_IDENTIFIER;
                   }
                   else
                   {
                       if(!strcmp(current_token, "package ") || !strcmp(current_token, "package   ")){
                           state = t_IDENTIFIER;
                       }
                       else{
                           unload_c(text);
                           if(!strcmp(current_token, "if")){
                               end_token(t_IF, &token);
                           }
                           else if(!strcmp(current_token, "for")){
                               end_token(t_FOR, &token);
                           }
                           else if(!strcmp(current_token, "string")){
                               end_token(t_STRING_ID, &token);
                           }
                           else if(!strcmp(current_token, "else")){
                               end_token(t_ELSE, &token);
                           }
                           else if(!strcmp(current_token, "return")){
                               end_token(t_RETURN, &token);
                           }
                           else if(!strcmp(current_token, "float64")){
                               end_token(t_FLOAT64, &token);
                           }
                           else if(!strcmp(current_token, "int")){
                               end_token(t_INT_ID, &token);
                           }
                           else if(!strcmp(current_token, "package main")){
                               end_token(t_PACKAGE_MAIN, &token);
                           }
                           else if(!strcmp(current_token, "func")){
                               end_token(t_FUNC, &token);
                           }
                           else{
                               end_token(t_IDENTIFIER, &token);
                           }
                           return token;
                       }
                   }
                   break;
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                   
               //ONE LINE COMMENT WITH //
               case ONE_LINE_COMMENT_1:
                   if (current_char == '/')
                   {
                       state = ONE_LINE_COMMENT_2;
                   }
                   else if (current_char == '*')
                   {
                       state = MULTILINE_1;
                   }
                   else
                   {
                       unload_c(text);
                       end_token(t_DIVIDE, &token);
                       return token;
                   }
                   break;
                   
               case ONE_LINE_COMMENT_2:
                   if (current_char == '\n')
                   {
                       end_token(t_EOL, &token);
                       First_token = true;
                       return token;
                   }
                   else if (current_char == EOF)
                   {
                       unload_c(text);
                       end_token(t_EOL, &token);
                       return token;
                   }
                   else
                   {
                       state = ONE_LINE_COMMENT_2;
                   }
                   break;

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
              
               // MULTILINE COMMENT WITH /*
               case MULTILINE_1:
                   if (current_char == '*')
                   {
                       state = MULTILINE_2;
                   }
                   else
                   {
                       state = MULTILINE_1;
                   }
                   break;
               case MULTILINE_2:
                   if (current_char == '/')
                   {
                       end_token(t_EOL, &token);
                       First_token = true;
                       return token;
                   }
                   else
                   {
                       state = MULTILINE_1;
                   }
                   break;
               
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                   
               // okrem nuly
               case t_INT_NON_ZERO:
                   if (current_char == '.')
                   {
                       state = DOT;
                   }
                   else if ((current_char == 'e') || (current_char == 'E'))
                   {
                       state = EXPONENT;
                   }
                   else if (isdigit(current_char))
                   {
                       state = t_INT_NON_ZERO;
                   }
                   else
                   {
                       unload_c(text);
                       end_token(t_INT_NON_ZERO, &token);
                       return token;
                   }
                   break;
               
               // zacina nulou
               case t_INT_ZERO:
                   if ((current_char == 'e') || (current_char == 'E'))
                   {
                       state = EXPONENT;
                   }
                   else if (current_char == '.')
                   {
                       state = DOT;
                   }
                   else if (current_char == '0')
                   {
                       fprintf(stderr , "Lexical error.\n");
                       exit(1);
                   }
                   else
                   {
                       unload_c(text);
                       end_token(t_INT_ZERO, &token);
                       return token;
                   }
                   break;
                   
               case DOT:
                   if (isdigit(current_char))
                   {
                       state = t_FLOAT;
                   }
                   else
                   {
                       fprintf(stderr , "Lexical error.\n");
                       exit(1);
                   }
                   break;
                   
               case t_FLOAT:
                   if (current_char == 'e' || current_char == 'E')
                   {
                       state = EXPONENT;
                   }
                   else if (isdigit(current_char))
                   {
                       state = t_FLOAT;
                   }
                   else
                   {
                       unload_c(text);
                       end_token(t_FLOAT, &token);
                       return token;
                   }
                   break;
                   
               case EXPONENT:
                   
                   if (isdigit(current_char))
                   {
                       state = EXPONENT2;
                   }
                   else if (current_char == '+' || current_char == '-'  )
                   {
                       state = PLUS_MINUS_EXPONENT;
                   }
                   else
                   {
                       fprintf(stderr , "Lexical error.\n");
                       exit(1);
                   }
                   break;
                   
               case PLUS_MINUS_EXPONENT:
                   if(isdigit(current_char))
                   {
                       state = EXPONENT2;
                   }
                   else
                   {
                       fprintf(stderr , "Lexical error.\n");
                       exit(1);
                   }
                   break;
                   
               case EXPONENT2:
                   if (isdigit(current_char))
                   {
                       state = EXPONENT2;
                   }
                   else
                   {
                       unload_c(text);
                       end_token(t_FLOAT, &token);
                       return token;
                   }
                   break;

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                   
               case STRING_START:
                   if (current_char == '"' )
                   {
                       end_token(t_STRING, &token);
                       return token;
                   }
                   else if (current_char == 92)
                   {
                       state = ES_STRING;
                   }
                   else if (current_char == EOF)
                   {
                       fprintf(stderr , "Lexical error.\n");
                       exit(1);
                   }
                       
                   else if (current_char > 31 && current_char != 39 && current_char != 92)
                   {
                       state = STRING_START;
                   }
                   break;
                   
               case ES_STRING:
                   if (current_char == 'x')
                   {
                       state = STRING_HEXA1;
                   }
                   else if ((current_char == 'n') || (current_char == 't') || (current_char == 's') || (current_char == 34) || (current_char == 92))
                   {
                       state = STRING_START;
                   }
                   else
                   {
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
                       fprintf(stderr , "Lexical error.\n");
                       exit(1);
                   }
                   break;
           }
       }
   }
