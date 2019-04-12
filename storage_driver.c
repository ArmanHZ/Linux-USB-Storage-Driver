#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

// usb_device_id from kernel using command lsusb
static struct usb_device_id storage_table[] = {
  // Vendor id: 058f  Product id: 6387
  { USB_DEVICE(0x058f, 0x6387) },
  {}  // Terminating entry
};

// Probe function: Called when USB is inserted to the PC
static int storage_probe(struct usb_interface *interface, const struct usb_device_id *id) {
  printk(KERN_INFO "[*] Project Storage (%04X:%04X) is plugged", id->idVendor, id->idProduct);
  return 0;
}

static void storage_disconnect(struct usb_interface* interface) {
  printk(KERN_INFO "[*] Project Storage is removed");
}

static struct usb_driver storage_driver = {
  .name = "Project Storage Driver",
  .id_table = storage_table,
  .probe = storage_probe,
  .disconnect = storage_disconnect,
};

static int __init storage_init(void) {
  int returnValue = -1;
  printk(KERN_INFO "[*] Project Storage constructor");
  returnValue = usb_register(&storage_driver);
  printk(KERN_INFO "Registration Complete");
  return returnValue;
}

static void __exit storage_exit(void) {
  printk(KERN_INFO "[*] Project Storage Disconnecting");
  usb_deregister(&storage_driver);
  printk(KERN_INFO "Disconnected!");
}

module_init(storage_init);
module_exit(storage_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("USB Storage Driver");
