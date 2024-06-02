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

/* TEST READER */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define DEVICE_NAME "/dev/test_task_dev"
#define BUFFER_SIZE 100

int main()
{
    char buffer[BUFFER_SIZE];
    int  fd;

    fd = open(DEVICE_NAME, O_RDWR);
    
    if (fd == -1) {
        perror("Error opening " DEVICE_NAME);
        exit(EXIT_FAILURE);
    }

    puts("reader: waiting for writer");
    read(fd, buffer, BUFFER_SIZE);
    printf("reader: read buffer: \"%s\"\n", buffer);

    close(fd);
    return 0;
}
