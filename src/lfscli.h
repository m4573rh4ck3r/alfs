#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#include "partition.h"

#define VERSION = "v0.1"

const char *Help =
"%s %s \n\
Usage: %s [options] <arg>\n\
\n\
Options:\n\
\t-R <size>\tsize of root partition\n\
\t-B <size>\tcreate seperate boot partition with specified size\n\
\t-S <size>\tcreate swap partition with specified size\n\
\t-E <size>\tcreate EFI partition with specified size\n\
\t-H <size>\tcreate seperate home partition with specified size\n\
\t-U <size>\tcreate seperate usr partition with specified size\n\
\t-v <size>\tverbose output\n\
\t-V <size>\tshow version\n\
\t-h <size>\tshow this help\n\
";

struct Alfs {
	struct Layout layout;
	bool verbose;
};

/*
 * R rootSize
 * B bootSize
 * S swapSize
 * E EFISize
 * H homeSize
 * U usrSize
 * v verbose
 * V Version
 * h help
 * */

struct Alfs unmarshalAlfs(int argc, char **argv) {
	struct Alfs alfs = {0};
	int opt;
	char *subopts, *value;

	while ((opt = getopt (argc, argv, "R:B:S:E:H:U:vVh")) != -1) {
		switch (opt){
			case 'R':
				alfs.layout.rootSize = optarg;
				break;
			case 'B':
				alfs.layout.withBoot = true;
				alfs.layout.bootSize = optarg;
				break;
			case 'S':
				alfs.layout.withSwap = true;
				alfs.layout.swapSize = optarg;
				break;
			case 'E':
				alfs.layout.withEFI = true;
				alfs.layout.EFISize = optarg;
				break;
			case 'H':
				alfs.layout.withHome = true;
				alfs.layout.homeSize = optarg;
				break;
			case 'U':
				alfs.layout.withUsr = true;
				alfs.layout.usrSize = optarg;
				break;
			case 'v':
				alfs.verbose = true;
				break;
			case 'V':
				printf("%s %s\n", argv[0], "v0.1");
				exit(EXIT_SUCCESS);
				break;
			case 'h':
				printf(Help, argv[0], "v0.1", argv[0]);
				exit(EXIT_SUCCESS);
				break;
			default:
				errno = EINVAL;
				perror("alfs");
				exit(EXIT_FAILURE);
		}
	}
	if (alfs.layout.rootSize == NULL) {
		alfs.layout.rootSize = "0";
	}
	return alfs;
}
