#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int writeFile(const char *path, const char *data, size_t len) {
	int fd = open(path, O_WRONLY | O_CREAT, S_IWRITE | S_IREAD | S_IRGRP | S_IROTH);
	if (fd == -1) {
		perror("vinit-daemonutil: failed to open file for writing");
		return -1;
	}

	if (write(fd, data, len) != len) {
		perror("vinit-daemonutil: failed to write to file");
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

char *readFile(const char *path) {
	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		perror("vinit-daemonutil: failed to open file for reading");
		return NULL;
	}

	struct stat st;
	if (fstat(fd, &st)) {
		perror("vinit-daemonutil: fstat() failed");
		close(fd);
		return NULL;
	}

	char *buf = malloc(sizeof(char) * st.st_size);
	if (!buf) {
		close(fd);
		perror("vinit-daemonutil: failed to allocate space in memory for reading file");
		return NULL;
	}

	if (read(fd, buf, st.st_size) != st.st_size) {
		close(fd);
		perror("vinit-daemonutil: failed to read all bytes from file");
		return buf;
	}

	close(fd);
	return buf;
}