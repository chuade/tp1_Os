#! /bin/bash
clear
make clean
make
if [ $? -eq 0 ]; then
	echo "[--] Compilation Success"
	echo "[--] Backup save..."
	sudo cp my_kernel_module.c /host_shared_rep/IFT2245/Backup/
	echo "[--] Synchronizing..."
	sync
	echo "[--] Clearing old kernel log"
	sudo dmesg -c > previous_kernel.log
	echo "[--] Loading kernel module..."
	sudo insmod my_kernel_module.ko
	echo "[--] List of loaded kernel modules"
	lsmod | grep my_kernel_module
	echo "[--] Printing kernel log -- after loading kernel module"
	dmesg
	echo "[--] Waiting for 5 seconds..."
	sleep 5
	echo "[--] Removing kernel module"
	sudo rmmod my_kernel_module.ko
	echo "[--] Printing kernel log -- after kernel module removal"
	dmesg
else
	echo "[!!] Compilation Error !"
fi
