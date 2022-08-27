#include <stddef.h>
#include <sys/types.h>
int writeFile(const char *path, const char *data, size_t len);
char *readFile(const char *path);

int checkService(const char *pidfile);
int stopService(const char *pidfile);
pid_t startService(char *const *argv, const char *pidfile);