#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "all.h"

pid_t runCmd(char *const *argv, const char *pidfile);

pid_t startService(char *const *argv, const char *pidfile) {
	pid_t pid = runCmd(argv, pidfile);
	if (pid == -1) return -1;

	if (pidfile) {
		char pidbuf[11];
		sprintf(pidbuf, "%u", pid);
		size_t len = strlen(pidbuf);

		if (writeFile(pidfile, pidbuf, len)) {
			return -1;
		}
	}

	return pid;
}

pid_t runCmd(char *const *argv, const char *pidfile) {
	pid_t p = fork();
	if (!p) {
		setsid();
		execvp(argv[0], argv);
		perror("vinit-daemonutil: failed to execute command");
		sleep(2);
		if (pidfile) if (!access(pidfile, W_OK)) unlink(pidfile);
		exit(-1);
	} else if (p < 0) {
		perror("vinit-daemonutil: failed to fork process");
		return -1;
	}

	return p;
}