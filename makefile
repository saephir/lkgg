INCLUDEDIR = /usr/include/ncursesw

gg: gg.o ggutils.o loop.o lock.o charconversion.o contacts.o colors.o err.o
	gcc -o gg gg.o ggutils.o loop.o lock.o charconversion.o contacts.o colors.o err.o -lncursesw -lpthread 
gg.o: gg.c
	gcc -I $(INCLUDEDIR) -c -o gg.o gg.c
ggutils.o: ggutils.c
	gcc -I $(INCLUDEDIR) -c -o ggutils.o ggutils.c
loop.o: loop.c
	gcc -I $(INCLUDEDIR) -c -o loop.o loop.c
lock.o: lock.c
	gcc -I $(INCLUDEDIR) -c -o lock.o lock.c
charconversion.o: charconversion.c
	gcc -I $(INCLUDEDIR) -c -o charconversion.o charconversion.c
contacts.o: contacts.c
	gcc -I $(INCLUDEDIR) -c -o contacts.o contacts.c
colors.o: colors.c
	gcc -I $(INCLUDEDIR) -c -o colors.o colors.c
err.o: err.c
	gcc -I $(INCLUDEDIR) -c -o err.o err.c

cleanup:
	rm -f *.o
