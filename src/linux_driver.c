/* linux_driver - Test task: Implementation of Linux driver
 * Copyright (C) 2024  Alexander (@alkuzin)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <linux/moduleparam.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/ktime.h>
#include <linux/wait.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include "../include/linux_driver.h"

/* module info */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexander (@alkuzin)");
MODULE_DESCRIPTION("Character device driver for inter-process communication");

/* define module ring buffer size param */
static s32 buffer_size = 1024;
/* S_IRUGO - read permissions for the owner, group, and others */
module_param(buffer_size, int, S_IRUGO);
MODULE_PARM_DESC(buffer_size, "Ring buffer size");

/* blocking/non-blocking read/write mode flag */
static int is_blocking = 1;

static DECLARE_WAIT_QUEUE_HEAD(read_queue);
static DECLARE_WAIT_QUEUE_HEAD(write_queue);

static char *device_buffer = NULL;
static s32   major_number;
static s32   minor_number;

static struct class *dev_class;
static struct cdev char_device;
static struct device *device;
static dev_t dev_number;

static dev_buf_info_t buffer_info;

/** @brief Set of operations that can be performed on a character device in the kernel. */
static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .open           = dev_open,
    .release        = dev_release,
    .read           = dev_read,
    .write          = dev_write,
    .unlocked_ioctl = dev_ioctl
};


static s32 __init linux_driver_init(void)
{
    s32 ret;

    printk(KERN_INFO DRIVER_NAME ": %s\n", "driver initialization");
    printk(KERN_INFO DRIVER_NAME ": allocating %d bytes for ring buffer\n", buffer_size);
    
    device_buffer = kmalloc(buffer_size, GFP_KERNEL);
    
    if (!device_buffer) {
        printk(KERN_ERR DRIVER_NAME ": %s\n", ": failed memory allocation: out of memory");
        return -ENOMEM;
    }

    /* clear ring buffer */
    memset(device_buffer, 0, buffer_size);

    /* clear device buffer info structure */
    memset(&buffer_info, 0, sizeof(dev_buf_info_t));
    
    printk(KERN_INFO DRIVER_NAME ": %s\n", "successfully allocated ring buffer memory");

    /* GFP_KERNEL means that allocation is performed on behalf of a process running in the kernel space */
    printk(KERN_INFO DRIVER_NAME ": %s\n", "character device initialization");

    /* allocating major number */
    ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    
    if (ret < 0) {
        printk(KERN_ERR DRIVER_NAME ": %s\n", "failed to allocate device number");
        return ret;
    }

    major_number = MAJOR(dev_number);
    minor_number = MINOR(dev_number);

    printk(KERN_INFO DRIVER_NAME ": set device number <major, minor>: <%d, %d>\n", major_number, minor_number);

    /* initializing character device structure */
    cdev_init(&char_device, &fops);

    /* adding character device to the system */
    ret = cdev_add(&char_device, dev_number, 1);
    
    if (ret < 0) {
        cdev_del(&char_device);
        unregister_chrdev_region(dev_number, 1);
        
        printk(KERN_ERR DRIVER_NAME ": %s\n", "failed to add cdev");
        return ret;
    }

    /* creating device and device class */
    dev_class = class_create(DEVICE_CLASS);

    if (!dev_class) {
        printk(KERN_ERR DRIVER_NAME ": %s\n", "failed to create device class");
        return -ENODEV;
    }

    device = device_create(dev_class, NULL, dev_number, NULL, DEVICE_NAME);

    if (!device) {
        printk(KERN_ERR DRIVER_NAME ": %s\n", "failed to create device");
        return -ENODEV;
    }

    printk(KERN_INFO DRIVER_NAME ": initialized character device class \"%s\"\n", DEVICE_CLASS);
    printk(KERN_INFO DRIVER_NAME ": initialized character device \"%s\"\n", DEVICE_NAME);
    return 0;
}

static void __exit linux_driver_exit(void)
{
    kfree(device_buffer);
    printk(KERN_INFO DRIVER_NAME ": %s\n", "ring buffer memory freed successfully");

    device_destroy(dev_class, dev_number);
    class_destroy(dev_class);
    cdev_del(&char_device);
    unregister_chrdev_region(dev_number, 1);

    printk(KERN_INFO DRIVER_NAME ": %s\n", "unregistered character device successfully");
    printk(KERN_INFO DRIVER_NAME ": %s\n", "exit");
}

static s32 dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_DEBUG DRIVER_NAME ": %s\n", "open character device");
    return 0;
}

static s32 dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_DEBUG DRIVER_NAME ": %s\n", "release character device");
    return 0;
}

static ssize_t dev_read(struct file *file, char *buffer, size_t length, loff_t *offset)
{
    char    date_buf[64];
    ssize_t bytes_read;
    ktime_t cur_time;
    struct  tm tm;
    s32     ret;
    
    printk(KERN_DEBUG DRIVER_NAME ": %s\n", "read character device");

    /* handle blocking/non-blocking mode of read operation */
    if (is_blocking)
        wait_event_interruptible(read_queue, device_buffer[0] != '\0');
    else {
        /* handle empty device buffer */
        if (device_buffer[0] == '\0')
            return -EAGAIN;
    }
    
    /* handle the end of the file */
    if (*offset >= buffer_size)
        return 0; 

    bytes_read = umin(length, buffer_size - *offset);
    ret        = copy_to_user(buffer, device_buffer + *offset, bytes_read);

    if (ret) {
        printk(KERN_ERR DRIVER_NAME ": %s\n", "failed to copy data to user space");
        return -EFAULT;
    }

    *offset += bytes_read;
    
    /* clear ring buffer */
    memset(device_buffer, 0, bytes_read);

    cur_time = ktime_get_real_seconds();

    buffer_info.last_read_time  = cur_time;
    buffer_info.last_read_pid   = current->pid;
    buffer_info.last_read_owner = current_uid().val;
    
    /* display last read time, PID & UID */
    time64_to_tm(cur_time, 0, &tm);
    snprintf(date_buf, sizeof(date_buf), DATE_FORMAT, tm.tm_mon + 1, tm.tm_mday, 
             tm.tm_year + 1900, tm.tm_hour + UTC_OFFSET, tm.tm_min, tm.tm_sec);

    printk(KERN_DEBUG DRIVER_NAME ": dev_read: [%s]\n", date_buf);
    printk(KERN_DEBUG DRIVER_NAME ": dev_read: PID: %d\n", buffer_info.last_write_pid);
    printk(KERN_DEBUG DRIVER_NAME ": dev_read: UID: %d\n", buffer_info.last_write_owner);
    
    return bytes_read;
}

static ssize_t dev_write(struct file *file, const char *buffer, size_t length, loff_t *offset)
{
    ktime_t cur_time;
    char    date_buf[64];
    struct  tm tm;
    s32     ret;

    printk(KERN_DEBUG DRIVER_NAME ": %s\n", "write to character device");

    /* handle blocking/non-blocking mode of read operation */
    if (is_blocking)
        wait_event_interruptible(write_queue, device_buffer[0] == '\0');
    else {
        /* handle filled device buffer */
        if (device_buffer[0] != '\0')
            return -EAGAIN;
    }

    /* handle incorrect argument */
    if (length > buffer_size)
        return -EINVAL;
    
    /* handle the end of the file */
    if (*offset >= buffer_size)
        return 0; 
    
    /* null-terminate the device buffer to treat it as a string */
    device_buffer[length] = '\0';

    ret = copy_from_user(device_buffer, buffer, length);

    if (ret) {
        printk(KERN_ERR DRIVER_NAME ": %s\n", "failed to copy data from user space");
        return -EFAULT;
    }

    wake_up_interruptible(&read_queue);
    
    cur_time = ktime_get_real_seconds();
    
    buffer_info.last_write_owner = current_uid().val;
    buffer_info.last_write_pid   = current->pid;
    buffer_info.last_write_time  = cur_time;
    
    /* display last write time, PID & UID */
    time64_to_tm(cur_time, 0, &tm);
    snprintf(date_buf, sizeof(date_buf), DATE_FORMAT, tm.tm_mon + 1, tm.tm_mday, 
             tm.tm_year + 1900, tm.tm_hour + UTC_OFFSET, tm.tm_min, tm.tm_sec);

    printk(KERN_DEBUG DRIVER_NAME ": dev_write: [%s]\n", date_buf);
    printk(KERN_DEBUG DRIVER_NAME ": dev_write: PID: %d\n", buffer_info.last_write_pid);
    printk(KERN_DEBUG DRIVER_NAME ": dev_write: UID: %d\n", buffer_info.last_write_owner);

    return length;
}

static long dev_ioctl(struct file *file, u32 cmd, unsigned long arg)
{
    dev_buf_info_t info;
    s32 ret;
    
    switch (cmd) {
        case IOCTL_BLOCK:
            printk(KERN_INFO DRIVER_NAME ": %s\n", "IOCTL_BLOCK");
            is_blocking = 1;
            printk(KERN_INFO DRIVER_NAME ": %s\n", "blocking mode was enabled");
            break;
        
        case IOCTL_NONBLOCK:
            printk(KERN_INFO DRIVER_NAME ": %s\n", "IOCTL_NONBLOCK");
            is_blocking = 0;
            printk(KERN_INFO DRIVER_NAME ": %s\n", "blocking mode was disabled");
            break;
        
        case IOCTL_BUFINFO:
            printk(KERN_INFO DRIVER_NAME ": %s\n", "IOCTL_BUFINFO");
            
            info = buffer_info;
            ret  = copy_to_user((dev_buf_info_t *)arg, &info, sizeof(dev_buf_info_t));

            if (ret) {
                printk(KERN_ERR DRIVER_NAME ": %s\n", "failed to copy buffer info to user space");
                return -EFAULT;
            }
            
            printk(KERN_INFO DRIVER_NAME ": %s\n", "buffer info was sent");
            break;
    
        default:
            printk(KERN_ERR DRIVER_NAME ": %s\n", "incorrect IOCTL command");
            return -EINVAL;
    }

    return 0;
}

/* Register initialization and exit functions */
module_init(linux_driver_init);
module_exit(linux_driver_exit);