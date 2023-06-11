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
int tant_que_label_count=0;
int fin_tant_que_label_count=0;
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
  symbol->variables_by_scope = get_new_scope_variables();
  return symbol;
}
VariablesByScope* get_new_scope_variables(){
  VariablesByScope* variables_by_scope = malloc(sizeof(VariablesByScope));
  for(int i=0; i<MAX_SYMBOL_TABLE_SIZE ; ++i)
  {
     variables_by_scope->variables[i] = NULL;
  }
  return variables_by_scope;
  
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
  perror("Can t find symbol");
  printf("Can t find symbol = %s\n", identifiant);
  printSymbols();
  exit(EXIT_FAILURE);
} 
Variable* findVariable(char* identifiant, VariablesByScope* variables_by_scope)
{
  while (variables_by_scope != NULL)
  {
    int i=0;
    do{
      if(variables_by_scope->variables[i] != NULL && strcmp(variables_by_scope->variables[i]->variable_name, identifiant)==0)
      { 
        return variables_by_scope->variables[i];
      }
      ++i;
    }while (variables_by_scope->variables[i]!= NULL);
    variables_by_scope = variables_by_scope->next_scope;
  }
  perror("Cant find var");
  printf("Can t find variable %s\n", identifiant);
  printSymbol(current_symbol);
  exit(EXIT_FAILURE);
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
  printf("\n");
  Symbol* symbol = findSymbol(symbol_name);
  if(symbol==NULL)
  {
    perror("SYMBOL NOT FOUND\n");
    return;
  }
  printf("Name : %s\n", symbol->symbol_name);
  printf("Memory : %d\n", symbol->current_memory_used);
  printf("Type : %d\n", symbol->type);
  VariablesByScope* variables_by_scope = symbol->variables_by_scope;
  while (variables_by_scope!=NULL)
  {
    int i=0;
    printf("---scope %p----\n", variables_by_scope);
    while (variables_by_scope->variables[i]!=NULL)
    {
      printf("    ");
      printf("Var : %s\n", variables_by_scope->variables[i]->variable_name);
      printf("      ");
      printf("Off : %d\n", variables_by_scope->variables[i]->offset_with_ebp);
      ++i;
    }
    variables_by_scope = variables_by_scope->next_scope;
  }
  printf("\n\n");
}
void verify_operand(enum Type type1, enum Type type2, enum Type reference)
{
  if(type2 == none)
  {
    if(type1 != reference)
    {
      perror("Mauvais type d operande");
      printf("Mauvais type d operande, devrait etre %d , mais est %d" , reference, type1);
      exit(EXIT_FAILURE);
    }
  }
  else if (reference==none)
  {
    if(type1 != type2)
    {
      perror("Mauvais type d operande");
      printf("Mauvais type d operande sont %d & %d", type1, type2);
      exit(EXIT_FAILURE);
    }
  }
  else if(type1 != type2 || type2 != reference)
  {
    perror("Mauvais type d operande");
    printf("Mauvais type d operande, devrait etre %d , mais sont %d & %d", reference, type1, type2);
    exit(EXIT_FAILURE);
  }
}

void verify_parameters(int nb_parameters, Variable* variableTable[], n_l_expression* l_expression)
{
  int i=0;
  if(l_expression != NULL){
    do{
      if(l_expression==NULL || l_expression->expression->type_value != variableTable[i]->type)
      {
        perror("EXIT FAILURE -- BAD FONCTION ENTRY\n");
        printf("i = %d\n", i);
        printf("expressi = %d\n", l_expression->expression->type_exp);
        printf("variable = %s\n", variableTable[i]->variable_name);
        printf("Type expressi = %d\n", l_expression->expression->type_value);
        printf("Type varTable = %d\n", variableTable[i]->type);
        exit(EXIT_FAILURE);
      }
      ++i;
      l_expression = l_expression->l_expression;
    }while (variableTable[i]!= NULL);
  }
  else
  {
    if(nb_parameters==0)return;
    perror("Necessite des parametres");
    printf("Necessite %d parametres", nb_parameters);
    exit(EXIT_FAILURE);
  }
  if(l_expression==NULL)return;
  perror("Trop de parametres\n");
  exit(EXIT_FAILURE);
}

n_l_expression* reverse_parameters(n_l_expression* parameters)
{
    n_l_expression* newHead = NULL;
    n_l_expression* current = parameters;

    while (current != NULL) {
        // Create a new node with the same data
        n_l_expression* newNode = malloc(sizeof(n_l_expression));
        newNode->expression = current->expression;
        newNode->l_expression = current->l_expression;

        // Insert the new node at the beginning of the new list
        newNode->l_expression = newHead;
        newHead = newNode;

        current = current->l_expression;
    }

    return newHead;
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
  if(fonctions!=NULL)
  {
    do {
      // printf("\n");  BUG RESOLUTION WHY FLUSH ??
      if (fonctions->fonction != NULL){
        Symbol* symbol = malloc(sizeof(Symbol));
        symbol->symbol_name = fonctions->fonction->identifiant ;
        symbol->type = fonctions->fonction->type;
        symbol->current_memory_used = 0;
        symbol->nb_built_in_parameters = 0;
        int j=0;
        VariablesByScope* variable_of_function_scope = malloc(sizeof(VariablesByScope));
        if(fonctions->fonction->parametres != NULL)
        {
          do {
            Variable* new_variable = malloc(sizeof(Variable));
            new_variable->variable_name = fonctions->fonction->parametres->variable->identifiant;
            new_variable->offset_with_ebp = 4+j*4;
            new_variable->type = fonctions->fonction->parametres->variable->type;
            variable_of_function_scope->variables[j] = new_variable;
            ++symbol->nb_built_in_parameters;
            ++j;
            fonctions->fonction->parametres = fonctions->fonction->parametres->l_declaration;
          } while(fonctions->fonction->parametres != NULL );
        }
        for(j; j<MAX_SYMBOL_TABLE_SIZE ; ++j)
        {
          variable_of_function_scope->variables[j] = NULL;
        }
        symbol->variables_by_scope = variable_of_function_scope;
        symbolTable[i] = symbol;
      }
      fonctions = fonctions->fonctions;
      ++i;
    } while(fonctions != NULL );
  }
  
  printifm("%%include\t'%s'\n","io.asm");
  printifm("%s","\nsection\t.bss\n");
  printifm("%s", "sinput:\tresb\t255\t;reserve a 255 byte space in memory for the users input string\n");
  printifm("%s","\nsection\t.text\n");
  printifm("%s","global _start\n");

  nasm_liste_fonctions(n->fonctions);
  printifm("%s","_start:\n");
  nasm_commande("mov", "ebp", "esp", NULL, "Met a jour ebp");

  nasm_liste_instructions(n->instructions);

  nasm_commande("mov", "eax", "1" , NULL, "EXIT CODE");
  nasm_commande("mov", "ebx", "0" , NULL, "RETURN CODE");
  nasm_commande("int", "0x80", NULL, NULL, "exit");
}
void nasm_liste_fonctions(n_l_fonctions *n) {
  if(n !=NULL)
  {
    do {
      if (n->fonction != NULL){
        nasm_fonction(n->fonction);
      }
      n = n->fonctions;
	  } while(n != NULL );
  }
}
void nasm_fonction(n_fonction* n)
{
  current_symbol = n->identifiant;
  char label_fonction[STRING_SIZE];
  sprintf(label_fonction, "_%s:\n", n->identifiant);
  printifm("%s",label_fonction);

  nasm_commande("mov", "ebp", "esp", NULL, "Met a jour ebp");

  nasm_liste_instructions(n->l_instructions);
  nasm_clean_local_variables(current_symbol, 1);
  current_symbol = GLOBAL_SCOPE_NAME;
  nasm_commande("ret", NULL, NULL, NULL, "Return");
}
void nasm_clean_local_variables(char* symbol_name, int clean_symbol_table)
{
  Symbol* symbol = findSymbol(symbol_name);
  int i=symbol->nb_built_in_parameters;
  if(clean_symbol_table)symbol->variables_by_scope->next_scope=NULL;
  VariablesByScope* variable_of_function_scope = symbol->variables_by_scope;
  while (variable_of_function_scope->variables[i] != NULL)
  {
    char string[STRING_SIZE];
    sprintf(string, "Depile local variable : %s", variable_of_function_scope->variables[i]->variable_name);
    nasm_commande("add", "esp", "4", NULL, string);
    if(clean_symbol_table)
    {
      variable_of_function_scope->variables[i]=NULL;
      symbol->current_memory_used -=4;
    }
    ++i;
  } 
}
void clean_under_scope_variables(char* symbol_name)
{
  Symbol* symbol = findSymbol(symbol_name);
  int i=0;
  VariablesByScope* current_variables_by_scope = symbol->variables_by_scope;
  VariablesByScope* previous_variables_by_scope = NULL;
  while (current_variables_by_scope->next_scope !=NULL){
    previous_variables_by_scope = current_variables_by_scope;
    current_variables_by_scope = current_variables_by_scope->next_scope;
  }
  if(current_variables_by_scope==symbol->variables_by_scope){
    perror("WRONG SCOPE FORMATTING");
    return;
  }
  while (current_variables_by_scope->variables[i] != NULL)
  {
    char string[STRING_SIZE];
    sprintf(string, "Depile local variable : %s", current_variables_by_scope->variables[i]->variable_name);
    nasm_commande("add", "esp", "4", NULL, string);
    current_variables_by_scope->variables[i]=NULL;
    symbol->current_memory_used -=4;
    ++i;
  } 
  free(previous_variables_by_scope->next_scope);
  previous_variables_by_scope->next_scope=NULL;
}
VariablesByScope* get_last_scope_variables(Symbol* symbol)
{
  VariablesByScope* variables_by_scope = symbol->variables_by_scope;
  while (variables_by_scope->next_scope !=NULL)variables_by_scope = variables_by_scope->next_scope;

  return variables_by_scope;
}
void nasm_clean_fonction_arguments(char* symbol_name)
{
  Symbol* symbol = findSymbol(symbol_name);
  symbol->variables_by_scope->next_scope=NULL;
  VariablesByScope* variable_of_function_scope = symbol->variables_by_scope;
  for(int i=0; i<symbol->nb_built_in_parameters; ++i)
  {
    char string[40];
    sprintf(string, "Depile local variable : %s", variable_of_function_scope->variables[i]->variable_name);
    nasm_commande("add", "esp", "4", NULL, string);
  } 
}

void nasm_liste_instructions(n_l_instructions *n) {
  if(n!=NULL)
  {
    do {
      if (n->instruction != NULL){
        nasm_instruction(n->instruction);
      }
      n = n->instructions;
	  } while(n != NULL );
  }
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
    
    Symbol* symbol = findSymbol(current_symbol);
    VariablesByScope* variable_by_scope = get_last_scope_variables(symbol);
    variable_by_scope->next_scope = get_new_scope_variables();

    char label_if[STRING_SIZE];
    char label_if_2[STRING_SIZE];
    char label_else[STRING_SIZE];
    char label_endif[STRING_SIZE];
    sprintf(label_if, "if%d", if_label_count);
    sprintf(label_else, "else%d", else_label_count);
    sprintf(label_endif, "endif%d", fin_label_count);

    ++if_label_count;
    ++else_label_count;
    ++fin_label_count;

    sprintf(label_if_2, "if%d", if_label_count);
    ++if_label_count;
    enum Condition_type next_condition_type;
    if(n->u.condition->l_sinon_si != NULL)
    {
      next_condition_type = type_else_if;
    }
    else if (n->u.condition->l_instructions_2 != NULL)
    {
      next_condition_type = type_else;
    }
    else{
      next_condition_type = type_end_if;
    }
    nasm_si(n->u.condition, next_condition_type, label_if, label_if_2, label_else, label_endif);
  
    strcpy(label_if, label_if_2);
    n_l_sinon_si* liste_sinon_si = n->u.condition->l_sinon_si;
    while (liste_sinon_si != NULL)
    {
      sprintf(label_if_2, "if%d", if_label_count);
      ++if_label_count;
      enum Condition_type next_condition_type;
      if(liste_sinon_si->l_sinon_si != NULL)
      {
        next_condition_type = type_else_if;
      }
      else if (n->u.condition->l_instructions_2 != NULL)
      {
        next_condition_type = type_else;
      } 
      else{
        next_condition_type = type_end_if;
      }
      nasm_si(liste_sinon_si->condition, next_condition_type , label_if, label_if_2  , label_else, label_endif);
      liste_sinon_si = liste_sinon_si->l_sinon_si;
      strcpy(label_if, label_if_2);
    }
    

    if(n->u.condition->l_instructions_2!=NULL)
    {
      sprintf(label_else, "%s:", label_else);
      nasm_commande(label_else, NULL, NULL, NULL, "Aller dans le sinon");
      nasm_liste_instructions(n->u.condition->l_instructions_2);
    }
    sprintf(label_endif, "%s:", label_endif);
    nasm_commande(label_endif, NULL, NULL, NULL, "Aller a la fin");
    clean_under_scope_variables(current_symbol);
	}
  if(n->type_instruction == i_boucle)
  {
    Symbol *symbol = findSymbol(current_symbol);
    VariablesByScope* variable_by_scope = get_last_scope_variables(symbol);
    variable_by_scope->next_scope = get_new_scope_variables();

    char label_tantque[STRING_SIZE];
    char label_end_tantque[STRING_SIZE];
    sprintf(label_end_tantque, "finTantQue%d", fin_tant_que_label_count);
    sprintf(label_tantque, "TantQue%d", tant_que_label_count);
    ++fin_tant_que_label_count;
    ++tant_que_label_count;
   
    char label_tantque2[STRING_SIZE];
    sprintf(label_tantque2, "%s:", label_tantque);
    nasm_commande(label_tantque2, NULL, NULL, NULL, "Entrer dans le tantque");
    nasm_exp(n->u.boucle->expr);
    verify_operand(n->u.boucle->expr->type_value, none, booleen);
    nasm_commande("pop", "eax", NULL, NULL, "dépile le résultat");
    nasm_commande("cmp", "eax", "1", NULL, " on verifie la condition");
    nasm_commande("jnz", label_end_tantque, NULL, NULL, "Aller a la fin");

    nasm_liste_instructions(n->u.boucle->l_instructions);
    //printSymbol(current_symbol);
    nasm_commande("jmp", label_tantque, NULL, NULL, "Aller au si");
    
    sprintf(label_end_tantque, "%s:", label_end_tantque);
    nasm_commande(label_end_tantque, NULL, NULL, NULL, "Sortie du tantque");
    clean_under_scope_variables(current_symbol);
    //printSymbol(current_symbol);
	}
  if(n->type_instruction == i_affectation)
  {
    Symbol* symbol = findSymbol(current_symbol);
    Variable* variable = findVariable(n->u.variable->identifiant, symbol->variables_by_scope);

    nasm_exp(n->u.variable->expr);
    if(n->u.variable->expr->type_value != variable->type){
      perror("WRONG VAR TYPE");
      exit(EXIT_FAILURE);
    }
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

    Variable* new_variable = malloc(sizeof(Variable));
    new_variable->variable_name = n->u.variable->identifiant;
    new_variable->type = n->u.variable->type;
    new_variable->offset_with_ebp = symbol->current_memory_used;
    
    int i=0;
    VariablesByScope* variable_of_scope = get_last_scope_variables(symbol);
    while (variable_of_scope->variables[i] != NULL)++i;
    variable_of_scope->variables[i] = new_variable;
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
      perror("BAD RETURN TYPE");
      printf("BAD RETURN TYPE, Symbole type = %d, return type = %d\n",findSymbol(current_symbol)->type, n->u.exp->type_value );
      exit(EXIT_FAILURE);
    }
    nasm_commande("pop", "eax", NULL, NULL, "Passe le retour par eax");
    nasm_clean_local_variables(current_symbol, 0);
    nasm_commande("ret", NULL, NULL, NULL, "Return");
  }
}
void nasm_si(n_condition* n, enum Condition_type next_condition_type, char* label_if, char* label_next_if, char* label_else, char* label_endif)
{
  sprintf(label_if, "%s:", label_if);
  nasm_commande(label_if, NULL, NULL, NULL, "Entrer dans le si");
  nasm_exp(n->evaluation->expr);
  verify_operand(n->evaluation->expr->type_value, none, booleen);
  nasm_commande("pop", "eax", NULL, NULL, "dépile le résultat"); 
  nasm_commande("cmp", "eax", "1", NULL, " on verifie la condition"); 

  if (next_condition_type == type_else_if)
  {
    nasm_commande("jnz", label_next_if, NULL, NULL, "Aller au prochain if");
  }
  else if(next_condition_type == type_else)
  {
    nasm_commande("jnz", label_else, NULL, NULL, "Aller au sinon"); 
  }
  else if (next_condition_type == type_end_if) 
  {
    nasm_commande("jnz", label_endif, NULL, NULL, "Aller a la fin");
  }
  nasm_liste_instructions(n->l_instructions);
  nasm_commande("jmp", label_endif, NULL, NULL, "Aller au si");
}
enum Type nasm_appel(n_appel* appel)
{
    Symbol* symbol = findSymbol(appel->identifiant);
    nasm_commande("push", "ebp", NULL, NULL, "Sauvegarde ebp");
    if(appel->parameters != NULL)
    {
      n_l_expression* parameters = reverse_parameters(appel->parameters);
      do {
        nasm_exp(parameters->expression);
        parameters = parameters->l_expression;
      } while(parameters != NULL );
      
    }
    verify_parameters(symbol->nb_built_in_parameters, symbol->variables_by_scope->variables, appel->parameters);
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
    Variable* variable = findVariable(n->u.variable->identifiant, symbol->variables_by_scope);
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
  enum Type type;
  nasm_exp(n->exp1);
  type = n->exp1->type_value;
  if(n->exp2!=NULL)
  {
    nasm_exp(n->exp2);
    nasm_commande("pop", "ebx", NULL, NULL, "depile la seconde operande dans ebx");
    verify_operand(n->exp2->type_value, n->exp1->type_value, none);
  }
  nasm_commande("pop", "eax", NULL, NULL, "depile la permière operande dans eax");
  if (n->type_operation == '+'){
      verify_operand(n->exp1->type_value, n->exp2->type_value, entier);
      nasm_commande("add", "eax", "ebx", NULL, "operation +");
  }
  else if(n->type_operation == '*'){
    verify_operand(n->exp1->type_value, n->exp2->type_value, entier);
    nasm_commande("imul", "eax", "ebx", NULL, "operation *");
  }
  else if(n->type_operation == '-'){
    if(n->exp2!=NULL)
    {
      verify_operand(n->exp1->type_value, n->exp2->type_value, entier);
      nasm_commande("sub", "eax", "ebx", NULL, "operation -");
    }
    else
    {
      verify_operand(n->exp1->type_value, none, entier);
      nasm_commande("mov", "ebx", "0", NULL, "Met 0 dans ebx");
      nasm_commande("sub", "ebx", "eax", NULL, "operation - sur lui meme");
      nasm_commande("mov", "eax", "ebx", NULL, "envoi ebx dans eax");
    }
  }
  else if(n->type_operation == '/'){
    verify_operand(n->exp1->type_value, n->exp2->type_value, entier);
    nasm_commande("mov", "edx", "0", NULL, "Met edx a 0");
    nasm_commande("idiv", "ebx", NULL, NULL, "operation /");
  }
  else if(n->type_operation == '%'){
    verify_operand(n->exp1->type_value, n->exp2->type_value, entier);
    nasm_commande("mov", "edx", "0", NULL, "");
    nasm_commande("idiv", "ebx", NULL, NULL, "operation /");   
    nasm_commande("mov", "eax", "edx", NULL, "");  
  }
  else if(n->type_operation == '<'){
    type = booleen;
    verify_operand(n->exp1->type_value, n->exp2->type_value, entier);
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("setl", "al", NULL, NULL, "set 1 dans al si eax < ebx");
    nasm_commande("movzx", "eax", "al", NULL, "move dans register que l on peut empiler");
  }
  else if(n->type_operation == '>'){
    type = booleen;
    verify_operand(n->exp1->type_value, n->exp2->type_value, entier);
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("setg", "al", NULL, NULL, "set 1 dans al si eax > ebx");
    nasm_commande("movzx", "eax", "al", NULL, "move dans register que l on peut empiler");
  }
  else if(n->type_operation == '|'){
    type = booleen;
    verify_operand(n->exp1->type_value, n->exp2->type_value, booleen);
     nasm_commande("or", "eax", "ebx", NULL, "opération |");
  }
  else if(n->type_operation == '&'){
    type = booleen;
    verify_operand(n->exp1->type_value, n->exp2->type_value, booleen);
    nasm_commande("and", "eax", "ebx", NULL, "opération &");
  }
  else if(n->type_operation == 'e'){
    type = booleen;
    verify_operand(n->exp1->type_value, n->exp2->type_value, none);
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("sete", "al", NULL, NULL, "met 1 dans al si eax == ebx");
    nasm_commande("movzx", "eax", "al", NULL, "met 0 ou al dans eax");
  }
  else if(n->type_operation == 'i'){
    type = booleen;
    verify_operand(n->exp1->type_value, n->exp2->type_value, entier);
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("setle", "al", NULL, NULL, "met 1 dans al si eax <= ebx");
    nasm_commande("movzx", "eax", "al", NULL, "met 0 ou al dans eax");
  }
  else if(n->type_operation == 's'){
    type = booleen;
    verify_operand(n->exp1->type_value, n->exp2->type_value, entier);
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("setge", "al", NULL, NULL, "met 1 dans al si eax >= ebx");
    nasm_commande("movzx", "eax", "al", NULL, "met 0 ou al dans eax");
  }
  else if(n->type_operation == '!'){
    type = booleen;
    verify_operand(n->exp1->type_value, none, booleen);
    nasm_commande("xor", "eax", "1", NULL, "Negation du boolean");
  }
  else if(n->type_operation == 'd'){
    type = booleen;
    verify_operand(n->exp1->type_value, n->exp2->type_value, entier);
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("setne", "al", NULL, NULL, "met 1 dans al si eax != ebx");
    nasm_commande("movzx", "eax", "al", NULL, "met 0 ou al dans eax");
  }
  nasm_commande("push", "eax" , NULL, NULL, "empile le résultat");  
  return type;
}
