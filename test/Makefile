CC     = gcc
CFLAGS = -Wall -Wextra -Werror -g

all:
	$(CC) $(CFLAGS) test_reader.c -o reader
	$(CC) $(CFLAGS) test_writer.c -o writer

clean:
	rm -rf *.o

fclean: clean
	rm -f reader writer

re: clean all