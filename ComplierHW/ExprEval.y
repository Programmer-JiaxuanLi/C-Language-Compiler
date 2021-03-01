%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yaccExample.h"
#include "Semantics.h"
#include "CodeGen.h"

extern int yylex();	/* The next token function. */
extern char *yytext;   /* The matched token text.  */
extern int yyleng;      /* The token text length.   */
extern int yyparse();
extern int regid;
void dumpTable();

char* fname;
extern struct list* varList;
extern struct list* addrList;
extern struct list* paraaddrList;
extern struct SymTab *table;
extern struct SymTab *lvartable;
extern struct SymTab *funtable;
extern struct SymEntry *entry;
extern int paranums;
extern int isBo;
extern int inpf;
extern int regid;

%}


%union {
  long val;
  char * string;
  struct ExprRes * ExprRes;
  struct InstrSeq * InstrSeq;
  struct BExprRes * BExprRes;
}

%type <ExprRes> BooleanExpr
%type <string> Id
%type <string> fId
%type <ExprRes> Factor
%type <ExprRes> Term
%type <ExprRes> Expr
%type <ExprRes> Hightest
%type <InstrSeq> StmtSeq
%type <InstrSeq> Stmt
%type <InstrSeq> Context
%type <InstrSeq> strcontext
%type <InstrSeq> varlist
//%type <BExprRes> BExpr
%type <ExprRes> BooleanSeq

%token boolean
%token While
%token Ident 		
%token IntLit
%token Read
%token Int
%token True
%token False
%token Write
%token AND
%token OR
%token IF
%token Else
%token EQ	
%token LT
%token LE
%token BT
%token BE
%token f
%token NE
%token Return
%token println
%token printsp
%token printstr
%%

Prog			:	Declarations StmtSeq  				{Finish($2); } ;
Declarations	:	Dec Declarations	    			{ };
Declarations	:					        			{ };

Dec             :   Int fId {fname = $2;} funbody
funbody         :   '('')' '{' StmtSeq Return BooleanExpr'}' {EnterName(funtable,fname, &entry);declareFunction(entry,$4,$6);};//{ declareFunction(entry,$7,$9) ; };

funbody         :  '('paras')' lvars ';'{ addressinit();} '{'StmtSeq Return BooleanExpr'}' {EnterName(funtable,fname, &entry); declareParameterFunction(entry,$8,$10) ;} ;

paras          :  paras','Int Id       {inpf = 1;paranums++;List_Append(varList,$4);}
paras          :  Int Id               {inpf = 1;paranums++;List_Append(varList,$2);}

lvars          :  lvars'~'Int Id       {List_Append(varList,$4);EnterName(lvartable,$4, &entry);dodeclaration("int",entry);}
lvars          :  Int Id               {List_Append(varList,$2);EnterName(lvartable,$2, &entry);dodeclaration("int",entry);}

Dec                :    Int Ident {EnterName(table, yytext, &entry); } arsize';'{};
arsize          :   '['IntLit mallocar']'  {} ;
mallocar        :   {Arraydodeclaration("intar",entry,yytext); } ;
arsize          :   {dodeclaration("int",entry);} ;

Dec             :   boolean Ident {EnterName(table, yytext, &entry); } boarsize';'{};
boarsize        :   '['IntLit bomallocar']'  { } ;
bomallocar      :   {Arraydodeclaration("boolar",entry,yytext); } ;
boarsize        :   {dodeclaration("boolean",entry); } ;

StmtSeq 		:	Stmt StmtSeq						{ $$ = AppendSeq($1, $2); } ;
StmtSeq		    :										{ $$ = NULL;} ;

Stmt            :   printsp BooleanExpr';'              { $$ = doPrintsp($2) ; };
Stmt            :   println ';'                         { $$ = doPrintln() ; };
Stmt			:	Write Context ';'				    { $$ = $2;  printf("测试bison方向");};
Stmt            :   printstr strcontext';'              { $$ = $2; };
Stmt            :   fId'('')'';'                        { $$ = callfunonly($1);};

strcontext      :   Ident                               { $$ = doPrintstr(yytext);}
Context         :   Context ',' BooleanSeq              { $$ = doAddPrint($1 ,$3); };
Context         :   BooleanSeq                          { $$ = doPrint($1); };

Stmt            :   Read'('varlist')'';'                { $$ = $3 ; } ;
varlist         :   varlist','Ident                     { $$ = doaddread($1,yytext) ; } ;
varlist         :   Ident                               { $$ = doread(yytext) ; } ;

Stmt            :   Id '['BooleanSeq']''=' BooleanSeq';'    {$$ = doArAssignment($1,$3,$6) ; } ;


Stmt            :   fId '('Idl')'';'                        { $$ = callParameterFun($1);};
Idl             :   Idl','Id                                {};
Idl             :   Id                                      {};

Stmt            :   Id '=' BooleanSeq ';'               { $$ = doBExprAssign($1, $3);} ;//之后不用结果不需要保存
//Stmt            :   Id '=''('BooleanExpr')'';'        { $$ = doBExprAssign($1, $4);} ;//之后不用结果不需要保存
Stmt            :   Id '=' True';'                      { $$ = doBExprTAssign($1);} ;//之后不用结果不需要保存
Stmt            :   Id '=' False';'                     { $$ = doBExprFAssign($1);} ;//之后不用结果不需要保存
//Stmt			:	IF '(' BExpr ')' '{' StmtSeq '}'	{ $$ = doIf($3, $6);};//不用结果不用保存。因为本来就已经是一个完整的句子了不需要保存寄存器的值

Stmt            :    IF '(' BooleanSeq  ')' '{' StmtSeq '}' Else '{'StmtSeq'}'    { $$ = doIf($3, $6, $10);};
Stmt            :    While '('BooleanSeq ')' '{'StmtSeq '}'                        { $$ = dowhile($3,$6);};

BooleanSeq         :    '!'BooleanExpr                       {$$ = doNotBoExpr($2); printf("在while括号里");} ;//要用值需要保存寄存器
BooleanSeq         :    BooleanSeq OR BooleanExpr            {$$ = doOrBoExpr($1,$3); } ;//要用值需要保存寄存器
BooleanSeq         :    BooleanSeq AND BooleanExpr           {$$ = doAndBoExpr($1,$3); } ;//要用值需要保存寄存器
BooleanSeq         :    BooleanExpr                          {$$ = $1;} ;

//BooleanExpr        :    '('BooleanSeq')'                      { $$ = $2;}//要用值需要保存寄存器
BooleanExpr        :    Expr EQ Expr                          { $$ = doBoExpr($1, $3);};//要用值需要保存寄存器
BooleanExpr        :    Expr LT Expr                          { $$ = doBoLTExpr($1, $3);};//要用值需要保存寄存器
BooleanExpr        :    Expr LE Expr                          { $$ = doBoLEExpr($1, $3);};//要用值需要保存寄存器
BooleanExpr        :    Expr BT Expr                          { $$ = doBoBTExpr($1, $3);};//要用值需要保存寄存器
BooleanExpr        :    Expr BE Expr                          { $$ = doBoBEExpr($1, $3);};//要用值需要保存寄存器
BooleanExpr        :    Expr NE Expr                          { $$ = doBoNEExpr($1, $3);};//要用值需要保存寄存器
BooleanExpr        :    Expr                                  { $$ = $1; } ;

//BExpr		 :	  Expr EQ Expr							 { $$ = doBExpr($1, $3);};//值只用于判断逻辑不会被后面的语句使用
//BExpr        :    Expr LT Expr                          { $$ = doBLTExpr($1, $3);};
//BExpr        :    Expr LE Expr                          { $$ = doBLEExpr($1, $3);};
//BExpr        :    Expr BT Expr                          { $$ = doBBTExpr($1, $3);};
//BExpr        :    Expr BE Expr                          { $$ = doBBEExpr($1, $3);};
//BExpr        :    Expr NE Expr                          { $$ = doBNEExpr($1, $3);};

Expr            :   Expr '-' Term                           { $$ = doMinus($1, $3); isBo = 0 ; printf("在while减号里");} ;//值需要被保存，归并至赋值语句后使用
Expr            :    Expr '+' Term                            { $$ = doAdd($1, $3); isBo = 0 ; } ;
Expr		    :	Term								    { $$ = $1;} ;
Term            :   Term '/' Hightest                       { $$ = doDivide($1, $3); isBo = 0;} ;
Term		    :	Term '*' Hightest						{ $$ = doMult($1, $3); isBo = 0;} ;
Term            :   Term '%' Hightest                       { $$ = doMod($1, $3); isBo = 0;} ;
Term		    :    Hightest    							{ $$ = $1; } ;
Hightest        :   Hightest '^' Factor                     { $$ = doExponentiation($1, $3);isBo = 0;}
Hightest        :   '-' Factor                              { $$ = doNegative($2); isBo = 0;} ;
Hightest        :   Factor                                  { $$ = $1; } ;
Factor          :   '('BooleanSeq')'                        { $$ = $2; } ;
Factor          :   IntLit                                  { $$ = doIntLit(yytext); isBo = 0;} ;
Factor          :   fId '('')'                              { $$ = callfun($1);};
Factor          :   Id                                      { $$ = doRval($1); } ;
Factor          :   Id '['BooleanSeq']'                     { $$ = doArRval($1,$3); } ;
Id			    : 	Ident									{ $$ = strdup(yytext); } ;
fId             :    f                                      { $$ = strdup(yytext); } ;
%%

yyerror(char *s)  {
  WriteIndicator(GetCurrentColumn());
  WriteMessage("Illegal Character in YACC");
}
