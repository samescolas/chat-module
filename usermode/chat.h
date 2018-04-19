#ifndef CHAT_H
# define CHAT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <openssl/rand.h>

#include "../common/char_ioctl.h"

#define DEVICE_NAME_LENGTH 18

typedef struct inbox {
	char	owner;
	char	name[DEVICE_NAME_LENGTH+1];
	char	key[32];
	char	iv[16];
	int		fd;
} inbox;


int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext);

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext);

#endif
