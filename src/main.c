#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <string.h>
#define LFS "/mnt/lfs"

void requireRoot() {
	uid_t uid = getuid();

	if (uid != 0) {
		errno = EPERM;
		perror("alfs");
		exit(EXIT_FAILURE);
	}
}

void createLFSDirIfNotExist() {
	struct stat st = {0};
	int mkdirLFSDirReturnCode;

	if (stat(LFS, &st) == -1) {
		mkdirLFSDirReturnCode = mkdir(LFS, 0700);
		if (mkdirLFSDirReturnCode == -1) {
			printf("%s: directory already exists\n", LFS);
		} else if(mkdirLFSDirReturnCode != 0) {
			errno = mkdirLFSDirReturnCode;
			perror("mkdir");
			exit(EXIT_FAILURE);
		}
		printf("created directory %s\n", LFS);
	}
}

void createToolsDirIfNotExist() {
	createLFSDirIfNotExist();

	struct stat st = {0};
	struct stat rootSt = {0};
	char toolsDir[15];
	int mkdirToolsDirReturnCode;

	strcat(toolsDir, LFS);
	strcat(toolsDir, "/tools");

	if (stat(toolsDir, &st) == -1) {
		mkdirToolsDirReturnCode = mkdir(toolsDir, 0700);
		if (mkdirToolsDirReturnCode == -1) {
			printf("%s: directory already exists\n", toolsDir);
		} else if (mkdirToolsDirReturnCode != 0) {
			errno = mkdirToolsDirReturnCode;
			perror("mkdir");
			exit(EXIT_FAILURE);
		}
		printf("created directory %s\n", toolsDir);
	}

	if (lstat("/root", &rootSt) == -1) {
		symlink("/root", toolsDir);
	}
}

void createSourcesDirIfNotExist() {
	createLFSDirIfNotExist();

	struct stat st = {0};
	char sourcesDir[17];
	int mkdirSourcesDirReturnCode;

	strcat(sourcesDir, LFS);
	strcat(sourcesDir, "/sources");

	if (stat(sourcesDir, &st) == -1) {
		mkdirSourcesDirReturnCode = mkdir(sourcesDir, 0700);
		if (mkdirSourcesDirReturnCode == -1) {
			printf("%s: directory already exists\n", sourcesDir);
		} else if(mkdirSourcesDirReturnCode != 0) {
			errno = mkdirSourcesDirReturnCode;
			perror("mkdir");
			exit(EXIT_FAILURE);
		}
		printf("created directory %s\n", sourcesDir);
	}
}

void createLFSGroupIfNotExist() {
	char *buf;
	size_t bufsize;
	const char *name;
	struct group grp;
	struct group *lfsGroupEntry;
	int grpReturnCode;

	bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (bufsize == -1)
		bufsize = 16384;
	buf = malloc(bufsize);
	if (buf == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	name = "lfs";

	grpReturnCode = getgrnam_r(name, &grp, buf, bufsize, &lfsGroupEntry);

	if (lfsGroupEntry == NULL) {
		if (grpReturnCode == 0) {
			printf("Could not find the lfs group in /etc/groups. Creating it...\n");
			system("groupadd lfs");
		}
		else {
			errno = grpReturnCode;
			exit(EXIT_FAILURE);
		}
	} else {
		printf("using existing lfs group\n");
	}
}

struct passwd getLFSPasswdEntry() {
	char *buf;
	size_t bufsize;
	int pwdReturnCode;
	struct passwd *lfsPasswdEntry;
	struct passwd pwd;

	createLFSGroupIfNotExist();

	bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufsize == -1)
		bufsize = 16384;
	buf = malloc(bufsize);
	if (buf == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	loadLFSUser:

	pwdReturnCode = getpwnam_r("lfs", &pwd, buf, bufsize, &lfsPasswdEntry);

	if (lfsPasswdEntry == NULL) {
		if (pwdReturnCode == 0) {
			printf("Could not find the lfs user in /etc/passwd. Creating it...\n");
			system("useradd -s /bin/bash -g lfs -m -k /dev/null lfs");
			goto loadLFSUser;
		}
		else {
			errno = pwdReturnCode;
			perror("getpwnam_r");
		}
		exit(EXIT_FAILURE);
	} else {
		printf("using existing lfs user\n");
	}
	return pwd;
}

int main() {
	requireRoot();
	createLFSDirIfNotExist();
	createToolsDirIfNotExist();
	createSourcesDirIfNotExist();

	struct passwd lfsPasswdEntry;
	int setUIDReturnCode;
	int setGIDReturnCode;
	int pid;

	lfsPasswdEntry = getLFSPasswdEntry();

	setGIDReturnCode = setgid(lfsPasswdEntry.pw_gid);
	if (setGIDReturnCode != 0) {
		perror("setgid");
		exit(EXIT_FAILURE);
	}

	setUIDReturnCode = setuid(lfsPasswdEntry.pw_uid);
	if (setUIDReturnCode != 0) {
		perror("setuid");
		exit(EXIT_FAILURE);
	}

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
