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

if [ -w /proc/sysrq-trigger ]; then
	echo "=> Sending SIGKILL to all processes"
	echo "i" > /proc/sysrq-trigger

	echo "=> Sending SIGTERM to all processes"
	echo "e" > /proc/sysrq-trigger
fi

echo "=> Syncing disks"
sync

echo "=> Disabling swap"
swapoff -a

echo "=> Unmounting filesystems"
umount -a

echo "=> Remounting rootfs as read-only"
mount -o ro,remount /