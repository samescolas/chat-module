#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#define BUF_LEN 1024
#define MAX_NAME_LENGTH 18
#define DEVA "/dev/hey"
#define DEVB "/dev/hi"
#define DEVICE_NAME_LENGTH 8

int		assign_name(int argc, char **argv, char *name)
{
	if (argc == 2)
	{
		strncpy(name, &argv[1][0], MAX_NAME_LENGTH);
		return (0);
	}
	return (-1);
	
}

int		main(int argc, char **argv)
{
	char	name[MAX_NAME_LENGTH+1];
	char	buf[BUF_LEN+1];

	bzero(name, MAX_NAME_LENGTH+1);
	bzero(buf, BUF_LEN);
	if (assign_name(argc, argv, name) < 0)
	{
		printf("usage:\t./chat a|b\n");
		return (0);
	}

	int		fds[2];
	char	read_device[DEVICE_NAME_LENGTH];
	char	write_device[DEVICE_NAME_LENGTH];

	bzero(read_device, DEVICE_NAME_LENGTH);
	bzero(write_device, DEVICE_NAME_LENGTH);
	/* Users will open opposite files for reading and writing. */
	strncpy(read_device, strcmp(name, "a") ? DEVA : DEVB, DEVICE_NAME_LENGTH);
	strncpy(write_device, strcmp(name, "a") ? DEVB : DEVA, DEVICE_NAME_LENGTH);
	read_device[DEVICE_NAME_LENGTH] = '\0';
	write_device[DEVICE_NAME_LENGTH] = '\0';
	if ((fds[0] = open(read_device, O_RDONLY)) < 0)
	{
		printf("Unable to open %s for reading.\n", read_device);
		return (0);
	}
	else
	{
		printf("Opened %s for reading.\n", read_device);
	}
	if ((fds[1] = open(write_device, O_WRONLY)) < 0)
	{
		printf("Unable to open %s for writing.\n", write_device);
		close(fds[0]);
		return (0);
	}
	else
	{
		printf("Opened %s for writing.\n", write_device);
	}

	pid_t	child;

	child = fork();
	if (child >= 0)
	{
		if (child == 0)
		{
			int bytes_read;
			// child will handle reading
			while (1)
			{
				for(int tl=0; tl<1000000; tl++);
				if ((bytes_read = read(fds[0], &buf, BUF_LEN)) > 0)
				{
					write(1, "\r", 1);
					buf[bytes_read] = '\0';
					printf("[%s]\t%s", strcmp(name, "a") ? "a" : "b", buf);
					write(1, "[", 1);
					write(1, name, 1);
					write(1, "]\t", 2);
				}
			}
		}
		else
		{
			// parent will handle gathering input
			char	read_buf[BUF_LEN+1];
			int		bytes;

			while (1)
			{
				bzero(read_buf, BUF_LEN);
				write(1, "[", 1);
				write(1, name, 1);
				write(1, "]\t", 2);
				if ( (bytes = read(0, &read_buf, BUF_LEN)) > 0)
				{
					write(fds[1], read_buf, bytes);
				}
			}
		}
	}
	close(fds[0]);
	close(fds[1]);
	//read(fd, &buf, BUF_LEN);
	//buf[BUF_LEN] = '\0';
	//printf("Message received from %s: %s\n", DEV1, buf);
	//write(fd, "Hey there.\n", 11);
	return (0);
}
