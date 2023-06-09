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
	printf("[booleen:%d]\n",valeur);
}
void afficher_n_variable(n_variable* variable, int indent)
{
	char string[15];
    sprintf(string, "<variable - %d>", variable->type);
	afficher(string, indent);
	if(variable->expr != NULL)
	{
		afficher(variable->identifiant , indent+1);
		afficher_n_exp(variable->expr, indent+1);
	} else
	{
		afficher(variable->identifiant , indent+1);
	}
	afficher("</variable>" , indent);
}

// DEFAULT -----------------------------------------------------------------------------------------------------------------
void afficher_n_programme(n_programme* prog,int indent){
	afficher("<programme>",indent);
	afficher_n_l_fonctions(prog->fonctions,indent+1);
	afficher_n_l_instructions(prog->instructions,indent+1);

	afficher("</programme>",indent);
}
void afficher_n_l_fonctions(n_l_fonctions* fonctions ,int indent){
	if(fonctions!=NULL)
	{
		afficher("<liste_fonctionss>",indent);
		do {
			if (fonctions->fonction != NULL){
				afficher_n_fonction(fonctions->fonction,indent+1);
			}
			fonctions = fonctions->fonctions;
		} while(fonctions != NULL );
		afficher("</liste_fonctionss>",indent);
	}
}
void afficher_n_fonction(n_fonction* fonction,int indent){
	afficher("<fonction>",indent+1);
	afficher(fonction->identifiant, indent+1);
	if(fonction->parametres!=NULL)
	{
		afficher_n_l_declaration(fonction->parametres, indent+2);
	}
	afficher_n_l_instructions(fonction->l_instructions,indent+2);
	afficher("</fonction>",indent+1);
}
void afficher_n_l_declaration(n_l_declaration* parametres, int indent)
{
	afficher("<liste_parametres>",indent);
	
	do {
		if (parametres->variable != NULL){
			afficher_n_variable(parametres->variable,indent+1);
		}
		parametres = parametres->l_declaration;
	} while(parametres != NULL );
	afficher("</liste_parametres>",indent);
}
void afficher_n_l_instructions(n_l_instructions* instructions ,int indent){
	if(instructions!=NULL)
	{
		afficher("<liste_instuctions>",indent);
		do {
			if (instructions->instruction != NULL){
				afficher_n_instruction(instructions->instruction,indent+1);
				instructions = instructions->instructions;
			}
		} while(instructions != NULL );
		afficher("</liste_instructions>",indent);
	}
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
	if ( instruction->type_instruction == i_appel_inst)
	{
		afficher("<  Appel  >",indent);
		afficher(instruction->u.appel->identifiant ,indent+1);
		afficher("</ Appel  >",indent);
	}
	if (instruction->type_instruction == i_retour)
	{
		afficher("<  <--  >",indent);
		afficher_n_exp(instruction->u.exp,indent+1);
		afficher("</ <--  >",indent);
	}
}
void afficher_n_exp(n_exp* exp ,int indent){
	if (exp->type_exp == i_operation){
		afficher_n_operation(exp->u.operation,indent);
	} else if (exp->type_exp == i_value){
		if(exp->type_value==booleen)
		{
			afficher_entier(exp->u.valeur,indent);
		}
		else{
			afficher_boolean(exp->u.valeur,indent);
		}
	} else if (exp->type_exp == i_variable){
		afficher("<variable>" , indent);
		afficher(exp->u.variable->identifiant,indent+1);
		afficher("</variable>" , indent);
	} else if (exp->type_exp == i_appel_expr)
	{
		afficher("<appel>" , indent);
		afficher(exp->u.appel->identifiant,indent+1);
		afficher("</appel>" , indent);
	}
	
}
void afficher_n_l_sinon_si(n_l_sinon_si* l_sinon_si ,int indent){
	afficher("<liste_conditions>",indent);
	do {
		if (l_sinon_si->condition != NULL){
			afficher_n_condition(l_sinon_si->condition,indent+1);
			l_sinon_si = l_sinon_si->l_sinon_si;
		}
	} while(l_sinon_si != NULL );
	afficher("</liste_conditions>",indent);
}
void afficher_n_operation(n_operation* operation ,int indent){
	afficher("<operation>",indent);
	afficher_n_exp(operation->exp1,indent+1);
	afficher_caractere(operation->type_operation,indent+1);
	if(operation->exp2 != NULL)
	{
		afficher_n_exp(operation->exp2,indent+1);
	}
	afficher("</operation>",indent);
}

// CONDITION ET BOUCLE -----------------------------------------------------------------------------------------------------------------
void afficher_n_condition(n_condition* condition, int indent)
{
	afficher("<condition>",indent);
	afficher_n_evaluation(condition->evaluation,indent+1);
	afficher("</condition>",indent);
	afficher_n_l_instructions(condition->l_instructions, indent+1);
	if(condition->l_instructions_2 !=NULL)
	{
		afficher("<sinon>",indent);
		afficher_n_l_instructions(condition->l_instructions_2, indent+1);
		afficher("</fin-conditionnel>",indent);
	}
}
void afficher_n_evaluation(n_evaluation* evaluation, int indent)
{
	afficher_n_exp(evaluation->expr ,indent);
}
void afficher_n_boucle(n_boucle* i_boucle, int indent)
{
	afficher("<tantque>",indent);
	afficher_n_exp(i_boucle->expr,indent+1);
	afficher("</tanque>",indent);
	afficher("<faire>", indent);
	afficher_n_l_instructions(i_boucle->l_instructions, indent+1);
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
n_programme* creer_n_programme(n_l_fonctions* fonctions, n_l_instructions* instructions, n_programme* programmeList){
  	n_programme* n = malloc(sizeof(n_programme));
  	n->fonctions = fonctions;
	n->instructions = instructions;
  
  	return n;
}
n_l_instructions* creer_n_l_instructions(n_instruction* instruction ,n_l_instructions* instructions){
  	n_l_instructions* n = malloc(sizeof(n_l_instructions));
  	n->instruction = instruction;
	n->instructions = NULL;
	
	if(instructions == NULL){
		return n;
	}
	n_l_instructions* l_instruction_pointer = instructions;
	while (l_instruction_pointer->instructions != NULL)l_instruction_pointer = l_instruction_pointer->instructions;
	l_instruction_pointer->instructions = n;

	return instructions;
}
n_l_fonctions* creer_n_l_fonctions(n_fonction* fonction ,n_l_fonctions* fonctions){
	n_l_fonctions* n = malloc(sizeof(n_l_fonctions));
	n->fonction = fonction;
	n->fonctions = NULL;

	if(fonctions==NULL)
	{
		return n;
	}
	n_l_fonctions* l_fonction_pointer= fonctions;
	while (l_fonction_pointer->fonctions != NULL)l_fonction_pointer = l_fonction_pointer->fonctions;
	l_fonction_pointer->fonctions = n;

	return fonctions;
}
n_l_declaration* creer_n_l_declaration(n_variable* variable, n_l_declaration* l_declaration)
{
	n_l_declaration* n = malloc(sizeof(n_l_declaration));
	n->variable = variable;
	n->l_declaration = l_declaration;

	return n;
}
n_l_expression* creer_n_l_expression(n_exp* expression, n_l_expression* l_expression){
	n_l_expression* n = malloc(sizeof(n_l_expression));
	n->expression = expression;
	n->l_expression = NULL;

	if(l_expression==NULL)
	{
		return n;
	}
	n_l_expression* l_expression_pointer= l_expression;
	while (l_expression_pointer->l_expression != NULL)l_expression_pointer = l_expression_pointer->l_expression;
	l_expression_pointer->l_expression = n;

	return l_expression;
}
n_l_sinon_si* creer_n_l_sinon_si(n_condition* condition, n_l_sinon_si* l_sinon_si)
{
	n_l_sinon_si* n =malloc(sizeof(n_l_sinon_si));
	n->condition = condition;
	n->l_sinon_si = l_sinon_si;

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

n_condition* creer_n_condition(int type, n_evaluation* evaluation, n_l_instructions* l_instructions, n_l_sinon_si* l_sinon_si, n_l_instructions* l_instructions_2){
	n_condition* condition = malloc(sizeof(n_condition));
	condition->type = type;
  	condition->evaluation = evaluation;
  	condition->l_instructions = l_instructions;
	condition->l_sinon_si = l_sinon_si;
  	condition->l_instructions_2 = l_instructions_2;
  	
	return condition;
}
n_evaluation* creer_n_evaluation(n_exp* expr){
	n_evaluation* evaluation = malloc(sizeof(n_evaluation));
	evaluation->expr = expr;

	return evaluation;
}
n_instruction* creer_n_boucle(n_exp* expr, n_l_instructions* l_instructions)
{
	n_instruction* n= malloc(sizeof(n_instruction));
	n_boucle* boucle = malloc(sizeof(n_boucle));
	n->type_instruction = i_boucle;
	boucle->expr = expr;
	boucle->l_instructions = l_instructions;
	n->u.boucle = boucle;

	return n;
}
n_fonction* creer_n_fonction(int type, char* identifiant ,n_l_declaration* l_declaration, n_l_instructions* l_instructions)
{
	n_fonction* n= malloc(sizeof(n_fonction));
	n->identifiant = identifiant;
	n->type = type;
	n->parametres = l_declaration;
	n->l_instructions = l_instructions;

	return n;
}
n_variable* creer_n_variable(int type, char* identifiant, n_exp* expr)
{
	n_variable* n= malloc(sizeof(n_variable));
	n->identifiant = identifiant;
	n->type = type;
	n->expr = expr;

	return n;
}
n_instruction* n_variable_to_n_instruction(n_variable* variable)
{
	n_instruction* n = malloc(sizeof(n_instruction));
	if(variable->type != none)
	{
		n->type_instruction = i_declaration;
	}
	else
	{
		n->type_instruction = i_affectation;
	}
	n->u.variable = variable;

	return n;
}
n_appel* creer_n_appel(char* identifiant, n_l_expression* l_expression)
{
	n_appel* n= malloc(sizeof(n_appel));
	n->identifiant = identifiant;
	n->parameters = l_expression;

	return n;
}
n_instruction* n_appel_to_n_instruction(n_appel* appel)
{
	n_instruction* n = malloc(sizeof(n_instruction));
	n->type_instruction = i_appel_inst;
	n->u.appel = appel;

	return n;
}
n_instruction* n_condition_to_n_instruction(n_condition* condition)
{
	if(condition->type!=i_si)exit(EXIT_FAILURE);

	n_instruction* n = malloc(sizeof(n_instruction));
	n->type_instruction = i_condition;
	n->u.condition = condition;

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
n_exp* n_variable_to_n_expression(n_variable* variable)
{
	n_exp* n = malloc(sizeof(n_exp));
	n->type_exp = i_variable;
	n->type_value = variable->type;
	n->u.variable = variable;

	return n;
}
n_exp* n_appel_to_n_expression(n_appel* appel)
{
	n_exp* n = malloc(sizeof(n_exp));
	n->type_exp = i_appel_expr;
	if(strcmp(appel->identifiant, "lire")==0)
	{
		n->type_value = entier;
	}
	else{
		n->type_value = none;
	}
	n->u.appel = appel;

	return n;
}
n_exp* creer_n_entier(int valeur, int sign){
  n_exp* n = malloc(sizeof(n_exp));
  n->type_exp = i_value;
  n->type_value = entier;
  if(sign > 0)
  {
	n->u.valeur = valeur;
  }
  else
  {
	n->u.valeur = -1*valeur;
  }
  return n;
}
n_exp* creer_n_boolean(int valeur)
{
	n_exp* n = malloc(sizeof(n_exp));
	n->type_exp = i_value;
	n->type_value = booleen;
	n->u.valeur = valeur;
  	return n;

}
n_exp* creer_n_operation(char type_operation,n_exp* exp1,n_exp* exp2){
  n_exp* n = malloc(sizeof(n_exp));
  n_operation* n_op = malloc(sizeof(n_operation));
  n->u.operation = n_op;
  n->type_exp = i_operation;
  n->type_value = exp1->type_value;
  n_op->type_operation = type_operation;
  n_op->exp1 = exp1;
  n_op->exp2 = exp2;
  return n;
}