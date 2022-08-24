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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <unistd.h>
#include "all.h"

enum haltmode {
	Reboot,
	Shutdown,
	Halt
};

#ifndef INIT_MAX_REQUEST
#define INIT_MAX_REQUEST 255
#endif

int main() {
	if (getpid() != 1) {
		return 1;
	}

	enum haltmode halt;
	char *shutdowncmd = NULL, *rebootcmd = NULL, *haltcmd = NULL;

	puts("init: starting");

	char *inittab = readFile("/etc/inittab");
	if (!inittab) {
		goto error;
	}
	
	struct inittab *parsed = parseInittab(inittab);
	if (!parsed) {
		free(inittab);
		goto error;
	}
	free(inittab);

	if (parsed->sysinit) {
		puts("init: entering 'sysinit'");
		if (runShellCmd(parsed->sysinit)) {
			fputs("init: failed to sysinit\n", stderr);
			goto error;
		}
	} else {
		fputs("init: sysinit hasn't been defined\n", stderr);
		goto error;
	}
	if (parsed->shutdown) {
		shutdowncmd = strdup(parsed->shutdown);
	}
	if (parsed->reboot) {
		rebootcmd = strdup(parsed->reboot);
	}
	if (parsed->halt) {
		haltcmd = strdup(parsed->halt);
	}
	if ((parsed->post) && (parsed->postn)) {
		for (size_t i = 0; i < parsed->postn; i++) {
			switch (parsed->post[i].type) {
				case wait: {
					runShellCmd(parsed->post[i].cmd);
					break;
				}
				case oneshot: {
					runAndFork(parsed->post[i].cmd);
					break;
				}
				case respawn: {
					respawnCmd(parsed->post[i].cmd);
					break;
				}
			}
		}
	}

	for (size_t i = 0; i < parsed->postn; i++) free(parsed->post[i].cmd);
	free(parsed->post);
	free(parsed->shutdown);
	free(parsed->sysinit);
	free(parsed->reboot);
	free(parsed->halt);
	free(parsed);

	if (mkfifo("/run/initctl", 0644)) {
		perror("init: failed to create fifo '/run/initctl'");
		goto infiniteloop;
	} else {
		int fd = open("/run/initctl", O_RDONLY);
		if (fd == -1) {
			perror("init: failed to open '/run/initctl' for reading");
			goto infiniteloop;
		}

		char request[INIT_MAX_REQUEST];
		while (1) {
			size_t n = read(fd, request, INIT_MAX_REQUEST);
			if (n == -1) {
				close(fd);
				unlink("/run/initctl");
				perror("init: failed to read from '/run/initctl'");
				goto infiniteloop;
			}

			if (!strncmp(request, "init_poweroff", n)) {
				halt = Shutdown;
				close(fd);
				unlink("/run/initctl");
				goto halt;
			} else if (!strncmp(request, "init_reboot", n)) {
				halt = Reboot;
				close(fd);
				goto halt;
			} else if (!strncmp(request, "init_halt", n)) {
				halt = Halt;
				close(fd);
				goto halt;
			}
		}
	}

	error:
	fputs("init: dropping to shell, type 'exit' to reboot\n", stderr);
	int ret = runShellCmd("exec sh");

	if (ret) fputs("init: shell exited with non-zero status\n", stderr);

	halt = Reboot;
	goto halt;

	infiniteloop:
	fputs("init: starting infinite loop\n", stderr);
	while (1) sleep(1);

	halt:
	puts("init: starting halting procedure");
	unlink("/run/initctl");

	switch (halt) {
		case Reboot: {
			if (rebootcmd) {
				if (runShellCmd(rebootcmd)) {
					fputs("init: reboot script returned non-zero exit status\n", stderr);
				}
				free(rebootcmd);
			}
			reboot(RB_AUTOBOOT);
			perror("init: failed to reboot");
			goto infiniteloop;
		}
		case Shutdown: {
			if (shutdowncmd) {
				if (runShellCmd(shutdowncmd)) {
					fputs("init: shutdown script returned non-zero exit status\n", stderr);
				}
				free(shutdowncmd);
			}
			reboot(RB_POWER_OFF);
			perror("init: failed to poweroff");
			goto infiniteloop;
		}
		case Halt: {
			if (haltcmd) {
				if (runShellCmd(haltcmd)) {
					fputs("init: halting script returned non-zero exit status\n", stderr);
				}
				free(haltcmd);
			}
			reboot(RB_HALT_SYSTEM);
			perror("init: failed to halt system");
			goto infiniteloop;
		}
	}
    return 0;
}