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
#include "curlfile.h"
#include "createlfsdirs.h"

#define LFS "/mnt/lfs"
#define TOOLSDIR "/mnt/lfs/tools"
#define SOURCESDIR "/mnt/lfs/sources"

void requireRoot() {
	if (getuid() != 0) {
		errno = EPERM;
		perror("alfs");
		exit(EXIT_FAILURE);
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
	// check if we are root
	requireRoot();

	// create essential directories
	createLFSDirIfNotExist();
	createToolsDirIfNotExist();
	createSourcesDirIfNotExist();

	// download wget list file and checksum file
	char *wgetListURL = "http://www.linuxfromscratch.org/lfs/view/stable-systemd/wget-list";
	char *md5sumsURL = "http://www.linuxfromscratch.org/lfs/view/stable-systemd/md5sums";

	char wgetListFILE[256] = "/mnt/lfs/sources/wget-list";
	char md5sumsFILE[256] = "/mnt/lfs/sources/md5sums";

	curlFile(wgetListURL, wgetListFILE);
	curlFile(md5sumsURL, md5sumsFILE);

	// download sources
	int wgetReturnCode;
	int checksumReturnCode;

	chdir(SOURCESDIR);
	wgetReturnCode = system("wget --input-file=/mnt/lfs/sources/wget-list --continue");
	if (wgetReturnCode != 0) {
		printf("WARNING: wget returned an error code\n");
	}
	checksumReturnCode = system("md5sum -c md5sums");
	if (checksumReturnCode != 0) {
		errno = checksumReturnCode;
		perror("md5sum");
		exit(EXIT_FAILURE);
	}

	// create lfs group and user if the don't exist yet
	struct passwd lfsPasswdEntry;
	int setUIDReturnCode;
	int setGIDReturnCode;

	lfsPasswdEntry = getLFSPasswdEntry();

	// build tools
	int status;
	int pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		int setUIDReturnCode;
		int setGIDReturnCode;

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
		printf("building tools...\n");
	} else {
		waitpid(pid, &status, 0);
		printf("tools built!\n");
	}
}
