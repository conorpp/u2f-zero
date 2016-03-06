
LDFLAGS = -lssl -lcrypto

all: signcert hex2pubkey

signcert: signcert.o
	$(CC) -O3 -Wall -Werror -o $@ $^ $(LDFLAGS)

hex2pubkey: hex2pubkey.o
	$(CC) -O3 -Wall -Werror -o $@ $^ $(LDFLAGS)

signcert.o: signcert.c
	$(CC) -c $(CFLAGS) -o $@ $^

hex2pubkey.o: hex2pubkey.c
	$(CC) -c $(CFLAGS) -o $@ $^

clean:
	rm -f $(obj) signcert hex2pubkey
