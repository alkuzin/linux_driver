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

/* TEST WRITER */

#include <string.h>

#include "test.h"

/** @brief Display list of available commands. */
static void help(void);


int main(int argc, char **argv)
{
    char buffer[BUFFER_SIZE] = "Message from writer";
    dev_buf_info_t info;
    int fd, ret;
     
    /* handle incorrect number of arguments */
    if (argc > 2) {
        help();
        exit(EXIT_FAILURE);
    }

    fd = open(DEVICE_NAME, O_RDWR);
    
    if (fd == -1) {
        perror("Error opening " DEVICE_NAME);
        exit(EXIT_FAILURE);
    }
    
    /* select mode (IOCTL_BLOCK by default) */
    if (argc == 2) {
        if (strncmp(argv[1], "--nonblock", 11) == 0)
            set_mode(fd, IOCTL_NONBLOCK, &info);
        else if (strncmp(argv[1], "--block", 8) == 0)
            set_mode(fd, IOCTL_BLOCK, &info);
        else if (strncmp(argv[1], "--incorrect", 12) == 0)
            set_mode(fd, IOCTL_INCORRECT_MODE, &info);
        else {
            help();
            exit(EXIT_FAILURE);
        }
    }

    printf("writer: writing buffer: \"%s\"\n", buffer);
    ret = write(fd, buffer, BUFFER_SIZE);
    
    if (ret == -1) {
        perror("write error");
        exit(EXIT_FAILURE);
    }
    
    set_mode(fd, IOCTL_BUFINFO, &info);
    display_buf_info(info);

    close(fd);
    return 0;
}

static void help(void)
{
    puts("Usage: \t./writer [argument]\n"
    "\n\t--nonblock  \t - test non-blocking mode of read/write operations.\n"
    "\n\t--block     \t - test blocking mode of read/write operations (default).\n"
    "\n\t--incorrect \t - test incorrect mode of read/write operations.\n");
}