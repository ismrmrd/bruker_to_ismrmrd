/* 
 * Sample Scanner2: 
 * Description: Count the number of characters and the number of lines 
 *              from standard input
 * Usage: (1) $ flex sample2.lex
 *        (2) $ gcc lex.yy.c -lfl
 *        (3) $ ./a.out
 *            stdin> whatever you like
 *	      stdin> Ctrl-D
 * Questions: Is it ok if we do not indent the first line?
 *            What will happen if we remove the second rule?
 */

%option noyywrap

%option c++
%{
#include "brukerparameterparser.hpp"
%}

%s PARSE_VARIABLE
%s PARSE_VISU
%s PARSE_INFO
%s PARSE_VARIABLE_SIZE
%s PARSE_VARIABLE_VALUE

varstart    "##$"
visstart    "$$ @vis= "
infostart   "##"
string      \<[^\>]*\>
integer     [\-0-9]+
float       [\-0-9]+"."[0-9]+([eE][-+]?[0-9]+)?
float2       [\-0-9]+([eE][-+]?[0-9]+)
labelval     [a-zA-z0-9_]+
ws          [ \t\n]+
infoval     [^#]+

%%
{ws}                /* do nothing */
{varstart} BEGIN(PARSE_VARIABLE);return VARIABLE_START;
{infostart} BEGIN(PARSE_INFO); return INFO_START;
{visstart}  BEGIN(PARSE_VISU); return VIS_START;
<PARSE_VARIABLE>[^\=]*   return VARIABLE_NAME;
<PARSE_VARIABLE>"=( "   BEGIN(PARSE_VARIABLE_SIZE);
<PARSE_VARIABLE>"="     BEGIN(PARSE_VARIABLE_VALUE);/* Do nothing */
<PARSE_VARIABLE_SIZE>[0-9]+ return VARIABLE_SIZE;
<PARSE_VARIABLE_SIZE>" )\n" BEGIN(PARSE_VARIABLE_VALUE);
<PARSE_VARIABLE_VALUE>"("   return MULTI_VALUE_START;
<PARSE_VARIABLE_VALUE>")"   return MULTI_VALUE_END;
<PARSE_VARIABLE_VALUE>{string}   return STRING_VALUE;
<PARSE_VARIABLE_VALUE>{float}   return FLOAT_VALUE;
<PARSE_VARIABLE_VALUE>{float2}   return FLOAT_VALUE;
<PARSE_VARIABLE_VALUE>{integer}   return INTEGER_VALUE;
<PARSE_VARIABLE_VALUE>{labelval}   return LABEL_VALUE;
<PARSE_VISU>{infoval} BEGIN(INITIAL);return VISU_VALUE;
<PARSE_INFO>{infoval} BEGIN(INITIAL);return INFO_VALUE;

.          /* Do nothing */
%%


