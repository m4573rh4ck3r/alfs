#!/bin/bash

set -euvo pipefail

if [ "$(cut -d: -f1 /etc/group | sort | grep -e "^lfs$" | wc -l)" -eq "0" ];
then
	groupsadd lfs;
fi

if [ "$(cut -d: -f1 /etc/passwd | sort | grep -e "^lfs$" | wc -l)" -eq "0" ];
then
	useradd -s /bin/bash -g lfs -m -k /dev/null lfs;
	passwd lfs
fi
