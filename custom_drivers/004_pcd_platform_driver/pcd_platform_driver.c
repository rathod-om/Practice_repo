/*
 *	
 *	
 *	Platfrom driver
 *	Om Rathod
 *	166857
 *	18-07-2025
 *
 *
 */

#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
#include<linux/platform_device.h>
#include<linux/slab.h>
#include<linux/mod_devicetable.h>
#include "platform.h"


#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt, __func__
/* REQUIRED MACROS */

/* Struct initialization */

/* Device private data struct */
struct pcdev_private_data{
	struct pcdev_platform_data pdata;
	char *buffer;
	dev_t dev_num;
	struct cdev cdev;
};

/* Driver Private data struct */
struct pcdrv_private_data{
	int total_device;
	dev_t device_num_base;
	struct class *class_pcd;
	struct device *device_pcd;	
};

struct pcdrv_private_data pcdrv_data;


struct device_config{
        int config_item1;
        int config_item2;

};

struct device_config pcdev_config[5] = {
        [0] = {.config_item1 = 60, .config_item2 = 21},
        [1] = {.config_item1 = 50, .config_item2 = 22},
        [2] = {.config_item1 = 40, .config_item2 = 23},
        [3] = {.config_item1 = 30, .config_item2 = 24},
        [4] = {.config_item1 = 20, .config_item2 = 25}
};

struct platform_device_id pcdevs_ids[] =
{
        [0] = {.name = "pcdev-A1x",.driver_data = 0},
        [1] = {.name = "pcdev-B1x",.driver_data = 1},
        [2] = {.name = "pcdev-C1x",.driver_data = 2},
        [3] = {.name = "pcdev-D1x",.driver_data = 3},
        [4] = {.name = "pcdev-E1x",.driver_data = 4},
        { }
};

struct platform_driver pcd_platform_driver = {
        .probe = pcd_platform_driver_probe,
        .remove = pcd_platform_driver_remove,
        .id_table = pcdevs_ids,
        .driver = {
                .name  = "pseudo-char-device"
        }
};





int check_permission(int dev_perm,int access_mode){
        if(dev_perm == RDWR ){
                return 0;
        }
        if ((dev_perm == RDONLY) && (access_mode & FMODE_READ) && !(access_mode & FMODE_WRITE) ){
                return 0;
        }
        if ((dev_perm == WRONLY) && (access_mode & FMODE_READ) && !(access_mode & FMODE_WRITE) ){
                return 0;
        }
        return -EPERM;
}





loff_t pcd_lseek(struct file *filp,loff_t off,int whence){
	return 0;
}
ssize_t pcd_read(struct file *filp,char __user *buff,size_t count,loff_t *f_pos){
	return 0;
}
ssize_t pcd_write(struct file *filp,const char __user *buff,size_t count,loff_t *f_pos){
	return -ENOMEM;
}

int pcd_open(struct inode *inode,struct file *filp){
	return 0;
}


int pcd_release(struct inode *inode,struct file *filp){
	pr_info("Release was sucessful\n");

	return 0;
}
struct file_operations pcd_fops = {
        .write = pcd_write,
        .open = pcd_open,
        .read = pcd_read,
        .llseek = pcd_lseek,
        .release = pcd_release,
        .owner = THIS_MODULE
};

/* Gets called when the device is  removed from the system */
int pcd_platform_driver_remove(struct platform_device *pdev){
	
	struct pcdev_private_data *dev_data = dev_get_drvdata(&pdev->dev);


	device_destroy(pcdrv_data.class_pcd,dev_data->dev_num);
	cdev_del(&dev_data->cdev);
	
	pcdrv_data.total_device--;
	pr_info("A device is removed\n");
	return 0;
}

/* Gets called when matched platform device is found */
int pcd_platform_driver_probe(struct platform_device *pdev){
	struct pcdev_private_data *dev_data;
	struct pcdev_platform_data *pdata;
	int ret;
	pr_info("A device is detected\n");
	/* 1. Get the platform data */
	pdata = (struct pcdev_platform_data * ) dev_get_platdata(&pdev->dev);
	if (!pdata){
		pr_info("No platform data available\n");
		ret = -EINVAL;
		goto out;
	}


	/* 2. Dynamically allocate memory for the device private data */
	dev_data = devm_kzalloc(&pdev->dev,sizeof(*dev_data), GFP_KERNEL);
	if (!dev_data){
		pr_info("Cannot allocate Memory\n");
		ret = -ENOMEM;
		goto out;
	}	

	dev_set_drvdata(&pdev->dev,dev_data);
	dev_data->pdata.size = pdata->size;
	dev_data->pdata.perm = pdata->size;
	dev_data->pdata.serial_number = pdata->serial_number;

	pr_info("Device Serial Number = %s\n",dev_data->pdata.serial_number);
	pr_info("Device Size = %d\n",dev_data->pdata.size);
	pr_info("Device Permission = %d\n",dev_data->pdata.perm);
	
	pr_info("config item 1 = %d\n",pcdev_config[pdev->id_entry->driver_data].config_item1);
	pr_info("config item 2 = %d\n",pcdev_config[pdev->id_entry->driver_data].config_item2);
	
	/* 3. Dynamically allocate memory for the device buffer using size information from the platform data */

	dev_data->buffer = devm_kzalloc(&pdev->dev,sizeof(dev_data->pdata.size), GFP_KERNEL);
        if (!(dev_data->buffer)){
                pr_info("Cannot allocate Memory\n");
                ret = -ENOMEM;
                goto dev_data_free;
        }

	/* 4. Get the device number */
	dev_data->dev_num = pcdrv_data.device_num_base +  pdev->id;

	/* 5. Do cdev init and cdev add */
	cdev_init(&dev_data->cdev,&pcd_fops);
	ret = cdev_add(&dev_data->cdev,dev_data->dev_num,1);
	if (ret < 0){
		pr_err("Cdev add failed");
		goto buffer_free;
	}
	/* 6. Create device file for the detected platform device */
	pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd,NULL,dev_data->dev_num,NULL,"pcdev-%d",pdev->id);
	
	if (IS_ERR(pcdrv_data.device_pcd)){
                pr_err("Class Creation is failed\n");
                ret = PTR_ERR(pcdrv_data.device_pcd);
		goto cdev_del;
        }
	pcdrv_data.total_device++;
	pr_info("The Probe was sucessfull\n");

	
	/* 7. Error handling */
	

	return 0;
cdev_del:
	cdev_del(&dev_data->cdev);

buffer_free:
	devm_kfree(&pdev->dev,dev_data->buffer);

dev_data_free:
	devm_kfree(&pdev->dev,dev_data);

out:
	pr_info("Device Probe failed");
	return ret;
}
/*
struct device_config{
	int config_item1;
	int config_item2;

};

struct device_config pcdev_config[5] = {
	[0] = {.config_item1 = 60, .config_item2 = 21},
	[1] = {.config_item1 = 50, .config_item2 = 22},
	[2] = {.config_item1 = 40, .config_item2 = 23},
	[3] = {.config_item1 = 30, .config_item2 = 24},
	[4] = {.config_item1 = 20, .config_item2 = 25}
};

struct platform_device_id pcdevs_ids[] = 
{
	[0] = {.name = "pcdev-A1x",.driver_data = 0},
	[1] = {.name = "pcdev-B1x",.driver_data = 1},
	[2] = {.name = "pcdev-C1x",.driver_data = 2},
	[3] = {.name = "pcdev-D1x",.driver_data = 3},
	[4] = {.name = "pcdev-E1x",.driver_data = 4},
	{ }
};

struct platform_driver pcd_platform_driver = {
	.probe = pcd_platform_driver_probe, 
	.remove = pcd_platform_driver_remove,
	.id_table = pcdevs_ids,
	.driver = {
		.name  = "pseudo-char-device"
	}
};
*/
#define MAX_DEVICES 10
static int __init pcd_init(void){
	
	/* 1. dynamically allocate a device number to MAX_DEVICES */
	int ret;
	ret = alloc_chrdev_region(&pcdrv_data.device_num_base,0,MAX_DEVICES,"pcdevs");
	if(ret < 0 ){
		pr_err("Alloc chrdev failed\n");
		return ret;
	}


	
	
	/* 2. Create device class under /sys/class  */
	pcdrv_data.class_pcd =  class_create("pcd_class");
	if (IS_ERR(pcdrv_data.class_pcd)){
		pr_err("Class Creation is failed\n");
		ret = PTR_ERR(pcdrv_data.class_pcd);
		unregister_chrdev_region(pcdrv_data.device_num_base,MAX_DEVICES);	
	}


	/* 3. Registerv Platform Drivers */

	platform_driver_register(&pcd_platform_driver);

	pr_info("pcd platform driver loaded\n");
	return 0;
}

static void __exit pcd_exit(void){
	
	platform_driver_unregister(&pcd_platform_driver);
	class_destroy(pcdrv_data.class_pcd);
	unregister_chrdev_region(pcdrv_data.device_num_base,MAX_DEVICES);
	
	pr_info("pcd platform driver unloaded\n");
}

module_init(pcd_init);
module_exit(pcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Om Rathod");
MODULE_DESCRIPTION("Platform Driver of kernel module ");



