all: ast.o parser.tab.c lex.yy.c
	gcc -c main.c lex.yy.c parser.tab.c
	gcc -o etapa3 main.o ast.o lex.yy.o parser.tab.o -lfl

parser.tab.c:
	bison -d -v parser.y --report-file parser.output

lex.yy.c:
	flex scanner.l

libast.o:
	gcc -c ast.c -o ast.o

clean:
	rm lex.yy.c parser.tab.c *.o parser.tab.h etapa3 *.output
