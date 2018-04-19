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
#include "./chat.h"

#define DEVA "/dev/sec4120"
#define DEVB "/dev/sec4121"

void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int ciphertext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();

  /* Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
  ciphertext_len = len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
  ciphertext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
  unsigned char *iv, unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int plaintext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();

  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    handleErrors();
  plaintext_len = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}

int		assign_names(int argc, char **argv, inbox inboxes[2])
{
	if (argc == 2)
	{
		inboxes[0].owner = argv[1][0];
		inboxes[1].owner = (argv[1][0] == 'a' ? 'b' : 'a');
		if (inboxes[0].owner != 'a' && inboxes[0].owner != 'b')
		{
			return (-1);
		}
		return (0);
	}
	return (-1);
	
}

void	write_user_prompt(char username)
{
	write(1, "[", 1);
	write(1, &username, 1);
	write(1, "] ", 2);
}

void	read_inbox(inbox inbox)
{

	int 			bytes_read;
	char			buf[MAX_ALLOWED_LEN+1];
	unsigned char	decryptedtext[MAX_ALLOWED_LEN];
	int				decryptedtext_len = 0;
	char			ciphertext_len_str[10];
	int				ciphertext_len = 0;

	memset(buf, 0, MAX_ALLOWED_LEN + 1);
	memset(decryptedtext, 0, MAX_ALLOWED_LEN);

	while (1)
	{
		// Do we want to rate limit these read requests or is this okay?
		if ((bytes_read = read(inbox.fd, &buf, MAX_ALLOWED_LEN)) > 0)
		{
			// clear current line
			if (ioctl(inbox.fd, IOCTL_READ_MSGLEN_FROM_KERNEL, ciphertext_len_str) < 0)
			{
				printf("Unable to determine message length.\n");
				continue ;
			}
			else
			{
				ciphertext_len = atoi(ciphertext_len_str);
			}
			write(1, "\r", 1);
			write_user_prompt(inbox.owner == 'a' ? 'b' : 'a');
			decryptedtext_len = decrypt(buf, ciphertext_len, inbox.key, inbox.iv, decryptedtext);
			decryptedtext[decryptedtext_len] = '\0';
			write(1, decryptedtext, decryptedtext_len);
			//write(1, buf, strlen(buf));
			write_user_prompt(inbox.owner);
			memset(buf, 0, bytes_read);
		}
		sleep(1);
	}
}

int		main(int argc, char **argv)
{
	char	me;
	char	you;
	inbox	inboxes[2];

	if (assign_names(argc, argv, inboxes) < 0)
	{
		printf("usage:\t./chat a|b\n");
		return (0);
	}
	else
	{
	}

	unsigned char	ciphertext[MAX_ALLOWED_LEN];
	int				ciphertext_len = 0;
	char			ciphertext_len_str[10];

	memset(inboxes[0].key, 0, 32);
	memset(inboxes[1].key, 0, 32);
	memset(inboxes[0].iv, 0, 16);
	memset(inboxes[1].iv, 0, 16);
	memset(ciphertext, 0, MAX_ALLOWED_LEN);
	if (!RAND_bytes(inboxes[0].key, 32))
	{
	printf("Error assigning key.\n");
	}
	else
	{
	}
	if (!RAND_bytes(inboxes[0].iv, 16))
	{
	printf("Error assigning iv.\n");
	}
	else
	{
	}
	memset(inboxes[0].name, 0, DEVICE_NAME_LENGTH);
	memset(inboxes[1].name, 0, DEVICE_NAME_LENGTH);

	/* Users will open opposite files for reading and writing. */
	strncpy(inboxes[0].name, inboxes[0].owner  == 'a' ? DEVA : DEVB, DEVICE_NAME_LENGTH);
	strncpy(inboxes[1].name, inboxes[0].owner == 'a' ? DEVB : DEVA, DEVICE_NAME_LENGTH);
	inboxes[0].name[DEVICE_NAME_LENGTH] = '\0';
	inboxes[1].name[DEVICE_NAME_LENGTH] = '\0';

	if ((inboxes[0].fd = open(inboxes[0].name, O_RDONLY)) < 0)
	{
		printf("Unable to open %s for reading.\n", inboxes[0].name);
		return (0);
	}
	else
	{
		ioctl(inboxes[0].fd, IOCTL_WRITE_KEY_TO_KERNEL, inboxes[0].key);
		ioctl(inboxes[0].fd, IOCTL_WRITE_IV_TO_KERNEL, inboxes[0].iv);
	}
	if ((inboxes[1].fd = open(inboxes[1].name, O_WRONLY)) <= 0)
	{
		printf("Unable to open %s for writing.\n", inboxes[1].name);
		close(inboxes[0].fd);
		return (0);
	}
	else
	{
	}

	pid_t	child = fork();
	int		tries = 0;
	char	tries_str[10];

	memset(tries_str, 0, 10);
	while (tries < 10 && ioctl(inboxes[1].fd, IOCTL_READ_KEY_FROM_KERNEL, (char *)inboxes[1].key) < 0)
	{
		sprintf(tries_str, "%d", 60 - tries);
		write(1, "\r", 1);
		write(0, "Waiting for key. ", 17);
		write(0, tries_str, strlen(tries_str));
		write(0, " seconds...", 11);
		tries++;
		sleep(1);
	}
	if (!inboxes[1].key[0])
		return (-1);
	while (tries < 10 && ioctl(inboxes[1].fd, IOCTL_READ_IV_FROM_KERNEL, (char *)inboxes[1].iv) < 0)
	{
		sprintf(tries_str, "%d", 60 - tries);
		write(1, "\r", 1);
		write(0, "Waiting for iv. ", 17);
		write(0, tries_str, strlen(tries_str));
		write(0, " seconds...", 11);
		tries++;
		sleep(1);
	}
	if (!inboxes[1].iv[0])
		return (-1);
	if (child >= 0)
	{
		if (child == 0)
		{
			read_inbox(inboxes[0]);
		}
		else
		{
			char	read_buf[MAX_ALLOWED_LEN+1];
			int		bytes;

			bzero(read_buf, MAX_ALLOWED_LEN + 1);
			while (1)
			{
				write_user_prompt(inboxes[0].owner);
				if ( (bytes = read(0, &read_buf, MAX_ALLOWED_LEN)) > 1)
				{
					ciphertext_len = encrypt(read_buf, bytes, inboxes[1].key, inboxes[1].iv, ciphertext);
					sprintf(ciphertext_len_str, "%d", ciphertext_len);
					memset(read_buf, 0, bytes);
					if (ciphertext_len < MAX_ALLOWED_LEN)
					{
						ioctl(inboxes[1].fd, IOCTL_WRITE_MSGLEN_TO_KERNEL, ciphertext_len_str);
						write(inboxes[1].fd, ciphertext, ciphertext_len);
					}
				}
			}
		}
	}
	close(inboxes[0].fd);
	close(inboxes[1].fd);
	return (0);
}
