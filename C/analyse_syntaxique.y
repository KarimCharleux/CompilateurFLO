%{
#include <stdlib.h>
#include <stdio.h>
#include "arbre_abstrait.h"
#include "analyse_lexicale.h"


//n_programme* yyparse();
int yylex();
int yyerror(const char *s);
n_programme* arbre_abstrait;
%}

%union {
    int entier;
    enum Type type;
    char* identifiant;
    n_variable* variable;
    n_programme* prog;
    n_l_instructions* l_inst;
    n_instruction* inst;
    n_fonction* fonction;
    n_l_fonctions* l_fonctions;
    n_l_declaration* l_declaration;
    n_l_expression* l_expression;
    n_exp* exp;
    n_appel* appel;
}


%define parse.error detailed
%define parse.lac full
//Symboles terminaux qui seront fournis par yylex(), ordre non important

%token <identifiant> IDENTIFIANT
%token <entier> ENTIER
%token PLUS
%token FOIS
%token DIVISER
%token MOINS
%token EQUAL
%token INFERIEUR
%token SUPERIEUR
%token SUPERIEUR_OU_EQUAL
%token INFERIEUR_OU_EQUAL
%token PARENTHESE_OUVRANTE
%token PARENTHESE_FERMANTE
%token POINT_VIRGULE
%token ECRIRE
%token FIN 0
%token SI
%token SINON
%token VRAI
%token FAUX
%token TANTQUE
%token OU
%token NON
%token LIRE
%token MAX
%token MODULO
%token VIRGULE
%token ACCOLADE_OUVRANTE
%token ACCOLADE_FERMANTE
%token ET
%token RETOURNER

//completer

%type <prog> prog
%type <l_inst> listeInstructions
%type <inst> instruction
%type <fonction> fonction
%type <l_fonctions> listeFonctions
%type <l_declaration> listDeclaration
%type <l_expression> listeExpression
%type <inst> ecrire
%type <inst> lire
%type <exp> expr
%type <exp> facteur
%type <variable> variable
%type <variable> declaration
%type <variable> affectation
%type <exp> produit
%type <exp> boolean
%type <exp> somme
%type <appel> appel
%token <type> TYPE

%%

//  ____        __             _ _   
// |  _ \  ___ / _| __ _ _   _| | |_ 
// | | | |/ _ \ |_ / _` | | | | | __|
// | |_| |  __/  _| (_| | |_| | | |_ 
// |____/ \___|_|  \__,_|\__,_|_|\__|
//                                   


prog: listeFonctions POINT_VIRGULE listeInstructions {
    arbre_abstrait =creer_n_programme($1, $3, NULL);
} 

listeInstructions: instruction {
    $$ =creer_n_l_instructions($1 ,NULL);
} 
listeInstructions: instruction listeInstructions {
    $$ =creer_n_l_instructions($1 ,$2);
} 
listeFonctions: fonction {
    $$ =creer_n_l_fonctions($1 ,NULL);
} 
listeFonctions: fonction listeFonctions {
    $$ =creer_n_l_fonctions($1 ,$2);
} 


// -----------------------------------------------------------------------------------------------------------------
//  ___           _                   _   _                 
// |_ _|_ __  ___| |_ _ __ _   _  ___| |_(_) ___  _ __  ___ 
//  | || '_ \/ __| __| '__| | | |/ __| __| |/ _ \| '_ \/ __|
//  | || | | \__ \ |_| |  | |_| | (__| |_| | (_) | | | \__ \
// |___|_| |_|___/\__|_|   \__,_|\___|\__|_|\___/|_| |_|___/

// FONCTIONS -----------------------------------------------------------------------------------------------------------------

fonction: TYPE IDENTIFIANT PARENTHESE_OUVRANTE PARENTHESE_FERMANTE ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE {
    $$ = creer_n_fonction($1 ,$2, NULL, $6);
}
fonction: TYPE IDENTIFIANT PARENTHESE_OUVRANTE listDeclaration PARENTHESE_FERMANTE ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE {
    $$ = creer_n_fonction($1 ,$2, $4, $7);
}

// LIRE ECRIRE -----------------------------------------------------------------------------------------------------------------

instruction: ecrire POINT_VIRGULE {
	$$ =$1;
}
ecrire: ECRIRE PARENTHESE_OUVRANTE expr PARENTHESE_FERMANTE {
	
	$$ =creer_n_ecrire($3);
}
instruction: lire POINT_VIRGULE{
	$$ =$1;
}
facteur: lire{
    $$ =n_appel_to_n_expression(creer_n_appel("lire", NULL));
}
lire: LIRE PARENTHESE_OUVRANTE PARENTHESE_FERMANTE {
	$$ =creer_n_lire();
}

//SI SINON TANQUE -----------------------------------------------------------------------------------------------------------------

instruction: SI PARENTHESE_OUVRANTE boolean PARENTHESE_FERMANTE ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE
{
    $$ = creer_n_condition($3, $6, NULL);
}
instruction: SI PARENTHESE_OUVRANTE boolean PARENTHESE_FERMANTE ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE SINON ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE
{
    $$ = creer_n_condition($3, $6, $10);
}
instruction: TANTQUE PARENTHESE_OUVRANTE boolean PARENTHESE_FERMANTE ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE
{
    $$ = creer_n_boucle($3, $6);
}

// DECLARATION AFFECTATION -----------------------------------------------------------------------------------------------------------------

listDeclaration: declaration{
    $$ = creer_n_l_declaration($1, NULL);
}
listDeclaration: declaration VIRGULE listDeclaration{
    $$ = creer_n_l_declaration($1, $3);
}
declaration: TYPE IDENTIFIANT{
	$$ = creer_n_variable($1, $2, NULL);
}
affectation: IDENTIFIANT EQUAL expr {
	$$ = creer_n_variable(-1, $1, $3);
}
affectation: TYPE IDENTIFIANT EQUAL expr {
	$$ = creer_n_variable($1, $2, $4);
}

variable: IDENTIFIANT{
    $$ = creer_n_variable(-1, $1, NULL);
}

instruction: affectation POINT_VIRGULE {
    $$ = n_variable_to_n_instruction($1);
}
instruction: declaration POINT_VIRGULE {
    $$ = n_variable_to_n_instruction($1);
}
facteur: variable{
    $$ = n_variable_to_n_expression($1);
}

// APPEL & RETOUR -----------------------------------------------------------------------------------------------------------------

instruction: RETOURNER expr POINT_VIRGULE{
    $$ = creer_n_retour($2);
}
appel: IDENTIFIANT PARENTHESE_OUVRANTE PARENTHESE_FERMANTE {
    $$ = creer_n_appel($1, NULL);
}
appel: IDENTIFIANT PARENTHESE_OUVRANTE listeExpression PARENTHESE_FERMANTE {
    $$ = creer_n_appel($1, $3);
}

instruction: appel POINT_VIRGULE{
    $$ = n_appel_to_n_instruction($1);
}
facteur: appel{
    $$ = n_appel_to_n_expression($1);
}


// -----------------------------------------------------------------------------------------------------------------
//   ____                                     _                                       _   _                 
//  / ___|_ __ __ _ _ __ ___  _ __ ___   __ _(_)_ __ ___    ___  _ __   ___ _ __ __ _| |_(_) ___  _ __  ___ 
// | |  _| '__/ _` | '_ ` _ \| '_ ` _ \ / _` | | '__/ _ \  / _ \| '_ \ / _ \ '__/ _` | __| |/ _ \| '_ \/ __|
// | |_| | | | (_| | | | | | | | | | | | (_| | | | |  __/ | (_) | |_) |  __/ | | (_| | |_| | (_) | | | \__ \
//  \____|_|  \__,_|_| |_| |_|_| |_| |_|\__,_|_|_|  \___|  \___/| .__/ \___|_|  \__,_|\__|_|\___/|_| |_|___/
//                                                              |_|                                         

// FACTEUR -----------------------------------------------------------------------------------------------------------------

facteur: PARENTHESE_OUVRANTE expr PARENTHESE_FERMANTE {
    $$ =$2 ;
}
facteur: ENTIER{
    $$ = creer_n_entier($1);
}

// PRODUIT -----------------------------------------------------------------------------------------------------------------

produit: produit FOIS facteur{
    $$ = creer_n_operation('*', $1 , $3);
}
produit: produit DIVISER facteur{
    $$ = creer_n_operation('/', $1 , $3);
}
produit: facteur{
    $$ = $1;
}

// SOMME -----------------------------------------------------------------------------------------------------------------

somme: produit{
    $$ = $1;
}
somme: somme PLUS produit{
    $$ = creer_n_operation('+', $1 , $3);
}
somme: somme MOINS produit{
    $$ = creer_n_operation('-', $1 , $3);
}
somme: somme MODULO produit{
    $$ =creer_n_operation('%', $1 , $3);
}

// BOOLEAN -----------------------------------------------------------------------------------------------------------------

boolean: somme{
    $$ = $1;
}
boolean: VRAI {
    $$ = creer_n_boolean(1);
}
boolean: FAUX {
    $$ = creer_n_boolean(0);
}
boolean: NON boolean {}

// EXPRESSION -----------------------------------------------------------------------------------------------------------------

expr: boolean{
    $$ = $1;
}
listeExpression: expr {
    $$ = creer_n_l_expression($1, NULL);
}
listeExpression: expr VIRGULE listeExpression {
    $$ = creer_n_l_expression($1, $3);
}


// -----------------------------------------------------------------------------------------------------------------
//   ____                                      _                     
//  / ___|___  _ __ ___  _ __   __ _ _ __ __ _(_)___  ___  _ __  ___ 
// | |   / _ \| '_ ` _ \| '_ \ / _` | '__/ _` | / __|/ _ \| '_ \/ __|
// | |__| (_) | | | | | | |_) | (_| | | | (_| | \__ \ (_) | | | \__ \
//  \____\___/|_| |_| |_| .__/ \__,_|_|  \__,_|_|___/\___/|_| |_|___/
//                      |_|                                          

boolean: boolean OU boolean {
    $$ =creer_n_operation('|', $1 , $3);
}
boolean: boolean ET boolean {
    $$ =creer_n_operation('&', $1, $3);
}
boolean: expr INFERIEUR expr {
    $$ =creer_n_operation('<', $1, $3);
}
boolean: expr SUPERIEUR expr {
    $$ =creer_n_operation('>', $1, $3);
}
boolean: expr INFERIEUR_OU_EQUAL expr {
    $$ =creer_n_operation('i', $1, $3);
}
boolean: expr SUPERIEUR_OU_EQUAL expr {
    $$ =creer_n_operation('s', $1, $3);
}
boolean: expr EQUAL EQUAL expr {
    $$ =creer_n_operation('e', $1, $4);
}



%%

int yyerror(const char *s) {
    printf("yyerror : %s\n",s);
    return 0;
}

