#include "../lidbg_target.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

struct hw_version_specific g_hw_version_specific[] =
{
	{
		//imx6q v1
		//.gpio_lcd_reset = 0,
		//.gpio_t123_reset = 0,
		//.gpio_dsi83_en = 0,

		//.gpio_usb_id = 0,
		//.gpio_usb_power = 0,
		//.gpio_usb_switch = 0,

		.gpio_int_gps = 1,

		.gpio_int_button_left1 = 102,
		.gpio_int_button_left2 = 103,
		.gpio_int_button_right1 = 104,
		.gpio_int_button_right2 = 105,

		.gpio_led1 = 31,
		//gpio_led2 = 0,

		.gpio_int_mcu_i2c_request = 204,
		.gpio_mcu_i2c_wakeup = 105,
		.gpio_mcu_wp = 9,
		.gpio_mcu_app = 8,
		.gpio_request_fastboot= 106,

		.gpio_ts_int = 110,
		.gpio_ts_rst = 111,


		//.i2c_bus_dsi83 = 0,
		.i2c_bus_bx5b3a = 0,
		.i2c_bus_ts = 0,
		.i2c_bus_gps = 0,
		//.i2c_bus_saf7741 = 0,
		.i2c_bus_tef6638 = 0,
		.i2c_bus_lpc = 2,
		.i2c_bus_addr = 0x14,
		.i2c_bus_addr_len = 2,

		.i2c_bus_fm1388=0,
		.spi_bus_fm1388=0,
		.spi_bus_ymu836=0,
		//.ad_val_mcu = 0,
		//.thermal_ctrl_en = 0,
		.cpu_freq_thermal =
		{
			{0, 0, 0, "0"}, //end flag
		},

		.fly_parameter_node = "/dev/block/mmcblk3p14",
	},


};
EXPORT_SYMBOL(g_hw_version_specific);

static int fly_interface_init(void)
{
	printk(KERN_CRIT " func : %s, LINE : %d\n" ,__func__,__LINE__);
	return 0;
}

static void fly_interface_exit(void)
{
	printk(KERN_CRIT " func : %s, LINE : %d\n" ,__func__,__LINE__);
}


module_init(fly_interface_init);
module_exit(fly_interface_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");



