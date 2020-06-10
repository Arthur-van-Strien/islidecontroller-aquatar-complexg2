#!/bin/bash

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root" 1>&2
	exit 1
fi

rsync --verbose --recursive --perms release@dev2.ibgnetworx.nl:/home/release/branches/aquatar_complexg2/* /
