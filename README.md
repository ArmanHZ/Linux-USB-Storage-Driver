Steps on how to make the driver work.

1) Run the make file together with the usb_driver.c
2) Install the module "sudo insmod usb_driver.ko"
3) Plug in your USB
4) Use "dmesg" to check if the USB is recognised, if not, you may have to change the Vendor and Device id's in usb_driver.c
4.1) If USB related info is not printed in dmesg, then use "sudo rmmod uas" and "sudo rmmod usb-storage or usb_storage"
5) The driver has registered the USB and allocated Character device regions which you can see in "ls /dev/" as usb_driver_dev, in "ls /sys/class" as usb_driver_sys, and "cat /proc/devices" as usb_driver_proc.
6) Compile the fileOp.c using "gcc -o fileOp fileOp.c"
7) run the fileOp using "./fileOp"
8) Read and Write to the character device and check for additional information using "dmesg"
9) To remove the module use "sudo rmmod usb_driver" (all the additional character devices ets. will be removed automatically).
10) Use "sudo make clean"
