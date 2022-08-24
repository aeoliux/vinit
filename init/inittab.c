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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "all.h"

struct inittab *parseInittab(const char *inittab) {
	struct inittab *parsed = malloc(1 * sizeof(struct inittab));
	if (!parsed) {
		perror("init: failed to allocate structure");
		return NULL;
	}
	parsed->sysinit.cmd = NULL;
	parsed->shutdown.cmd = NULL;
	parsed->post = NULL;
	parsed->postn = 0;

	char *dup = strdup(inittab);
	char **lines = splitString(dup, "\n");
	if (!lines) {
		free(dup);
		free(parsed);
		perror("init: failed to parse inittab");
		return NULL;
	}
	free(dup);

	size_t current_post = 0;
	for (size_t i = 0; lines[i] != NULL; i++) {
		if (!strcmp(lines[i], "")) continue;
		char **tokens = splitString(lines[i], ":");
		if (!tokens) {
			perror("init: failed to parse inittab");
			free(parsed);
			for (size_t i2 = 0; lines[i2] != NULL; i2++) free(lines[i2]);
			free(lines);
			return NULL;
		}
		if (!strcmp(tokens[0], "sysinit")) {
			parsed->sysinit.cmd = strdup(tokens[1]);
		} else if (!strcmp(tokens[0], "shutdown")) {
			parsed->shutdown.cmd = strdup(tokens[1]);
		} else if (!strcmp(tokens[0], "postn")) {
			sscanf(tokens[1], "%lu", &parsed->postn);

			parsed->post = malloc(sizeof(struct script) * parsed->postn);
			if (!parsed->post) parsed->postn = 0;
		} else if (!strcmp(tokens[0], "post")) {
			if (!parsed->post) continue;

			if (!strcmp(tokens[1], "wait")) parsed->post[current_post].type = wait;
			else if (!strcmp(tokens[1], "oneshot")) parsed->post[current_post].type = oneshot;
			else if (!strcmp(tokens[1], "respawn")) parsed->post[current_post].type = respawn;
			else goto free_tokens;

			parsed->post[current_post].cmd = strdup(tokens[2]);
			current_post++;
			if (current_post > parsed->postn) {
				parsed->postn = current_post;
			}
		}

		free_tokens:
		for (size_t i2 = 0; tokens[i2] != NULL; i2++) free(tokens[i2]);
		free(tokens);
	}

	for (size_t i2 = 0; lines[i2] != NULL; i2++) free(lines[i2]);
	free(lines);

	return parsed;
}