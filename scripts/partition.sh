#!/bin/bash

set -euo pipefail

if [ -z "$1" ];
then
	echo "Please specify the device for lfs" >&2
	exit 1;
else
	export DEVICE_PARTITION="$1";
fi

#crucial variables
export LFS=/mnt/lfs

#get maximum amount of disk size and ram size
export TOTAL_DISK_SIZE=$(sfdisk -s "$DEVICE_PARTITION")
export TOTAL_RAM_SIZE=$(grep MemTotal /proc/meminfo | awk '{print $2}')

#Set partition numbers
export BOOT_PARTITION="${DEVICE_PARTITION}1"
export SWAP_PARTITION="${DEVICE_PARTITION}2"
export ROOT_PARTITION="${DEVICE_PARTITION}3"

#Set partition sizes in KB
export BOOT_PARTITION_SIZE=10000000
export SWAP_PARTITION_SIZE=$(python -c "print(round(${TOTAL_RAM_SIZE}/2))")

#Delete all current partitions on the device
sfdisk --delete "$DEVICE_PARTITION"

#Create the new partitions
sgdisk -n 1:0:${BOOT_PARTITION_SIZE}K "$DEVICE_PARTITION" # create boot partition
sgdisk -n 2:0:+${SWAP_PARTITION_SIZE}K "$DEVICE_PARTITION" # create swap partition
sgdisk -n 3:0:0 "$DEVICE_PARTITION" # create root partition

#Create the filesystems
mkfs.fat -F32 "$BOOT_PARTITION"
mkswap "$SWAP_PARTITION"
mkfs -v -t ext4 "$ROOT_PARTITION"

#Mount the partitions
mkdir -pv "$LFS"
mount -v -t ext4 "$ROOT_PARTITION" "$LFS"
mkdir -pv "${LFS}/boot"
mount "$BOOT_PARTITION" "${LFS}/boot"
/sbin/swapon -v "$SWAP_PARTITION"
