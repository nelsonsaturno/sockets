all: sem_cli sem_svr

sem_cli: sem_cli.o
	gcc sem_cli.o -o sem_cli

sem_svr: sem_svr.o
	gcc sem_svr.o -o sem_svr

clean:
	rm -f *.o *.gch main || true
	rm sem_svr sem_cli || true
