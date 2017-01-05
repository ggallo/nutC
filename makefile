OPTS = -D_GNU_SOURCE -pthread -g -Wall

all: server client

server: server.c board.o message.o utils.o thread_func.o sq_state.o s_queue.o globals.h
	gcc ${OPTS} -o server.out server.c board.o message.o utils.o thread_func.o sq_state.o s_queue.o

client: client.c message.o utils.o globals.h
	gcc ${OPTS} -o client.out client.c message.o utils.o

board.o: board.c board.h globals.h message.h
	gcc ${OPTS} -c board.c

message.o: message.c message.h globals.h
	gcc ${OPTS} -c message.c

utils.o: utils.c utils.h message.h board.h
	gcc ${OPTS} -c utils.c

s_queue.o: s_queue.c s_queue.h
	gcc ${OPTS} -c s_queue.c

sq_state.o: sq_state.c sq_state.h
	gcc ${OPTS} -c sq_state.c

thread_func.o: thread_func.c thread_func.h
	gcc ${OPTS} -c thread_func.c

clean:
	rm -f *.o
	rm -f server.out
	rm -f client.out
	rm -f *~

clean-debug:
	rm -r *.dSYM
