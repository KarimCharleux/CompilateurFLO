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
    n_programme* prog;
    n_l_instructions* l_inst;
    n_instruction* inst;
    n_exp* exp;
}


%define parse.error detailed
%define parse.lac full
//Symboles terminaux qui seront fournis par yylex(), ordre non important

%token IDENTIFIANT
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

//completer

%type <prog> prog
%type <l_inst> listeInstructions
%type <inst> instruction
%type <inst> ecrire
%type <inst> lire
%type <exp> expr
%type <exp> facteur
%type <exp> produit
%type <exp> boolean

%%

prog: listeInstructions {
arbre_abstrait =creer_n_programme($1);
} 

listeInstructions: instruction {
$$ =creer_n_l_instructions($1 ,NULL);
} 

listeInstructions: instruction listeInstructions {
$$ =creer_n_l_instructions($1 ,$2);
} 

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

instruction: IDENTIFIANT EQUAL expr POINT_VIRGULE{
	creer_n_affectation($3);
}




facteur: PARENTHESE_OUVRANTE expr PARENTHESE_FERMANTE {
    $$ =$2 ;
}
facteur: ENTIER{
    $$ = creer_n_entier($1);
}

produit: produit FOIS facteur{
    $$ =creer_n_operation('*', $1 , $3);
}
produit: produit DIVISER facteur{
    $$ =creer_n_operation('/', $1 , $3);
}
produit: facteur{
    $$ = $1;
}

expr: produit{
    $$ = $1;
}

expr: expr PLUS produit{
	$$ =creer_n_operation('+', $1, $3);
}


expr: expr MOINS expr{
    $$ =creer_n_operation('-', $1 , $3);
}

expr: expr MODULO expr{
    $$ =creer_n_operation('%', $1 , $3);
}

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

boolean: NON boolean {}

boolean: VRAI {
    $$ = creer_n_boolean(1);
}
boolean: FAUX {
    $$ = creer_n_boolean(0);
}

%%

int yyerror(const char *s) {
    printf("yyerror : %s\n",s);
    return 0;
}

