<<<<<<< HEAD

/*
 * rtc-max8998.c 
 *
 * PMIC rtc utility driver 
 *
 * Copyright (C) 2009 Samsung Electronics.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/power_supply.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/irq.h>
#include <asm/mach-types.h>
#include <mach/hardware.h>
#include <linux/rtc.h>
#include <linux/bcd.h>


#include <linux/i2c.h>






#define MAX8998_REG_SECOND		0
#define MAX8998_REG_MIN			1	
#define MAX8998_REG_HOUR		2
#define MAX8998_REG_WEEKDAY		3
#define MAX8998_REG_DATE		4	
#define MAX8998_REG_MONTH		5
#define MAX8998_REG_YEAR0		6
#define MAX8998_REG_YEAR1		7


#define MAX8998_RTC_LEN			8


#define MAX8998_RTC_TIME_ADDR		0
#define MAX8998_RTC_ALARMO_ADDR		8
#define MAX8998_RTC_ALARM1_ADDR		8


#define MAX8998RTC_DEBUG    0

#if MAX8998RTC_DEBUG    
#define max8998_dbg	pr_info
#else

#define	max8998_dbg(...)	
#endif


/* Slave address */
#define MAX8998_RTC_SLAVE_ADDR	0x0D

/* Register address */
static struct i2c_driver max8998_rtc_i2c_driver;
static struct i2c_client *max8998_rtc_i2c_client = NULL;

unsigned short max8998_rtc_ignore[] = {I2C_CLIENT_END };
static unsigned short max8998_rtc_normal_i2c[] = { I2C_CLIENT_END };
static unsigned short max8998_rtc_probe[] = {6, MAX8998_RTC_SLAVE_ADDR >> 1, I2C_CLIENT_END };
/*
static struct i2c_client_address_data max8998_rtc_addr_data = {
	.normal_i2c	= max8998_rtc_normal_i2c,
	.ignore		= max8998_rtc_ignore,
	.probe		= max8998_rtc_probe,
};
*/

static int max8998_rtc_i2c_read(struct i2c_client *client, u8 reg, u8 *data, int len)
{
	int ret;
	u8 buf[2];
	struct i2c_msg msg[2];

	max8998_dbg("%s\n", __func__);

	buf[0] = reg; 

	msg[0].addr = client->addr;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = buf;

	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len = len;
	msg[1].buf = data;

	ret = i2c_transfer(client->adapter, msg, 2);
	if (ret != 2) 
		return -EIO;

#if MAX8998RTC_DEBUG
	int i;
	for(i = 0; i < len ; i++)
		max8998_dbg(" %d : %x \n",i,*(data + i));
#endif

	return 0;
}

static int max8998_rtc_i2c_write(struct i2c_client *client, u8 reg, u8 *data,int len)
{
	int ret;
	struct i2c_msg msg[1];

	max8998_dbg("%s\n", __func__);

	data[0] = reg; 

#if MAX8998RTC_DEBUG
	int i;
	for(i = 0; i < len ; i++)
		max8998_dbg(" %d : %x \n",i,*(data + i));
#endif

	msg[0].addr = client->addr;
	msg[0].flags = 0;
	msg[0].len = len;
	msg[0].buf = data;


	ret = i2c_transfer(client->adapter, msg, 1);
	max8998_dbg(" max8998 i2c write ret %d\n",ret);

	if (ret != 1) 
		return -EIO;


	return 0;
}


static int max8998_rtc_i2c_read_time(struct i2c_client *client, struct rtc_time *tm)
{
        int ret;
        u8 regs[MAX8998_RTC_LEN];

        ret = max8998_rtc_i2c_read(client, MAX8998_RTC_TIME_ADDR,regs,MAX8998_RTC_LEN);
        if (ret < 0)
                return ret;

        tm->tm_sec =  bcd2bin(regs[MAX8998_REG_SECOND]);
        tm->tm_min =  bcd2bin(regs [MAX8998_REG_MIN]);
        tm->tm_hour = bcd2bin(regs[MAX8998_REG_HOUR] & 0x3f);
	if(regs[MAX8998_REG_HOUR] & (1 << 7))
        	tm->tm_hour += 12;
        tm->tm_wday = bcd2bin(regs[MAX8998_REG_WEEKDAY]);
        tm->tm_mday = bcd2bin(regs[MAX8998_REG_DATE]);
        tm->tm_mon =  bcd2bin(regs [MAX8998_REG_MONTH]) - 1;
        tm->tm_year = bcd2bin(regs[MAX8998_REG_YEAR0]) +
                      bcd2bin(regs[MAX8998_REG_YEAR1]) * 100 - 1900;


	
        return 0;
}


static int max8998_rtc_i2c_set_time(struct i2c_client *client, struct rtc_time *tm)
{
        int ret;
        u8 regs[MAX8998_RTC_LEN + 1];

	tm->tm_year += 1900;

	regs[MAX8998_REG_SECOND + 1] = bin2bcd(tm->tm_sec);
	regs[MAX8998_REG_MIN + 1] = bin2bcd(tm->tm_min);
	regs[MAX8998_REG_HOUR + 1] = bin2bcd(tm->tm_hour);
	regs[MAX8998_REG_WEEKDAY + 1] = bin2bcd(tm->tm_wday);
	regs[MAX8998_REG_DATE + 1] = bin2bcd(tm->tm_mday);
	regs[MAX8998_REG_MONTH + 1] = bin2bcd(tm->tm_mon + 1);
	regs[MAX8998_REG_YEAR0 + 1] = bin2bcd(tm->tm_year % 100);
	regs[MAX8998_REG_YEAR1 + 1] = bin2bcd((tm->tm_year /100) );

        ret = max8998_rtc_i2c_write(client, MAX8998_RTC_TIME_ADDR,regs,MAX8998_RTC_LEN +1);

	return ret;
}

int max8998_rtc_read_time(struct rtc_time *tm)
{
	int ret;

	max8998_dbg("%s \n", __func__);

	ret = max8998_rtc_i2c_read_time(max8998_rtc_i2c_client, tm);

	max8998_dbg("read %s time %02d.%02d.%02d %02d/%02d/%02d\n",__func__,
		 tm->tm_year, tm->tm_mon, tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec);
	return ret;
}

int max8998_rtc_set_time(struct rtc_time *tm)
{
	max8998_dbg("%s  %02d.%02d.%02d %02d/%02d/%02d\n",__func__,
		 tm->tm_year, tm->tm_mon, tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec);
	return max8998_rtc_i2c_set_time(max8998_rtc_i2c_client, tm);
}

static int
max8998_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENODEV;
	max8998_rtc_i2c_client = client;
	return 0;
}

static int max8998_remove(struct i2c_client *client)
{
	return 0;
}

static struct i2c_device_id max8998_id[] = {
	{ "rtc_max8998", 0 },
	{ }
};

static struct i2c_driver max8998_rtc_i2c_driver = {
	.driver = {
		.name = "rtc_max8998",
		.owner = THIS_MODULE,
	},
	.probe = max8998_probe,
	.remove = max8998_remove,
	.id_table = max8998_id,
};


static int __init s3c_max8998_rtc_init(void)
{
	int ret;
	max8998_dbg("%s\n", __func__);

	if ((ret = i2c_add_driver(&max8998_rtc_i2c_driver)))
		pr_err("%s: Can't add max8998_rtc_ i2c drv\n", __func__);
	return ret;
}

static void __exit s3c_max8998_rtc_exit(void)
{
	max8998_dbg("%s\n", __func__);
	i2c_del_driver(&max8998_rtc_i2c_driver);
}

module_init(s3c_max8998_rtc_init);
module_exit(s3c_max8998_rtc_exit);

MODULE_AUTHOR("samsung");
MODULE_DESCRIPTION("MX 8998 rtc driver");
MODULE_LICENSE("GPL");
=======
/*
 * RTC driver for Maxim MAX8998
 *
 * Copyright (C) 2010 Samsung Electronics Co.Ltd
 * Author: Minkyu Kang <mk7.kang@samsung.com>
 * Author: Joonyoung Shim <jy0922.shim@samsung.com>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/bcd.h>
#include <linux/rtc.h>
#include <linux/platform_device.h>
#include <linux/mfd/max8998.h>
#include <linux/mfd/max8998-private.h>
#include <linux/delay.h>

#define MAX8998_RTC_SEC			0x00
#define MAX8998_RTC_MIN			0x01
#define MAX8998_RTC_HOUR		0x02
#define MAX8998_RTC_WEEKDAY		0x03
#define MAX8998_RTC_DATE		0x04
#define MAX8998_RTC_MONTH		0x05
#define MAX8998_RTC_YEAR1		0x06
#define MAX8998_RTC_YEAR2		0x07
#define MAX8998_ALARM0_SEC		0x08
#define MAX8998_ALARM0_MIN		0x09
#define MAX8998_ALARM0_HOUR		0x0a
#define MAX8998_ALARM0_WEEKDAY		0x0b
#define MAX8998_ALARM0_DATE		0x0c
#define MAX8998_ALARM0_MONTH		0x0d
#define MAX8998_ALARM0_YEAR1		0x0e
#define MAX8998_ALARM0_YEAR2		0x0f
#define MAX8998_ALARM1_SEC		0x10
#define MAX8998_ALARM1_MIN		0x11
#define MAX8998_ALARM1_HOUR		0x12
#define MAX8998_ALARM1_WEEKDAY		0x13
#define MAX8998_ALARM1_DATE		0x14
#define MAX8998_ALARM1_MONTH		0x15
#define MAX8998_ALARM1_YEAR1		0x16
#define MAX8998_ALARM1_YEAR2		0x17
#define MAX8998_ALARM0_CONF		0x18
#define MAX8998_ALARM1_CONF		0x19
#define MAX8998_RTC_STATUS		0x1a
#define MAX8998_WTSR_SMPL_CNTL		0x1b
#define MAX8998_TEST			0x1f

#define HOUR_12				(1 << 7)
#define HOUR_PM				(1 << 5)
#define ALARM0_STATUS			(1 << 1)
#define ALARM1_STATUS			(1 << 2)

enum {
	RTC_SEC = 0,
	RTC_MIN,
	RTC_HOUR,
	RTC_WEEKDAY,
	RTC_DATE,
	RTC_MONTH,
	RTC_YEAR1,
	RTC_YEAR2,
};

struct max8998_rtc_info {
	struct device		*dev;
	struct max8998_dev	*max8998;
	struct i2c_client	*rtc;
	struct rtc_device	*rtc_dev;
	int irq;
	bool lp3974_bug_workaround;
};

static void max8998_data_to_tm(u8 *data, struct rtc_time *tm)
{
	tm->tm_sec = bcd2bin(data[RTC_SEC]);
	tm->tm_min = bcd2bin(data[RTC_MIN]);
	if (data[RTC_HOUR] & HOUR_12) {
		tm->tm_hour = bcd2bin(data[RTC_HOUR] & 0x1f);
		if (data[RTC_HOUR] & HOUR_PM)
			tm->tm_hour += 12;
	} else
		tm->tm_hour = bcd2bin(data[RTC_HOUR] & 0x3f);

	tm->tm_wday = data[RTC_WEEKDAY] & 0x07;
	tm->tm_mday = bcd2bin(data[RTC_DATE]);
	tm->tm_mon = bcd2bin(data[RTC_MONTH]);
	tm->tm_year = bcd2bin(data[RTC_YEAR1]) + bcd2bin(data[RTC_YEAR2]) * 100;
	tm->tm_year -= 1900;
}

static void max8998_tm_to_data(struct rtc_time *tm, u8 *data)
{
	data[RTC_SEC] = bin2bcd(tm->tm_sec);
	data[RTC_MIN] = bin2bcd(tm->tm_min);
	data[RTC_HOUR] = bin2bcd(tm->tm_hour);
	data[RTC_WEEKDAY] = tm->tm_wday;
	data[RTC_DATE] = bin2bcd(tm->tm_mday);
	data[RTC_MONTH] = bin2bcd(tm->tm_mon);
	data[RTC_YEAR1] = bin2bcd(tm->tm_year % 100);
	data[RTC_YEAR2] = bin2bcd((tm->tm_year + 1900) / 100);
}

static int max8998_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	struct max8998_rtc_info *info = dev_get_drvdata(dev);
	u8 data[8];
	int ret;

	ret = max8998_bulk_read(info->rtc, MAX8998_RTC_SEC, 8, data);
	if (ret < 0)
		return ret;

	max8998_data_to_tm(data, tm);

	return rtc_valid_tm(tm);
}

static int max8998_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	struct max8998_rtc_info *info = dev_get_drvdata(dev);
	u8 data[8];
	int ret;

	max8998_tm_to_data(tm, data);

	ret = max8998_bulk_write(info->rtc, MAX8998_RTC_SEC, 8, data);

	if (info->lp3974_bug_workaround)
		msleep(2000);

	return ret;
}

static int max8998_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct max8998_rtc_info *info = dev_get_drvdata(dev);
	u8 data[8];
	u8 val;
	int ret;

	ret = max8998_bulk_read(info->rtc, MAX8998_ALARM0_SEC, 8, data);
	if (ret < 0)
		return ret;

	max8998_data_to_tm(data, &alrm->time);

	ret = max8998_read_reg(info->rtc, MAX8998_ALARM0_CONF, &val);
	if (ret < 0)
		return ret;

	alrm->enabled = !!val;

	ret = max8998_read_reg(info->rtc, MAX8998_RTC_STATUS, &val);
	if (ret < 0)
		return ret;

	if (val & ALARM0_STATUS)
		alrm->pending = 1;
	else
		alrm->pending = 0;

	return 0;
}

static int max8998_rtc_stop_alarm(struct max8998_rtc_info *info)
{
	int ret = max8998_write_reg(info->rtc, MAX8998_ALARM0_CONF, 0);

	if (info->lp3974_bug_workaround)
		msleep(2000);

	return ret;
}

static int max8998_rtc_start_alarm(struct max8998_rtc_info *info)
{
	int ret;
	u8 alarm0_conf = 0x77;

	/* LP3974 with delay bug chips has rtc alarm bugs with "MONTH" field */
	if (info->lp3974_bug_workaround)
		alarm0_conf = 0x57;

	ret = max8998_write_reg(info->rtc, MAX8998_ALARM0_CONF, alarm0_conf);

	if (info->lp3974_bug_workaround)
		msleep(2000);

	return ret;
}

static int max8998_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct max8998_rtc_info *info = dev_get_drvdata(dev);
	u8 data[8];
	int ret;

	max8998_tm_to_data(&alrm->time, data);

	ret = max8998_rtc_stop_alarm(info);
	if (ret < 0)
		return ret;

	ret = max8998_bulk_write(info->rtc, MAX8998_ALARM0_SEC, 8, data);
	if (ret < 0)
		return ret;

	if (info->lp3974_bug_workaround)
		msleep(2000);

	if (alrm->enabled)
		ret = max8998_rtc_start_alarm(info);

	return ret;
}

static int max8998_rtc_alarm_irq_enable(struct device *dev,
					unsigned int enabled)
{
	struct max8998_rtc_info *info = dev_get_drvdata(dev);

	if (enabled)
		return max8998_rtc_start_alarm(info);
	else
		return max8998_rtc_stop_alarm(info);
}

static irqreturn_t max8998_rtc_alarm_irq(int irq, void *data)
{
	struct max8998_rtc_info *info = data;

	rtc_update_irq(info->rtc_dev, 1, RTC_IRQF | RTC_AF);

	return IRQ_HANDLED;
}

static const struct rtc_class_ops max8998_rtc_ops = {
	.read_time = max8998_rtc_read_time,
	.set_time = max8998_rtc_set_time,
	.read_alarm = max8998_rtc_read_alarm,
	.set_alarm = max8998_rtc_set_alarm,
	.alarm_irq_enable = max8998_rtc_alarm_irq_enable,
};

static int __devinit max8998_rtc_probe(struct platform_device *pdev)
{
	struct max8998_dev *max8998 = dev_get_drvdata(pdev->dev.parent);
	struct max8998_platform_data *pdata = dev_get_platdata(max8998->dev);
	struct max8998_rtc_info *info;
	int ret;

	info = kzalloc(sizeof(struct max8998_rtc_info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->dev = &pdev->dev;
	info->max8998 = max8998;
	info->rtc = max8998->rtc;
	info->irq = max8998->irq_base + MAX8998_IRQ_ALARM0;

	platform_set_drvdata(pdev, info);

	info->rtc_dev = rtc_device_register("max8998-rtc", &pdev->dev,
			&max8998_rtc_ops, THIS_MODULE);

	if (IS_ERR(info->rtc_dev)) {
		ret = PTR_ERR(info->rtc_dev);
		dev_err(&pdev->dev, "Failed to register RTC device: %d\n", ret);
		goto out_rtc;
	}

	ret = request_threaded_irq(info->irq, NULL, max8998_rtc_alarm_irq, 0,
			"rtc-alarm0", info);

	if (ret < 0)
		dev_err(&pdev->dev, "Failed to request alarm IRQ: %d: %d\n",
			info->irq, ret);

	dev_info(&pdev->dev, "RTC CHIP NAME: %s\n", pdev->id_entry->name);
	if (pdata->rtc_delay) {
		info->lp3974_bug_workaround = true;
		dev_warn(&pdev->dev, "LP3974 with RTC REGERR option."
				" RTC updates will be extremely slow.\n");
	}

	return 0;

out_rtc:
	platform_set_drvdata(pdev, NULL);
	kfree(info);
	return ret;
}

static int __devexit max8998_rtc_remove(struct platform_device *pdev)
{
	struct max8998_rtc_info *info = platform_get_drvdata(pdev);

	if (info) {
		free_irq(info->irq, info);
		rtc_device_unregister(info->rtc_dev);
		kfree(info);
	}

	return 0;
}

static const struct platform_device_id max8998_rtc_id[] = {
	{ "max8998-rtc", TYPE_MAX8998 },
	{ "lp3974-rtc", TYPE_LP3974 },
	{ }
};

static struct platform_driver max8998_rtc_driver = {
	.driver		= {
		.name	= "max8998-rtc",
		.owner	= THIS_MODULE,
	},
	.probe		= max8998_rtc_probe,
	.remove		= __devexit_p(max8998_rtc_remove),
	.id_table	= max8998_rtc_id,
};

static int __init max8998_rtc_init(void)
{
	return platform_driver_register(&max8998_rtc_driver);
}
module_init(max8998_rtc_init);

static void __exit max8998_rtc_exit(void)
{
	platform_driver_unregister(&max8998_rtc_driver);
}
module_exit(max8998_rtc_exit);

MODULE_AUTHOR("Minkyu Kang <mk7.kang@samsung.com>");
MODULE_AUTHOR("Joonyoung Shim <jy0922.shim@samsung.com>");
MODULE_DESCRIPTION("Maxim MAX8998 RTC driver");
MODULE_LICENSE("GPL");
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
