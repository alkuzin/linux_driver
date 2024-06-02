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

 /**
 * @file  ldriver.h
 * @brief Contains linux driver macros and functions.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   1.06.2024 
 */

#ifndef _TEST_TASK_LINUX_DRIVER_H_
#define _TEST_TASK_LINUX_DRIVER_H_

#include <linux/types.h>

#define DEVICE_NAME  "test_task_dev"
#define DRIVER_NAME  "linux_driver"
#define DEVICE_CLASS "test_task_dev_class"

/**
 * @brief Driver entry point. 
 * 
 * @return 0 - in case of success. 
 * @return -1 - in case of error. 
 */
static s32 __init linux_driver_init(void);

/** @brief Driver exit handler. */
static void __exit linux_driver_exit(void);

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
 * @return negative number in case of error. 
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

/**
 * @brief Perform device I/O control operation.
 *
 * @param [in] file - given file structure.
 * @param [in] cmd - given command number to perform.
 * @param [in] arg - given argument for the command.
 * @return 0 - in case of success. 
 * @return negative number in case of error. 
 */
static long dev_ioctl(struct file *file, u32 cmd, unsigned long arg);

#endif /* _TEST_TASK_LINUX_DRIVER_H_ */ 