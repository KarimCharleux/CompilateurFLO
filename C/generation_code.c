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

// -----------------------------------------------------------------------------------------------------------------
//  ____        __             _ _   
// |  _ \  ___ / _| __ _ _   _| | |_ 
// | | | |/ _ \ |_ / _` | | | | | __|
// | |_| |  __/  _| (_| | |_| | | |_ 
// |____/ \___|_|  \__,_|\__,_|_|\__|
//  

// DEFAULT -----------------------------------------------------------------------------------------------------------------
void nasm_prog(n_programme *n) {
  
  printifm("%%include\t'%s'\n","io.asm");
  printifm("%s","\nsection\t.bss\n");
  printifm("%s", "sinput:\tresb\t255\t;reserve a 255 byte space in memory for the users input string\n");
  printifm("%s","\nsection\t.text\n");
  printifm("%s","global _start\n");
  printifm("%s","_start:\n");

  nasm_liste_fonctions(n->fonctions);

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
  nasm_liste_instructions(n->l_instructions);
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

  }
  if(n->type_instruction == i_declaration)
  {
    nasm_exp(n->u.variable.expr);
    nasm_commande("pop", "eax", NULL, NULL, "Recupere leresultqt dans eax");

  }
  if (n->type_instruction == i_fonction)
  {
    
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
	      //WORKING ON IT
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
