# Bortoli Gianluca, n° 159993

LEX = lex
YACC = yacc -d
CC = clang -std=c99

interpreter: y.tab.o lex.yy.o myCalc.o calcInterpreter.o
	$(CC) -o interpreter y.tab.o lex.yy.o myCalc.o calcInterpreter.o -ll -lm

calcInterpreter.o: calcInterpreter.c

myCalc.o: myCalc.c

lex.yy.o: lex.yy.c y.tab.h

y.tab.c y.tab.h: yacc.y
	$(YACC) -v yacc.y

lex.yy.c: lex.l
	$(LEX) lex.l

clean:
	rm -f *.o lex.yy.c *.tab.* interpreter *.output