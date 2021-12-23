# C compiler
HOST_CC = gcc  -O2

HOST_CC_FLAGS =  -lncurses

compile_all: sserver_host sclient

compile_sserver: sserver

compile_sclient: sclient

run_server: sserver
	./sserver

run_client: sclient
	 ./sclient



sserver: sserver.o
	$(HOST_CC) sserver.o  -o sserver $(HOST_CC_FLAGS)

sclient: client.o
	$(HOST_CC) sclient.o  -o sclient $(HOST_CC_FLAGS)



sserver.o: super_pong_server.c super_pong.h
	$(HOST_CC) -c super_pong_server.c -o sserver.o  $(HOST_CC_FLAGS)

client.o: human-control-client.c remote-char.h
	$(HOST_CC) -c super_pong_client.c -o sclient.o  $(HOST_CC_FLAGS)

clean:
	rm -f *.o *~ sserver sclient
