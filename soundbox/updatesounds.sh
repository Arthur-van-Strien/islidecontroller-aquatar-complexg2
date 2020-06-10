#!/bin/bash

# rsync -v -r release@dev.ibgnetworx.nl:/home/release/branches/columbuszoo/var/audio/* /home/pi/music/

mpc update

mpc clear
mpc update

sleep 2

pushd /home/pi/music/
for f in *.m*; do
  echo "Adding $f..."
  mpc add "$f"
done
for f in *.wav; do
  echo "Adding $f..."
  mpc add "$f"
done
popd

mpc update
sudo /etc/init.d/mpd restart
