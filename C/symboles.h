/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SYMBOLES_H_INCLUDED
# define YY_YY_SYMBOLES_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    FIN = 0,                       /* FIN  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENTIFIANT = 258,             /* IDENTIFIANT  */
    ENTIER = 259,                  /* ENTIER  */
    PLUS = 260,                    /* PLUS  */
    FOIS = 261,                    /* FOIS  */
    DIVISER = 262,                 /* DIVISER  */
    MOINS = 263,                   /* MOINS  */
    EQUAL = 264,                   /* EQUAL  */
    DIFFERENT = 265,               /* DIFFERENT  */
    INFERIEUR = 266,               /* INFERIEUR  */
    SUPERIEUR = 267,               /* SUPERIEUR  */
    SUPERIEUR_OU_EQUAL = 268,      /* SUPERIEUR_OU_EQUAL  */
    INFERIEUR_OU_EQUAL = 269,      /* INFERIEUR_OU_EQUAL  */
    PARENTHESE_OUVRANTE = 270,     /* PARENTHESE_OUVRANTE  */
    PARENTHESE_FERMANTE = 271,     /* PARENTHESE_FERMANTE  */
    POINT_VIRGULE = 272,           /* POINT_VIRGULE  */
    ECRIRE = 273,                  /* ECRIRE  */
    SI = 274,                      /* SI  */
    SINON = 275,                   /* SINON  */
    VRAI = 276,                    /* VRAI  */
    FAUX = 277,                    /* FAUX  */
    TANTQUE = 278,                 /* TANTQUE  */
    OU = 279,                      /* OU  */
    NON = 280,                     /* NON  */
    LIRE = 281,                    /* LIRE  */
    MODULO = 282,                  /* MODULO  */
    VIRGULE = 283,                 /* VIRGULE  */
    ACCOLADE_OUVRANTE = 284,       /* ACCOLADE_OUVRANTE  */
    ACCOLADE_FERMANTE = 285,       /* ACCOLADE_FERMANTE  */
    ET = 286,                      /* ET  */
    RETOURNER = 287,               /* RETOURNER  */
    TYPE = 288                     /* TYPE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 14 "analyse_syntaxique.y"

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
    n_l_sinon_si* l_sinon_si;
    n_exp* exp;
    n_appel* appel;
    n_condition* condition;
    n_evaluation* evaluation;

#line 116 "symboles.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SYMBOLES_H_INCLUDED  */
