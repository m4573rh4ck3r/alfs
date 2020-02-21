#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

void createProfileFile() {
	FILE *fp;

	fp = fopen("/home/lfs/.bash_profile", "w+");
	if (fp == NULL) {
		perror("fopen /home/lfs/.bash_profile");
		exit(EXIT_FAILURE);
	}
	fprintf(fp, "exec env -i HOME=$HOME TERM=$TERM PS1='\\u:\\w\\$ ' /bin/bash");
	fclose(fp);
}

void createRCFile() {
	FILE *fp;

	fp = fopen("/home/lfs/.bashrc", "w+");
	if (fp == NULL) {
		perror("fopen /home/lfs/.bashrc");
		exit(EXIT_FAILURE);
	}
	fprintf(fp, "set +h\numask 022\nLFS=/mnt/lfs\nLC_ALL=POSIX\nLFS_TGT=$(uname -m)-lfs-linux-gnu\nPATH=/tools/bin:/bin:/usr/bin\nexport LFS LC_ALL LFS_TGT PATH");
	fclose(fp);
}

void createEnvFiles() {
	createProfileFile();
	createRCFile();
}
