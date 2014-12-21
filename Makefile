# Bortoli Gianluca, n° 159993

LEX = lex
YACC = yacc -d
CC = gcc -std=c99

interpreter: y.tab.o lex.yy.o myClac.o calcInterpreter.o
	$(CC) -o interpreter y.tab.o lex.yy.o myClac.o calcInterpreter.o -ll -lm

myClac.o: myClac.c

calcInterpreter.o: calcInterpreter.c

lex.yy.o: lex.yy.c y.tab.h

y.tab.c y.tab.h: yacc.y
	$(YACC) -v yacc.y

lex.yy.c: lex.l
	$(LEX) lex.l

clean:
	-rm -f *.o lex.yy.c *.tab.* interpreter *.output