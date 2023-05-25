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

/* fonction locale, permet d'afficher un commentaire */
void nasm_comment(char *comment) {
  if(comment != NULL) {
    printifm("\t\t ; %s", comment);//le point virgule indique le début d'un commentaire en nasm. Les tabulations sont là pour faire jolie.
  }
  printifm("%s","\n");
}

/*
Affiche une commande nasm sur une ligne
Par convention, les derniers opérandes sont nuls si l'opération a moins de 3 arguments.
*/
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


void nasm_prog(n_programme *n) {
  
  printifm("%%include\t'%s'\n","io.asm");
  printifm("%s","\nsection\t.bss\n");
  printifm("%s", "sinput:\tresb\t255\t;reserve a 255 byte space in memory for the users input string\n");
  printifm("%s","\nsection\t.text\n");
  printifm("%s","global _start\n");
  printifm("%s","_start:\n");
  nasm_liste_instructions(n->instructions);
  //pour quitter le programme
  nasm_commande("mov", "eax", "1" , NULL, "1 est le code de SYS_EXIT");
  nasm_commande("int", "0x80", NULL, NULL, "exit");
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
		nasm_commande("pop", "eax", NULL, NULL, NULL); //on dépile la valeur d'expression sur eax
		nasm_commande("call", "iprintLF", NULL, NULL, NULL); //on envoie la valeur d'eax sur la sortie standard
	}
  if(n->type_instruction == i_lire){
	}
  if(n->type_instruction == i_condition){
    nasm_exp(n->u.condition.expr);
    //nasm_commande("pop", "eax", NULL, NULL, "dépile le résultat");
    // test if >1
    nasm_commande("cmp", "eax", "1", NULL, "compare");
    nasm_commande("setg", "al", NULL, NULL, "met 1 dans al si eax > ebx");
    nasm_commande("movzx", "eax", "al", NULL, "met 0 ou al dans eax");

    nasm_commande("cmp", "eax", "1", NULL, " on verifie la condition");
    // on crée les labels pour le si, le else et la fin
    char label_if[10];
    sprintf(label_if, "if%d", if_label_count);
    char label_else[10];
    sprintf(label_else, "else%d", else_label_count);
    char label_endif[10];
    sprintf(label_endif, "endif%d", fin_label_count);

    nasm_commande("jnz", label_else, NULL, NULL, "Aller au sinon");
    sprintf(label_if, "if%d:", label_count);
    nasm_commande(label_if, NULL, NULL, NULL, "Entrer dans le si");
    nasm_liste_instructions(n->u.condition.l_instructions);
    nasm_commande("jmp", label_endif, NULL, NULL, "Aller au si");

    if(n->u.condition.l_instructions_2==NULL)
    {
      sprintf(label_endif, "endif%d:", fin_label_count);
      nasm_commande(label_endif, NULL, NULL, NULL, "Aller a la fin");
    }
    else
    {
      sprintf(label_else, "else%d:", label_count);
      nasm_commande(label_else, NULL, NULL, NULL, "Aller dans le sinon");
      nasm_liste_instructions(n->u.condition.l_instructions_2);
      sprintf(label_endif, "endif%d:", fin_label_count);
      nasm_commande(label_endif, NULL, NULL, NULL, "Aller a la fin");
    }
	}
  if(n->type_instruction == i_boucle)
  {

	}

}

void nasm_exp(n_exp* n){
	if (n->type_exp == i_operation){
		nasm_operation(n->u.operation);
	} else if (n->type_exp == i_entier){
	      char buffer[10];
	      sprintf(buffer, "%d", n->u.valeur);
      	nasm_commande("push", buffer, NULL, NULL, NULL) ;
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
    nasm_commande("imul", "ebx", NULL, NULL, "operation *");
  }
  else if(n->type_operation == '-'){
    nasm_commande("sub", "ebx", NULL, NULL, "operation -");
  }
  else if(n->type_operation == '/'){
    nasm_commande("idiv", "ebx", NULL, NULL, "operation /");
  }
  else if(n->type_operation == '%'){
    nasm_commande("mov", "edx", "0", NULL, "");
    nasm_commande("idiv", "ebx", NULL, NULL, "operation /");   
    nasm_commande("mov", "eax", "edx", NULL, "");  
  }
  else if(n->type_operation == '<'){
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("setl", "al", NULL, NULL, "set 1 dans al si eax < ebx");
    nasm_commande("movzx", "eax", "al", NULL, "met 0 ou al dans eax");
  }
  else if(n->type_operation == '>'){
    nasm_commande("cmp", "eax", "ebx", NULL, "compare");
    nasm_commande("setg", "al", NULL, NULL, "met 1 dans al si eax > ebx");
    nasm_commande("movzx", "eax", "al", NULL, "met 0 ou al dans eax");
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
  nasm_commande("push",  "eax" , NULL, NULL, "empile le résultat");  
}
