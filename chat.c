#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#define BUF_LEN 1024
#define DEVA "/dev/hey"
#define DEVB "/dev/hi"
#define DEVICE_NAME_LENGTH 12

int		assign_names(int argc, char **argv, char *me, char *you)
{
	if (argc == 2)
	{
		*me = argv[1][0];
		*you = (*me == 'a' ? 'b' : 'a');
		if (*me != 'a' && me[0] != 'b')
			return (-1);
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

void	read_inbox(int fd, char me, char you)
{

	int 	bytes_read;
	char	buf[BUF_LEN+1];

	bzero(buf, BUF_LEN + 1);
	while (1)
	{
		// Do we want to rate limit these read requests or is this okay?
		// for (int tl=0; tl<1000000; tl++);
		if ((bytes_read = read(fd, &buf, BUF_LEN)) > 0)
		{
			// clear current line
			write(1, "\r", 1);
			write_user_prompt(you);
			write(1, buf, strlen(buf));
			write_user_prompt(me);
			bzero(buf, bytes_read);
		}
	}
}

int		main(int argc, char **argv)
{
	char	me;
	char	you;

	if (assign_names(argc, argv, &me, &you) < 0)
	{
		printf("usage:\t./chat a|b\n");
		return (0);
	}

	int		fds[2];
	char	r_device[DEVICE_NAME_LENGTH];
	char	wr_device[DEVICE_NAME_LENGTH];

	bzero(r_device, DEVICE_NAME_LENGTH);
	bzero(wr_device, DEVICE_NAME_LENGTH);
	/* Users will open opposite files for reading and writing. */
	strncpy(r_device, (me == 'a' ? DEVA : DEVB), DEVICE_NAME_LENGTH);
	strncpy(wr_device, (me == 'a' ? DEVB : DEVA), DEVICE_NAME_LENGTH);
  printf("READ: %s\n", r_device);
  printf("WRITE: %s\n", wr_device);
	if ((fds[0] = open(r_device, O_RDONLY)) < 0)
	{
		printf("Unable to open %s for reading.\n", r_device);
		return (0);
	}
	else
	{
		printf("Opened %s for reading.\n", r_device);
	}
	if ((fds[1] = open(wr_device, O_WRONLY)) < 0)
	{
		printf("Unable to open %s for writing.\n", wr_device);
		close(fds[0]);
		return (0);
	}
	else
	{
		printf("Opened %s for writing.\n", wr_device);
	}

	pid_t	child = fork();

	if (child >= 0)
	{
		if (child == 0)
		{
			read_inbox(fds[0], me, you);
		}
		else
		{
			char	read_buf[BUF_LEN+1];
			int		bytes;

			bzero(read_buf, BUF_LEN + 1);
			while (1)
			{
				write_user_prompt(me);
				if ( (bytes = read(0, &read_buf, BUF_LEN)) > 0)
				{
					write(fds[1], read_buf, bytes);
					bzero(read_buf, bytes);
				}
			}
		}
	}
	close(fds[0]);
	close(fds[1]);
	return (0);
}
