#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arbre_abstrait.h"

/* Abbréviations: 
  * n -> noeud
  * l -> liste
*/


//affiche l'entier avec sa valeur après un certain nombres d'espaces et un retour à la ligne
void afficher(char* s,int indent){
	for(int i = 0; i < indent; i++){
	printf(" "); 
	}
	printf("%s\n",s);
}

//affiche l'entier avec sa valeur après un certain nombres d'espaces
void afficher_caractere(char c,int indent){
	for(int i = 0; i < indent; i++){
	printf(" "); 
	}
	printf("%c\n",c);
}

//affiche la chaine de caractère s après un certain nombres d'espaces
void afficher_entier(int valeur,int indent){
	for(int i = 0; i < indent; i++){
	printf(" "); 
	
	}
	printf("[Entier:%d]\n",valeur);
}



void afficher_n_programme(n_programme* prog,int indent){
	afficher("<programme>",indent);
	afficher_n_l_instructions(prog->instructions,indent+1);
	afficher("</programme>",indent);
}




void afficher_n_l_instructions(n_l_instructions* instructions ,int indent){
	afficher("<liste_instuctions>",indent);
	
	do {
		if (instructions->instruction != NULL){
			afficher_n_instruction(instructions->instruction,indent+1);
		}
		instructions = instructions->instructions;
	} while(instructions != NULL );
	afficher("</liste_instructions>",indent);
}


void afficher_n_instruction(n_instruction* instruction ,int indent){
	if(instruction->type_instruction == i_ecrire){
		afficher("<ecrire>",indent);
		afficher_n_exp(instruction->u.exp,indent+1);
		afficher("</ecrire>",indent);
	}
	if(instruction->type_instruction == i_condition){
		afficher_n_condition(instruction->u.condition,indent+1);
	}
	if(instruction->type_instruction == i_boucle)
	{
		afficher_n_boucle(instruction->u.boucle, indent+1);
	}
}


void afficher_n_exp(n_exp* exp ,int indent){
	if (exp->type_exp == i_operation){
		afficher_n_operation(exp->u.operation,indent);
	} else if (exp->type_exp == i_entier){
		afficher_entier(exp->u.valeur,indent);
	}
}


void afficher_n_operation(n_operation* operation ,int indent){
	afficher("<operation>",indent);
	afficher_caractere(operation->type_operation,indent+1);
	afficher_n_exp(operation->exp1,indent+1);
	afficher_n_exp(operation->exp2,indent+1);
	afficher("</operation>",indent);
}
void afficher_n_condition(n_condition condition, int indent)
{
	afficher("<condition>",indent);
	afficher_n_exp(condition.expr,indent+1);
	afficher("</condition>",indent);
	afficher_n_l_instructions(condition.l_instructions, indent+1);
	if(condition.l_instructions_2 !=NULL)
	{
		afficher("<sinon>",indent);
		afficher_n_l_instructions(condition.l_instructions_2, indent+1);
		afficher("</fin-conditionnel>",indent);
	}
}
void afficher_n_boucle(n_boucle i_boucle, int indent)
{
	afficher("<tantque>",indent);
	afficher_n_exp(i_boucle.expr,indent+1);
	afficher("</tanque>",indent);
	afficher("<faire>", indent);
	afficher_n_l_instructions(i_boucle.l_instructions, indent+1);
	afficher("</faire>", indent);
}

n_programme* creer_n_programme(n_l_instructions* instructions){
  n_programme* n = malloc(sizeof(n_programme));
  n->instructions = instructions;
  return n;
}

n_l_instructions* creer_n_l_instructions(n_instruction* instruction ,n_l_instructions* instructions){
  n_l_instructions* n = malloc(sizeof(n_l_instructions));
  n->instruction = instruction;
  n->instructions = instructions;
  return n;
}

n_instruction* creer_n_ecrire(n_exp* exp){
  n_instruction* n = malloc(sizeof(n_instruction));
  n->type_instruction = i_ecrire;
  n->u.exp = exp;
  return n;
}

n_instruction* creer_n_lire(){
  n_instruction* n = malloc(sizeof(n_instruction));
  n->type_instruction = i_lire;
  return n;
}

n_exp* creer_n_entier(int valeur){
  n_exp* n = malloc(sizeof(n_exp));
  n->type_exp = i_entier;
  n->u.valeur = valeur;
  return n;
}

n_exp* creer_n_operation(char type_operation,n_exp* exp1,n_exp* exp2){
  n_exp* n = malloc(sizeof(n_exp));
  n_operation* n_op = malloc(sizeof(n_operation));
  n->u.operation = n_op;
  n->type_exp = i_operation;
  n_op->type_operation = type_operation;
  n_op->exp1 = exp1;
  n_op->exp2 = exp2;
  return n;
}

n_instruction* creer_n_condition(n_exp* expr, n_l_instructions* l_instructions, n_l_instructions* l_instructions_2)
{
	n_instruction* n = malloc(sizeof(n_instruction));
  	n->type_instruction = i_condition;
  	n->u.condition.expr = expr;
  	n->u.condition.l_instructions = l_instructions;
  	n->u.condition.l_instructions_2 = l_instructions_2;
  	return n;
}

n_instruction* creer_n_boucle(n_exp* expr, n_l_instructions* l_instructions)
{
	n_instruction* n= malloc(sizeof(n_instruction));
	n->type_instruction = i_boucle;
	n->u.boucle.expr = expr;
	n->u.boucle.l_instructions = l_instructions;

	return n;
}