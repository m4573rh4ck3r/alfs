#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>

#include "curlfile.h"
#include "createlfsdirs.h"
#include "lfscli.h"
#include "lfsenv.h"
#include "lfsuser.h"

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

int main(int argc, char **argv) {
	// check if we are root
	requireRoot();

	struct Alfs alfs = {0};
	alfs = unmarshalAlfs(argc, argv);

	// create essential directories
	createLFSDirIfNotExist();

	printf("creating partitions...\n");
	partition("/dev/sdc", &alfs.layout);

	printf("mounting partitions...\n");
	mountLFSPartitions(alfs.layout);

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
		printf("setting up environment...\n");
		createEnvFiles();
		printf("building tools...\n");
	} else {
		waitpid(pid, &status, 0);
		printf("tools built!\n");
	}
}
