#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>

#define LFS "/mnt/lfs"

void requireRoot() {
	uid_t uid = getuid();

	if (uid != 0) {
		errno = EPERM;
		perror("alfs");
		exit(EXIT_FAILURE);
	}
}

struct passwd getLFSPasswdEntry() {
	char *buf;
	size_t bufsize;
	int pwdReturnCode;
	struct passwd *lfsPasswdEntry;
	struct passwd pwd;

	bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufsize == -1)
		bufsize = 16384;
	buf = malloc(bufsize);
	if (buf == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	pwdReturnCode = getpwnam_r("lfs", &pwd, buf, bufsize, &lfsPasswdEntry);

	if (lfsPasswdEntry == NULL) {
		if (pwdReturnCode == 0) {
			perror("Could not find the lfs user in /etc/passwd");
		}
		else {
			errno = pwdReturnCode;
			perror("getpwnam_r");
		}
		exit(EXIT_FAILURE);
	}
	return pwd;
}

int main() {
	requireRoot();

	struct passwd lfsPasswdEntry;
	lfsPasswdEntry = getLFSPasswdEntry();
	setuid(lfsPasswdEntry.pw_uid);
	setgid(lfsPasswdEntry.pw_gid);
	int pid;
	pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		wait(NULL);
	} else {
		printf("building tools...\n");
		exit(0);
	}
	printf("tools built!\n");
}
