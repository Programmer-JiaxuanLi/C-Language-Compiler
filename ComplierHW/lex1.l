%{
#include "yaccExample.h"
#include "ExprEval.tab.h"

#define YY_INPUT(buf,result,max_size) \
	{ int c = GetSourceChar(); \
          result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
        }

%}

letter [A-Za-z]
digit [0-9]

%%
f{digit}*           {return f;}
read                {return Read;}
printstr            {return printstr;}
printsp             {return printsp;}
println             {return println;}
boolean             {return boolean;}
while               {return While;}
return              {return Return;} 
true                {return True;}
false               {return False;}
if					{return IF;}
else                {return Else;}
int					{return Int;}
print				{return Write;}
{letter}({letter}|{digit})*	{return Ident;}
{digit}{digit}*		{return IntLit;}
\=\=		     	{return EQ; }
\<                  {return LT; }
\<\=                {return LE; }
\>                  {return BT; }
\>\=                {return BE; }
\!\=                {return NE; }
\=					{return '=';}
\+					{return '+';}
\*					{return '*';}
\;					{return ';';}
\:                  {return ':';}
\{					{return '{';}
\}					{return '}';}
\(					{return '(';}
\)					{return ')';}
\-                  {return '-';}
\/                  {return '/';}
\%                  {return '%';}
\^                  {return '^';}
\!                  {return '!';}
\[                  {return '[';}
\]                  {return ']';}
\~                  {return '~';}
\|\|                {return OR ;}
\&\&                {return AND;}
[ ] 			    {}
\t				    {}
\r				    {}
\n				    {}
\,                  {return ',';}
.				{WriteIndicator(GetCurrentColumn());
				   WriteMessage("Illegal Character in lex");}

%%

	
yywrap () {
}
			
	
