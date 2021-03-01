/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     boolean = 258,
     While = 259,
     Ident = 260,
     IntLit = 261,
     Read = 262,
     Int = 263,
     True = 264,
     False = 265,
     Write = 266,
     AND = 267,
     OR = 268,
     IF = 269,
     Else = 270,
     EQ = 271,
     LT = 272,
     LE = 273,
     BT = 274,
     BE = 275,
     f = 276,
     NE = 277,
     Return = 278,
     println = 279,
     printsp = 280,
     printstr = 281
   };
#endif
/* Tokens.  */
#define boolean 258
#define While 259
#define Ident 260
#define IntLit 261
#define Read 262
#define Int 263
#define True 264
#define False 265
#define Write 266
#define AND 267
#define OR 268
#define IF 269
#define Else 270
#define EQ 271
#define LT 272
#define LE 273
#define BT 274
#define BE 275
#define f 276
#define NE 277
#define Return 278
#define println 279
#define printsp 280
#define printstr 281




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 32 "ExprEval.y"
{
  long val;
  char * string;
  struct ExprRes * ExprRes;
  struct InstrSeq * InstrSeq;
  struct BExprRes * BExprRes;
}
/* Line 1529 of yacc.c.  */
#line 109 "ExprEval.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

