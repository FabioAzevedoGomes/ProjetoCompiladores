        /* DEFINICOES */
%{
    #include "tokens.h"
    /* Linha atual do scanner */
    int current_line = 1;
    /* Funcao que conta quantidade de linhas ('\n') em uma string 'str' */
    int count_newlines(char* str);
%}

    /* Expressoes Regulares*/
letra           [A-Za-z]
digito          [0-9]
alfanumerico    {letra}|{digito}

/* Identificador: Caractere alfabeito seguido de 0 ou mais caracteres alfanumericos */
identificador   ({letra}|_)({alfanumerico}|_)*

/* Char: qualquer caractere entre aspas simples*/
/* TODO evitar situacoes do tipo ''' e ' \n ', retornando como um TK_ERRO apenas*/
literal_char    '.' 
/* String: 0 ou mais caracteres entre aspas duplas*/
/* TODO evitar situacoes onde ha quebra de linha dentro da string como "this is \n not valid"*/
literal_string  \".+\"
/* Int: Qualquer sequencia de 1 ou mais digitos precedidos de + ou - (opcional) */
literal_int     (\+|\-)?{digito}+
/* Float: Um inteiro seguido de um ponto e 1 ou mais digitos e/ou o caractere E|e seguido de um inteiro */
literal_float   {literal_int}(\.{digito}+)((E|e){literal_int})?

/* Conjunto de todos os literais */
literal    {literal_char}|{literal_string}|{literal_int}|{literal_float}

/* Comentario simples: Tudo que segue na linha após '//' */
comentario_simples  \/\/.*\n
/* Comentario multi-linha: Tudo presente entre '/*' e '* /'(Podendo ocupar mais de 1 linha) */
/* TODO utilizar outro metodo para avaliacao de comentario multi linha */
/* TODO talvez o metodo BEGIN para alterar o estado do analisador durante a avaliacao de comentarios (e talvez para strings tambem) */
comentario_multilinha \/\*(.|\n)*\*\/

%%  
    /* REGRAS E ACOES */

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
">>"        {return TK_OC_SR;}
"<<"        {return TK_OC_SL;}

    /* Literais */
{literal_char}      {return TK_LIT_CHAR;}
{literal_string}    {return TK_LIT_STRING;}
{literal_int}       {return TK_LIT_INT;}
{literal_float}     {return TK_LIT_FLOAT;}
"true"              {return TK_LIT_TRUE;}
"false"             {return TK_LIT_FALSE;}

    /* Identificadores */
{identificador}    {return TK_IDENTIFICADOR;}

    /* Ignora o comentario simples e incrementa a linha atual */
{comentario_simples}    {current_line++;}
    /* Ignora o comentario multi-linha e conta suas linhas */
{comentario_multilinha}   {current_line += count_newlines(yytext);}

    /* Ignora espacos e TABS */
" "|\t {}

    /* Contador de Linhas */
\n     {current_line++;}

    /* Nao e permitido literais 'colados' a identificadores (ex. 12nome, identificador1.34e-3, "string"identifier)  */
{literal}{identificador} {return TOKEN_ERRO;}
{identificador}{literal}   {return TOKEN_ERRO;}

    /* Caracteres que nao forem identificados pelas regras definidas sao considerados erros */
.      {return TOKEN_ERRO; }

%%
    /* FUNCOES OPCIONAIS */

int get_line_number(void){
    return current_line;
} 

int count_newlines(char* str){
    int line_count = 0;
    char* ptr = strchr(str, '\n');
    while(ptr != NULL){
        line_count++;
        ptr = strchr(ptr+1, '\n');
    }
    
    return line_count;
}
