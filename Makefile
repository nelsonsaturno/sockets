all: MoriahServer MoriahClient

MoriahServer: MoriahServer.c
	gcc -o sem_svr MoriahServer.c

MoriahClient: MoriahClient.c
	gcc -o sem_cli MoriahClient.c
