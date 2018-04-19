CC := gcc
NAME := chat
LINKS := -lcrypto -lssl

all:
	$(CC) -o $(NAME) $(NAME).c crypto.c $(LINKS)

clean:
	rm -f $(NAME)
