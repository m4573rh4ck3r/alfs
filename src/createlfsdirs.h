#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define LFS "/mnt/lfs"
#define TOOLSDIR "/mnt/lfs/tools"
#define SOURCESDIR "/mnt/lfs/sources"

void createLFSDirIfNotExist() {
	int lfsDirReturnCode;

	lfsDirReturnCode = mkdir(LFS, 0700);
	if (lfsDirReturnCode == 0) {
		printf("%s: directory created\n", LFS);
	} else if(errno == EEXIST) {
		printf("%s: directory exists\n", LFS);
	} else {
		char msg[100];
		strcat(msg, "mkdir ");
		strcat(msg, LFS);
		perror(msg);
		exit(EXIT_FAILURE);
	}
}

void createToolsDirIfNotExist() {
	int toolsDirReturnCode;
	int toolsDirLinkReturnCode;

	toolsDirReturnCode = mkdir(TOOLSDIR, 0700);
	if (toolsDirReturnCode == 0) {
		printf("%s: directory created\n", TOOLSDIR);
	} else if (errno == EEXIST) {
		printf("%s: directory exists\n", TOOLSDIR);
	} else {
		char msg[100];
		strcat(msg, "mkdir ");
		strcat(msg, TOOLSDIR);
		perror(msg);
		exit(EXIT_FAILURE);
	}

	toolsDirLinkReturnCode = symlink(TOOLSDIR, "/tools");
	if (toolsDirReturnCode == 0) {
		printf("created symlink for tools directory\n");
	} else {
		perror("symlink");
		exit(EXIT_FAILURE);
	}
}

void createSourcesDirIfNotExist() {
	int sourcesDirReturnCode;

	sourcesDirReturnCode = mkdir(SOURCESDIR, 0700);
	if (sourcesDirReturnCode == 0) {
		printf("%s: directory created\n", SOURCESDIR);
	} else if (errno == EEXIST) {
		printf("%s: directory exists\n", SOURCESDIR);
	} else {
		char msg[100];
		strcat(msg, "mkdir ");
		strcat(msg, SOURCESDIR);
		perror(msg);
		exit(EXIT_FAILURE);
	}
}

