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

	bzero(name, MAX_NAME_LENGTH);
	if (assign_name(argc, argv, name) < 0)
	{
		printf("usage:\t./chat a|b\n");
		return (0);
	}

	int		fds[2];
	char	read_device[12];
	char	write_device[12];

	strncpy(read_device, strcmp(name, "a") ? DEVA : DEVB, 12);
	strncpy(write_device, strcmp(name, "a") ? DEVB : DEVA, 12);
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
		return (getchar());
		close(fds[0]);
		close(fds[1]);
	}
	//read(fd, &buf, BUF_LEN);
	//buf[BUF_LEN] = '\0';
	//printf("Message received from %s: %s\n", DEV1, buf);
	//write(fd, "Hey there.\n", 11);
	return (0);
}
