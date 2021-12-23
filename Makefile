# C compiler
HOST_CC = gcc  -O2

HOST_CC_FLAGS =  -lncurses

compile_all: sserver_host sclient

compile_sserver: sserver

compile_sclient: sclient

run_sserver: sserver
	./sserver

run_sclient: sclient
	 ./sclient



sserver: sserver.o
	$(HOST_CC) sserver.o  -o sserver $(HOST_CC_FLAGS)

sclient: sclient.o
	$(HOST_CC) sclient.o  -o sclient $(HOST_CC_FLAGS)



sserver.o: server_super_pong.c super_pong.h
	$(HOST_CC) -c server_super_pong.c -o sserver.o  $(HOST_CC_FLAGS)

sclient.o: client_super_pong.c super_pong.h
	$(HOST_CC) -c client_super_pong.c -o sclient.o  $(HOST_CC_FLAGS)

clean:
	rm -f *.o *~ sserver sclient
