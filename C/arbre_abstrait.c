#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arbre_abstrait.h"

/* Abbréviations: 
  * n -> noeud
  * l -> liste
*/


/* -----------------------------------------------------------------------------------------------------------------
#     _     __  __ _      _                      
#    / \   / _|/ _(_) ___| |__   __ _  __ _  ___ 
#   / _ \ | |_| |_| |/ __| '_ \ / _` |/ _` |/ _ \
#  / ___ \|  _|  _| | (__| | | | (_| | (_| |  __/
# /_/   \_\_| |_| |_|\___|_| |_|\__,_|\__, |\___|
#                                    |___/      
*/

//  PRIMAIRE -----------------------------------------------------------------------------------------------------------------
void afficher(char* s,int indent){
	for(int i = 0; i < indent; i++){
	printf(" "); 
	}
	printf("%s\n",s);
}
void afficher_caractere(char c,int indent){
	for(int i = 0; i < indent; i++){
	printf(" "); 
	}
	printf("%c\n",c);
}
void afficher_entier(int valeur,int indent){
	for(int i = 0; i < indent; i++){
	printf(" "); 
	
	}
	printf("[Entier:%d]\n",valeur);
}
void afficher_boolean(int valeur,int indent){
	for(int i = 0; i < indent; i++){
	printf(" "); 
	
	}
	printf("[Boolean:%d]\n",valeur);
}
void afficher_n_variable(n_variable variable, int indent)
{
	if(variable.expr != NULL)
	{
		afficher("<variable>", indent);
		afficher(variable.identifiant , indent+1);
		afficher_n_exp(variable.expr, indent+1);
		afficher("<variable>" , indent);
	} else
	{
		afficher("<variable>" , indent);
		afficher(variable.identifiant , indent+1);
		afficher("</variable>" , indent);
	}
}

// DEFAULT -----------------------------------------------------------------------------------------------------------------
void afficher_n_programme(n_programme* prog,int indent){
	afficher("<programme>",indent);
	afficher_n_l_fonctions(prog->fonctions,indent+1);
	afficher("</programme>",indent);
}
void afficher_n_l_fonctions(n_l_fonctions* fonctions ,int indent){
	afficher("<liste_instuctions>",indent);
	
	do {
		if (fonctions->fonction != NULL){
			afficher_n_fonction(fonctions->fonction,indent+1);
		}
		fonctions = fonctions->fonctions;
	} while(fonctions != NULL );
	afficher("</liste_instructions>",indent);
}
void afficher_n_fonction(n_fonction* fonction,int indent){
	afficher("<fonction>",indent+1);
	afficher("%d", fonction->type);
	afficher(fonction->identifiant, indent+1);
	afficher_n_l_instructions(fonction->l_instructions,indent+2);
	afficher("</fonction>",indent+1);
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
	if(instruction->type_instruction == i_lire){
		afficher("<lire>",indent);
	}
	if(instruction->type_instruction == i_condition){
		afficher_n_condition(instruction->u.condition,indent+1);
	}
	if(instruction->type_instruction == i_boucle)
	{
		afficher_n_boucle(instruction->u.boucle, indent+1);
	}
	if (instruction->type_instruction == i_declaration || instruction->type_instruction == i_affectation)
	{
		afficher_n_variable(instruction->u.variable, indent+1);
	}	
}
void afficher_n_exp(n_exp* exp ,int indent){
	if (exp->type_exp == i_operation){
		afficher_n_operation(exp->u.operation,indent);
	} else if (exp->type_exp == i_entier){
		afficher_entier(exp->u.valeur,indent);
	} else if (exp->type_exp == i_boolean){
		afficher_boolean(exp->u.valeur,indent);
	} else if (exp->type_exp == i_variable){
		afficher(exp->u.identifiant,indent);
	}
}
void afficher_n_operation(n_operation* operation ,int indent){
	afficher("<operation>",indent);
	afficher_n_exp(operation->exp1,indent+1);
	afficher_caractere(operation->type_operation,indent+1);
	afficher_n_exp(operation->exp2,indent+1);
	afficher("</operation>",indent);
}

// CONDITION ET BOUCLE -----------------------------------------------------------------------------------------------------------------
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



/* -----------------------------------------------------------------------------------------------------------------
#   ____                                                 _     
#  / ___|_ __ ___  ___ _ __   _ __   ___   ___ _   _  __| |___ 
# | |   | '__/ _ \/ _ \ '__| | '_ \ / _ \ / _ \ | | |/ _` / __|
# | |___| | |  __/  __/ |    | | | | (_) |  __/ |_| | (_| \__ \
#  \____|_|  \___|\___|_|    |_| |_|\___/ \___|\__,_|\__,_|___/                                                                  
*/

// DEFAULT -----------------------------------------------------------------------------------------------------------------
n_programme* creer_n_programme(n_l_fonctions* fonctions){
  	n_programme* n = malloc(sizeof(n_programme));
  	n->fonctions = fonctions;
  
  	return n;
}
n_l_instructions* creer_n_l_instructions(n_instruction* instruction ,n_l_instructions* instructions){
  	n_l_instructions* n = malloc(sizeof(n_l_instructions));
  	n->instruction = instruction;
  	n->instructions = instructions;
  	
	return n;
}
n_l_fonctions* creer_n_l_fonctions(n_fonction* fonction ,n_l_fonctions* fonctions){
	n_l_fonctions* n = malloc(sizeof(n_l_fonctions));
	n->fonction = fonction;
	n->fonctions = fonctions;

	return n;
}


// INSTRUCTION -----------------------------------------------------------------------------------------------------------------
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
n_fonction* creer_n_fonction(int type, char* identifiant , n_l_instructions* l_instructions)
{
	n_fonction* n= malloc(sizeof(n_instruction));
	n->type = type;
	n->identifiant = identifiant;
	n->l_instructions = l_instructions;

	return n;
}
n_instruction* creer_n_affectation(char* identifiant, n_exp* expr)
{
	n_instruction* n= malloc(sizeof(n_instruction));
	n->type_instruction = i_affectation;
	n->u.variable.identifiant = identifiant;
	n->u.variable.expr = expr;

	return n;
}
n_instruction* creer_n_variable(int type, char* identifiant, n_exp* expr)
{
	n_instruction* n= malloc(sizeof(n_instruction));
	n->type_instruction = i_declaration;	
	n->u.variable.identifiant = identifiant;
	n->u.variable.expr = expr;

	return n;
}
n_instruction* creer_n_appel(char* identifiant)
{
	n_instruction* n= malloc(sizeof(n_instruction));
	n->type_instruction = i_appel;
	n->u.identifiant = identifiant;

	return n;
}
n_instruction* creer_n_retour(n_exp* expr)
{
	n_instruction* n= malloc(sizeof(n_instruction));
	n->type_instruction = i_retour;
	n->u.exp = expr;

	return n;
}


// EXPRESSION -----------------------------------------------------------------------------------------------------------------
n_exp* get_n_variable(char* identifiant)
{
	n_exp* n = malloc(sizeof(n_exp));
	n->type_exp = i_variable;
	n->u.identifiant = identifiant;

	return n;
}
n_exp* creer_n_entier(int valeur){
  n_exp* n = malloc(sizeof(n_exp));
  n->type_exp = i_entier;
  n->u.valeur = valeur;
  return n;
}
n_exp* creer_n_boolean(int valeur)
{
	n_exp* n = malloc(sizeof(n_exp));
	n->type_exp = i_boolean;
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