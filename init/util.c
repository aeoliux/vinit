/*
MIT License

Copyright (c) 2022 Zapomnij

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

char **splitString(char *buf, const char *splitter) {
	if (!buf) {
		errno = EINVAL;
		return NULL;
	}
	char **ret = malloc(1 * sizeof(char *));
	if (!ret) return NULL;

	size_t currentIndex = 0, size = 1;
	char *token = strtok(buf, splitter);
	while (token) {
		ret[currentIndex] = strdup(token);

		token = strtok(NULL, splitter);

		currentIndex++;
		if (currentIndex >= size) {
			size++;
			ret = realloc(ret, size * sizeof(char *));
			if (!ret) {
				return NULL;
			}
		}
	}
	ret[currentIndex] = NULL;

	return ret;
}

char *readFile(const char *path) {
	struct stat st;
	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		perror("init: failed to read file");
		return NULL;
	}

	if (fstat(fd, &st)) {
		perror("init: fstat() failed");
		return NULL;
	}

	char *buf = malloc(st.st_size);
	if (!buf) {
		perror("init: memory allocation failed");
		return NULL;
	}

	if (read(fd, buf, st.st_size) != st.st_size) {
		perror("init: failed to read all bytes from file");
		return NULL;
	}

	close(fd);
	return buf;
}

pid_t runAndFork(char *const cmd) {
	pid_t f = fork();
	if (!f) {
		char *shell = getenv("SHELL");
		if (!shell) shell = "/bin/sh";
		char *const argv[] = {shell, "-c", cmd, NULL};

		execvp(argv[0], argv);
		perror("init: failed to execute command");
		exit(255);
	} else if (f < 0) {
		perror("init: failed to fork()");
		return -1;
	}

	return f;
}

int runShellCmd(char *const cmd) {
	pid_t f = runAndFork(cmd);
	if (f == -1) return -1;

	int status;
	do {
		if (waitpid(f, &status, WUNTRACED | WNOHANG)) {
			perror("init: waitpid() failed");
			return -1;
		}
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));

	return WEXITSTATUS(status);
}

pid_t respawnCmd(char *const cmd) {
	pid_t f = fork();
	if (!f) {
		while (1) {
			int ret = runShellCmd(cmd);
			if ((ret == 255) || (ret == -1)) {
				exit(-1);
			}
		}
	} else if (f < 0) {
		perror("init: fork() failed");
		return -1;
	}

	return f;
}