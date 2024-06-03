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

#ifndef _TEST_TASK_LINUX_DRIVER_TEST_H_
#define _TEST_TASK_LINUX_DRIVER_TEST_H_

#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

#define DEVICE_NAME "/dev/test_task_dev"
#define BUFFER_SIZE 20

#define IOCTL_BLOCK          0
#define IOCTL_NONBLOCK       1
#define IOCTL_BUFINFO        _IOR('k', 2, dev_buf_info_t)
#define IOCTL_INCORRECT_MODE 99

/**
 * @brief Device buffer information structure.
 * 
 * Contains information about the time the last operation was
 * read and written to the buffer, as well as the identifiers 
 * of the owners and processes that completed these operations.
 */
typedef struct {
    time_t last_read_time;
    time_t last_write_time;
    pid_t  last_read_pid;
    pid_t  last_write_pid;
    uid_t  last_read_owner;
    uid_t  last_write_owner;
} dev_buf_info_t;

/**
 * @brief Set ioctl mode.
 * 
 * @param [in] fd - given device file descriptor.
 * @param [in] mode - given IOCTL mode.
 * @param [out] arg -given IOCTL argument (device buffer info structure).
 */
void set_mode(int fd, int mode, dev_buf_info_t *arg);

/**
 * @brief Display buffer information.
 * 
 * @param [in] buffer_info - given device buffer info structure.
 */
void display_buf_info(dev_buf_info_t info);

#endif /* _TEST_TASK_LINUX_DRIVER_TEST_H_ */ 