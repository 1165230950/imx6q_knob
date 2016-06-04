#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>


static int export_api_init(void)
{
	printk(KERN_CRIT "FUNC : %s LINE : %d \n",__func__,__LINE__);
	return 0;
}

static void export_api_exit(void)
{

}


module_init(export_api_init);
module_exit(export_api_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

