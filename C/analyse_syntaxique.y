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

instruction: ecrire {
	$$ =$1;
}

ecrire: ECRIRE PARENTHESE_OUVRANTE expr PARENTHESE_FERMANTE POINT_VIRGULE {
	
	$$ =creer_n_ecrire($3);
}

instruction: lire {
	$$ =$1;
}

lire: LIRE PARENTHESE_OUVRANTE PARENTHESE_FERMANTE POINT_VIRGULE {
	$$ =creer_n_lire();
}

instruction: SI PARENTHESE_OUVRANTE expr PARENTHESE_FERMANTE ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE POINT_VIRGULE
{
    $$ = creer_n_condition($3, $6, NULL);
}

instruction: SI PARENTHESE_OUVRANTE expr PARENTHESE_FERMANTE ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE SINON ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE POINT_VIRGULE
{
    $$ = creer_n_condition($3, $6, $10);
}

instruction: TANTQUE PARENTHESE_OUVRANTE expr PARENTHESE_FERMANTE ACCOLADE_OUVRANTE listeInstructions ACCOLADE_FERMANTE POINT_VIRGULE
{
    $$ = creer_n_boucle($3, $6);
}







expr: expr PLUS expr{
	$$ =creer_n_operation('+', $1, $3);
}

expr: expr FOIS expr{
	$$ =creer_n_operation('*', $1 , $3);
}

expr: expr MOINS expr{
    $$ =creer_n_operation('-', $1 , $3);
}

expr: expr MODULO expr{
    $$ =creer_n_operation('%', $1 , $3);
}

expr: expr DIVISER expr POINT_VIRGULE{
    $$ =creer_n_operation('/', $1 , $3);
}

expr: PARENTHESE_OUVRANTE expr PARENTHESE_FERMANTE  POINT_VIRGULE{
	$$ =$2 ;
}

expr: ENTIER{
	$$ = creer_n_entier($1);
}

expr: expr OU expr POINT_VIRGULE {
    $$ =creer_n_operation('|', $1 , $3);
}

expr: expr ET expr {
    $$ =creer_n_operation('&', $1, $3);
}

expr: NON expr POINT_VIRGULE {}


%%

int yyerror(const char *s) {
    printf("yyerror : %s\n",s);
    return 0;
}

