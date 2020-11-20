OBJ := obj/
INC := include/
SRC := src/
DOC := doc/
TST := tests/

# Rebuild
all:
	make clean & make comp

# Make entire compiler
comp: dirs classes parser scanner
	g++ -c ${SRC}main.c -o ${OBJ}main.o -I ${INC} -Wall
	g++ -c ${INC}parser.tab.c -I ${INC} -o ${OBJ}parser.tab.o
	g++ -c ${SRC}lex.yy.c -I ${INC} -o ${OBJ}lex.yy.o
	g++ -o etapa6 ${OBJ}main.o\
				  ${OBJ}Manager.o\
				  ${OBJ}Type.o\
				  ${OBJ}Token.o\
				  ${OBJ}Node.o\
				  ${OBJ}Symbol.o\
				  ${OBJ}SymbolTable.o\
				  ${OBJ}Tac.o\
				  ${OBJ}Exceptions.o\
				  ${OBJ}Vertex.o\
				  ${OBJ}Edge.o\
				  ${OBJ}Color.o\
				  ${OBJ}Graph.o\
				  ${OBJ}ASM.o\
				  ${OBJ}lex.yy.o\
				  ${OBJ}parser.tab.o\
				  -lfl -lm -Wall

# Data classes implemented
classes:
	g++ -c ${SRC}Manager.cpp -I ${INC} -o ${OBJ}Manager.o -Wall
	g++ -c ${SRC}Type.cpp -I ${INC} -o ${OBJ}Type.o -Wall
	g++ -c ${SRC}Token.cpp -I ${INC} -o ${OBJ}Token.o -Wall
	g++ -c ${SRC}Node.cpp -I ${INC} -o ${OBJ}Node.o -Wall
	g++ -c ${SRC}Symbol.cpp -I ${INC} -o ${OBJ}Symbol.o -Wall
	g++ -c ${SRC}SymbolTable.cpp -I ${INC} -o ${OBJ}SymbolTable.o -Wall
	g++ -c ${SRC}Tac.cpp -I ${INC} -o ${OBJ}Tac.o -Wall
	g++ -c ${SRC}Exceptions.cpp -I ${INC} -o ${OBJ}Exceptions.o -Wall
	g++ -c ${SRC}Vertex.cpp -I ${INC} -o ${OBJ}Vertex.o -Wall
	g++ -c ${SRC}Edge.cpp -I ${INC} -o ${OBJ}Edge.o -Wall
	g++ -c ${SRC}Color.cpp -I ${INC} -o ${OBJ}Color.o -Wall
	g++ -c ${SRC}Graph.cpp -I ${INC} -o ${OBJ}Graph.o -Wall
	g++ -c ${SRC}ASM.cpp -I ${INC} -o ${OBJ}ASM.o -Wall

# Bison parser
parser:
	bison -d -v --output=${INC}parser.tab.c ${SRC}parser.y --report-file ${DOC}parser.output

# Flex scanner
scanner:
	flex -o ${SRC}lex.yy.c ${SRC}scanner.l

# Directories
dirs:
	mkdir -p ${OBJ}
	mkdir -p ${DOC}
	mkdir -p ${TST}

# Tests
test:
	@cat -n ${TST}$(in)
	@#valgrind --leak-check=full --track-origins=yes ./etapa6 < ${TST}$(in) > output.s
	@./etapa6 < ${TST}$(in) > output.s
	@gcc output.s

# Clean
clean:
	rm ${SRC}lex.yy.c ${INC}parser.tab.* ${OBJ}*.o etapa6 ${DOC}*.output *.s a.out
