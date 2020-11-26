#include <stdio.h>
#include <string.h>
#include "semantika.h"
#include "tokenList.h"
#include "error.h"
#include "symtable.h"
#include "scanner.h"



void goThroughList(TDLList *L) {
    TDLLFirst(L);

	printf("Som vo funkcii gotrough\n");
	tBSTNodePtrLocal node;
	BSTInitLocal(&node);

	// Chod na koniec zoznamu
    while (L->Act != NULL) {
		// Ak je najdeny typ deklaracia -> :=
		if(L->Act->tdata.type == t_DEFINITION_2){
			// Vrat sa na identifikator a zavola kontrolu deklaracie
			TDLLPred(L);
			dec_var_control(L, &node);
		} 

		if(L->Act->tdata.type == t_IF){
			TDLLSucc(L);
			if_control(L, &node);
		}

		if(L->Act->tdata.type == t_FOR){
			for_control(L, &node);
		}

		//printf("lexem: %s, typ: %d\n", L->Act->tdata.lex, L->Act->tdata.type);
		// Ak je najdeny typ priradenie -> = 
		if(L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->tdata.type == t_ASSIGN){
			TDLLElemPtr temp_node = L->Act;
			while(temp_node->tdata.type != t_EOL){
				if(temp_node->lptr->tdata.type == t_EOL){
					assign_vals_control(L, &node);
					break;
				}
				TDLLPred(L);
				temp_node = temp_node->lptr;
			}
		}
		
	TDLLSucc(L);
    }
}


void if_control(TDLList *L, tBSTNodePtrLocal *node){
	printf("\n\n SOM VO FUKNCII IFCONTROL\n\n");
	
	// Ak su oba identifikatory
	if(L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->rptr->tdata.type == t_IDENTIFIER){
		int typ1;
		int typ2;
		// je prvy v strome?
		if(BSTSearchLocal(*node, L->Act->tdata.lex, &typ1, "null")){
		}else error_call(ERR_SEM_UNDEF, L);
		
		if(BSTSearchLocal(*node, L->Act->rptr->rptr->tdata.lex, &typ2, "null")){
		}else error_call(ERR_SEM_UNDEF, L);
		
		if(!(typ1 == typ2)) error_call(ERR_SEM_EXCOMPAT, L);

	}else if(L->Act->tdata.type == t_IDENTIFIER && L->Act->rptr->rptr->tdata.type != t_IDENTIFIER){
		int typ1;
		
		if(BSTSearchLocal(*node, L->Act->tdata.lex, &typ1, "null")){
		}
		else error_call(ERR_SEM_UNDEF, L);

		if(!(typ1 == L->Act->rptr->rptr->tdata.type)) error_call(ERR_SEM_EXCOMPAT, L);

	}else if(L->Act->tdata.type != t_IDENTIFIER && L->Act->rptr->rptr->tdata.type == t_IDENTIFIER){
		int typ2;
		if(BSTSearchLocal(*node, L->Act->rptr->rptr->tdata.lex, &typ2, "null")){
			
		}else error_call(ERR_SEM_UNDEF, L);
		if(!(L->Act->tdata.type == typ2)) error_call(ERR_SEM_EXCOMPAT, L);
	}

	if(!(L->Act->tdata.type == L->Act->rptr->rptr->tdata.type)){
		error_call(ERR_SEM_EXCOMPAT, L);
	}


	//Skontroluje ci ide o relacne operatory
	if(L->Act->rptr->tdata.type != t_GREATER){
		printf("Nerovna sa >\n");
		if(L->Act->rptr->tdata.type != t_GREATEROREQUAL){
			printf("Nerovna sa >=\n");
			if(L->Act->rptr->tdata.type != t_LESS){
				printf("Nerovna sa <\n");
				if(L->Act->rptr->tdata.type != t_LESSOREQUAL){
					printf("Nerovna sa <=\n");
					if(L->Act->rptr->tdata.type != t_EQUAL){
						printf("Nerovna sa =\n");
						if(L->Act->rptr->tdata.type != t_NOT_EQUAL){
							printf("Nerovna sa !=\n");
							error_call(ERR_SEM_EXCOMPAT, L);
						}
					}	
				}
			}
		}
	}

	printf("Vsetko preslo v poriadku\n");
}

void for_control(TDLList *L, tBSTNodePtrLocal *node){
	printf("\n\n SOM VO FUNKCII FORCONTROL\n\n");

	// ak je prvy lexem bodkociarka
	if(L->Act->rptr->tdata.type == T_SEMICOLON){
		TDLLSucc(L); // chod na dalsi lexem
	// Ak prvy lexem nie je bodkociarka
	}else{
		// chod na lexem za for
		TDLLSucc(L);
		// deklaruj identifikator
		dec_var_control(L, node);
		
		while(L->Act->tdata.type != t_EOL){
			printf("lexem za deklaraciou: %s\n", L->Act->tdata.lex);
			TDLLSucc(L);
		}
	}

}


void dec_var_control(TDLList *L, tBSTNodePtrLocal *node){

	printf("\n\n SOM V DECVARCONTROL\n\n");
	// Nazov premennej na deklarovanie
	char *id = L->Act->tdata.lex;
	int type;
	char *data = "null";

	// pocet stringov
	int string_count = 0;
	// pocet cisel vo vyraze
	int int_count = 0;
	// pocet floatov vo vyraze
	int float_count = 0;
	// pocet identifikatorov
	int identifier = 0;

	
	// Chod na dalsi prvok teda  ->  :=
	TDLLSucc(L);
	// Chod na koniec riadku
	while(L->Act->tdata.type != t_EOL){
		
		// Ak ide o integer
		if(L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ZERO){
			// Uloz jeho typ
			type = 24;
			// Zapocitaj najdenie integeru
			int_count++;

		// Ak ide o float
		}else if(L->Act->tdata.type == t_FLOAT64 || L->Act->tdata.type == t_FLOAT){
			// Uloz jeho typ
			type = L->Act->tdata.type;
			// zapocitaj najdenie floatu
			float_count++;

		// Ak ide o string
		}else if(L->Act->tdata.type == t_STRING){
			// uloz jeho typ
			type = t_STRING_ID;
			// zapocitaj najdenie stringu
			string_count++;

		// Ak ide o identifikator
		}else if(L->Act->tdata.type == t_IDENTIFIER){
			// Premenna na ulozenie typu identifikatoru v strome
			int datovytyp;
			// Nazov najdeneho identifikatoru
			char *name = L->Act->tdata.lex;

			// Ak bol najdeny identifikator uloz jeho typ do premennej datovytyp
			if (BSTSearchLocal(*node, name, &datovytyp, data)) {
				type = datovytyp;
				identifier++;
    		}
		}
		printf("Prechadzam while v decvarcontrol\n");
		if(L->Act->tdata.type == T_SEMICOLON) break;
		TDLLSucc(L);
	}

	printf("Chem priradit hodnoty\n");
	// Ak boli na priradenie iba string
	if(string_count != 0 && int_count == 0 && float_count == 0){
		// Uloz uzol so stringom do stromu
		BSTInsertLocal(node, id,&type, data);

	// ak boli na priradenie vo vyraze iba integre 
	}else if(string_count == 0 && int_count != 0 && float_count == 0){
		// uloz uzol s integerom do stromu
		BSTInsertLocal(node, id, &type, data);

	// ak boli na priradenie vo vyraze iba floaty
	}else if(string_count == 0 && int_count == 0 && float_count != 0){
		// tak uloz uzol s typom float do stromu
		BSTInsertLocal(node, id, &type, data);

	// Ak sa nasiel identifikator na priradenie
	}else if(identifier != 0 && string_count == 0 && int_count == 0 && float_count == 0){
		// Uloz uzol s typom ako mal najdeny identifikator do stromu
		BSTInsertLocal(node, id, &type, data);

	// Inac ide o chybu
	}else{
		error_call(ERR_SEM_DATATYPE, L);
	}
}


// kontroluje priradnie pri viacerych premennych a vyrazoch
// param1: Zoznam
// param2: Aktualny Token ako clen stromu
// ak prvy identifikator tak nalavo je EOL(47)
// act_Token_ptr je vzdy identifikator
// Rozhodne bude treba upravit pracu so stromom
// Doplnit ak ide o vyrazy a nie priamo hodnoty
void assign_vals_control(TDLList *L, tBSTNodePtrLocal *node){
	
	printf("\n\n SOM V ASSIGNVALSCONTROL\n\n");
	char *id[256];
	int type[256];
	char *data = "null";
	int string_count = 0;
	int int_count = 0;
	int float_count = 0;
	int identifier = 0;
	int id_insert = 0;

	// Pocet premennych nalavo od =
	// po ukonceni cyklu bude L->act ukazovat na =
	while(L->Act->tdata.type != t_ASSIGN){
	 	if(L->Act->tdata.type == t_IDENTIFIER){
			id[identifier] = L->Act->tdata.lex;
			identifier++;
		}
		TDLLSucc(L);
	}


	// pocet identifikatorov napravo od =
	int identif_count = 0;
	// pocet ciarok napravo od = (priradena 1 kvoli tomu,ze za poslednou hodnotou nie je ciarka)
	int comma_count = 0;


	// prechadzaj pokym neprejdeme na koniec riadku
	while(L->Act->tdata.type!= t_EOL){
			if(L->Act->tdata.type == t_INT_NON_ZERO || L->Act->tdata.type == t_INT_ZERO){
				type[int_count] = t_INT_ID;
				int_count++;
			}else if(L->Act->tdata.type == t_FLOAT64 || L->Act->tdata.type == t_FLOAT){
				type[float_count] = t_FLOAT64;
				float_count++;
			}else if(L->Act->tdata.type == t_STRING){
				type[string_count] = t_STRING_ID;
				string_count++;
			}else if(L->Act->tdata.type == t_IDENTIFIER){
				int datovytyp;
				char *name = L->Act->tdata.lex;
				if (BSTSearchLocal(*node, name, &datovytyp, data)) {
					type[identif_count] = datovytyp;
					identif_count++;
    			}
				identif_count++;
			}

			if(L->Act->tdata.type == t_COMMA || L->Act->rptr->tdata.type == t_EOL){
				comma_count++;

				if(string_count != 0 && int_count == 0 && float_count == 0){
					BSTInsertLocal(node, id[id_insert], type, data);
					id_insert++;
					string_count = 0;
					int_count = 0;
					float_count = 0;
					identif_count = 0;
				}else if(string_count == 0 && int_count != 0 && float_count == 0){
					BSTInsertLocal(node, id[id_insert], type, data);
					id_insert++;
					string_count = 0;
					int_count = 0;
					float_count = 0;
					identif_count = 0;
				}else if(string_count == 0 && int_count == 0 && float_count != 0){
					BSTInsertLocal(node, id[id_insert], type, data);
					id_insert++;
					string_count = 0;
					int_count = 0;
					float_count = 0;
					identif_count = 0;
				}else if(identif_count != 0 && string_count == 0 && int_count == 0 && float_count == 0){
					BSTInsertLocal(node, id[id_insert], type, data);
					id_insert++;
					string_count = 0;
					int_count = 0;
					float_count = 0;
					identif_count = 0;
				}else{
					error_call(ERR_SEM_DATATYPE, L);
				}

			}
		
		TDLLSucc(L);
	}
	if(identifier != comma_count) error_call(ERR_SEM_DATATYPE, L);
}


