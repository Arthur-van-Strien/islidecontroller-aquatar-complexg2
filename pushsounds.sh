#!/bin/bash

set -o errexit

if [ $# == 0 ]; then
	echo "Usage: $0 [IP-address]"
	exit
fi

IP=$1

rsync --verbose --recursive --exclude=.svn --delete soundbox/ pi@$IP:/home/pi/music/
ssh -i ~/.ssh/id_rsa pi@$IP chmod a+x /home/pi/music/updatesounds.sh
ssh -i ~/.ssh/id_rsa pi@$IP sudo /home/pi/music/updatesounds.sh
