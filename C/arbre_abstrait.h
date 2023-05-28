#pragma once

/* Abbréviations: 
  * n -> noeud
  * l -> liste
  * exp -> expression (arithmétique)
*/

enum Type {none = -1, boolean = 0, entier = 1}; 

/* -----------------------------------------------------------------------------------------------------------------
#  ____  _                   _                       
# / ___|| |_ _ __ _   _  ___| |_ _   _ _ __ ___  ___ 
# \___ \| __| '__| | | |/ __| __| | | | '__/ _ \/ __|
#  ___) | |_| |  | |_| | (__| |_| |_| | | |  __/\__ \
# |____/ \__|_|   \__,_|\___|\__|\__,_|_|  \___||___/
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
typedef struct n_fonction n_fonction;
typedef struct n_l_fonctions n_l_fonctions;
typedef struct n_l_declaration n_l_declaration;
typedef struct n_l_expression n_l_expression;
typedef struct n_appel n_appel;





// SECONDAIRES -----------------------------------------------------------------------------------------------------------------
struct n_fonction
{
  char* identifiant;
  enum Type type;
  n_l_declaration* parametres;
  n_l_instructions* l_instructions;
};
struct n_variable{
  char* identifiant;
  enum Type type;
  n_exp* expr;
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
struct n_appel
{
  char* identifiant;
  n_l_expression* parameters;
};
struct n_l_fonctions
{
  n_fonction* fonction;
	n_l_fonctions* fonctions;
};
struct n_l_declaration
{
  n_variable* variable;
  n_l_declaration* l_declaration;
};
struct n_l_expression
{
  n_exp* expression;
  n_l_expression* l_expression;
};





// PRINCIPALES -----------------------------------------------------------------------------------------------------------------
struct n_programme{ 
  n_l_fonctions* fonctions;
  n_l_instructions* instructions;
};
struct n_l_instructions{
	n_instruction* instruction;
	n_l_instructions* instructions;
};
struct n_instruction{
  enum { i_ecrire, i_lire, i_condition, i_boucle, i_affectation, i_declaration, i_appel, i_retour} type_instruction; // pour le moment une instruction peut-être uniquement de type écrire. Il faudra ajouter affectation, exécution de fonction, si,sinon, etc...
  union{ 
    n_exp* exp; // pour ecrire(exp);
    n_condition* condition; 
    n_boucle* boucle;
    n_variable* variable;
    n_fonction* fonction;
    n_appel* appel;
  }u;
};

struct n_exp{
  enum {i_value, i_operation, i_variable} type_exp; // pour le moment une expression  peut-être une opération ou un entier
  enum Type type_value;
  union{ 
    n_operation* operation;
    int valeur;
    n_variable* variable;
  }u;
};
struct n_operation{
  char type_operation;
  n_exp* exp1;
  n_exp* exp2;
};



/* -----------------------------------------------------------------------------------------------------------------
#     _     __  __ _      _                      
#    / \   / _|/ _(_) ___| |__   __ _  __ _  ___ 
#   / _ \ | |_| |_| |/ __| '_ \ / _` |/ _` |/ _ \
#  / ___ \|  _|  _| | (__| | | | (_| | (_| |  __/
# /_/   \_\_| |_| |_|\___|_| |_|\__,_|\__, |\___|
#                                    |___/      
*/
void afficher_n_programme(n_programme* prog,int indent);
void afficher_n_l_instructions(n_l_instructions* instructions ,int indent);
void afficher_n_instruction(n_instruction* instruction ,int indent);
void afficher_n_l_fonctions(n_l_fonctions* fonctions ,int indent);
void afficher_n_fonction(n_fonction* fonction,int indent);
void afficher_n_ecrire(n_ecrire* ecrire,int indent);
void afficher_n_lire(n_lire* lire,int indent);
void afficher_n_exp(n_exp* exp ,int indent);
void afficher_n_operation(n_operation* operation ,int indent);
void afficher_n_condition(n_condition* i_condition, int indent);
void afficher_n_boucle(n_boucle* i_boucle, int indent);
void afficher_n_variable(n_variable* variable, int indent);
void afficher_n_l_declaration(n_l_declaration* parametres, int indent);

/* -----------------------------------------------------------------------------------------------------------------
#   ____                                                 _     
#  / ___|_ __ ___  ___ _ __   _ __   ___   ___ _   _  __| |___ 
# | |   | '__/ _ \/ _ \ '__| | '_ \ / _ \ / _ \ | | |/ _` / __|
# | |___| | |  __/  __/ |    | | | | (_) |  __/ |_| | (_| \__ \
#  \____|_|  \___|\___|_|    |_| |_|\___/ \___|\__,_|\__,_|___/                                                                  
*/

// DEFAULT -----------------------------------------------------------------------------------------------------------------
n_programme* creer_n_programme(n_l_fonctions* fonctions, n_l_instructions* instructions, n_programme* programmeList);
n_l_instructions* creer_n_l_instructions(n_instruction* instruction ,n_l_instructions* instructions);
n_l_fonctions* creer_n_l_fonctions(n_fonction* fonction ,n_l_fonctions* fonctions);
n_l_declaration* creer_n_l_declaration(n_variable* variable, n_l_declaration* l_declaration);
n_l_expression* creer_n_l_expression(n_exp* expression, n_l_expression* l_expression);
n_fonction* creer_n_fonction(int type, char* identifiant ,n_l_declaration* l_declaration, n_l_instructions* l_instructions);
n_variable* creer_n_variable(int type, char* identifiant, n_exp* expr);
n_appel* creer_n_appel(char* identifiant, n_l_expression* l_expression);

// INSTRUCTION -----------------------------------------------------------------------------------------------------------------
n_instruction* creer_n_ecrire(n_exp* exp);
n_instruction* creer_n_lire();
n_instruction* creer_n_max(n_exp* exp);

n_instruction* creer_n_condition(n_exp* expr, n_l_instructions* l_instructions, n_l_instructions* l_instructions_2);
n_instruction* creer_n_boucle(n_exp* expr, n_l_instructions* l_instructions);
n_instruction* creer_n_retour(n_exp* expr);

n_instruction* n_appel_to_n_instruction(n_appel* appel);
n_instruction* n_variable_to_n_instruction(n_variable* variable);

// EXPRESSION -----------------------------------------------------------------------------------------------------------------
n_exp* n_variable_to_n_expr(n_variable* variable);
n_exp* creer_n_entier(int valeur);
n_exp* creer_n_boolean(int valeur);
n_exp* creer_n_operation(char type_operation, n_exp* exp1, n_exp* exp2);

