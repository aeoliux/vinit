#!/bin/sh

#
#MIT License
#
#Copyright (c) 2022 Zapomnij
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.
#

export PATH=/sbin:/usr/sbin:/bin:/usr/sbin
umask 022

harderror() {
	if [ -x "/sbin/sulogin" ]; then
		echo "System initializing failed. Login as root to fix problems"
		sulogin
	else
		echo "sulogin not found. Click enter to reboot"
		read
	fi

	mount -o ro,remount /
	sync
	initreq -s init_reboot -o hard
}

echo "=> Mounting virtual filesystems"
mount -n -t proc proc /proc

if ! mountpoint /dev > /dev/null 2>&1; then
	mount -n -t devtmpfs devtmpfs /dev
fi
mkdir -p /dev/pts /dev/shm
mount -n -t devpts devpts /dev/pts
mount -n -t tmpfs shm /dev/shm

if ! mountpoint /sys > /dev/null 2>&1; then
	mount -n -t sysfs sysfs /sys
fi
mount -t tmpfs run /run

if grep -wq cgroup /proc/filesystems; then
	if [ -d /sys/fs/cgroup ]; then
		mount -t cgroup cgroup /sys/fs/cgroup
	else
		mkdir -p /dev/cgroup
		mount -t cgroup cgroup /dev/cgroup
	fi
fi

if [ -x /etc/rc.conf ]; then
	echo "=> Loading rc configuration"
	. /etc/rc.conf
elif [ -x /usr/etc/rc.conf ]; then
	echo "=> Loading rc configuration"
	. /usr/etc/rc.conf
fi

if [ -n "${MOUNT_TMPFS}" ]; then
	mount -t tmpfs tmpfs /tmp
fi

echo "=> Running fsck"
fsck -mpf
fsckret=${?}
if [ "${fsckret}" -eq 32 ]; then
	fsck
elif [ "${fsckret}" -gt 33 ]; then
	echo "Filesystem is corrupted. Cannot continue"
	harderror
fi


echo "=> Remount rootfs as read-write"
mount -o rw,remount /
echo "=> Mounting filesystems"
mount -a
echo "=> Enabling swaps"
swapon -a

if [ -n "${HWCLOCK}" ]; then
	if [ -x /sbin/hwclock ]; then
		echo "=> Setting system time from hardware clock"
		case "${HWCLOCK}" in
			"UTC")
				hwclock --utc --hctosys
				;;
			"localtime")
				hwclock --localtime --hctosys
				;;
			*)
				hwclock --systohc
				;;
		esac
	fi
fi

if [ -n "${HOSTNAME}" ]; then
	echo "=> Setting hostname to ${HOSTNAME}"
	hostname ${HOSTNAME}
fi

if [ -n "${KEYMAP}" ]; then
	echo "=> Loading keymap ${KEYMAP}"
	if [ -w /proc/sys/dev/mac_hid/keyboard_sends_linux_keycodes ]; then
		echo 1 > /proc/sys/dev/mac_hid/keyboard_sends_linux_keycodes
	fi
	loadkeys ${KEYMAP}
fi

[ -f /etc/mtab ] && rm /etc/mtab
echo "=> Linking mtab"
ln -s /proc/self/mounts /etc/mtab

if [ -x /sbin/sysctl -a -r /etc/sysctl.conf ]; then
	echo "=> Setting kernel parameters via sysctl"
	sysctl -e -p /etc/sysctl.conf
fi

if [ -n "${SERVICES}" ]; then
	echo "=> Starting services"
	for serv in ${SERVICES}; do
		echo "	-> starting ${serv}"
		${PREFIX}/etc/rc.d/${serv} start
		if [ "${?}" -ne 0 ]; then
			echo "${serv} failed"
		fi
	done
fi