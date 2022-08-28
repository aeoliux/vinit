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

if ! mountpoint /sys > /dev/null 2>&1; then
	mount -n -t sysfs sysfs /sys
fi
mount -t tmpfs run /run

if mountpoint /dev > /dev/null 2>&1; then
	umount /dev
fi
mount -n -t devtmpfs devtmpfs /dev
mkdir -p /dev/pts /dev/shm
mount -n -t devpts devpts /dev/pts
mount -t tmpfs shm /dev/shm

if grep -wq cgroup /proc/filesystems && [ -d /sys/fs/cgroup ]; then
	echo "=> Mounting cgroup filesystems"
	mount -t cgroup2 cgroup /sys/fs/cgroup

	cd /sys/fs/cgroup
	mkdir -p cpu && mount -t cgroup -o cpu cpu cpu
	mkdir -p cpuacct && mount -t cgroup -o cpuacct cpuacct cpuacct
	mkdir -p cpuset && mount -t cgroup -o cpuset cpuset cpuset
	mkdir -p memory && mount -t cgroup -o memory memory memory
	mkdir -p devices && mount -t cgroup -o devices devices devices
	mkdir -p freezer && mount -t cgroup -o freezer freezer freezer
	mkdir -p net_cls && mount -t cgroup -o net_cls net_cls net_cls
	mkdir -p perf_event && mount -t cgroup -o perf_event perf_event perf_event
	mkdir -p net_prio && mount -t cgroup -o net_prio net_prio net_prio
	mkdir -p hugetlb && mount -t cgroup -o hugetlb hugetlb hugetlb
	mkdir -p pids && mount -t cgroup -o pids pids pids
	mkdir -p rdma && mount -t cgroup -o rdma rdma rdma

	cd /
fi
if grep -wq efivarfs /proc/filesystems && [ -d /sys/firmware/efi/efivars ]; then
	echo "=> Mounting efivarfs"
	mount -t efivarfs efivarfs /sys/firmware/efi/efivars
fi
if grep -wq pstore /proc/filesystems && [ -d /sys/fs/pstore ]; then
	echo "=> Mounting pstore"
	mount -t pstore pstore /sys/fs/pstore
fi
if grep -wq securityfs /proc/filesystems; then
	echo "=> Mounting securityfs"
	[ ! -d /sys/kernel/security ] && mkdir -p /sys/kernel/security
	mount -t securityfs securityfs /sys/kernel/security
fi
if grep -wq configfs /proc/filesystems && [ -d /sys/kernel/config ]; then
	echo "=> Mounting configfs"
	mount -t configfs configfs /sys/kernel/config
fi

echo "=> Linking stdin, stdout, stderr and fd"
ln -sf /proc/self/fd/0 /dev/stdin
ln -sf /proc/self/fd/1 /dev/stdout
ln -sf /proc/self/fd/2 /dev/stderr
ln -sf /proc/self/fd /dev/fd

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

echo "=> Touching /var/run/utmp"
touch /var/run/utmp

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
	if [ -d /var/log/services ]; then
		echo "=> Saving old logs"
		cd /var/log/services
		for log in $(find . -type f -name '*.log'); do
			[ -f "${log}.old" ] && rm -f "${log}.old"
			mv "${log}" "${log}.old"
		done
		cd /
	else
		echo "=> Creating directory for services' logs"
		mkdir -p /var/log/services
	fi

	echo "=> Starting services"
	for serv in ${SERVICES}; do
		echo "	-> starting ${serv}"
		${PREFIX}/etc/rc.d/${serv} start
		if [ "${?}" -ne 0 ]; then
			echo "${serv} failed"
		fi
	done
fi