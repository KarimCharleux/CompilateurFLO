#pragma once

/* Abbréviations: 
  * n -> noeud
  * l -> liste
  * exp -> expression (arithmétique)
*/


typedef struct n_programme n_programme; /* Noeud du type programme */
typedef struct n_l_instructions n_l_instructions; /* Noeud du type liste d'instructions */
typedef struct n_instruction n_instruction; /* Noeud du type liste instruction */
typedef struct n_ecrire n_ecrire; /* Noeud du type instruction ecrire */
typedef struct n_lire n_lire;
typedef struct n_exp n_exp; /* Noeud du type expression (arithmétique) */
typedef struct n_operation n_operation; /* Noeud du type operation (arithmétique) */
typedef struct n_condition n_condition;
typedef struct n_boucle n_boucle;
typedef struct n_variable n_variable;





struct n_variable{
  char* identifiant;
  n_exp* expr;
};



struct n_programme{   // pour le moment un programme est juste une liste d'instructions. Il faudra ajouter une liste de définitions de fonctions.
	n_l_instructions* instructions;	
};

struct n_l_instructions{
	n_instruction* instruction;
	n_l_instructions* instructions;
};

struct n_condition
{
  n_exp* expr;
  n_l_instructions* l_instructions;
  n_l_instructions* l_instructions_2;
};

struct n_boucle
{
  n_exp* expr;
  n_l_instructions* l_instructions;
};

struct n_instruction{
  enum { i_ecrire, i_lire, i_condition, i_boucle, i_affectation, i_declaration} type_instruction; // pour le moment une instruction peut-être uniquement de type écrire. Il faudra ajouter affectation, exécution de fonction, si,sinon, etc...
  union{ 
    n_exp* exp; // pour ecrire(exp);
    n_condition condition; 
    n_boucle boucle;
    n_variable variable;
  }u;
};

struct n_exp{
  enum { i_operation, i_entier, i_boolean, i_variable} type_exp; // pour le moment une expression  peut-être une opération ou un entier
  union{ 
    n_operation* operation;
    int valeur;
    char* identifiant;
  }u;
};

struct n_operation{
  char type_operation;
  n_exp* exp1;
  n_exp* exp2;
};




void afficher_n_programme(n_programme* prog,int indent);
void afficher_n_l_instructions(n_l_instructions* instructions ,int indent);
void afficher_n_instruction(n_instruction* instruction ,int indent);
void afficher_n_ecrire(n_ecrire* ecrire,int indent);
void afficher_n_lire(n_lire* lire,int indent);
void afficher_n_exp(n_exp* exp ,int indent);
void afficher_n_operation(n_operation* operation ,int indent);
void afficher_n_condition(n_condition i_condition, int indent);
void afficher_n_boucle(n_boucle i_boucle, int indent);
void afficher_n_variable(n_variable variable, int indent);

n_programme* creer_n_programme(n_l_instructions* instructions);
n_l_instructions* creer_n_l_instructions(n_instruction* instruction ,n_l_instructions* instructions);
n_instruction* creer_n_ecrire(n_exp* exp);
n_instruction* creer_n_lire();
n_instruction* creer_n_max(n_exp* exp);

n_instruction* creer_n_variable(int type, char* id, n_exp* expr);
n_instruction* creer_n_affectation(char* id, n_exp* expr);
n_instruction* creer_n_variable(int type, char* id, n_exp* expr);
n_exp* get_n_variable(char* identifiant);

n_exp* creer_n_entier(int valeur);
n_exp* creer_n_boolean(int valeur);
n_exp* creer_n_operation(char type_operation, n_exp* exp1, n_exp* exp2);

n_instruction* creer_n_condition(n_exp* expr, n_l_instructions* l_instructions, n_l_instructions* l_instructions_2);
n_instruction* creer_n_boucle(n_exp* expr, n_l_instructions* l_instructions);