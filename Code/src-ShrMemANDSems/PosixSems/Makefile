all: 	semtest  \
	semtest3 \
	semtest3a \
	namedSem 

semtest: semtest.c
	gcc semtest.c -o semtest -lpthread

semtest3: semtest3.c
	gcc  semtest3.c -o semtest3 -lpthread

semtest3a: semtest3a.c
	gcc  semtest3a.c -o semtest3a -lpthread

namedSem: namedSem.c
	gcc -g namedSem.c -o  namedSem -lpthread

clean:
	rm -f semtest namedSem  \
		semtest3 semtest3a

