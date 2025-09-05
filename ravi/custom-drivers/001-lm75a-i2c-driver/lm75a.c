#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>

#define LM75A_TEMP_REG 0x00

/* Read temperature in millidegree Celsius */
static int lm75a_read_temp(struct device *dev,
                enum hwmon_sensor_types type,
                u32 attr, int channel,
                long *val)
{
        struct i2c_client *client;
        int temp_raw;
        s16 temp_val;

        if (type != hwmon_temp || attr != hwmon_temp_input)
                return -EOPNOTSUPP;

        /* Retrieve the i2c_client we stored in probe */
        client = dev_get_drvdata(dev);
        if (!client)
                return -EINVAL;

        /* Perform I²C read */
        temp_raw = i2c_smbus_read_word_data(client, LM75A_TEMP_REG);
        if (temp_raw < 0) {
                dev_err(dev, "LM75A: Failed to read temperature register\n");
                return temp_raw;
        }

        /* LM75A: 9-bit left-justified, bits [15:7] */
        temp_raw = be16_to_cpu((__force __be16)temp_raw);
        temp_val = temp_raw >> 7;

        *val = temp_val * 500;  /* 0.5°C per LSB, report in millidegree */

        pr_info("LM75A read callback: %ld m°C\n", *val);

        return 0;
}

static umode_t lm75a_is_visible(const void *data,
                enum hwmon_sensor_types type,
                u32 attr, int channel)
{
        if (type == hwmon_temp && attr == hwmon_temp_input)
                return 0444;
        return 0;
}

static const struct hwmon_channel_info *lm75a_info[] = {
        HWMON_CHANNEL_INFO(temp, HWMON_T_INPUT),
        NULL
};

static const struct hwmon_ops lm75a_hwmon_ops = {
        .is_visible = lm75a_is_visible,
        .read = lm75a_read_temp,
};

static const struct hwmon_chip_info lm75a_chip_info = {
        .ops = &lm75a_hwmon_ops,
        .info = lm75a_info,
};

static int lm75a_probe(struct i2c_client *client,
                const struct i2c_device_id *id)
{
        struct device *hwmon_dev;

        hwmon_dev = devm_hwmon_device_register_with_info(&client->dev,
                        "lm75a",
                        client,   /* stored as drvdata */
                        &lm75a_chip_info,
                        NULL);
        if (IS_ERR(hwmon_dev)) {
                dev_err(&client->dev, "Failed to register hwmon device\n");
                return PTR_ERR(hwmon_dev);
        }

        dev_info(&client->dev, "LM75A sensor detected at 0x%x\n", client->addr);
        return 0;
}

static int lm75a_remove(struct i2c_client *client)
{
        pr_info("LM75A removed\n");
        return 0;
}

static const struct of_device_id lm75a_of_match[] = {
        { .compatible = "lm75a" },
	{ }
};
MODULE_DEVICE_TABLE(of, lm75a_of_match);

static const struct i2c_device_id lm75a_id[] = {
        { "lm75a", 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, lm75a_id);

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
MODULE_DESCRIPTION("LM75A I2C Temperature Sensor Driver with hwmon interface");

