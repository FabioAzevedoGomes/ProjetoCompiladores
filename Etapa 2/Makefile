all: parser.tab.c lex.yy.c
	gcc -c lex.yy.c parser.tab.c
	gcc -o etapa2 lex.yy.o parser.tab.o -lfl

parser.tab.c:
	bison -d parser.y

lex.yy.c:
	flex scanner.l

clean:
	rm lex.yy.c parser.tab.c *.o *.h etapa2
