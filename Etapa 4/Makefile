all: ast.o symbol_table.o parser.tab.c lex.yy.c
	gcc -c main.c lex.yy.c 
	gcc -c parser.tab.c
	gcc -o etapa4 main.o ast.o symbol_table.o lex.yy.o parser.tab.o -lfl -Wall

parser.tab.c:
	bison -d -v parser.y --report-file parser.output

lex.yy.c:
	flex scanner.l

ast.o:
	gcc -c ast.c -o ast.o -Wall

symbol_table.o:
	gcc -c symbol_table.c -o symbol_table.o -Wall

clean:
	rm lex.yy.c parser.tab.c *.o etapa4 *.output
