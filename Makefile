gtk-memory: gtk-memory.o cards.o fifo.o logic.o interface.o fileMg.o
	gcc -std=c11 -Wall -Wextra gtk-memory.o cards.o fifo.o logic.o interface.o fileMg.o -o gtk-memory `pkg-config gtk+-3.0 --libs --cflags` -lm

cards.o: cards.c cards.h
	gcc -c -std=c11 -Wall -Wextra cards.c -o cards.o `pkg-config gtk+-3.0 --libs --cflags`-lm

fifo.o: win-fifo.c fifo.h
	gcc -c -std=c11 -Wall -Wextra win-fifo.c -o fifo.o -lm

logic.o: logic.c logic.h cards.h interface.h
	gcc -c -std=c11 -Wall -Wextra logic.c -o logic.o `pkg-config gtk+-3.0 --libs --cflags` -lm

fileMg.o: fileMg.c fileMg.h logic.h cards.h interface.h
	gcc -c -std=c11 -Wall -Wextra fileMg.c -o fileMg.o `pkg-config gtk+-3.0 --libs --cflags` -lm

interface.o: interface.c interface.h logic.h cards.h fileMg.h
	gcc -c -std=c11 -Wall -Wextra  interface.c -o interface.o `pkg-config gtk+-3.0 --libs --cflags` -lm

gtk-memory.o: gtk-memory.c fifo.h cards.h logic.h interface.h fileMg.h
	gcc -c -std=c11 -Wall -Wextra gtk-memory.c -o gtk-memory.o `pkg-config gtk+-3.0 --libs --cflags` -lm

Clean: 
	rm *.o gtk-memory *.dat
