sudo nano /etc/rc.local

"#!/bin/bash
# Print the IP address
_IP=$(hostname -I) || true
if [ "$_IP" ]; then
  printf "My IP address is %s\n" "$_IP"
fi

rm /tmp/app.log 2> /dev/null
sudo /home/pi/App.out > /tmp/app.log 2>&1 &

exit 0"

mount tmp ram

sudo nano /etc/fstab

##RAMDISK##
none /tmp tmpfs defaults 0 0