/* -*-C++-*- */
%{
/* 
   We need trailing context for the distribution modifiers T(,) and
   I(,).  The BUGS language allows these modifiers to follow a
   syntactically valid stochastic relation separated by only white
   space.  To distinguish a modifier from the start of another
   stochastic relation, "T" and "I" are reserved names when followed
   by an opening bracket, but otherwise we allow the use of "T" and
   "I" as variable names in user code.
*/

#include <compiler/ParseTree.h>
#include "remap.h"
#include "parser.hh"

#include <stdexcept>

/* 
   The default error function yy_fatal_error will terminate the
   program. To avoid this, we define our own error function which
   throws an exception. Note: gcc -Wunused-function will generate a
   warning about yy_fatal_error defined but not used.
*/
[[noreturn]] void jags_scanner_error(const char *msg);
#define YY_FATAL_ERROR(msg) jags_scanner_error (msg)

%}

EXPONENT	[eE][+-]?[0-9]+
BRACKET		[ \t]*\(

%x COMMENT

%option nounput
%option yylineno
%option nodefault

%%
"var"			return VAR;
"data"                  return DATA;
"model"			return MODEL;
"for"        	        return FOR;
"in"                    return IN;
";"			return ';';
","			return ',';
":"			return ':';
"["			return '[';
"]"			return ']';
"("			return '(';
")"			return ')';
"{"			return '{';
"}"			return '}';
"<="                    return LE;
"<"                     return LT;
">="                    return GE;
">"                     return GT;
"&&"                    return AND;
"||"                    return OR;
"!="                    return NE;
"!"                     return NOT;
"=="                    return EQ;
"="			return '=';
"~"			return '~';
"<-"			return ARROW;
"+"			return '+';
"-"			return '-';
"^"                     return '^';
"**"                    return '^';
"*"			return '*';
"/"			return '/';
"|>"                    return PIPE; 

"T"/{BRACKET}           return 'T';
"I"/{BRACKET}           return 'I';

"%"+[^% \t\r\n]*"%" {
    /* Special operators, e.g. %*% for matrix multiplication */
    yylval.stringptr = new std::string(yytext);
    return SPECIAL;
}

"/*"           BEGIN(COMMENT);
<COMMENT>[^*]*          ; /* Eat up anything that's not a '*' */
<COMMENT>"*"+[^*/]*     ; /* Eat up '*'s not followed by a '/'  */
<COMMENT>"*"+"/"        BEGIN(INITIAL);

[ \t\r\n\f]+            /* Eat whitespace */
"#".*\n                 /* Eat comments */

([0-9]+){EXPONENT}? {
  yylval.val = atof(yytext); return DOUBLE;
}
([0-9]+"."[0-9]*){EXPONENT}? {
  yylval.val = atof(yytext); return DOUBLE;
}
("."[0-9]+){EXPONENT}? {
  yylval.val = atof(yytext); return DOUBLE;
}

[a-zA-Z]+[a-zA-Z0-9\._]*	 {
  yylval.stringptr = new std::string(yytext);
  return NAME;
}

<<EOF>>	yyterminate();

\357\277\275 {
    /* Slightly more informative than returning BADCHAR (See below) */
    jags_scanner_error("Coding error in file (UTF-8 replacement char U+FFFD found by parser)");
}

. {
  /* Default rule for unmatched input.
     We return a BADCHAR which is not matched by any grammar rule,
     and so causes a syntax error.
  */
  return BADCHAR;
}

%%

int yywrap()
{
  return 1;
}

[[noreturn]] void jags_scanner_error (char const *msg)
{
    throw std::runtime_error(msg);
}
