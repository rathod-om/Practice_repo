/*
 *
 *
 *      Device Setup
 *      Om Rathod
 *      166857
 *      18-07-2025
 *
 *
 */


#include<linux/module.h>
#include<linux/platform_device.h>
#include "platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

void pcdev_release (struct device *dev){
	pr_info("Device Released\n");
}

/* Create two platform data */
struct pcdev_platform_data pcdev_pdata[5] = {
	[0] = {
		.size = 512,
		.perm = RDWR,
		.serial_number = "PCDEV1111"
	},
	[1] = {
		.size = 1024,
                .perm = RDONLY,
                .serial_number = "PCDEV2222"
	},
	[2] = {
                .size = 512,
                .perm = WRONLY,
                .serial_number = "PCDEV3333"
        },
	[3] = {
                .size = 128,
                .perm = RDWR,
                .serial_number = "PCDEV4444"
        },
	[4] = {
                .size = 256,
                .perm = RDWR,
                .serial_number = "PCDEV5555"
        }
};

/* 1. Create two platform devices */

struct platform_device platform_pcdev_1={
	.name = "pcdev-A1x",
	.id = 0,
	.dev = {
		.platform_data = &pcdev_pdata[0],
		.release = pcdev_release
	}
};

struct platform_device platform_pcdev_2={
        .name = "pcdev-B1x",
	.id = 1,
	.dev = {
                .platform_data = &pcdev_pdata[1],
		.release = pcdev_release
        }

};

struct platform_device platform_pcdev_3={
        .name = "pcdev-C1x",
        .id = 2,
        .dev = {
                .platform_data = &pcdev_pdata[2],
                .release = pcdev_release
        }
};

struct platform_device platform_pcdev_4={
        .name = "pcdev-D1x",
        .id = 3,
        .dev = {
                .platform_data = &pcdev_pdata[3],
                .release = pcdev_release
        }
};

struct platform_device platform_pcdev_5={
        .name = "pcdev-E1x",
        .id = 4,
        .dev = {
                .platform_data = &pcdev_pdata[4],
                .release = pcdev_release
        }
};

struct platform_device *platform_pcdevs[] = 
{
	&platform_pcdev_1,
	&platform_pcdev_2,
	&platform_pcdev_3,
	&platform_pcdev_4,
	&platform_pcdev_5
};

/* Initializing platform */
static int __init pcdev_platform_init(void){
	/* Register platform device */
	/*
	platform_device_register(&platform_pcdev_1);
	platform_device_register(&platform_pcdev_2);
	*/
	
	platform_add_devices(platform_pcdevs,ARRAY_SIZE(platform_pcdevs));



	pr_info("Module inserted\n");
	return 0;
}


static void __exit pcdev_platform_exit(void){
	platform_device_unregister(&platform_pcdev_1);
        platform_device_unregister(&platform_pcdev_2);
        platform_device_unregister(&platform_pcdev_3);
        platform_device_unregister(&platform_pcdev_4);
        platform_device_unregister(&platform_pcdev_5);
	pr_info("Module removed\n");
}

/* Registeration */
module_init(pcdev_platform_init);
module_exit(pcdev_platform_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Pseudo platform device drivers");
