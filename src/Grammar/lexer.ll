%{ /* -*- C++ -*- */

#include <cerrno>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <string>

#include "Frontend/Driver.h"
#include "parser.h"

%}

%option noyywrap nounput noinput batch

%x COMMENT

ID          [a-zA-Z_][a-zA-Z_0-9]*

DEC_INT     [1-9][0-9]*
OCT_INT     0[0-7]*
HEX_INT     0[xX][0-9a-fA-F]+

DEC_FLOAT   ([0-9]+\.[0-9]*|\.[0-9]+)([eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+
HEX_FLOAT   0[xX]([0-9a-fA-F]+\.?[0-9a-fA-F]*|\.[0-9a-fA-F]+)[pP][+-]?[0-9]+

BLANK       [ \t\r]

%{
    static yy::parser::symbol_type
    makeIntConst(const std::string& text, const yy::parser::location_type& loc);

    static yy::parser::symbol_type
    makeFloatConst(const std::string& text, const yy::parser::location_type& loc);

    #define YY_USER_ACTION loc.columns(yyleng);
%}

%%

%{
    yy::location& loc = drv.getLocation();
    loc.step();
%}

{BLANK}+        loc.step();
\n+             loc.lines(yyleng); loc.step();

"//".*          loc.step();

"/*"            BEGIN(COMMENT);
<COMMENT>{
    "*/"        BEGIN(INITIAL); loc.step();
    [^*\n]+     loc.step();
    "*"         loc.step();
    \n+         loc.lines(yyleng); loc.step();
    <<EOF>>     {
                    throw yy::parser::syntax_error(
                        loc, "unterminated block comment");
                }
}

"const"         return yy::parser::make_CONST       (loc);
"int"           return yy::parser::make_INT         (loc);
"float"         return yy::parser::make_FLOAT       (loc);
"void"          return yy::parser::make_VOID        (loc);
"if"            return yy::parser::make_IF          (loc);
"else"          return yy::parser::make_ELSE        (loc);
"while"         return yy::parser::make_WHILE       (loc);
"break"         return yy::parser::make_BREAK       (loc);
"continue"      return yy::parser::make_CONTINUE    (loc);
"return"        return yy::parser::make_RETURN      (loc);

"+"             return yy::parser::make_PLUS        (loc);
"-"             return yy::parser::make_MINUS       (loc);
"*"             return yy::parser::make_STAR        (loc);
"/"             return yy::parser::make_SLASH       (loc);
"%"             return yy::parser::make_PERCENT     (loc);
"="             return yy::parser::make_ASSIGN      (loc);
"=="            return yy::parser::make_EQUAL       (loc);
"!="            return yy::parser::make_NOT_EQUAL   (loc);
"<"             return yy::parser::make_LESS        (loc);
">"             return yy::parser::make_GREATER     (loc);
"<="            return yy::parser::make_LESS_EQ     (loc);
">="            return yy::parser::make_GREATER_EQ  (loc);
"&&"            return yy::parser::make_AND         (loc);
"||"            return yy::parser::make_OR          (loc);
"!"             return yy::parser::make_NOT         (loc);
"("             return yy::parser::make_LPAREN      (loc);
")"             return yy::parser::make_RPAREN      (loc);
"["             return yy::parser::make_LBRACKET    (loc);
"]"             return yy::parser::make_RBRACKET    (loc);
"{"             return yy::parser::make_LBRACE      (loc);
"}"             return yy::parser::make_RBRACE      (loc);
","             return yy::parser::make_COMMA       (loc);
";"             return yy::parser::make_SEMICOLON   (loc);

{DEC_FLOAT}     return makeFloatConst(yytext, loc);
{HEX_FLOAT}     return makeFloatConst(yytext, loc);

{DEC_INT}       return makeIntConst(yytext, loc);
{OCT_INT}       return makeIntConst(yytext, loc);
{HEX_INT}       return makeIntConst(yytext, loc);

{ID}            return yy::parser::make_ID(yytext, loc);

.               {
                    throw yy::parser::syntax_error(
                        loc, "invalid character: " + std::string(yytext));
                }

<<EOF>>         return yy::parser::make_YYEOF(loc);

%%

static yy::parser::symbol_type
makeIntConst(const std::string& text, const yy::parser::location_type& loc)
{
    errno = 0;

    // Base 0 handles decimal, octal and hex prefixes.
    unsigned long long value = strtoull(text.c_str(), nullptr, 0);

    if (errno == ERANGE || value > UINT32_MAX)
        throw yy::parser::syntax_error(loc, "integer is out of range: " + text);

    // Wrap to a 32-bit int like competition compilers do (e.g. 2147483648).
    return yy::parser::make_INT_CONST(
        static_cast<int>(static_cast<uint32_t>(value)), loc);
}

static yy::parser::symbol_type
makeFloatConst(const std::string& text, const yy::parser::location_type& loc)
{
    errno = 0;

    float value = strtof(text.c_str(), nullptr);

    if (errno == ERANGE)
        throw yy::parser::syntax_error(loc, "float is out of range: " + text);

    return yy::parser::make_FLOAT_CONST(value, loc);
}
