#pragma once

#include "stdio.h"

int yylex(void);
void nom_token( int token, char *nom, char *valeur );
void afficher_analyse_lexicale();


