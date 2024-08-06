/* SPDX-License-Identifier: GPL-2.0
 * 
 * jetclocks.c - jetclocks kernel module
 *
 * Based on the NVIDIA MODS kernel driver by NVIDIA CORPORATION. 
 * The jetclocks kernel module is a facility that provides a friendly API
 * for applications in user space to manipulate the clocks in Nvidia
 * Jetson Orin machines.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/reset.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "jetclocks.h"

struct jetclocks {
    struct cdev cdev;
    struct device *dev;
    struct class *cls;
    struct clk *clk;
};

static unsigned major;

static int is_clock_enabled(const char *clock, struct jetclocks *dev)
{
    int ret;
    
    dev->clk = devm_clk_get(dev->dev, clock);
    if (IS_ERR(dev->clk)){
	return PTR_ERR(dev->clk);
    }
    ret = __clk_is_enabled(dev->clk);

    return ret;
}

static int clock_enable(const char *clock,  struct jetclocks *dev)
{
    int ret;
  
    dev->clk = devm_clk_get(dev->dev, clock);
    if (IS_ERR(dev->clk))
	return PTR_ERR(dev->clk);

    ret = clk_prepare_enable(dev->clk);
    if (ret) {
	dev_err(dev->dev, "Clock: %s prepare/enable failed\n", clock);
	return ret;
    }
    
    return 0;
}

static int clock_disable(const char *clock, struct jetclocks *dev)
{
    dev->clk = devm_clk_get(dev->dev, clock);
    if (IS_ERR(dev->clk))
	return PTR_ERR(dev->clk);

    if(__clk_is_enabled(dev->clk))
	clk_disable_unprepare(dev->clk);
    
    return 0;
}

static int clock_set_rate(const char *clock,  struct jetclocks *dev, unsigned long rate)
{
    int ret;
  
    dev->clk = devm_clk_get(dev->dev, clock);
    if (IS_ERR(dev->clk))
	return PTR_ERR(dev->clk);

    ret = clk_set_rate(dev->clk, rate);
    if (ret) {
	dev_err(dev->dev, "Clock: %s set rate failed\n", clock);
	return -EINVAL;
    }
    
    return 0;
}

static unsigned long clock_get_rate(const char *clock,  struct jetclocks *dev)
{
    unsigned long ret;
  
    dev->clk = devm_clk_get(dev->dev, clock);
    if (IS_ERR(dev->clk))
	return PTR_ERR(dev->clk);

    ret = clk_get_rate(dev->clk);
    
    return ret;
}

static int jetclocks_open(struct inode * inode, struct file * filp)
{
    struct jetclocks *jetclocks_dev;

    jetclocks_dev = container_of(inode->i_cdev, struct jetclocks, cdev);
    filp->private_data = jetclocks_dev;

    try_module_get(THIS_MODULE);

    return 0;
}

static int jetclocks_release(struct inode * inode, struct file * filp)
{
    filp->private_data = NULL;
    
    module_put(THIS_MODULE);

    return 0;
}

static long jetclocks_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct jetclocks *jetclocks_dev = filp->private_data;
    struct jetclk clock = {0};
    int ret = 0;
    
    switch(cmd){
    case CLK_ENABLE:
	if(copy_from_user(&clock, (struct jetclk *) arg, sizeof(clock))) {
	    pr_err("jetclocks - Error getting clock name\n");
	    return -EFAULT;
	}
	
	ret = clock_enable(clock.clk, jetclocks_dev);
	if (!(ret)) {
	    pr_info("jetclocks - clock %s enabled\n", clock.clk);
	}
	
	break;
    case CLK_DISABLE:
	if(copy_from_user(&clock, (struct jetclk *) arg, sizeof(clock))) {
	    pr_err("jetclocks - Error getting clock name\n");
	    return -EFAULT;
	}

	ret = clock_disable(clock.clk, jetclocks_dev);
	if (!(ret)) {
	    pr_info("jetclocks - clock %s disabled\n", clock.clk);
	}
	break;
    case CLK_IS_ENABLED:
	if(copy_from_user(&clock, (struct jetclk *) arg, sizeof(clock))) {
	    pr_err("jetclocks - Error getting clock name\n");
	    return -EFAULT;
	}

	clock.clk_enabled = is_clock_enabled(clock.clk, jetclocks_dev);
	if(copy_to_user((struct jetclk *) arg, &clock, sizeof(clock))) { 
	    pr_err("jetclocks - Error sending clock %s status\n", clock.clk);
	    return -EFAULT;
	}
	break;
    case CLK_SET_RATE:
	if(copy_from_user(&clock, (struct jetclk *) arg, sizeof(clock))) {
	    pr_err("jetclocks - Error getting clock name\n");
	    return -EFAULT;
	}
	
	ret = clock_set_rate(clock.clk, jetclocks_dev, clock.clk_set_rate);
	if (!(ret)) {
	    pr_info("jetclocks - clock %s rate set\n", clock.clk);
	}
	break;
    case CLK_GET_RATE:
	if(copy_from_user(&clock, (struct jetclk *) arg, sizeof(clock))) {
	    pr_err("jetclocks - Error getting clock name\n");
	    return -EFAULT;
	}
	
	clock.clk_rate = clock_get_rate(clock.clk, jetclocks_dev);
	if(copy_to_user((struct jetclk *) arg, &clock, sizeof(clock))) { 
	    pr_err("jetclocks - Error sending clock %s rate\n", clock.clk);
	    return -EFAULT;
	}
	break;
    default:
	return -ENOTTY;
    }
    return ret;
} 

struct file_operations jetclocks_fops = {
owner:          THIS_MODULE,
open:           jetclocks_open,
release:        jetclocks_release,
unlocked_ioctl: jetclocks_ioctl,
};

static int jetclocks_probe(struct platform_device *pdev)
{
    struct jetclocks *jetclocks_dev;
    dev_t major_devt;
    struct device *sysdevice;
    int ret = 0;
    
    pr_info("Probing jetclocks\n");
    
    jetclocks_dev = devm_kzalloc(&pdev->dev, sizeof(*jetclocks_dev), GFP_KERNEL);
    if (!jetclocks_dev)
	return -ENOMEM;

    jetclocks_dev->dev = &pdev->dev;
    platform_set_drvdata(pdev, jetclocks_dev);
    
    /* Character device */

    ret = alloc_chrdev_region(&major_devt, 0, 1, "jetclocks");    
    if (ret < 0) {
	pr_err("Device number could not be allocated\n");
	return -EINVAL;
    }

    major = MAJOR(major_devt);

    jetclocks_dev->cls = class_create(THIS_MODULE, "jetclocks");
    if (IS_ERR(jetclocks_dev->cls)) {
	pr_err("Device class can not be created\n");
	unregister_chrdev_region(major_devt, 1);
	return PTR_ERR(jetclocks_dev->cls);
    }

    sysdevice = device_create(jetclocks_dev->cls, NULL, major_devt, NULL, "jetclocks");
    
    if (IS_ERR(sysdevice)) {
	pr_err("Cannot create device file\n");
	class_destroy(jetclocks_dev->cls);	
	unregister_chrdev_region(major_devt, 1);
	return PTR_ERR(sysdevice);
    }
       
    cdev_init(&jetclocks_dev->cdev, &jetclocks_fops);
    
    if (cdev_add(&jetclocks_dev->cdev, major_devt, 1) < 0) {
	pr_err("Registering of device to kernel failed\n");
	device_destroy(jetclocks_dev->cls, major_devt);
	class_destroy(jetclocks_dev->cls);
	unregister_chrdev_region(major_devt, 1);
	return -ENODEV;
    }
    
    pr_info("jetclocks module loaded\n");

    return 0;
}

static int jetclocks_remove(struct platform_device *pdev)
{
    struct jetclocks *jetclocks_dev;
    dev_t major_devt = MKDEV(major, 0);
    int ret = 0;
    
    pr_info("Removing jetclocks\n");
    
    jetclocks_dev = platform_get_drvdata(pdev);
    
    cdev_del(&jetclocks_dev->cdev);
    device_destroy(jetclocks_dev->cls, major_devt);
    class_destroy(jetclocks_dev->cls);
    unregister_chrdev_region(major_devt, 1);
    
    pr_info("jetclocks module unloaded\n");
    
    return ret;
}

static const struct of_device_id jetclocks_of_match[] = {
    { .compatible = "nvidia,jetclocks"},
    { }
};

MODULE_DEVICE_TABLE(of, jetclocks_of_match);

static struct platform_driver jetclocks_driver = {
    .driver = {
        .name = "jetclocks",
        .of_match_table = jetclocks_of_match,
    },
    .probe = jetclocks_probe,
    .remove = jetclocks_remove,
};

module_platform_driver(jetclocks_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Rubberazer <rubberazer@outlook.com>");
MODULE_DESCRIPTION("Jetson Orin clocks from user space");

