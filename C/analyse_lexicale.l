/*
 * Analyseur lexical du compilateur L en FLEX
 * Representer tout ce qui peut etre decrit par regex
 */
 
%{
#include "arbre_abstrait.h"
#include "symboles.h"
%}
%option yylineno
%option nounput
%option noinput

entier               ([1-9][0-9]*|0)
chiffre              [0-9]
lettre               [a-zA-Z]
alphanum             ({lettre}|{chiffre})
espace               [ \t\r\n]

%%
"#".*                { /* Ignorer commentaires */  }
{espace}+            { /* Ignorer espaces */       }
"+"                  { return PLUS;                }
"*"                  { return FOIS;                }
"/"                  { return DIVISER;             }
"-"                  { return MOINS;               }
"="                  { return EQUAL;               }
"<"                  { return INFERIEUR;           }
">"                  { return SUPERIEUR;           }
">="                 { return SUPERIEUR_OU_EQUAL;  }
"<="                 { return INFERIEUR_OU_EQUAL;  }
"("                  { return PARENTHESE_OUVRANTE; }
")"                  { return PARENTHESE_FERMANTE; }
"{"                  { return ACCOLADE_OUVRANTE;   }
"}"                  { return ACCOLADE_FERMANTE;   }
"ecrire"             { return ECRIRE;              }
";"                  { return POINT_VIRGULE;       }
","                  { return VIRGULE;             }
"si"                 { return SI;                  }
"et"                 { return ET;                  }
"ou"                 { return OU;                  }
<<EOF>>              { return FIN;                 }  /* Fin de fichier */
"sinon"              { return SINON;               }
"Vrai"               { return VRAI;                }
"Faux"               { return FAUX;                }
"tantque"            { return TANTQUE;             }
"ou"                 { return OU;                  }
"non"                { return NON;                 }
"lire"               { return LIRE;                }
"retourner"          { return RETOURNER;           }
"max"                { return MAX;                 }
"boolean"            { return TYPE_BOOLEAN;        }
"entier"             { return TYPE_ENTIER;         }
"%"                  { return MODULO;              }
{entier}             { yylval.entier=atoi(yytext); return ENTIER;}
{lettre}{alphanum}*  { yylval.identifiant=strdup(yytext);  return IDENTIFIANT;}


.                    {                                /* Erreur, autre caractètre quelconque */
                       fprintf( stderr, "Ligne %d: caractère invalide: %s\n", yylineno,yytext ); \
                       exit(-1); 
                     } 
%%
int yywrap(){
  return 1;
}

/*******************************************************************************
 * Fonction auxiliaire qui donne le nom des différents tokens
 ******************************************************************************/
//  prend le int qui correspond au caractere defini


int token_fermes[] = {PLUS,FOIS,PARENTHESE_OUVRANTE,PARENTHESE_FERMANTE,ECRIRE,POINT_VIRGULE, SI, SINON, VRAI, FAUX, TANTQUE, OU, NON, LIRE, MAX, MODULO, ACCOLADE_OUVRANTE, ACCOLADE_FERMANTE, EQUAL, VIRGULE, SUPERIEUR_OU_EQUAL, INFERIEUR_OU_EQUAL, ET, OU, INFERIEUR, SUPERIEUR, DIVISER, MOINS};
char *str_token[] = {"+","*","(",")", "ecrire", ";", "si", "sinon", "vrai", "faux", "tantque", "||", "!", "lire", "max", "%", "{", "}", "=", ",", ">=", "<=", "&", "|", "<", ">", "/", "-"};

int nbMotsClefs = 28;

void nom_token( int token, char *nom, char *valeur ) {   

  for(int i = 0; i < nbMotsClefs;i++){
  	if(token == token_fermes[i]){
  	  strcpy( nom, "symbole" );
  	  strcpy( valeur, str_token[i]);
  	  return;
  	}
  }
  
  if( token == ENTIER ) {
    strcpy( nom, "entier" );
    strcpy( valeur, yytext );
  }
  else if( token == IDENTIFIANT ) {
    strcpy( nom, "identifiant" );  
    strcpy( valeur, yytext );      
  }
}

/*******************************************************************************
 * Fonction auxiliaire appelée par le compilateur en mode -l pour tester l'analyse
 * lexicale. Le fichier source doit être indiqué dans la variable globale yyin.
 * Cette fonction imprime le résultat de l'analyse lexicale sur la sortie standard.
 ******************************************************************************/
void afficher_analyse_lexicale() {
  int uniteCourante;
  char nom[100];
  char valeur[100]; 
    uniteCourante = yylex();  
  while (uniteCourante != FIN) {
    nom_token( uniteCourante, nom, valeur );
    printf("%s\t%s\t%s\n", yytext, nom, valeur);
    uniteCourante = yylex(); 
  } 
}
