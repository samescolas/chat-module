#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUF_LEN 1024
#define MAX_NAME_LENGTH 18
#define DEV1 "/dev/hey"
#define DEV2 "/dev/hi"

int		main(int argc, char **argv)
{
	static char	name[MAX_NAME_LENGTH+1];
	static char	buf[BUF_LEN+1];

	if (argc == 2)
	{
		strncpy(name, &argv[1][0], MAX_NAME_LENGTH);
	}
	else
	{
		name[0] = 'a';
		name[1] = '\0';
	}
	printf("Welcome, %s.\n", name);
	static int	fd;

	if ((fd = open(DEV1, O_RDWR)) < 0)
	{
		if ((fd = open(DEV2, O_RDWR)) < 0)
		{
			printf("Unable to open %s or %s.\n", DEV1, DEV2);
			return (1);
		}
		else
		{
			read(fd, &buf, BUF_LEN);
			buf[BUF_LEN] = '\0';
			printf("Message received from %s: %s\n", DEV2, buf);
			return (getchar());
		}
	}
	else
	{
		read(fd, &buf, BUF_LEN);
		buf[BUF_LEN] = '\0';
		printf("Message received from %s: %s\n", DEV1, buf);
		return (getchar());
	}
	return (0);
}
