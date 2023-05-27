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
    char* identifiant;
    n_programme* prog;
    n_l_instructions* l_inst;
    n_instruction* inst;
    n_fonction* fonction;
    n_l_fonctions* l_fonctions;
    n_exp* exp;
}


%define parse.error detailed
%define parse.lac full
//Symboles terminaux qui seront fournis par yylex(), ordre non important

%token <identifiant> IDENTIFIANT
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
%token <entier> ENTIER
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
%token TYPE_BOOLEAN
%token TYPE_ENTIER

//completer

%type <prog> prog
%type <l_inst> listeInstructions
%type <inst> instruction
%type <fonction> fonction
%type <l_fonctions> listeFonctions
%type <inst> ecrire
%type <inst> lire
%type <exp> expr
%type <exp> facteur
%type <exp> variable
%type <exp> produit
%type <exp> boolean
%type <exp> somme

%%

//  ____        __             _ _   
// |  _ \  ___ / _| __ _ _   _| | |_ 
// | | | |/ _ \ |_ / _` | | | | | __|
// | |_| |  __/  _| (_| | |_| | | |_ 
// |____/ \___|_|  \__,_|\__,_|_|\__|
//                                   


prog: listeFonctions {
arbre_abstrait =creer_n_programme($1);
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

fonction: IDENTIFIANT PARENTHESE_OUVRANTE PARENTHESE_FERMANTE ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE POINT_VIRGULE{
    $$ = creer_n_fonction($1 , $5);
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
lire: LIRE PARENTHESE_OUVRANTE PARENTHESE_FERMANTE {
	$$ =creer_n_lire();
}

//SI SINON TANQUE -----------------------------------------------------------------------------------------------------------------

instruction: SI PARENTHESE_OUVRANTE boolean PARENTHESE_FERMANTE ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE POINT_VIRGULE
{
    $$ = creer_n_condition($3, $6, NULL);
}
instruction: SI PARENTHESE_OUVRANTE boolean PARENTHESE_FERMANTE ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE SINON ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE POINT_VIRGULE
{
    $$ = creer_n_condition($3, $6, $10);
}
instruction: TANTQUE PARENTHESE_OUVRANTE boolean PARENTHESE_FERMANTE ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE POINT_VIRGULE
{
    $$ = creer_n_boucle($3, $6);
}

// DECLARATION AFFECTATION -----------------------------------------------------------------------------------------------------------------

instruction: TYPE_BOOLEAN IDENTIFIANT POINT_VIRGULE{
	$$ = creer_n_variable(0, $2, NULL);
}
instruction: TYPE_ENTIER IDENTIFIANT POINT_VIRGULE {
	$$ = creer_n_variable(1, $2, NULL);
}
instruction: IDENTIFIANT EQUAL expr POINT_VIRGULE {
	$$ = creer_n_affectation($1, $3);
}
instruction: TYPE_BOOLEAN IDENTIFIANT EQUAL expr POINT_VIRGULE {
	$$ = creer_n_variable(1, $2, $4);
}
instruction: TYPE_ENTIER IDENTIFIANT EQUAL expr POINT_VIRGULE {
	$$ = creer_n_variable(1, $2, $4);
}


// -----------------------------------------------------------------------------------------------------------------
//   ____                                     _                                       _   _                 
//  / ___|_ __ __ _ _ __ ___  _ __ ___   __ _(_)_ __ ___    ___  _ __   ___ _ __ __ _| |_(_) ___  _ __  ___ 
// | |  _| '__/ _` | '_ ` _ \| '_ ` _ \ / _` | | '__/ _ \  / _ \| '_ \ / _ \ '__/ _` | __| |/ _ \| '_ \/ __|
// | |_| | | | (_| | | | | | | | | | | | (_| | | | |  __/ | (_) | |_) |  __/ | | (_| | |_| | (_) | | | \__ \
//  \____|_|  \__,_|_| |_| |_|_| |_| |_|\__,_|_|_|  \___|  \___/| .__/ \___|_|  \__,_|\__|_|\___/|_| |_|___/
//                                                              |_|                                         

// VARIABLE -----------------------------------------------------------------------------------------------------------------

variable: IDENTIFIANT{
    $$ = get_n_variable($1);
}

// FACTEUR -----------------------------------------------------------------------------------------------------------------

facteur: variable{
    $$ = $1;
}
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

