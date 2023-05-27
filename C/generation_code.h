#ifndef __GENERATION_CODE__
#define __GENERATION_CODE__

enum Type {boolean = 0, entier = 1}; 

typedef struct
{
  enum Type l_instructions;
  char* function_name[];
} symbolTable;

void nasm_prog(n_programme *n);
void nasm_liste_fonctions(n_l_fonctions *n);
void nasm_fonction(n_fonction* n);
void nasm_liste_instructions(n_l_instructions *n);
void nasm_instruction(n_instruction* n);
void nasm_exp(n_exp* n);
void nasm_operation(n_operation* n);


#endif
