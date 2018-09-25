testlib : main.o bitmap.o hash.o list.o
	gcc -W -o testlib main.o bitmap.o hash.o list.o
main.o : main.c
	gcc -W -c main.c
bitmap.o: bitmap.c bitmap.h
	gcc -W -c bitmap.c
hash.o: hash.c hash.h
	gcc -W -c hash.c
list.o: list.c list.h
	gcc -W -c list.c
clean:
	rm testlib main.o bitmap.o hash.o list.o
