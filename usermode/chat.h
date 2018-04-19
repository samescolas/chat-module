#ifndef CHAT_H
# define CHAT_H

#define DEVICE_NAME_LENGTH 18

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

typedef struct inbox {
	char	owner;
	char	name[DEVICE_NAME_LENGTH+1];
	char	key[32];
	char	iv[16];
	int		fd;
} inbox;

#endif
