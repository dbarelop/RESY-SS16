#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xc7dcf939, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x4b85b0c7, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xfe1f32b5, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x7eddedb3, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x615ee1ed, __VMLINUX_SYMBOL_STR(kobject_put) },
	{ 0x5c082778, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x656198fb, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x4a8b0931, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xffd55fba, __VMLINUX_SYMBOL_STR(cdev_alloc) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0xfbc74f64, __VMLINUX_SYMBOL_STR(__copy_from_user) },
	{ 0x126440e4, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0x92bf91ad, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x8e360574, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x531e5bd2, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "A862A5683D96BC23E0DD3E0");
