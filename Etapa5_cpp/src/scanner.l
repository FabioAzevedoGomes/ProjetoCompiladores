/* DEFINICOES */
%{
    #include "../include/Type.h"  // For type enums
    #include "../include/Token.h"  // For Token data type
    #include "../include/Node.h"   // For Node data type
    #include "../include/Symbol.h" // For Symbol data type
    #include "../include/parser.tab.h"

    extern "C" int yylex();

    /* Linha atual do scanner */
    int current_line = 1;

    /**
     * Atribui o valor lexico a estrutura do token lido
     * O numero de linha e token ja estao disponiveis em:
     * - yytext:            Contem o token lido
     * - get_line_number(): Retorna a linha atual
     * @param type Tipo deste token
     */
    void atrib_lexico(int type);

%}

/* Expressoes Regulares*/
letra           [A-Za-z]
digito          [0-9]
alfanumerico    {letra}|{digito}

/* Identificador: Caractere alfabetico seguido de 0 ou mais caracteres alfanumericos */
identificador   ({letra}|_)({alfanumerico}|_)*

/* Char: qualquer caractere entre aspas simples que nao e aspas simples ('), nova linha (\n) ou vazio ("") */
literal_char    '[^'""\n]?'
/* String: 1 ou mais caracteres entre aspas duplas*/
literal_string  \"[^\n"]*\"
/* Int: Qualquer sequencia de 1 ou mais digitos precedidos de + ou - (opcional) */
literal_int     {digito}+
/* Float: Um inteiro seguido de um ponto e 1 ou mais digitos e, opcionalmente, o caractere E|e seguido de um inteiro */
literal_float   {literal_int}(\.{digito}+)((E|e)(\+|\-)?{literal_int})?

/* Conjunto de todos os literais */
literal    {literal_char}|{literal_string}|{literal_int}|{literal_float}

/* Comentario simples: Tudo que segue na linha após '//' */
comentario_simples  \/\/.*\n

/* Comentario multilinha: Tudo que esta entre '/ *' e '* /' */
comentario_multilinha_inicio "/*"
comentario_multilinha_fim    "*/"

/* Caracteres Especiais:
 Sao eles:
 Virgula (,)
 Ponto e virgula (;)
 Dois pontos (:)
 Abre e fecha parenteses ( )
 Abre e fecha colchetes [ ]
 Abre e fecha chaves { }
 Mais (+)
 Menos (-)
 Barra reta (|)
 Asterisco (*)
 Barra para frente (/)
 Menor (<)
 Maior (>)
 Igual (=)
 Exclamacao (!)
 E-comercial (&)
 Por cento (%)
 Sustenido (#)
 Chapeu (^)
 Ponto (.)
 Cifrao ($)
 Interrogacao (?)
*/
caractere_especial ","|";"|":"|"("|")"|"["|"]"|"{"|"}"|"+"|"-"|"|"|"*"|"/"|"<"|">"|"="|"!"|"&"|"%"|"#"|"^"|"."|"$"|"?"

/* Estado do analisador representando a leitura do conteudo de um comentario */
%x COMENTARIO

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
{caractere_especial} { atrib_lexico(yytext[0]); return yytext[0];}

    /* Operadores Compostos */
"<="        { atrib_lexico(TK_OC_LE);  return TK_OC_LE;}
">="        { atrib_lexico(TK_OC_GE);  return TK_OC_GE;}
"=="        { atrib_lexico(TK_OC_EQ);  return TK_OC_EQ;}
"!="        { atrib_lexico(TK_OC_NE);  return TK_OC_NE;}
"&&"        { atrib_lexico(TK_OC_AND); return TK_OC_AND;}
"||"        { atrib_lexico(TK_OC_OR);  return TK_OC_OR;}
">>"        { atrib_lexico(TK_OC_SR);  return TK_OC_SR;}
"<<"        { atrib_lexico(TK_OC_SL);  return TK_OC_SL;}

    /* Literais */
{literal_char}      { atrib_lexico(TK_LIT_CHAR);   return TK_LIT_CHAR;}
{literal_string}    { atrib_lexico(TK_LIT_STRING); return TK_LIT_STRING;}
{literal_int}       { atrib_lexico(TK_LIT_INT);    return TK_LIT_INT;}
{literal_float}     { atrib_lexico(TK_LIT_FLOAT);  return TK_LIT_FLOAT;}
"true"              { atrib_lexico(TK_LIT_TRUE);   return TK_LIT_TRUE;}
"false"             { atrib_lexico(TK_LIT_FALSE);  return TK_LIT_FALSE;}

    /* Identificadores */
{identificador}    { atrib_lexico(TK_IDENTIFICADOR); return TK_IDENTIFICADOR;}

    /* Ignora o comentario simples e incrementa a linha atual */
{comentario_simples}    {current_line++;}

    /* Comentario multi-linhas */
    
 /* Ao identificar '/ *' inicia o estado interno de comentario*/
{comentario_multilinha_inicio}          {BEGIN COMENTARIO;}
 /* Ignora tudo que esta dentro do comentario*/
<COMENTARIO>[^\n]                       {}
 /* Contabiliza as linhas */
<COMENTARIO>\n                          {current_line++;}
 /* Ao identificar '* /', encerra o estado interno de comentario */
<COMENTARIO>{comentario_multilinha_fim} {BEGIN 0;}

    /* Ignora espacos e TABS */
" "|\t {}

    /* Contador de Linhas */
\n     {current_line++;}

    /* Nao e permitido literais 'colados' a identificadores (ex. 12nome)  */
{literal}{identificador}   {return TOKEN_ERRO;}

    /* Caracteres que nao forem identificados pelas regras definidas acima sao considerados erros */
.      {return TOKEN_ERRO; }

%%

int get_line_number(void){
    return current_line;
}

void atrib_lexico(int type)
{
    char x; 
    int i;

    // Token value
    Value v;

    // Preenche o tipo do token e seu valor na estrutura valor_lexico
    switch(type)
    {
        // Literals
        case TK_LIT_CHAR:
            v.character = *(yytext+sizeof(char));
            yylval.lexval = new Token(get_line_number(), CAT_LITERAL, v, 3);
            break;
        case TK_LIT_STRING:
            for(x = 'a', i = 0; x != '\0'; i++)
                x = yytext[i];
            yytext[i-2] = '\0';
            v.name = strdup(yytext + 1);
            yylval.lexval = new Token(get_line_number(), CAT_LITERAL, v, 0);
            break;
        case TK_LIT_INT:
            v.integer = atoi(yytext);
            yylval.lexval = new Token(get_line_number(), CAT_LITERAL, v, 1);
            break;
        case TK_LIT_FLOAT:
            v.floating = atof(yytext);
            yylval.lexval = new Token(get_line_number(), CAT_LITERAL, v, 2);
            break;
        case TK_LIT_TRUE:
            v.boolean = 1; // 1 para representar o booleano true
            yylval.lexval = new Token(get_line_number(), CAT_LITERAL, v, 4);
            break;
        case TK_LIT_FALSE:
            v.boolean = 0; // 0 para representar o booleano false
            yylval.lexval = new Token(get_line_number(), CAT_LITERAL, v, 4);
            break;
        // Identifiers
        case TK_IDENTIFICADOR:
            v.name = strdup(yytext); // Copia o nome do identificador
            yylval.lexval = new Token(get_line_number(), CAT_IDENTIFIER, v, 0);
            break;
        // Composite operators
        case TK_OC_LE:
        case TK_OC_GE:
        case TK_OC_EQ:
        case TK_OC_NE:
        case TK_OC_AND:
        case TK_OC_OR:
        case TK_OC_SR:
        case TK_OC_SL:
            v.name = strdup(yytext); // Copia a cadeia de caracteres do operador
            yylval.lexval = new Token(get_line_number(), CAT_COMPOSITE_OPERATOR, v, 0);
            break;
        // Special characters
        default:
            v.name = strdup(yytext); // Copia o caractere
            yylval.lexval = new Token(get_line_number(), CAT_SPECIAL_CHARACTER, v, 0);
            break;

    }
}