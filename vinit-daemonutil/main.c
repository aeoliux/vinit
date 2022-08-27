#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include "all.h"

#define MIN_LENGHT 9

#define usage(x) printf("Usage: %s [ -P pidfile ] [ -n name ] [ -w ] [ -N ] [ -h ] [ -S command || -X || -C ]\n", x)
#define checkop() { \
	if (op != bufop) { \
		usage(argv[0]); \
		return 1; \
	} \
}

enum operation {
	bufop,
	start,
	stop,
	check
};

int main(int argc, char **argv) {
	int opt, enpidfile = 1, waitfor = 0;
	enum operation op = bufop;
	char *pidfile = NULL, *name = NULL;
	while ((opt = getopt(argc, argv, ":P:n:wNhSXC")) != -1) {
		switch (opt) {
			case 'S':
				checkop();
				op = start;
				break;
			case 'X':
				checkop();
				op = stop;
				break;
			case 'C':
				checkop();
				op = check;
				break;
			case 'P':
				pidfile = strdup(optarg);
				break;
			case 'n':
				name = strdup(optarg);
				break;
			case 'w':
				waitfor = 1;
				break;
			case 'N':
				enpidfile = 0;
				break;
			case 'h':
				usage(argv[0]);
				puts("\t-S\t\t---> Start process");
				puts("\t-X\t\t---> Stop process");
				puts("\t-C\t\t---> Check if process is running\n");
				puts("\t-P pidfile\t---> Select pidfile path");
				puts("\t-n name\t---> Define name for new process");
				puts("\t-w\t\t---> Wait until process finishes");
				puts("\t-N\t\t---> Do not create pidfile");
				puts("\t-h\t\t---> Display this message");
				free(name);
				free(pidfile);
				return 0;
			case '?':
				usage(argv[0]);
				free(pidfile);
				free(name);
				return 1;
			case ':':
				usage(argv[0]);
				return 1;
		}
	}
	if (argc < 2) {
		usage(argv[0]);
		free(pidfile);
		free(name);
		return 1;
	}

	if (!name) {
		if (!pidfile && enpidfile) name = strdup(argv[optind]);
	}
	if (!pidfile) {
		if (enpidfile) {
			pidfile = malloc(sizeof(char) * (1 + MIN_LENGHT + strlen(name)));
			if (!pidfile) {
				perror("vinit-daemonutil: failed to allocate space in memory for pidfile path");
				free(name);
				return 1;
			}
			sprintf(pidfile, "/run/%s.pid", name);
		}
	}

	int ret = 0;
	switch (op) {
		case bufop:
			usage(argv[0]);
			ret = 1;
			break;
		case start: {
			char **args = &argv[optind];
			pid_t pid = startService(args, pidfile);
			if (pid == -1) {
				ret = 1;
			}

			if (waitfor) {
				int status;
				do {
					if (waitpid(pid, &status, WUNTRACED) == -1) {
						perror("vinit-daemon: failed to waitpid()");
						ret = 1;
						break;
					}
				} while (!WIFEXITED(status) && !WIFSIGNALED(status));

				unlink(pidfile);
				ret = WEXITSTATUS(status);
			}

			break;
		}
        case stop:
			if (stopService(pidfile)) {
				ret = 1;
			} else if (enpidfile) unlink(pidfile);
			break;
        case check:
			if (checkService(pidfile)) {
				puts("vinit-daemonutil: process is not running. Deleting pidfile");
				ret = 1;
				if (enpidfile) unlink(pidfile);
			}
            break;
    }

    free(name);
	free(pidfile);

	return ret;
}