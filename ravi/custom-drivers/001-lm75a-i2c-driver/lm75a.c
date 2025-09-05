#include<linux/module.h>
#include<linux/i2c.h>
#include<linux/init.h>



#define LM75A_TEMP_REG 0X00


static int lm75a_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
	int temp_raw;
	s16 temp_val;
	int temp_c, temp_c_frac;

	pr_info("LM75A sensor detected at 0x%x\n", client->addr);
	temp_raw = i2c_smbus_read_word_data(client,LM75A_TEMP_REG);
	if(temp_raw < 0){
		pr_err("Failed to read temperature\n");
		return temp_raw;
	}

	temp_raw = be16_to_cpu((__force __be16)temp_raw);

	temp_val = temp_raw >> 7;
	
	temp_c = temp_val / 2;
    	temp_c_frac = (temp_val % 2) * 5;
	pr_info("Temperature: %d.%d degree celsius\n",temp_c,temp_c_frac);
	return 0;
}

static int lm75a_remove(struct i2c_client *client){
	pr_info("LM75A removed\n");
	return 0;
}

static const struct of_device_id lm75a_of_match[] = {
	{ .compatible = "nxp,lm75a",},
	{ }
};

static const struct i2c_device_id lm75a_id[] = {
        { "lm75a", 0 },
        { }
};

MODULE_DEVICE_TABLE(i2c,lm75a_id);

static struct i2c_driver lm75a_driver = {
        .driver = {
                .name = "lm75a",
		.of_match_table = lm75a_of_match,
        },
        .probe = lm75a_probe,
        .remove = lm75a_remove,
        .id_table = lm75a_id,
};

module_i2c_driver(lm75a_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Om Rathod");
MODULE_DESCRIPTION("LM75A I2C Temperature Sensor Driver");
