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
	int commandSize = snprintf(NULL, 0, "sgdisk -n %d:0:%s %s", number, &size);
	char *command = malloc(commandSize + 1);
	snprintf(command, commandSize + 1, "sgdisk -n %s:0:%s %s", number, &size);
	system(&command);
	free(command);
}

void deletePartitions(char *device) {
	int commandSize = snprintf(NULL, 0, "sfdisk --delete %s", &device);
	char *command = malloc(commandSize + 1);
	snprintf(command, commandSize + 1, "sfdisk --delete %s", &device);
	system(&command);
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
		createPartition(device, partitionCounter, layout.bootSize);
		bootNum = partitionCounter;
		partitionCounter++;
	}

	// check if an EFI partition is wanted
	if (layout.withEFI) {
		createPartition(device, partitionCounter, layout.EFISize);
		EFINum = partitionCounter;
		partitionCounter++;
	}

	// check if a Swap partition is wanted
	if (layout.withSwap) {
		createPartition(device, partitionCounter, layout.swapSize);
		swapNum = partitionCounter;
		partitionCounter++;
	}

	// check if a seperate /home partition is wanted
	if (layout.withHome) {
		createPartition(device, partitionCounter, layout.homeSize);
		homeNum = partitionCounter;
		partitionCounter++;
	}

	// check if a seperate /usr partition is wanted
	if (layout.withUsr) {
		createPartition(device, partitionCounter, layout.usrSize);
		usrNum = partitionCounter;
		partitionCounter++;
	}

	// check if root partition size is provided otherwise use remaining disk space
	if (&layout.rootSize != NULL) {
		createPartition(device, partitionCounter, layout.rootSize);
	} else {
		char *rootSize = "0";
		createPartition(device, partitionCounter, rootSize);
	}
	rootNum = partitionCounter;
}
