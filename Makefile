build: poker_tournament.o
	gcc poker_tournament.o -o poker_tournament

poker_tournament.o:	tema1.c info.h
	gcc -c tema1.c -o poker_tournament.o

clean:
	rm -rf poker_tournament poker_tournament.o