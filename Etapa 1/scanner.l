        /* Definicoes */
%{
    #include "tokens.h"
    int current_line = 1;
%}

    /* Expressoes Regulares*/
letra           [A-Za-z]
digito          [0-9]
alfanumerico    {letra}|{digito}

identificador   ({letra}|_)({alfanumerico}|_)*

literal_char    '.'
literal_string  \".+\"
literal_int     (\+|\-)?{digito}+
literal_float   {literal_int}(\.{digito}+)((E|e){literal_int})?

comentario_simples  \/\/.*\n
    /* TODO Comentarios multi-linha*/

%%  /* Regras e Acoes*/

    /* Palavras Reservadas */
"int"       { return TK_PR_INT; }
"float"     { return TK_PR_FLOAT; }
"bool"      { return TK_PR_BOOL; }
"char"      { return TK_PR_CHAR; }
"string"    { return TK_PR_STRING; }
"if"        { return TK_PR_IF; }
"then"      { return TK_PR_THEN; }
"else"      { return TK_PR_ELSE; }
"while"     { return TK_PR_WHILE; }
"do"        { return TK_PR_DO; }
"input"     { return TK_PR_INPUT; }
"output"    { return TK_PR_OUTPUT; }
"return"    { return TK_PR_RETURN; }
"const"     { return TK_PR_CONST; }
"static"    { return TK_PR_STATIC; }
"foreach"   { return TK_PR_FOREACH; }
"for"       { return TK_PR_FOR; }
"switch"    { return TK_PR_SWITCH; }
"case"      { return TK_PR_CASE; }
"break"     { return TK_PR_BREAK; }
"continue"  { return TK_PR_CONTINUE; }
"class"     { return TK_PR_CLASS; }
"private"   { return TK_PR_PRIVATE; }
"public"    { return TK_PR_PUBLIC; }
"protected" { return TK_PR_PROTECTED; }
"end"       { return TK_PR_END; }
"default"   { return TK_PR_DEFAULT; }

    /* Caracteres Especiais */
","         {return ',';}
";"         {return ';';}
":"         {return ':';}
"("         {return '(';}
")"         {return ')';}
"["         {return '[';}
"]"         {return ']';}
"{"         {return '{';}
"}"         {return '}';}
"+"         {return '+';}
"-"         {return '-';}
"|"         {return '|';}
"*"         {return '*';}
"/"         {return '/';}
"<"         {return '<';}
">"         {return '>';}
"="         {return '=';}
"!"         {return '!';}
"&"         {return '&';}
"%"         {return '%';}
"#"         {return '#';}
"^"         {return '^';}
"."         {return '.';}
"$"         {return '$';}

    /* Operadores Compostos */
"<="        {return TK_OC_LE;}
">="        {return TK_OC_GE;}
"=="        {return TK_OC_EQ;}
"!="        {return TK_OC_NE;}
"&&"        {return TK_OC_AND;}
"||"        {return TK_OC_OR;}
">>"        {return TK_OC_SL;}
"<<"        {return TK_OC_SR;}

    /* Literais */
{literal_char}      {return TK_LIT_CHAR;}
{literal_string}    {return TK_LIT_STRING;}
{literal_int}       {return TK_LIT_INT;}
{literal_float}     {return TK_LIT_FLOAT;}
"true"              {return TK_LIT_TRUE;}
"false"             {return TK_LIT_FALSE;}

    /* Identificadores */
{identificador}    {return TK_IDENTIFICADOR;}

    /* Ignora comentarios*/
{comentario_simples}    {current_line++;}

    /* Ignora espacos e TABS*/
" "|\t {}

    /* Contador de Linhas*/
\n         {current_line++;}

.           {return TOKEN_ERRO; }
%%      /* Funcoes Opcionais */
int get_line_number(void){
    return current_line;
} 
