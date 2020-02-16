#!/bin/bash

set -euvo pipefail

export LFS=/mnt/lfs

#Download all sources
mkdir -v "${LFS}/sources"
chmod -v a+wt "${LFS}/sources"

wget http://www.linuxfromscratch.org/lfs/view/stable-systemd/wget-list
wget http://www.linuxfromscratch.org/lfs/view/stable-systemd/md5sums

wget --input-file=wget-list --continue --directory-prefix="${LFS}/sources"

pushd "${LFS}/sources"
md5sum -c md5sums
popd
