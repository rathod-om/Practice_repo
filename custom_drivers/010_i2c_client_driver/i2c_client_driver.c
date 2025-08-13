#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/slab.h>
#include<linux/delay.h>
#include<linux/i2c.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<asm/uaccess.h>


struct i2c_data{
	struct i2c_client *client;
	dev_t dev;
	u8 *buff;
	u16 value;
	struct cdev cdev;
	struct class *class;
};



/* Probe function To invoke the i2c driver */
static int ds3231_probe(struct i2c_client *client,const struct i2c_device_id *id){
	int result;
	struct i2c_data *data;

	pr_info("Probe funtion is invoked\n");
	data = devm_kzalloc(&client->dev,sizeof(struct i2c_data), GFP_KERNEL);	
	data->value = 30;
	data->buff = devm_kzalloc(&client->dev,data->value, GFP_KERNEL);	
	i2c_set_clientdata(client,data);

	result = alloc_chrdev_region(&data->dev,0,1,"i2c_drv");

	if (result < 0){
		pr_alert("Unable to register Device\n");
		unregister_chrdev_region(&data->dev,1);
		return -1;
	}

	pr_info("Major Number = %d",MAJOR(data->dev));

	
	if (data->class = create_class(THIS_MODULE,"i2c_driver") == NULL ){
		pr_info("Unable to create device class\n");
		unregister_chrdev_region(&data->dev,1);
		return -1;
	}

	if (device_create(data->class,NULL,data->dev,NULL,"i2c_drv-%d",0) == NULL){
		pr_info("Unable to create device\n");
		class_destroy(data->class);
                unregister_chrdev_region(&data->dev,1);
                return -1;
	}

	cdev_init(&data->cdev,&fops);

	if(cdev_add(&data->cdev,data->dev,1) == -1){
		pr_info("Unable to add device\n");
		device_destroy(data->class,data->dev);
		class_destroy(data->class);
                unregister_chrdev_region(&data->dev,1);
                return -1;
	}

}


static const struct i2c_device_id i2c_ids[]{
	{"ds3231",0},
	{ }
};

MODULE_DEVICE_TABLE(i2c,i2c_ids);


struct i2c_driver ds3231_i2c_drv{
	.driver = {
		.name = "ds3231",
		.owner = THIS_MODULE,
	},
		.probe = ds3231_probe,
		.remove = ds3231_remove,
		.id_table = i2c_ids,
};




/* Initializing Module */
static int __init i2c_client_drv_init(void)
{
	pr_info("I2C initiated\n");
	return i2c_add_driver(&i2c_data);
}

/* Exit Module */
static void __exit i2c_client_drv_exit(void){
	pr_info("I2C exited\n");
	i2c_del_driver(&i2c_data);
}

module_init(i2c_client_drv_init);
module_exit(i2c_client_drv_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Om Rathod");
MODULE_DESCRIPTION("I2C client driver");
//MODULE_INFO(board,"beagle bone");


