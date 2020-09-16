all: parser.tab.c lex.yy.c
	gcc -c main.c lex.yy.c parser.tab.c
	gcc -o etapa2 main.o lex.yy.o parser.tab.o -lfl

parser.tab.c:
	bison -d -v parser.y --report-file parser.output

lex.yy.c:
	flex scanner.l

clean:
	rm lex.yy.c parser.tab.c *.o *.h etapa2 *.output
