#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define MIN(a,b) (((a) <= (b)) ? (a) : (b))
#define BULK_EP_OUT 0x01
#define BULK_EP_IN 0x82
#define MAX_PKT_SIZE 512

static struct usb_device *device;
static struct usb_class_driver class;
static unsigned char bulk_buf[MAX_PKT_SIZE];

static int storage_open(struct inode *i, struct file *f) {
    return 0;
}

static int storage_close(struct inode *i, struct file *f) {
    return 0;
}

static ssize_t storage_read(struct file *f, char __user *buf, size_t cnt, loff_t *off) {
    int retval;
    int read_cnt;

    /* Read the data from the bulk endpoint */
    retval = usb_bulk_msg(device, usb_rcvbulkpipe(device, BULK_EP_IN), bulk_buf, MAX_PKT_SIZE, &read_cnt, 5000);
    if (retval) {
        printk(KERN_ERR "Bulk message returned %d\n", retval);
        return retval;
    }
    if (copy_to_user(buf, bulk_buf, MIN(cnt, read_cnt))) {
        return -EFAULT;
    }

    return MIN(cnt, read_cnt);
}

static ssize_t storage_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off) {
    int retval;
    int wrote_cnt = MIN(cnt, MAX_PKT_SIZE);

    if (copy_from_user(bulk_buf, buf, MIN(cnt, MAX_PKT_SIZE))) {
        return -EFAULT;
    }

    /* Write the data into the bulk endpoint */
    retval = usb_bulk_msg(device, usb_sndbulkpipe(device, BULK_EP_OUT), bulk_buf, MIN(cnt, MAX_PKT_SIZE), &wrote_cnt, 5000);
    if (retval) {
        printk(KERN_ERR "Bulk message returned %d\n", retval);
        return retval;
    }

    return wrote_cnt;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = storage_open,
    .release = storage_close,
    .read = storage_read,
    .write = storage_write,
};

static int storage_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int retval;

    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;

    iface_desc = interface->cur_altsetting;
    printk(KERN_INFO "Storage USB interface %d now probed: (%04X:%04X)\n", iface_desc->desc.bInterfaceNumber, id->idVendor, id->idProduct);
    printk(KERN_INFO "ID->bNumEndpoints: %02X\n", iface_desc->desc.bNumEndpoints);
    printk(KERN_INFO "ID->bInterfaceClass: %02X\n", iface_desc->desc.bInterfaceClass);

    int i;
    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
        endpoint = &iface_desc->endpoint[i].desc;
        printk(KERN_INFO "ED[%d]->bEndpointAddress: 0x%02X\n", i, endpoint->bEndpointAddress);
        printk(KERN_INFO "ED[%d]->bmAttributes: 0x%02X\n", i, endpoint->bmAttributes);
        printk(KERN_INFO "ED[%d]->wMaxPacketSize: 0x%04X (%d)\n", i, endpoint->wMaxPacketSize, endpoint->wMaxPacketSize);
    }
    device = interface_to_usbdev(interface);
    class.name = "usb/storage_driver%d";
    class.fops = &fops;
    if ((retval = usb_register_dev(interface, &class)) < 0) {
        printk(KERN_ERR "Not able to get a minor for this device.");
    } else {
        printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
    }

    printk(KERN_INFO "*** USB Probed ***");
    return retval;
}

static void storage_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "*** USB Disconnected ***");
    usb_deregister_dev(interface, &class);
}

static struct usb_device_id storage_table[] = {
    { USB_DEVICE(0x058F, 0x6387) },
    {} /* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, storage_table);

static struct usb_driver storage_driver = {
    .name = "storage_driver",
    .probe = storage_probe,
    .disconnect = storage_disconnect,
    .id_table = storage_table,
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
