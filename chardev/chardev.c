/*
 * Filename:	chardev.c
 *
 * Author:		Oxnz
 * Email:		yunxinyi@gmail.com
 * Created:		2016-09-28 16:09:00 CST
 * Last-update:	2016-09-28 16:09:00 CST
 * Description: anchor
 *
 * Version:		0.0.1
 * Revision:	[NONE]
 * Revision history:	[NONE]
 * Date Author Remarks:	[NONE]
 *
 * License:
 * Copyright (c) 2016 Oxnz
 *
 * Distributed under terms of the [LICENSE] license.
 * [license]
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define DEVNAME "chardev"
#define BUFLEN 80

static int dev_open(struct inode *, struct file *);
static int dev_close(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static int major; // major number assigned to our device driver
static int nopen;
static char msgbuf[BUFLEN];
static char *msgptr;

static struct file_operations fops = {
	.read = dev_read,
	.write = dev_write,
	.open = dev_open,
	.release = dev_close
};

static int __init init_driver(void) {
	major = register_chrdev(0, DEVNAME, &fops);
	if (major < 0) {
		printk(KERN_ALERT "registering chardev driver failed with %d\n", major);
		return major;
	}

	printk(KERN_INFO "chardev driver registered with major number: %d\n", major);

	return 0;
}

static void __exit exit_driver(void) {
	unregister_chrdev(major, DEVNAME);
	printk(KERN_ALERT "unregistering chardev driver\n");
}

static int dev_open(struct inode *inode, struct file *_file) {
	static int counter = 0;
	if (nopen) {
		printk(KERN_ALERT "already opened: nopen = %d\n", nopen);
		return -EBUSY;
	}
	++nopen;
	printk(KERN_INFO "chardev opened: nopen = %d\n", nopen);
	sprintf(msgbuf, "counter = %d\n", counter++);
	msgptr = msgbuf;
	try_module_get(THIS_MODULE);
	return 0;
}

static int dev_close(struct inode *inode, struct file *_file) {
	--nopen;
	module_put(THIS_MODULE);
	printk(KERN_INFO "chardev closed: nopen = %d\n", nopen);
	return 0;
}

static ssize_t dev_read(struct file *filp, char *buffer, size_t len, loff_t *offset) {
	ssize_t nread = 0;
	if (*msgptr == 0)
		return 0;
	while (len && *msgptr) {
		put_user(*(msgptr++), buffer++);
		--len;
		++nread;
	}

	return nread;
}

static ssize_t dev_write(struct file *filp, const char *buf, size_t len, loff_t *offset) {
	printk(KERN_INFO "received %ld bytes msg: %s\n", len, buf);
	return len;
}

module_init(init_driver);
module_exit(exit_driver);

MODULE_LICENSE("MIT");
MODULE_AUTHOR("oxnz");
MODULE_DESCRIPTION("chardev driver");
