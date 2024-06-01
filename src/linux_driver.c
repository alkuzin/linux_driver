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

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>


/* module info */
MODULE_LICENSE("GPL v3");
MODULE_AUTHOR("Alexander (@alkuzin)");
MODULE_DESCRIPTION("Character device driver for inter-process communication");

/**
 * @brief Driver entry point. 
 * 
 * @return 0 - in case of success. 
 * @return -1 - in case of error. 
 */
static s32 driver_init(void);

/** @brief Driver exit handler. */
static void driver_exit(void);


static s32 driver_init(void)
{
    printk(KERN_INFO "%s\n", "linux_driver: initialization");
    // TODO: implement driver
    return 0;
}

static void driver_exit(void)
{
    printk(KERN_INFO "%s\n", "linux_driver: exit");
}

/* Register initialization and exit functions */
module_init(driver_init);
module_exit(driver_exit);