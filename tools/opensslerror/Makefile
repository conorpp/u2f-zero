src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS = -lcrypto

error: $(obj)
	$(CC) -O3 -Wall -Werror -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(obj) error
