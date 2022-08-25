#!/bin/sh

export PATH=/sbin:/usr/sbin:/bin:/usr/bin
umask 022

if [ -x /etc/rc.conf ]; then
	. /etc/rc.conf
elif [ -x /usr/etc/rc.conf ]; then
	. /usr/etc/rc.conf
fi

if [ -n "${SERVICES}" ]; then
	echo "=> Stopping services"
	reverse=""
	for i in ${SERVICES}; do
		reverse="${reverse} ${i}"
	done

	for i in ${reverse}; do
		echo "	-> stopping ${i}"
		${PREFIX}/etc/rc.d/${i} stop
	done
fi

echo "=> Disabling swap"
swapoff -a

echo "=> Unmounting filesystems"
umount -a

echo "=> Syncing disks"
sync

echo "=> Remounting rootfs as read-only"
mount -o ro,remount /