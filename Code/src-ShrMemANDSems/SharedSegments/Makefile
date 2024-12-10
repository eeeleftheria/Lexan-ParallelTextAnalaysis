all: shareMem1 \
	shareMem2 \
	systemV-semaphores \
	sem-server \
	sem-client

shareMem1: shareMem1.c
	gcc -g shareMem1.c -o shareMem1

shareMem2: shareMem2.c
	gcc -g shareMem2.c -o shareMem2

systemV-semaphores: systemV-semaphores.c
	gcc -g systemV-semaphores.c -o systemV-semaphores

sem-server: sem-server.c
	gcc -g sem-server.c -o sem-server

sem-client: sem-client.c
	gcc -g sem-client.c -o sem-client


clean:
	rm -f shareMem1 shareMem2 posix-sample \
		sem-client sem-server

