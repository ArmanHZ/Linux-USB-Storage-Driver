#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>


static struct usb_device *device;

// usb_device_id from kernel using command lsusb
static struct usb_device_id storage_table[] = {
  // Vendor id: 058f  Product id: 6387
  { USB_DEVICE(0x058f, 0x6387) },
  {}  // Terminating entry
};

// Probe function: Called when USB is inserted to the PC
// Also prints info about the USB to kernel. (dmesg)
static int storage_probe(struct usb_interface *interface, const struct usb_device_id *id) {
  struct usb_host_interface *host_interface;
  struct usb_endpoint_descriptor *endpoint_descriptor;

  host_interface = interface->cur_altsetting;
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
  printk(KERN_INFO "USB Storage is removed");
}

static struct usb_driver storage_driver = {
  .name = "USB Storage Driver",
  .id_table = storage_table,
  .probe = storage_probe,
  .disconnect = storage_disconnect,
};

static int __init storage_init(void) {
  return usb_register(&storage_driver);
}

static void __exit storage_exit(void) {
  usb_deregister(&storage_driver);
}

module_init(storage_init);
module_exit(storage_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("USB Storage Driver");
