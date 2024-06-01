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
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/fs.h>


#define DEVICE_NAME "test_task_dev"

/* module info */
MODULE_LICENSE("GPL v3");
MODULE_AUTHOR("Alexander (@alkuzin)");
MODULE_DESCRIPTION("Character device driver for inter-process communication");

/* define module ring buffer size param */
static s32 buffer_size = 1024;
/* S_IRUGO - read permissions for the owner, group, and others */
module_param(buffer_size, int, S_IRUGO);

/**
 * @brief Driver entry point. 
 * 
 * @return 0 - in case of success. 
 * @return -1 - in case of error. 
 */
static s32 __init driver_init(void);

/** @brief Driver exit handler. */
static void __exit driver_exit(void);

/**
 * @brief Open function for character device.
 * 
 * @param [in] inode - given inode structure.
 * @param [in] file - given file structure.
 * @return 0 - in case of success. 
 * @return -1 - in case of error. 
 */
static s32 dev_open(struct inode *inode, struct file *file);

/**
 * @brief Release function for character device.
 * 
 * @param [in] inode - given inode structure.
 * @param [in] file - given file structure.
 * @return 0 - in case of success. 
 * @return -1 - in case of error. 
 */
static s32 dev_release(struct inode *inode, struct file *file);

/**
 * @brief Read data from the character device.
 *
 * @param [in] file - given file structure.
 * @param [out] buffer - given buffer to store the read data.
 * @param [in] length - given number of bytes to read.
 * @param [in] offset - given file offset.
 * @return Number of bytes read on success, or an error code on failure.
 */
static ssize_t dev_read(struct file *file, char *buffer, size_t length, loff_t *offset);

/**
 * @brief Write data to the character device.
 *
 * @param [in] file - given file structure.
 * @param [in] buffer - given buffer of data to be written.
 * @param [in] length - given number of bytes to write.
 * @param [in] offset - given file offset.
 * @return Number of bytes written on success, or an error code on failure.
 */
static ssize_t dev_write(struct file *file, const char *buffer, size_t length, loff_t *offset);

/** @brief Set of operations that can be performed on a character device in the kernel. */
static struct file_operations fops = {
    .open    = dev_open,
    .release = dev_release,
    .read    = dev_read,
    .write   = dev_write
};

static char buffer[buffer_size];
static s32  major_number;


static s32 __init driver_init(void)
{
    printk(KERN_INFO "%s\n", "linux_driver: initialization");

    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number. Status code: %d\n", major_number);
        return major_number;
    }

    printk(KERN_INFO "linux_driver: registered correctly with major number: %d \n", major_number);
    printk(KERN_INFO "linux_driver: set the ring buffer size to %d bytes\n", buffer_size);

    // TODO: implement driver
    return 0;
}

static void __exit driver_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "%s\n", "linux_driver: unregister character device");
    printk(KERN_INFO "%s\n", "linux_driver: exit");
}

/* Register initialization and exit functions */
module_init(driver_init);
module_exit(driver_exit);