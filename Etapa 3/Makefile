all: ast.o parser.tab.c lex.yy.c
	gcc -c main.c lex.yy.c 
	gcc -c parser.tab.c
	gcc -o etapa3 main.o ast.o lex.yy.o parser.tab.o -lfl -Wall

parser.tab.c:
	bison -d -v parser.y --report-file parser.output

lex.yy.c:
	flex scanner.l

ast.o:
	gcc -c ast.c -o ast.o -Wall

clean:
	rm lex.yy.c parser.tab.c *.o etapa3 *.output
