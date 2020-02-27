#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <sys/mount.h>

struct Layout {
	bool withSwap;
	bool withHome;
	bool withBoot;
	bool withUsr;
	bool withEFI;
	char *rootSize;
	char *rootPartition;
	char *homeSize;
	char *homePartition;
	char *swapSize;
	char *swapPartition;
	char *bootSize;
	char *bootPartition;
	char *usrSize;
	char *usrPartition;
	char *EFISize;
	char *EFIPartition;
};

void createPartition(char *device, int number, char *size) {
	int commandSize = snprintf(NULL, 0, "sgdisk -n %d:0:+%s %s", number, size, device);
	char *command = malloc(commandSize + 1);
	snprintf(command, commandSize + 1, "sgdisk -n %d:0:+%s %s", number, size, device);
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

char *getPartitionName(char *device, int partitionNumber) {
	int size = snprintf(NULL, 0, "%s%d", device, partitionNumber);
	char *partitionName = malloc(size + 1);
	snprintf(partitionName, size + 1, "%s%d", device, partitionNumber);
	return partitionName;
}

void createFilesystem(char *partitionName, char *type) {
	int commandSize = snprintf(NULL, 0, "mkfs.%s %s", type, partitionName);
	char *command = malloc(commandSize + 1);
	snprintf(command, commandSize + 1, "mkfs.%s %s", type, partitionName);
	system(command);
	free(command);
}

void partition(char *device, struct Layout *layout) {
	deletePartitions(device);

	int partitionCounter = 1; // counter for partition number

	// check if a seperate /boot partition is wanted
	if (&layout->withBoot != NULL && &layout->withBoot) {
		printf("creating boot partition...\n");
		createPartition(device, partitionCounter, layout->bootSize);
		layout->bootPartition = getPartitionName(device, partitionCounter);
		createFilesystem(layout->bootPartition, "ext4");

		partitionCounter++;
	}

	// check if an EFI partition is wanted
	if (&layout->withEFI != NULL && &layout->withEFI) {
		printf("creating efi partition...\n");
		createPartition(device, partitionCounter, layout->EFISize);
		layout->EFIPartition = getPartitionName(device, partitionCounter);
		createFilesystem(layout->EFIPartition, "fat -F32");

		partitionCounter++;
	}

	// check if a Swap partition is wanted
	if (&layout->withSwap != NULL && &layout->withSwap) {
		printf("creating swap partition...\n");
		createPartition(device, partitionCounter, layout->swapSize);

		layout->swapPartition = getPartitionName(device, partitionCounter);

		int mkswapCommandSize = snprintf(NULL, 0, "mkswap %s", layout->swapPartition);
		char *mkswapCommand = malloc(mkswapCommandSize  + 1);
		snprintf(mkswapCommand, mkswapCommandSize + 1, "mkswap %s", layout->swapPartition);
		system(mkswapCommand);
		free(mkswapCommand);

		int swaponCommandSize = snprintf(NULL, 0, "swapon %s", layout->swapPartition);
		char *swaponCommand = malloc(swaponCommandSize + 1);
		snprintf(swaponCommand, swaponCommandSize + 1, "swapon %s", layout->swapPartition);
		system(swaponCommand);
		free(swaponCommand);

		partitionCounter++;
	}

	// check if a seperate /home partition is wanted
	if (&layout->withHome != NULL && &layout->withHome) {
		printf("creating home partition...\n");
		createPartition(device, partitionCounter, layout->homeSize);
		layout->homePartition = getPartitionName(device, partitionCounter);
		createFilesystem(layout->homePartition, "ext4");

		partitionCounter++;
	}

	// check if a seperate /usr partition is wanted
	if (&layout->withUsr != NULL && &layout->withUser) {
		printf("creating usr partition\n");
		createPartition(device, partitionCounter, layout->usrSize);
		layout->usrPartition = getPartitionName(device, partitionCounter);
		createFilesystem(layout->usrPartition, "ext4");

		partitionCounter++;
	}
	printf("creating root partition...\n");
	// check if root partition size is provided otherwise use remaining disk space
	if (&layout->rootSize != NULL) {
		createPartition(device, partitionCounter, layout->rootSize);
	} else {
		char *rootSize = "0";
		createPartition(device, partitionCounter, rootSize);
	}

	layout->rootPartition = getPartitionName(device, partitionCounter);
	createFilesystem(layout->rootPartition, "ext4");
}

void mountPartition(char *s, char *d, char *t) {
	const char *src = s;
	const char *dst = d;
	const char *type = t;
	const unsigned long mntflags = 0;

	int returnCode = mount(src, dst, type, mntflags, NULL);
	if (returnCode == -1) {
		perror("mount");
		exit(EXIT_FAILURE);
	}
}

void mountLFSPartitions(struct Layout layout) {
	printf("mounting %s on %s with type ext4\n", layout.rootPartition, LFS);
	mountPartition(layout.rootPartition, LFS, "ext4");
	
	if (layout.withBoot) {
		int bootMpSize = snprintf(NULL, 0, "%s/boot", LFS);
		char *bootMp = malloc( bootMpSize + 1);
		snprintf(bootMp, bootMpSize + 1, "%s/boot", LFS);
		mountPartition(layout.bootPartition, bootMp, "ext4");
		free(bootMp);
	}

	if (layout.withEFI) {
		int efiMpSize = snprintf(NULL, 0, "%s/boot/efi", LFS);
		char *efiMp = malloc(efiMpSize + 1);
		snprintf(efiMp, efiMpSize + 1, "%s/boot/efi", LFS);
		mountPartition(layout.EFIPartition, efiMp, "vfat");
		free(efiMp);
	}
	if (layout.withHome) {
		int homeMpSize = snprintf(NULL, 0, "%s/home", LFS);
		char *homeMp = malloc(homeMpSize + 1);
		snprintf(homeMp, homeMpSize + 1, "%s/home", LFS);
		mountPartition(layout.homePartition, homeMp, "ext4");
		free(homeMp);
	}
	if (layout.withUsr) {
		int usrMpSize = snprintf(NULL, 0, "%s/usr", LFS);
		char *usrMp = malloc(usrMpSize + 1);
		snprintf(usrMp, usrMpSize + 1, "%s/usr", LFS);
		mountPartition(layout.usrPartition, usrMp, "ext4");
		free(usrMp);
	}
}
