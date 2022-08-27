#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "all.h"

int stopService(const char *pidfile) {
	char *buf = readFile(pidfile);
	if (!buf) return -1;

	pid_t pid = -1;
	sscanf(buf, "%u", &pid);
	free(buf);
	if (pid == -1) {
		fputs("vinit-daemonutil: pidfile has invalid content\n", stderr);
		return -1;
	}

	if (kill(pid, SIGKILL) == -1) {
		perror("vinit-daemonutil: failed to kill process");
		return -1;
	}

	return 0;
}