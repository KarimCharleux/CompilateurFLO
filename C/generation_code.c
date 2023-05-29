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

/* -----------------------------------------------------------------------------------------------------------------
//  _____           _     
// |_   _|__   ___ | |___ 
//   | |/ _ \ / _ \| / __|
//   | | (_) | (_) | \__ \
//   |_|\___/ \___/|_|___/                        
*/  

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
  Symbol* symbol = malloc(sizeof(Symbol));
  symbol->symbol_name = GLOBAL_SCOPE_NAME ;
  symbol->type = entier;
  symbol->current_memory_used = 0;
  symbol->nb_built_in_parameters = 0;
  for(int i=0; i<MAX_SYMBOL_TABLE_SIZE ; ++i)
  {
     symbol->variables[i] = NULL;
  }

  return symbol;
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
void printSymbols()
{
  int i=0;
  do{
    printSymbol(symbolTable[i]->symbol_name);
    ++i;
  }while (symbolTable[i]!= NULL);
}
void printSymbol(char* symbol_name)
{
  Symbol* symbol = findSymbol(symbol_name);
  if(symbol==NULL)
  {
    printf("SYMBOL NOT FOUND\n");
    return;
  }
  printf("Name : %s\n", symbol->symbol_name);
  printf("Memory : %d\n", symbol->current_memory_used);
  printf("Type : %d\n", symbol->type);
  int i=0;
  while (symbol->variables[i]!=NULL)
  {
    printf("    ");
    printf("Var : %s\n", symbol->variables[i]->variable_name);
    printf("      ");
    printf("Off : %d\n", symbol->variables[i]->offset_with_ebp);
    ++i;
  }
}

void verify_parameters(Variable* variableTable[], n_l_expression* l_expression)
{
  int i=0;
  do{
    if(l_expression==NULL || l_expression->expression->type_value != variableTable[i]->type)
    {
      printf("EXIT FAILURE -- BAD FONCTION ENTRY\n");
      printf("list exp = %p\n", l_expression);
      printf("expressi = %p\n", l_expression->expression);
      printf("variable = %p\n", variableTable[i]);
      printf("Type expressi = %d\n", l_expression->expression->type_value);
      printf("Type varTable = %d\n", variableTable[i]->type);
      exit(EXIT_FAILURE);
    }
    ++i;
    l_expression = l_expression->l_expression;
  }while (variableTable[i]!= NULL);
  if(l_expression == NULL)
  {
    return;
  }
  printf("Trop de parametres\n");
  exit(EXIT_FAILURE);
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
   // printf("\n");  BUG RESOLUTION WHY FLUSH ??
		if (fonctions->fonction != NULL){
      Symbol* symbol = malloc(sizeof(Symbol));
      symbol->symbol_name = fonctions->fonction->identifiant ;
      symbol->type = fonctions->fonction->type;
      symbol->current_memory_used = 0;
      symbol->nb_built_in_parameters = 0;
      int j=0;
      if(fonctions->fonction->parametres != NULL)
      {
        do {
          Variable* new_variable = malloc(sizeof(Variable));
          new_variable->variable_name = fonctions->fonction->parametres->variable->identifiant;
          new_variable->offset_with_ebp = 4+j*4;
          new_variable->type = fonctions->fonction->parametres->variable->type;
          symbol->variables[j] = new_variable;
          ++symbol->nb_built_in_parameters;
          ++j;
          fonctions->fonction->parametres = fonctions->fonction->parametres->l_declaration;
        } while(fonctions->fonction->parametres != NULL );
      }
      for(j; j<MAX_SYMBOL_TABLE_SIZE ; ++j)
      {
        symbol->variables[j] = NULL;
      }
      symbolTable[i] = symbol;
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
  nasm_commande("mov", "ebp", "esp", NULL, "Met a jour ebp");

  nasm_liste_instructions(n->instructions);

  nasm_commande("mov", "eax", "1" , NULL, "1 est le code de SYS_EXIT");
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
  char label_fonction[STRING_SIZE];
  sprintf(label_fonction, "_%s:\n", n->identifiant);
  printifm("%s",label_fonction);

  nasm_commande("mov", "ebp", "esp", NULL, "Met a jour ebp");

  nasm_liste_instructions(n->l_instructions);
  current_symbol = GLOBAL_SCOPE_NAME;
}
void nasm_clean_local_variables(char* symbol_name)
{
  Symbol* symbol = findSymbol(symbol_name);
  int i=symbol->nb_built_in_parameters;
  
  while (symbol->variables[i] != NULL)
  {
    char string[40];
    sprintf(string, "Depile local variable : %s", symbol->variables[i]->variable_name);
    nasm_commande("add", "esp", "4", NULL, string);
    symbol->variables[i]=NULL;
    symbol->current_memory_used -=4;
    ++i;
  } 
}
void nasm_clean_fonction_arguments(char* symbol_name)
{
  Symbol* symbol = findSymbol(symbol_name);
  for(int i=0; i<symbol->nb_built_in_parameters; ++i)
  {
    char string[40];
    sprintf(string, "Depile local variable : %s", symbol->variables[i]->variable_name);
    nasm_commande("add", "esp", "4", NULL, string);
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
    char label_if[STRING_SIZE];
    sprintf(label_if, "if%d", if_label_count);
    char label_else[STRING_SIZE];
    sprintf(label_else, "else%d", else_label_count);
    char label_endif[STRING_SIZE];
    sprintf(label_endif, "endif%d", fin_label_count);

    nasm_exp(n->u.condition->expr);
    nasm_commande("pop", "eax", NULL, NULL, "dépile le résultat"); 
    nasm_commande("cmp", "eax", "1", NULL, " on verifie la condition"); 

    if(n->u.condition->l_instructions_2!=NULL)
    {
      nasm_commande("jnz", label_else, NULL, NULL, "Aller au sinon"); 
    }
    else
    {
      nasm_commande("jnz", label_endif, NULL, NULL, "Aller a la fin");
    }
    
    sprintf(label_if, "if%d:", label_count);
    nasm_commande(label_if, NULL, NULL, NULL, "Entrer dans le si");
    nasm_liste_instructions(n->u.condition->l_instructions);
    nasm_commande("jmp", label_endif, NULL, NULL, "Aller au si");

    if(n->u.condition->l_instructions_2!=NULL)
    {
      sprintf(label_else, "else%d:", label_count);
      nasm_commande(label_else, NULL, NULL, NULL, "Aller dans le sinon");
      nasm_liste_instructions(n->u.condition->l_instructions_2);
    }
    sprintf(label_endif, "endif%d:", fin_label_count);
    nasm_commande(label_endif, NULL, NULL, NULL, "Aller a la fin");

	}
  if(n->type_instruction == i_boucle)
  {
    char label_tantque[STRING_SIZE];
    char tantque[STRING_SIZE];
    sprintf(tantque, "TantQue%d", if_label_count);
    char label_end_tantque[STRING_SIZE];
    sprintf(label_end_tantque, "finTantQue%d", fin_label_count);

    sprintf(label_tantque, "TantQue%d:", label_count);
    nasm_commande(label_tantque, NULL, NULL, NULL, "Entrer dans le tantque");

    nasm_exp(n->u.condition->expr);
    nasm_commande("pop", "eax", NULL, NULL, "dépile le résultat");
    nasm_commande("cmp", "eax", "1", NULL, " on verifie la condition");
    nasm_commande("jnz", label_end_tantque, NULL, NULL, "Aller a la fin");

    nasm_liste_instructions(n->u.condition->l_instructions);
    nasm_commande("jmp", tantque, NULL, NULL, "Aller au si");
    
    sprintf(label_end_tantque, "finTantQue%d:", label_count);
    nasm_commande(label_end_tantque, NULL, NULL, NULL, "Sortie du tantque");
	}
  if(n->type_instruction == i_affectation)
  {
    Symbol* symbol = findSymbol(current_symbol);
    Variable* variable = findVariable(n->u.variable->identifiant, symbol->variables);

    nasm_exp(n->u.variable->expr);
    nasm_commande("pop", "eax", NULL, NULL, "Recupere le resultat dans eax");
    char variable_adresse[STRING_SIZE];
    if(variable->offset_with_ebp>0)
    {
      sprintf(variable_adresse, "[ebp+%d]", variable->offset_with_ebp);
    }
    else
    {
      sprintf(variable_adresse, "[ebp%d]", variable->offset_with_ebp);
    }
    nasm_commande("mov", variable_adresse, "eax", NULL, "Remplace par la nouvelle valeur");
  }
  if(n->type_instruction == i_declaration)
  {
    Symbol* symbol = findSymbol(current_symbol);
    if(n->u.variable->expr != NULL)
    {
      nasm_exp(n->u.variable->expr);
    }
    else
    {
      nasm_commande("push", "0", NULL, NULL, "Insere 0 dans la pile");
    }
    symbol->current_memory_used -= 4; 
    int i=0;
    while (symbol->variables[i] != NULL)++i;
    Variable* new_variable = malloc(sizeof(Variable));
    new_variable->variable_name = n->u.variable->identifiant;
    new_variable->type = n->u.variable->type;
    new_variable->offset_with_ebp = symbol->current_memory_used;
    symbol->variables[i] = new_variable;
  }
  if(n->type_instruction == i_appel_inst)
  {
     nasm_appel(n->u.appel);
  }
  if(n->type_instruction == i_retour)
  { 
    nasm_exp(n->u.exp);
    if(strcmp(current_symbol, GLOBAL_SCOPE_NAME)==0 || (findSymbol(current_symbol)->type != n->u.exp->type_value))
    {
      printf("Retrun type 1 = %d\n", findSymbol(current_symbol)->type);
      printf("Retrun type 2 = %d\n", n->u.exp->type_value);
      exit(43);
    }
    nasm_commande("pop", "eax", NULL, NULL, "Passe le retour par eax");
    nasm_clean_local_variables(current_symbol);
    nasm_commande("ret", NULL, NULL, NULL, "Return");
  }
}
enum Type nasm_appel(n_appel* appel)
{
   Symbol* symbol = findSymbol(appel->identifiant);
    if(appel->parameters != NULL)
    {
      nasm_commande("push", "ebp", NULL, NULL, "Sauvegarde ebp");
      n_l_expression* parameters = appel->parameters;
      do {
        nasm_exp(parameters->expression);
        parameters = parameters->l_expression;
      } while(parameters != NULL );
      verify_parameters(symbol->variables, appel->parameters);
    }
    char label_appel[STRING_SIZE];
    sprintf(label_appel, "_%s", appel->identifiant);
    nasm_commande("call", label_appel, NULL, NULL, "Appelle le label");
    nasm_clean_fonction_arguments(appel->identifiant);
    nasm_commande("pop", "ebp", NULL, NULL, "Recupere ebp");
    
    return symbol->type; 
}
void nasm_exp(n_exp* n){
	if (n->type_exp == i_operation){
		n->type_value =  nasm_operation(n->u.operation);
	} else if (n->type_exp == i_value){
	      char buffer[STRING_SIZE];
	      sprintf(buffer, "%d", n->u.valeur);
      	nasm_commande("push", buffer, NULL, NULL, NULL) ;
	}
  else if (n->type_exp == i_variable){
    Symbol* symbol = findSymbol(current_symbol);
    Variable* variable = findVariable(n->u.variable->identifiant, symbol->variables);
    n->type_value = variable->type;
    char variable_adresse[STRING_SIZE];
    if(variable->offset_with_ebp>0)
    {
      sprintf(variable_adresse, "[ebp+%d]", variable->offset_with_ebp);
    }
    else
    {
      sprintf(variable_adresse, "[ebp%d]", variable->offset_with_ebp);
    }
	  nasm_commande("mov", "eax", variable_adresse, NULL, "Recupere la variable");
    nasm_commande("push", "eax", NULL, NULL, "Empile le resultat");
	}
  else if (n->type_exp == i_appel_expr)
  {
    if(strcmp(n->u.appel->identifiant, "lire")==0)
    {
      nasm_commande("mov", "eax", "sinput", NULL, "charge l’adresse sinput");
      nasm_commande("call", "readline", NULL, NULL, "appelle readline de io.asm");
      nasm_commande("call", "atoi", NULL, NULL, "appelle atoi de io.asm");
      nasm_commande("push", "eax", NULL, NULL, "empile eax");
    }
    else{
      n->type_value = nasm_appel(n->u.appel);
      nasm_commande("push", "eax", NULL, NULL, "Empile le resultat");
    }
  }
  
}
enum Type nasm_operation(n_operation* n){
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
  return n->exp1->type_value;
}
