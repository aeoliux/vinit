#include <unistd.h>
#include <sys/wait.h>

void handle_sigchld() {
	int wstat;
	wait3(&wstat, WNOHANG, NULL);
}