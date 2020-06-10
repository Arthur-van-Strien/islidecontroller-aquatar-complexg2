#!/bin/bash

mount -t tmpfs -o size=30m tmpfs /var/www/slide/snapshots/

cd /var/engine/
chmod a+x sc.sh
chmod a+x slidecontroller
screen -d -m -S slidecontroller ./sc.sh
echo "iSlideController started"
echo "  Attach: screen -r slidecontroller"
echo "  Detach: Ctrl+A, D"
