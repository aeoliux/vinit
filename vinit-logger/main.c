#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

int main(int argc, char **argv) {
	int opt;
	char *logfile = strdup("/var/log/daemons.log");
	while ((opt = getopt(argc, argv, ":L:")) != -1) {
		switch (opt) {
			case 'L':
				free(logfile);
				logfile = strdup(optarg);
				break;
			case ':':
				fputs("vinit-logger: incorrect usage\n", stderr);
				return 1;
			case '?':
				fputs("vinit-logger: incorrect usage\n", stderr);
				return 1;
		}
	}

	if (!argv[optind]) {
		fputs("vinit-logger: incorrect usage\n", stderr);
		return 1;
	}

	char **args = argv + optind;

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	int fd = open(logfile, O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
	free(logfile);
	if (fd == -1) {
		return 1;
	}
	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	time_t crtime = time(NULL);
	if (crtime == (time_t) -1) {
		perror("vinit-logger: failed to obtain current time");
	} else {
		char *date = ctime(&crtime);
		if (!date) {
			perror("vinit-logger: failed to obtain formated date");
		} else if (write(fd, date, strlen(date)) == -1) {
			perror("vinit-logger: failed to write date");
		}
	}

	execvp(args[0], args);

	perror("vinit-logger: failed to execute command");
	return 1;
}