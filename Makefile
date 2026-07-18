CC=gcc

CFLAGS=$(shell pkg-config --cflags liboqs)

LIBS=$(shell pkg-config --libs liboqs) -lssl -lcrypto

all:
	$(CC) final_client.c crypto.c -o final_client $(CFLAGS) $(LIBS)
	$(CC) final_server.c crypto.c -o final_server $(CFLAGS) $(LIBS)

clean:
	rm -f final_client final_server
