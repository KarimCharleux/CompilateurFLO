#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arbre_abstrait.h"
#include "generation_code.h"

//pour afficher le code uniquement si l'option afficher_nasm vaut 1
#define printifm(format, ...) if(afficher_nasm){ printf(format, __VA_ARGS__); }

int afficher_nasm = 1;
int if_label_count= 0;
int else_label_count= 0;
int fin_label_count= 0;
int label_count= 0;
char* current_symbol = GLOBAL_SCOPE_NAME;
Symbol* symbolTable[MAX_SYMBOL_TABLE_SIZE] = {NULL};

// -----------------------------------------------------------------------------------------------------------------
//  _____           _     
// |_   _|__   ___ | |___ 
//   | |/ _ \ / _ \| / __|
//   | | (_) | (_) | \__ \
//   |_|\___/ \___/|_|___/                        
//  

void nasm_comment(char *comment) {
  if(comment != NULL) {
    printifm("\t\t ; %s", comment);//le point virgule indique le début d'un commentaire en nasm. Les tabulations sont là pour faire jolie.
  }
  printifm("%s","\n");
}
void nasm_commande(char *opcode, char *op1, char *op2, char *op3, char *comment) {
  printifm("\t%s", opcode);
  if(op1 != NULL) {
    printifm("\t%s", op1);
    if(op2 != NULL) {
      printifm(", %s", op2);
      if(op3 != NULL) {
        printifm(", %s", op3);
      }
    }
  }
  nasm_comment(comment);
}
Symbol* creer_global_scope()
{
  Symbol* symbolTable = malloc(sizeof(Symbol));
  symbolTable->symbol_name = GLOBAL_SCOPE_NAME ;
  symbolTable->type = entier;
  for(int i=0; i<MAX_SYMBOL_TABLE_SIZE ; ++i)
  {
     symbolTable->variables[i] = NULL;
  }

  return symbolTable;
}
Symbol* findSymbol(char* identifiant)
{
  int i=0;
  do{
    if(symbolTable[i] != NULL && strcmp(symbolTable[i]->symbol_name, identifiant)==0)
    { 
      return symbolTable[i];
    }
    ++i;
  }while (symbolTable[i]!= NULL);

  return NULL;
} 
Variable* findVariable(char* identifiant, Variable* variableTable[])
{
  int i=0;
  do{
    if(variableTable[i] != NULL && strcmp(variableTable[i]->variable_name, identifiant)==0)
    { 
      return variableTable[i];
    }
    ++i;
  }while (variableTable[i]!= NULL);

  return NULL;
} 
void printSymbol(char* symbol_name)
{
  Symbol* symbol = findSymbol(symbol_name);
  printf("Name : %s\n", symbol->symbol_name);
  printf("Memory : %d\n", symbol->current_memory_used);
  printf("Type : %d\n", symbol->type);
  int i=0;
  while (symbol->variables[i]!=NULL)
  {
    printf("    ");
    printf("Var : %s\n", symbol->variables[i]->variable_name);
    ++i;
  }
}



// -----------------------------------------------------------------------------------------------------------------
//  ____        __             _ _   
// |  _ \  ___ / _| __ _ _   _| | |_ 
// | | | |/ _ \ |_ / _` | | | | | __|
// | |_| |  __/  _| (_| | |_| | | |_ 
// |____/ \___|_|  \__,_|\__,_|_|\__|
//  

// DEFAULT -----------------------------------------------------------------------------------------------------------------
void nasm_prog(n_programme *n) {
  symbolTable[0] = creer_global_scope();
  int i=1;
  n_l_fonctions* fonctions = n->fonctions;
  do {
		if (fonctions->fonction != NULL){
      Symbol* Symbol = malloc(sizeof(Symbol));
      Symbol->symbol_name = fonctions->fonction->identifiant ;
      Symbol->type = fonctions->fonction->type;
      Symbol->current_memory_used = 0;
      for(int i=0; i<MAX_SYMBOL_TABLE_SIZE ; ++i)
      {
        Symbol->variables[i] = NULL;
      }
      symbolTable[i] = Symbol;
      printSymbol(Symbol->symbol_name);
		}
		fonctions = fonctions->fonctions;
    ++i;
	} while(fonctions != NULL );
  
  printifm("%%include\t'%s'\n","io.asm");
  printifm("%s","\nsection\t.bss\n");
  printifm("%s", "sinput:\tresb\t255\t;reserve a 255 byte space in memory for the users input string\n");
  printifm("%s","\nsection\t.text\n");
  printifm("%s","global _start\n");

  nasm_liste_fonctions(n->fonctions);

  printifm("%s","_start:\n");

  nasm_liste_instructions(n->instructions);

  nasm_commande("mov", "eax", "0" , NULL, "1 est le code de SYS_EXIT");
  nasm_commande("int", "0x80", NULL, NULL, "exit");
}
void nasm_liste_fonctions(n_l_fonctions *n) {
	do {
		if (n->fonction != NULL){
			nasm_fonction(n->fonction);
		}
		n = n->fonctions;
	} while(n != NULL );
}
void nasm_fonction(n_fonction* n)
{
  current_symbol = n->identifiant;
  char label_fonction[15];
  sprintf(label_fonction, "_%s:\n", n->identifiant);
  printifm("%s",label_fonction);
  nasm_liste_instructions(n->l_instructions);
  nasm_clean_local_variables(n->identifiant);
  nasm_commande("ret", NULL, NULL, NULL, "Return");
}
void nasm_clean_local_variables(char* symbol_name)
{
  printSymbol(symbol_name);
  Symbol* symbol = findSymbol(symbol_name);
  int i=0;
  while (symbol->variables[i] != NULL)
  {
    char string[40];
    sprintf(string, "Depile local variable : %s", symbol->variables[i]->variable_name);
    nasm_commande("pop", "eax", NULL, NULL, string);
    symbol->variables[i]=NULL;
    ++i;
  } 
}
void nasm_liste_instructions(n_l_instructions *n) {
	do {
		if (n->instruction != NULL){
			nasm_instruction(n->instruction);
		}
		n = n->instructions;
	} while(n != NULL );
}
void nasm_instruction(n_instruction* n){
	if(n->type_instruction == i_ecrire){
		nasm_exp(n->u.exp);
		nasm_commande("pop", "eax", NULL, NULL, "depile sur eax");
		nasm_commande("call", "iprintLF", NULL, NULL, "envoie eax sur la sortie standard");
	}
  if(n->type_instruction == i_lire){
    nasm_commande("mov", "eax", "sinput", NULL, "charge l’adresse sinput");
    nasm_commande("call", "readline", NULL, NULL, "appelle readline de io.asm");
    nasm_commande("call", "atoi", NULL, NULL, "appelle atoi de io.asm");
    nasm_commande("push", "eax", NULL, NULL, "empile eax");
	}
  if(n->type_instruction == i_condition){
    char label_if[10];
    sprintf(label_if, "if%d", if_label_count);
    char label_else[10];
    sprintf(label_else, "else%d", else_label_count);
    char label_endif[10];
    sprintf(label_endif, "endif%d", fin_label_count);

    nasm_exp(n->u.condition.expr);
    nasm_commande("pop", "eax", NULL, NULL, "dépile le résultat"); 
    nasm_commande("cmp", "eax", "1", NULL, " on verifie la condition"); 

    if(n->u.condition.l_instructions_2!=NULL)
    {
      nasm_commande("jnz", label_else, NULL, NULL, "Aller au sinon"); 
    }
    else
    {
      nasm_commande("jnz", label_endif, NULL, NULL, "Aller a la fin");
    }
    
    sprintf(label_if, "if%d:", label_count);
    nasm_commande(label_if, NULL, NULL, NULL, "Entrer dans le si");
    nasm_liste_instructions(n->u.condition.l_instructions);
    nasm_commande("jmp", label_endif, NULL, NULL, "Aller au si");

    if(n->u.condition.l_instructions_2!=NULL)
    {
      sprintf(label_else, "else%d:", label_count);
      nasm_commande(label_else, NULL, NULL, NULL, "Aller dans le sinon");
      nasm_liste_instructions(n->u.condition.l_instructions_2);
    }
    sprintf(label_endif, "endif%d:", fin_label_count);
    nasm_commande(label_endif, NULL, NULL, NULL, "Aller a la fin");

	}
  if(n->type_instruction == i_boucle)
  {
    char label_tantque[15];
    char tantque[15];
    sprintf(tantque, "TantQue%d", if_label_count);
    char label_end_tantque[15];
    sprintf(label_end_tantque, "finTantQue%d", fin_label_count);

    sprintf(label_tantque, "TantQue%d:", label_count);
    nasm_commande(label_tantque, NULL, NULL, NULL, "Entrer dans le tantque");

    nasm_exp(n->u.condition.expr);
    nasm_commande("pop", "eax", NULL, NULL, "dépile le résultat");
    nasm_commande("cmp", "eax", "1", NULL, " on verifie la condition");
    nasm_commande("jnz", label_end_tantque, NULL, NULL, "Aller a la fin");

    nasm_liste_instructions(n->u.condition.l_instructions);
    nasm_commande("jmp", tantque, NULL, NULL, "Aller au si");
    
    sprintf(label_end_tantque, "finTantQue%d:", label_count);
    nasm_commande(label_end_tantque, NULL, NULL, NULL, "Sortie du tantque");
	}
  if(n->type_instruction == i_affectation)
  {
    Symbol* symbol = findSymbol(current_symbol);
    Variable* variable = findVariable(n->u.variable.identifiant, symbol->variables);

    nasm_exp(n->u.variable.expr);
    nasm_commande("pop", "eax", NULL, NULL, "Recupere le resultat dans eax");

    char variable_adresse[15];
    sprintf(variable_adresse, "[ebp-%d]", variable->offset_with_ebp);
    nasm_commande("mov", variable_adresse, "eax", NULL, "Remplace par la nouvelle valeur");
  }
  if(n->type_instruction == i_declaration)
  {
    Symbol* symbol = findSymbol(current_symbol);
    if(n->u.variable.expr != NULL)
    {
      nasm_exp(n->u.variable.expr);
    }
    else
    {
      nasm_commande("push", "0", NULL, NULL, "Insere 0 dans la pile");
    }

    symbol->current_memory_used += 4; 
    int i=0;
    while (symbol->variables[i] != NULL)++i;
    Variable* new_variable = malloc(sizeof(Variable));
    new_variable->variable_name = n->u.variable.identifiant;
    new_variable->offset_with_ebp = symbol->current_memory_used;
    symbol->variables[i] = new_variable;
  }
  if(n->type_instruction == i_appel)
  {
    char label_appel[15];
    sprintf(label_appel, "_%s", n->u.identifiant);
    nasm_commande("call", label_appel, NULL, NULL, "Appelle le label");
  }
  if(n->type_instruction == i_retour)
  { 
    if(strcmp(current_symbol, GLOBAL_SCOPE_NAME)==0 || (n->u.exp->type_exp!=0 && findSymbol(current_symbol)->type != n->u.exp->type_exp))
    {
      printf("Retrun type 1 = %d\n", findSymbol(current_symbol)->type);
      printf("Retrun type 2 =%d\n", n->u.exp->type_exp);
      exit(43);
    }
    nasm_exp(n->u.exp);
    nasm_commande("pop", "eax", NULL, NULL, "Recupere leresultqt dans eax");
    current_symbol = GLOBAL_SCOPE_NAME;
  }
}
void nasm_exp(n_exp* n){
	if (n->type_exp == i_operation){
		nasm_operation(n->u.operation);
	} else if (n->type_exp == i_entier || n->type_exp == i_boolean){
	      char buffer[10];
	      sprintf(buffer, "%d", n->u.valeur);
      	nasm_commande("push", buffer, NULL, NULL, NULL) ;
	}
  else if (n->type_exp == i_variable){
    Symbol* symbol = findSymbol(current_symbol);
    Variable* variable = findVariable(n->u.identifiant, symbol->variables);

    char variable_adresse[15];
    sprintf(variable_adresse, "[ebp-%d]", variable->offset_with_ebp);
	  nasm_commande("mov", "eax", variable_adresse, NULL, "Recupere la variable");
    nasm_commande("push", "eax", NULL, NULL, "Empile le resultat");
	}
}
void nasm_operation(n_operation* n){
  nasm_exp(n->exp1);
  nasm_exp(n->exp2);

  nasm_commande("pop", "ebx", NULL, NULL, "depile la seconde operande dans ebx");
  nasm_commande("pop", "eax", NULL, NULL, "depile la permière operande dans eax");
  if (n->type_operation == '+'){
      nasm_commande("add", "eax", "ebx", NULL, "operation +");
  }
  else if(n->type_operation == '*'){
    nasm_commande("imul", "eax", "ebx", NULL, "operation *");
  }
  else if(n->type_operation == '-'){
    nasm_commande("sub", "eax", "ebx", NULL, "operation -");
  }
  else if(n->type_operation == '/'){
    nasm_commande("idiv", "eax", "ebx", NULL, "operation /");
  }
  else if(n->type_operation == '%'){
    nasm_commande("mov", "edx", "0", NULL, "");
    nasm_commande("idiv", "ebx", NULL, NULL, "operation /");   
    nasm_commande("mov", "eax", "edx", NULL, "");  
  }
  else if(n->type_operation == '<'){
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("setl", "al", NULL, NULL, "set 1 dans al si eax < ebx");
    nasm_commande("movzx", "eax", "al", NULL, "move dans register que l on peut empiler");
  }
  else if(n->type_operation == '>'){
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("setg", "al", NULL, NULL, "set 1 dans al si eax > ebx");
    nasm_commande("movzx", "eax", "al", NULL, "move dans register que l on peut empiler");
  }
  else if(n->type_operation == '|'){
     nasm_commande("or", "eax", "ebx", NULL, "opération |");
  }
  else if(n->type_operation == '&'){
    nasm_commande("and", "eax", "ebx", NULL, "opération &");
  }
  else if(n->type_operation == 'e'){
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("sete", "al", NULL, NULL, "met 1 dans al si eax == ebx");
    nasm_commande("movzx", "eax", "al", NULL, "met 0 ou al dans eax");
  }
  else if(n->type_operation == 'i'){
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("setle", "al", NULL, NULL, "met 1 dans al si eax <= ebx");
    nasm_commande("movzx", "eax", "al", NULL, "met 0 ou al dans eax");
  }
  else if(n->type_operation == 's'){
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("setge", "al", NULL, NULL, "met 1 dans al si eax >= ebx");
    nasm_commande("movzx", "eax", "al", NULL, "met 0 ou al dans eax");
  }
  nasm_commande("push", "eax" , NULL, NULL, "empile le résultat");  
}
