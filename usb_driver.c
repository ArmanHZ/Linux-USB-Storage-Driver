
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <asm/errno.h>

int character_dev_setup(void);
void character_dev_cleanup(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file*);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define DEVICE_NAME "storage_driver"
#define BUF_LEN 80

static int MAJOR;
static int DEVICE_OPEN = 0;
static char msg[BUF_LEN];
static char msg2[BUF_LEN];
static char *msg_ptr;
static char *msg_ptr2;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static struct usb_device *device;

// Also prints the device info to dmesg
static int device_probe(struct usb_interface *interface, const struct usb_device_id *id) {
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	int i;
	
	iface_desc = interface->cur_altsetting;
	printk(KERN_INFO "Device interface %d now probed: (%04X:%04X)\n", iface_desc->desc.bInterfaceNumber, id->idVendor, id->idProduct);
	printk(KERN_INFO "ID->bNumEndpoints: %02X\n", iface_desc->desc.bNumEndpoints);
	printk(KERN_INFO "ID->bInterfaceClass: %02X\n", iface_desc->desc.bInterfaceClass);
	
	for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
		endpoint = &iface_desc->endpoint[i].desc;
		printk(KERN_INFO "ED[%d] bEndpointAddress: 0x%02X\n", i, endpoint->bEndpointAddress);
		printk(KERN_INFO "ED[%d] bmAttributes: 0x%02X\n", i, endpoint->bmAttributes);
	}
	device = interface_to_usbdev(interface);
	return 0;
}

static void device_disconnect(struct usb_interface *interface) {
	printk(KERN_INFO "Device interface %d now disconnected\n", interface->cur_altsetting->desc.bInterfaceNumber);
}

static struct usb_device_id device_table[] = {
	{ USB_DEVICE(0x058F, 0x6387) },
	{ }
};

MODULE_DEVICE_TABLE (usb, device_table);

static struct usb_driver device_driver = {
	.name = "My USB Device",
	.probe = device_probe,
	.disconnect = device_disconnect,
	.id_table = device_table
};

void character_dev_cleanup(void) {
	unregister_chrdev(MAJOR, DEVICE_NAME);
}

int character_dev_setup(void) {
	MAJOR = register_chrdev(0, DEVICE_NAME, &fops);
	if (MAJOR < 0) {
		printk(KERN_ALERT "Registering character device failed with %d\n", MAJOR);
		return MAJOR;
	}
	printk(KERN_INFO "Device was assigned Major number %d\n", MAJOR);
	printk(KERN_INFO "Please create a device file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, MAJOR);
	
	return 0;
}

static int device_open(struct inode *inode, struct file *file) {
	static int counter = 0;
	if (DEVICE_OPEN)
		return -EBUSY;
	
	DEVICE_OPEN++;
	sprintf(msg, "Device accessed %d times.\n", counter++);
	printk(KERN_INFO "Current Message buffer: %s", msg2);
	msg_ptr = msg;
	msg_ptr2 = msg2;
	
	try_module_get(THIS_MODULE);
	
	return 0;
}

static int device_release(struct inode *inode, struct file *file) {
	DEVICE_OPEN--;
	module_put(THIS_MODULE);
	
	return 0;
}

static ssize_t device_read(struct file *file, char *buffer, size_t length, loff_t *offset) {
	printk(KERN_INFO "Device Read");
	int bytes_read = 0;
	
	// if end of the message, return 0
	if (*msg_ptr == 0)
		return 0;
	
	while (length && *msg_ptr) {
		// Using put_user to copy data from kernel data segment to the user data segment
		put_user(*(msg_ptr++), buffer++);
		length--;
		bytes_read++;
	}
	
	return bytes_read;
}

/* This function is called when somebody tries to 
 * write into our device file. */
static ssize_t device_write(struct file *file, const char *buffer, size_t length, loff_t *offset) {
	printk(KERN_INFO "Device Write");
	int ret;
	size_t input_buffer_length = strlen(buffer);
	printk ("device_write(%p, %s, %ld)", file, buffer, input_buffer_length);
	ret = copy_from_user(msg2, buffer, length);
	printk(KERN_INFO "Wrote %ld characters to device", input_buffer_length);
	msg_ptr2 = msg2;

	return ret;
}

static int __init device_init(void) {
	int ret;
	character_dev_setup();
	ret = usb_register(&device_driver);
	return ret;
}

static void __exit device_exit(void) {
	character_dev_cleanup();
	usb_deregister(&device_driver);
}

module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("USB Storage Driver");
