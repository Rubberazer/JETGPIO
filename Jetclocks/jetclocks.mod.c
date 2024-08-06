#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x8aa1adfc, "module_layout" },
	{ 0xd9ec2871, "platform_driver_unregister" },
	{ 0x11825f30, "__platform_driver_register" },
	{ 0xb6e6d99d, "clk_disable" },
	{ 0xb077e70a, "clk_unprepare" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x815588a6, "clk_enable" },
	{ 0x76d9b876, "clk_set_rate" },
	{ 0x877cf29f, "_dev_err" },
	{ 0x7c9a7371, "clk_prepare" },
	{ 0xc6a4a872, "__clk_is_enabled" },
	{ 0x556e4390, "clk_get_rate" },
	{ 0x605b25f, "devm_clk_get" },
	{ 0xdcb764ad, "memset" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0x908e5601, "cpu_hwcaps" },
	{ 0x69f38847, "cpu_hwcap_keys" },
	{ 0x14b89635, "arm64_const_caps_ready" },
	{ 0x3ea1b6e4, "__stack_chk_fail" },
	{ 0x2ac7ef9d, "cdev_add" },
	{ 0xee789116, "cdev_init" },
	{ 0x774854c, "device_create" },
	{ 0xab430bfd, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xd60acae0, "devm_kmalloc" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x6c4d42dc, "class_destroy" },
	{ 0xadf38724, "device_destroy" },
	{ 0x2e3a7d93, "cdev_del" },
	{ 0x92997ed8, "_printk" },
	{ 0x4b0a3f52, "gic_nonsecure_priorities" },
	{ 0xfa651c73, "try_module_get" },
	{ 0xc47f82c6, "module_put" },
	{ 0x1fdc7df2, "_mcount" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cnvidia,jetclocks");
MODULE_ALIAS("of:N*T*Cnvidia,jetclocksC*");
