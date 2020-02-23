#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>

struct Layout {
	bool withSwap;
	bool withHome;
	bool withBoot;
	bool withUsr;
	bool withEFI;
	char *rootSize;
	char *homeSize;
	char *swapSize;
	char *bootSize;
	char *usrSize;
	char *EFISize;
};

void createPartition(char *device, int number, char *size) {
	int commandSize = snprintf(NULL, 0, "sgdisk -n %d:0:%s %s", number, size, device);
	char *command = malloc(commandSize + 1);
	snprintf(command, commandSize + 1, "sgdisk -n %d:0:%s %s", number, size, device);
	system(command);
	free(command);
}

void deletePartitions(char *device) {
	printf("deleting partitions...\n");
	int commandSize = snprintf(NULL, 0, "sfdisk --delete %s", device);
	char *command = malloc(commandSize + 1);
	snprintf(command, commandSize + 1, "sfdisk --delete %s", device);
	system(command);
	free(command);
}

void partition(char *device, struct Layout layout) {
	deletePartitions(device);

	int partitionCounter = 1; // counter for partition number
	int bootNum;
	int EFINum;
	int swapNum;
	int homeNum;
	int usrNum;
	int rootNum;

	// check if a seperate /boot partition is wanted
	if (layout.withBoot) {
		printf("creating boot partition...\n");
		createPartition(device, partitionCounter, layout.bootSize);
		bootNum = partitionCounter;
		partitionCounter++;
	}

	// check if an EFI partition is wanted
	if (layout.withEFI) {
		printf("creating efi partition...\n");
		createPartition(device, partitionCounter, layout.EFISize);
		EFINum = partitionCounter;
		partitionCounter++;
	}

	// check if a Swap partition is wanted
	if (layout.withSwap) {
		printf("creating swap partition...\n");
		createPartition(device, partitionCounter, layout.swapSize);
		swapNum = partitionCounter;
		partitionCounter++;
	}

	// check if a seperate /home partition is wanted
	if (layout.withHome) {
		printf("creating home partition...\n");
		createPartition(device, partitionCounter, layout.homeSize);
		homeNum = partitionCounter;
		partitionCounter++;
	}

	// check if a seperate /usr partition is wanted
	if (layout.withUsr) {
		printf("creating usr partition\n");
		createPartition(device, partitionCounter, layout.usrSize);
		usrNum = partitionCounter;
		partitionCounter++;
	}
	printf("creating root partition...\n");
	// check if root partition size is provided otherwise use remaining disk space
	if (&layout.rootSize != NULL) {
		createPartition(device, partitionCounter, layout.rootSize);
	} else {
		char *rootSize = "0";
		createPartition(device, partitionCounter, rootSize);
	}
	rootNum = partitionCounter;
}
