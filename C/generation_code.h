#ifndef __GENERATION_CODE__
#define __GENERATION_CODE__

#define MAX_SYMBOL_TABLE_SIZE 10
#define GLOBAL_SCOPE_NAME "_global"

typedef struct
{
  char* variable_name;
  int offset_with_ebp;
} Variable;

typedef struct
{
  enum Type type;
  char* symbol_name;
  enum Type* var_type;
  int current_memory_used;
  Variable* variables[MAX_SYMBOL_TABLE_SIZE];
} Symbol;

void nasm_clean_local_variables(char* symbol_name);

void nasm_prog(n_programme *n);
void nasm_liste_fonctions(n_l_fonctions *n);
void nasm_fonction(n_fonction* n);
void nasm_liste_instructions(n_l_instructions *n);
void nasm_instruction(n_instruction* n);
void nasm_exp(n_exp* n);
void nasm_operation(n_operation* n);


#endif
