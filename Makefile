CC=gcc
CXX=g++
FLEX=flex
BISON=bison


.lex: lex.l
	$(FLEX) lex.l
.syntax: syntax.y
	$(BISON) -v -t -d syntax.y
splc: .lex .syntax
	mkdir -p bin && $(CXX) syntax.tab.c -lfl -ly -o bin/splc
clean:
	@rm -f lex.yy.c syntax.tab.*
	@rm -r bin
all: splc