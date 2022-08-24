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
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "../init/all.h"

#define usage(x) { \
	printf("Usage: %s -s signal [ -o option1 ] [ -o option2 ] ...\n", x); \
}

void array_free(void **arr, size_t size);

int main(int argc, char **argv) {
	int opt, raw = 0;

	char *signal = NULL, **opts = malloc(sizeof(char *) * 1);
	if (!opts) {
		perror("initreq: allocation error");
		return 1;
	}
	opts[0] = NULL;
	size_t opts_size = 1, index = 0;
	while ((opt = getopt(argc, argv, ":s:o:hVv")) != -1) {
		switch (opt) {
			case 's':
				signal = strdup(optarg);
				break;
			case 'o':
				if (index >= opts_size) {
					opts_size++;
					opts = realloc(opts, opts_size * sizeof(char *));
					if (!opts) {
						free(signal);
						perror("initreq: allocation error");
						return -1;
					}
				}

				opts[index] = strdup(optarg);
				index++;
				break;
			case 'v':
				raw = 1;
				break;
			case 'h':
				usage(argv[0]);
				printf("\t-s signalcontent \t---> specifies signal content\n");
				printf("\t-o option\t\t---> specifies signal option\n");
				printf("\t-V\t\t\t---> show program version\n");
				printf("\t-h\t\t\t---> Shows this message\n");
				return 0;
			case 'V':
				printf("VInit init requesting tool v%s\n", VERSION);
				return 0;
			case ':':
				usage(argv[0]);
				return 1;
			case '?':
				usage(argv[0]);
				return 1;
		}
	}

	if (!signal) {
		usage(argv[0]);
		return 1;
	}

	int fd = open("/run/initctl", O_WRONLY);
	if (fd == -1) {
		perror("initreq: unable to open initctl");
		free(signal);
		array_free((void **) opts, opts_size);
		return 1;
	}

	size_t n = strlen(signal) + 1;
	if (opts[0]) {
		for (size_t i = 0; i < opts_size; i++) {
			n += strlen(opts[i]) + 1;
		}
	}

	char *content = malloc(n * sizeof(char));
	if (!content) {
		perror("initreq: allocation error");
		free(signal);
		array_free((void **) opts, opts_size);
		return 1;
	}

	strcpy(content, signal);
	if (opts[0]) {
		strcat(content, "\n");
		for (size_t i = 0; i < opts_size - 1; i++) {
			strcat(content, opts[i]);
			strcat(content, "\n");
		}

		strcat(content, opts[opts_size - 1]);
	}
	
	if (raw) {
		printf("Raw message:\n%s\nEND\n", content);
	}

	if (write(fd, content, n) != n) {
		perror("initreq: failed to write to initctl");
		array_free((void **) opts, opts_size);
		free(signal);
		free(content);
		return 1;
	}

	array_free((void **) opts, opts_size);
	free(signal);
	free(content);
	return 0;
}

void array_free(void **arr, size_t size) {
	if (arr[0]) for (size_t i = 0; i < size; i++) free(arr[i]);

	free(arr);
}