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

#include <time.h>

#include "test.h"

void set_mode(int fd, int mode, dev_buf_info_t *arg)
{
    int ret;

    switch (mode) {
        case IOCTL_BLOCK:
            puts("set mode: IOCTL_BLOCK");
            break;
        
        case IOCTL_NONBLOCK:
            puts("set mode: IOCTL_NONBLOCK");
            break;
        
        case IOCTL_BUFINFO:
            puts("set mode: IOCTL_BUFINFO");
            break;
    
        default:
            printf("incorrect mode: %d\n", mode);
            exit(EXIT_FAILURE);
            break;
    }

    ret = ioctl(fd, mode, arg);

    if (ret < 0) {
        perror("set ioctl mode error");
        exit(EXIT_FAILURE);
    }
}

void display_buf_info(dev_buf_info_t info)
{
    struct tm *last_read_timeinfo, *last_write_timeinfo;
    time_t     last_read_raw_time, last_write_raw_time;

    char last_read_time_str[30];
    char last_write_time_str[30];

    last_read_raw_time  = (time_t)info.last_read_time;
    last_write_raw_time = (time_t)info.last_write_time;
    last_read_timeinfo  = localtime(&last_read_raw_time);
    last_write_timeinfo = localtime(&last_write_raw_time);
    
    strftime(last_read_time_str, sizeof(last_read_time_str), "%d-%m-%Y %H:%M:%S", last_read_timeinfo);    
    printf("last read time:  %s\n", last_read_time_str);
    
    strftime(last_write_time_str, sizeof(last_write_time_str), "%d-%m-%Y %H:%M:%S", last_write_timeinfo);    
    printf("last write time: %s\n", last_write_time_str);
    
    printf("last read PID:        %d\n", info.last_read_pid);
    printf("last write PID:       %d\n", info.last_write_pid);
    printf("last read owner UID:  %d\n", info.last_read_owner);
    printf("last write owner UID: %d\n", info.last_write_owner);
}