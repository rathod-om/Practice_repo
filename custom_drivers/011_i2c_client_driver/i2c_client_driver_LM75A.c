#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/i2c.h>
#include <linux/of.h>

#define DRIVER_NAME   "lm75a"
#define DEVICE_NAME   "lm75a_dev"

struct lm75a_data {
        struct i2c_client *client;
        struct cdev cdev;
        dev_t dev;
        struct class *class;
        struct device *device;
};

static int lm75a_open(struct inode *inode, struct file *file)
{
        struct lm75a_data *dev;

        dev = container_of(inode->i_cdev, struct lm75a_data, cdev);
        file->private_data = dev;

        return 0;
}

static ssize_t lm75a_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
        struct lm75a_data *dev = file->private_data;
        u8 temp_reg = 0x00;
        u8 buf[2];
        int ret;
        short raw_temp;
        int temp_c;
        char kbuf[32];
        int len;

        // Write pointer to temp register before read (some devices auto-increment this)
        ret = i2c_master_send(dev->client, &temp_reg, 1);
        if (ret < 0) {
                dev_err(&dev->client->dev, "Failed to write temp register: %d\n", ret);
                return ret;
        }

        // Read 2 bytes from LM75A temp register
        ret = i2c_master_recv(dev->client, buf, 2);
        if (ret < 0) {
                dev_err(&dev->client->dev, "Failed to read temp data: %d\n", ret);
                return ret;
        }

        /*
         * LM75A datasheet:
         * Temperature is 9-bit two's complement stored left-justified in two bytes
         * Bits 15:7 contain temperature data. Shift right 7 bits to get signed value.
         */
        raw_temp = (buf[0] << 8) | buf[1];
        raw_temp >>= 7;

        // Convert raw_temp to milli-degrees Celsius (each bit = 0.5 degrees)
        temp_c = raw_temp * 500;

        len = snprintf(kbuf, sizeof(kbuf), "Temperature: %d.%03d°C\n",
                        temp_c / 1000, abs(temp_c % 1000));

        if (*ppos >= len)
                return 0;

        // Adjust count if user buffer is smaller than remaining data
        if (count > len - *ppos)
                count = len - *ppos;

        if (copy_to_user(user_buf, kbuf + *ppos, count))
                return -EFAULT;

        *ppos += count;
        return count;
}

static int lm75a_release(struct inode *inode, struct file *file)
{
        return 0;
}

static const struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = lm75a_open,
        .read = lm75a_read,
        .release = lm75a_release,
};

static int lm75a_probe(struct i2c_client *client)
{
        int ret;
        struct lm75a_data *dev;

        dev_info(&client->dev, "LM75A: probe called for device at 0x%02x\n", client->addr);

        dev = devm_kzalloc(&client->dev, sizeof(*dev), GFP_KERNEL);
        if (!dev)
                return -ENOMEM;

        dev->client = client;
        i2c_set_clientdata(client, dev);

        // Allocate char device region
        ret = alloc_chrdev_region(&dev->dev, 0, 1, DRIVER_NAME);
        if (ret) {
                dev_err(&client->dev, "Failed to allocate char device region: %d\n", ret);
                return ret;
        }

        dev_info(&client->dev, "Allocated char device region: major=%d, minor=%d\n", 
                 MAJOR(dev->dev), MINOR(dev->dev));

        cdev_init(&dev->cdev, &fops);
        dev->cdev.owner = THIS_MODULE;
        ret = cdev_add(&dev->cdev, dev->dev, 1);
        if (ret) {
                dev_err(&client->dev, "Failed to add char device: %d\n", ret);
                goto unregister_chrdev;
        }

        dev->class = class_create(DRIVER_NAME);
        if (IS_ERR(dev->class)) {
                ret = PTR_ERR(dev->class);
                dev_err(&client->dev, "Failed to create device class: %d\n", ret);
                goto del_cdev;
        }

        dev->device = device_create(dev->class, &client->dev, dev->dev, dev, DEVICE_NAME);
        if (IS_ERR(dev->device)) {
                ret = PTR_ERR(dev->device);
                dev_err(&client->dev, "Failed to create device: %d\n", ret);
                goto destroy_class;
        }

        dev_info(&client->dev, "LM75A driver loaded successfully\n");
        dev_info(&client->dev, "Device created: /dev/%s\n", DEVICE_NAME);
        dev_info(&client->dev, "Class created: /sys/class/%s\n", DRIVER_NAME);
        
        return 0;

destroy_class:
        class_destroy(dev->class);
del_cdev:
        cdev_del(&dev->cdev);
unregister_chrdev:
        unregister_chrdev_region(dev->dev, 1);
        return ret;
}

static void lm75a_remove(struct i2c_client *client)
{
        struct lm75a_data *dev = i2c_get_clientdata(client);

        if (dev) {
                device_destroy(dev->class, dev->dev);
                class_destroy(dev->class);
                cdev_del(&dev->cdev);
                unregister_chrdev_region(dev->dev, 1);
                dev_info(&client->dev, "LM75A driver removed\n");
        }
}

static const struct i2c_device_id lm75a_ids[] = {
        { "lm75a", 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, lm75a_ids);

#ifdef CONFIG_OF
static const struct of_device_id lm75a_of_match[] = {
        { .compatible = "ti,lm75a" },
        { .compatible = "lm75a" },
        { }
};
MODULE_DEVICE_TABLE(of, lm75a_of_match);
#endif

static struct i2c_driver lm75a_driver = {
        .driver = {
                .name = DRIVER_NAME,
                .owner = THIS_MODULE,
#ifdef CONFIG_OF
                .of_match_table = lm75a_of_match,
#endif
        },
        .probe = lm75a_probe,
        .remove = lm75a_remove,
        .id_table = lm75a_ids,
};

static int __init lm75a_init(void)
{
    pr_info("LM75A: module init\n");
    return i2c_add_driver(&lm75a_driver);
}

static void __exit lm75a_exit(void)
{
    pr_info("LM75A: module exit\n");
    i2c_del_driver(&lm75a_driver);
}

module_init(lm75a_init);
module_exit(lm75a_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Om Rathod");
MODULE_DESCRIPTION("I2C Driver for LM75A Temperature Sensor");
MODULE_VERSION("1.0");
