/*
 *	
 *	I2C device driver for client device DS3231
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "i2c_drv"
#define CLASS_NAME "i2c_driver"


static int my_release(struct inode *, struct file *);
struct i2c_data {
	struct i2c_client *client;
	dev_t dev;
	u8 *buff;
	u16 value;
	struct cdev cdev;
	struct class *class;
};

static ssize_t my_read(struct file *filp,char *buff,size_t count,loff_t *offset){
	struct i2c_data *dev = (struct i2c_data *) filp->private_data;
        struct i2c_adapter *adap = dev->client->adapter;
        struct i2c_msg msg;
        char *temp;
        int ret;

	temp = kmalloc(count,GFP_KERNEL);
	
	msg.addr = 0x68;
        msg.flags = 0; 
	msg.flags|= I2C_M_RD;
	msg.len = count;
        msg.buf = temp;

        ret = i2c_transfer(adap,&msg,1);
        if (ret >= 0 ){
		ret = copy_to_user(buff,temp,count)? -EFAULT: count;
	}
	
	kfree(temp);

        return ret;
	

}



static ssize_t my_write(struct file *filp,const char *buff,size_t count,loff_t *offset){
	struct i2c_data *dev = (struct i2c_data *) filp->private_data;
	struct i2c_adapter *adap = dev->client->adapter;
	struct i2c_msg msg;
	char *temp;
	int ret;

	temp = memdup_user(buff,count);
	
	msg.addr = 0x68;
	msg.flags = 0;
	msg.len = count;
	msg.buf = temp;

	ret = i2c_transfer(adap,&msg,1);
	kfree(temp);

	return (ret == 1?count:ret);

}

static int my_open(struct inode *inode,struct file *filp){
	struct i2c_data *dev= container_of(inode->i_cdev,struct i2c_data,cdev);
       	
	if (dev == NULL){
		pr_info("There is no data\n");
		return -ENOMEM;
	}
	filp->private_data = dev;

	return 0;
}

int my_release(struct inode *inode,struct file *filp){
        pr_info("closed sucessfully\n");
        return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.read = my_read,
	.write = my_write,
	.release = my_release
};


/* Probe function */
static int ds3231_probe(struct i2c_client *client/*, const struct i2c_device_id *id*/)
{
	int result;
	struct i2c_data *data;

	pr_info("ds3231: probe called\n");

	data = devm_kzalloc(&client->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->client = client;
	data->value = 30;

	data->buff = devm_kzalloc(&client->dev, data->value, GFP_KERNEL);
	if (!data->buff)
		return -ENOMEM;

	i2c_set_clientdata(client, data);

	result = alloc_chrdev_region(&data->dev, 0, 1, DEVICE_NAME);
	if (result < 0) {
		pr_err("Failed to allocate chrdev region\n");
		return result;
	}

	pr_info("ds3231: major = %d, minor = %d\n", MAJOR(data->dev), MINOR(data->dev));

	cdev_init(&data->cdev, &fops);
        data->cdev.owner = THIS_MODULE;

        if (cdev_add(&data->cdev, data->dev, 1) < 0) {
                device_destroy(data->class, data->dev);
                class_destroy(data->class);
                unregister_chrdev_region(data->dev, 1);
                return -1;
        }

	data->class = class_create(CLASS_NAME);
	if (IS_ERR(data->class)) {
		unregister_chrdev_region(data->dev, 1);
		return PTR_ERR(data->class);
	}

	if (IS_ERR(device_create(data->class, NULL, data->dev, NULL, DEVICE_NAME))) {
		class_destroy(data->class);
		unregister_chrdev_region(data->dev, 1);
		return -1;
	}
/*
	cdev_init(&data->cdev, &fops);
	data->cdev.owner = THIS_MODULE;

	if (cdev_add(&data->cdev, data->dev, 1) < 0) {
		device_destroy(data->class, data->dev);
		class_destroy(data->class);
		unregister_chrdev_region(data->dev, 1);
		return -1;
	}
*/
	return 0;
}

/* Remove function */
static void ds3231_remove(struct i2c_client *client)
{
	struct i2c_data *data = i2c_get_clientdata(client);

	cdev_del(&data->cdev);
	device_destroy(data->class, data->dev);
	class_destroy(data->class);
	unregister_chrdev_region(data->dev, 1);

	pr_info("ds3231: removed\n");
}

static const struct i2c_device_id i2c_ids[] = {
	{ "ds3231", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, i2c_ids);

static struct i2c_driver ds3231_i2c_drv = {
	.driver = {
		.name = "ds3231",
		.owner = THIS_MODULE,
	},
	.probe = ds3231_probe,
	.remove = ds3231_remove,
	.id_table = i2c_ids,
};

static int __init i2c_client_drv_init(void)
{
	pr_info("ds3231: I2C driver init\n");
	return i2c_add_driver(&ds3231_i2c_drv);
}

static void __exit i2c_client_drv_exit(void)
{
	pr_info("ds3231: I2C driver exit\n");
	i2c_del_driver(&ds3231_i2c_drv);
}

module_init(i2c_client_drv_init);
module_exit(i2c_client_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Om Rathod");
MODULE_DESCRIPTION("I2C Client Driver for DS3231 RTC");

