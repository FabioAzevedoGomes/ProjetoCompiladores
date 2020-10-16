OBJ := obj/
INC := include/
SRC := src/
DOC := doc/

all: dirs stack.o error_report.o st_management.o symbol_table.o tree.o types.o parser.tab.c lex.yy.c
	gcc -c ${SRC}main.c -o ${OBJ}main.o	-Wall
	gcc -c ${INC}parser.tab.c -o ${OBJ}parser.tab.o
	gcc -c ${SRC}lex.yy.c -o ${OBJ}lex.yy.o
	gcc -o etapa4 ${OBJ}main.o ${OBJ}types.o ${OBJ}stack.o ${OBJ}error_report.o ${OBJ}symbol_table_management.o ${OBJ}symbol_table.o ${OBJ}tree.o ${OBJ}lex.yy.o ${OBJ}parser.tab.o -lfl -Wall

parser.tab.c:
	bison -d -v --output=${INC}parser.tab.c ${SRC}parser.y --report-file ${DOC}parser.output

lex.yy.c:
	flex -o ${SRC}lex.yy.c ${SRC}scanner.l

tree.o:
	gcc -c ${SRC}tree.c -o ${OBJ}tree.o -I ${INC} -Wall

stack.o:
	gcc -c ${SRC}stack.c -o ${OBJ}stack.o -I ${INC} -Wall

error_report.o:
	gcc -c ${SRC}error_report.c -o ${OBJ}error_report.o -I ${INC} -Wall

types.o:
	gcc -c ${SRC}types.c -o ${OBJ}types.o -I ${INC} -Wall

st_management.o:
	gcc -c ${SRC}symbol_table_management.c -o ${OBJ}symbol_table_management.o -I ${INC} -Wall

symbol_table.o:
	gcc -c ${SRC}symbol_table.c -o ${OBJ}symbol_table.o -I ${INC} -Wall

dirs:
	mkdir -p ${OBJ}
	mkdir -p ${DOC}
    
test:
	@cat -n ${SRC}test.c
	@valgrind --leak-check=full --track-origins=yes ./etapa4 < ${SRC}test.c
	@#./etapa4 < ${SRC}test.c

clean:
	rm ${SRC}lex.yy.c ${INC}parser.tab.* ${OBJ}*.o etapa4 ${DOC}*.output
