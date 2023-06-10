#ifndef __GENERATION_CODE__
#define __GENERATION_CODE__

#define MAX_SYMBOL_TABLE_SIZE 10
#define STRING_SIZE 30
#define GLOBAL_SCOPE_NAME "_global"

typedef struct
{
  char* variable_name;
  enum Type type;
  int offset_with_ebp;
} Variable;

typedef struct VariablesByScope VariablesByScope;
struct VariablesByScope
{
  Variable *variables[MAX_SYMBOL_TABLE_SIZE];
  VariablesByScope* next_scope;
};


typedef struct
{
  char* symbol_name;
  enum Type type;
  int current_memory_used;
  int nb_built_in_parameters;
  VariablesByScope* variables_by_scope;
} Symbol;

void nasm_clean_local_variables(char* symbol_name, int clean_symbol_table);
void printSymbols();
void printSymbol(char* symbole_name);

void nasm_prog(n_programme *n);
void nasm_liste_fonctions(n_l_fonctions *n);
void nasm_fonction(n_fonction* n);
void nasm_liste_instructions(n_l_instructions *n);
void nasm_instruction(n_instruction* n);
void nasm_exp(n_exp* n);
enum Type nasm_operation(n_operation* n);
enum Type nasm_appel(n_appel* appel);
VariablesByScope* get_new_scope_variables();
void clean_under_scope_variables(char* symbol_name);
VariablesByScope* get_last_scope_variables(Symbol* symbol);
Symbol* findSymbol(char* identifiant);

#endif
