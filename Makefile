default: clean
	clear
	flex lexer.l
	bison -dv parser.y
	gcc -o parser parser.tab.c lex.yy.c semantic_analizer.c assembly_generator.c -lfl

clean:
	rm -f parser parser.tab.c parser.tab.h parser.output lex.yy.c

