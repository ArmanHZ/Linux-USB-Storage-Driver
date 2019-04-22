#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "USB Storage"

static struct usb_driver storage_driver = {
  .name = "USB Storage Driver",
  .id_table = storage_table,
  .probe = storage_probe,
  .disconnect = storage_disconnect,
};

static struct usb_device *device;
static struct cdev *my_cdev;
dev_t device_number; // Number that kernel will give us

// usb_device_id from kernel using command lsusb
static struct usb_device_id storage_table[] = {
  // Vendor id: 058f  Product id: 6387
  { USB_DEVICE(0x058f, 0x6387) },
  {}  // Terminating entry
};

// Registerin the device with the system
// Creating a device file in the process
static int driver_entry(void) {
  int return_value = alloc_chrdev_region(&device_number, 0, 1, DEVICE_NAME);
  if (return_value < 0) {
    printk(KERN_ALLERT "Failed to allocate a major number");
    return return_value;
  }
  int major_number = MAJOR(device_number);
  printk(KERN_INFO "Storage USB: major number is %d", major_number);
  printk(KERN_INFO "\tuse \t"mknod /dev/%s c %d 0\" for device file", DEVICE_NAME, major_number);

  my_cdev = cdev_alloc();
  my_cdev->ops = &fops;
  my_cdev->owner = THIS_MODULE;
  return_value = cdev_add(my_cdev, dev_num, 1);
  if (return_value < 0) {
    printk(KERN_ALLERT "Unable to add Character device to kernel");
    return return_value;
  }
  sema_init(&storage_driver.sem, 1);

  return 0;
}

static void driver_exit(void) {
  cdev_del(my_cdev);
  unregister_chrdev_region(device_number, 1);
  printk(KERN_ALLERT "Unloaded module");
}

// Probe function: Called when USB is inserted to the PC
// Also prints info about the USB to kernel. (dmesg)
static int storage_probe(struct usb_interface *interface, const struct usb_device_id *id) {
  struct usb_host_interface *host_interface;
  struct usb_endpoint_descriptor *endpoint_descriptor;

  host_interface = interface->cur_altsetting;

  printk(KERN_INFO "*** USB Storage Probed ***");
  printk(KERN_INFO "USB Storage with interface no: %d, vendor id: %04X, product id: %04X, is now Probbed\n",
    host_interface->desc.bInterfaceNumber, id->idVendor, id->idProduct);
  printk(KERN_INFO "Number of Endpoints: %02X\n", host_interface->desc.bInterfaceNumber);
  printk(KERN_INFO "Interface Class id: %02X\n", host_interface->desc.bInterfaceClass);

  int i;
  for (i = 0; i < host_interface->desc.bNumEndpoints; i++) {
      endpoint_descriptor = &host_interface->endpoint[i].desc;
      printk(KERN_INFO "ED[%d]: Endpoint Address: 0x%02X\n", i, endpoint_descriptor->bEndpointAddress);
      printk(KERN_INFO "ED[%d]: bmAttributes: 0x%02X\n", i, endpoint_descriptor->bmAttributes);
      printk(KERN_INFO "ED[%d]: Max Packet Size: 0x%04X (%d)\n", i, endpoint_descriptor->wMaxPacketSize, endpoint_descriptor->wMaxPacketSize);
  }
  device = interface_to_usbdev(interface);
  return 0;
}

static void storage_disconnect(struct usb_interface* interface) {
  printk(KERN_INFO "*** USB Storage is removed ***");
}


static int __init storage_init(void) {
  printk(KERN_INFO "*** USB Storage Init function ***");
  return usb_register(&storage_driver);
}

static void __exit storage_exit(void) {
  usb_deregister(&storage_driver);
}

module_init(storage_init);
module_exit(storage_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("USB Storage Driver");
