#!/bin/bash

set -euvo pipefail

export LFS=/mnt/lfs

mkdir -v "${LFS}/tools"
ln -sv "${LFS}/tools" /

chown -v lfs "${LFS}/tools"
chown -v lfs "${LFS}/sources"
