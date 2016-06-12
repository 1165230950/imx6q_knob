#include "../../soc/lidbg_target.h"
#include "../../com_api/gpio/gpio.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/input.h>
#include <linux/gpio.h>

extern int gpio_get_value(unsigned gpio);
extern bool SOC_IO_Input(u32 group, u32 index, u32 pull);
extern bool SOC_IO_ISR_Enable(u32 irq);
extern bool SOC_IO_ISR_Add(u32 irq, u32  interrupt_type, pinterrupt_isr func, void *dev);


#define CREATE_KTHREAD(func,data)\
	do{\
		struct task_struct *task;\
		printk(KERN_CRIT "create kthread %s\n","_"#func);\
		task = kthread_create(func, data, "_"#func);\
		if(IS_ERR(task))\
		{\
			printk(KERN_CRIT "Unable to start thread.\n");\
		}\
		else wake_up_process(task);\
	}while(0)\



struct work_struct work_right_knob_k1;
struct work_struct work_right_knob_k2;
struct work_struct work_left_knob_k3;
struct work_struct work_left_knob_k4;

struct input_dev *right_knob_dev_k1;
struct input_dev *right_knob_dev_k2;
struct input_dev *left_knob_dev_k3;
struct input_dev *left_knob_dev_k4;

static int k1_val;
static int k2_val;
static int k3_val;
static int k4_val;
static void work_right_knob_k1_fn(struct work_struct *work)
{

	k1_val = gpio_get_value(102);
	k2_val = gpio_get_value(103);
	k3_val = gpio_get_value(104);
	k4_val = gpio_get_value(105);

	if(k1_val)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k1_val);
		/*        if(g_var.recovery_mode == 1)
			  SOC_Key_Report(KEY_UP, KEY_PRESSED_RELEASED);
			  else
			  SOC_Key_Report(KEY_VOLUMEUP, KEY_PRESSED_RELEASED);
		 */
	}
	if(k1_val == 0)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k1_val);
		/*if(g_var.recovery_mode == 1)
		  SOC_Key_Report(KEY_DOWN, KEY_PRESSED_RELEASED);
		  else
		  SOC_Key_Report(KEY_VOLUMEDOWN, KEY_PRESSED_RELEASED);
		 */
	}

}

static void work_right_knob_k2_fn(struct work_struct *work)
{
	k1_val = gpio_get_value(102);
	k2_val = gpio_get_value(103);
	k3_val = gpio_get_value(104);
	k4_val = gpio_get_value(105);

	if(k2_val)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k2_val);
	}
	if(k2_val == 0)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k2_val);
	}

}

static void work_left_knob_k3_fn(struct work_struct *work)
{
	k1_val = gpio_get_value(102);
	k2_val = gpio_get_value(103);
	k3_val = gpio_get_value(104);
	k4_val = gpio_get_value(105);

	if(k3_val)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k3_val);
	}

	if(k3_val == 0)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k3_val);
	}
}

static void work_left_knob_k4_fn(struct work_struct *work)
{
	k1_val = gpio_get_value(102);
	k2_val = gpio_get_value(103);
	k3_val = gpio_get_value(104);
	k4_val = gpio_get_value(105);

	if(k4_val)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k4_val);
	}
	if(k4_val == 0)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k4_val);
	}
}


irqreturn_t irq_right_knob_k1(int irq, void *dev_id)
{
	//lidbg("irq_left_button1: %d\n", irq);
	k1_val = SOC_IO_Input(BUTTON_LEFT_2, BUTTON_LEFT_2, 0);
	if(!work_pending(&work_left_knob_k3))
		schedule_work(&work_left_knob_k3);
	return IRQ_HANDLED;

}
irqreturn_t irq_right_knob_k2(int irq, void *dev_id)
{
	k2_val = SOC_IO_Input(BUTTON_LEFT_1, BUTTON_LEFT_1, 0 /*GPIO_CFG_PULL_UP*/);
	if(!work_pending(&work_left_knob_k4))
		schedule_work(&work_left_knob_k4);
	return IRQ_HANDLED;
}




irqreturn_t irq_left_knob_k3(int irq, void *dev_id)
{
	k3_val = SOC_IO_Input(BUTTON_RIGHT_2, BUTTON_RIGHT_2, 0 /*GPIO_CFG_PULL_UP*/);
	if(!work_pending(&work_right_knob_k1))
		schedule_work(&work_right_knob_k2);
	return IRQ_HANDLED;
}
irqreturn_t irq_left_knob_k4(int irq, void *dev_id)
{
	printk("hello button2\n");
	k4_val = SOC_IO_Input(BUTTON_RIGHT_1, BUTTON_RIGHT_1, 0 /*GPIO_CFG_PULL_UP*/);
	if(!work_pending(&work_right_knob_k2))
		schedule_work(&work_right_knob_k2);
	return IRQ_HANDLED;
}





/*
   int button_suspend(void)
   {
   SOC_IO_ISR_Disable(BUTTON_LEFT_1);
   SOC_IO_ISR_Disable(BUTTON_LEFT_2);
   SOC_IO_ISR_Disable(BUTTON_RIGHT_1);
   SOC_IO_ISR_Disable(BUTTON_RIGHT_2);
   return 0;
   }
   int button_resume(void)
   {

   IO_CONFIG_INPUT(0, BUTTON_LEFT_1);
   IO_CONFIG_INPUT(0, BUTTON_LEFT_2);
   IO_CONFIG_INPUT(0, BUTTON_RIGHT_1);
   IO_CONFIG_INPUT(0, BUTTON_RIGHT_2);

   SOC_IO_ISR_Enable(BUTTON_LEFT_1);
   SOC_IO_ISR_Enable(BUTTON_LEFT_2);
   SOC_IO_ISR_Enable(BUTTON_RIGHT_1);
   SOC_IO_ISR_Enable(BUTTON_RIGHT_2);

   return 0;
   }*/
void button_init(void)
{
	int button_en = 1;

	printk(KERN_CRIT "button_init\n");
	if(button_en)
	{
		INIT_WORK(&work_left_knob_k3, work_left_knob_k3_fn);
		INIT_WORK(&work_left_knob_k4, work_left_knob_k4_fn);

		INIT_WORK(&work_right_knob_k1, work_right_knob_k1_fn);
		INIT_WORK(&work_right_knob_k2, work_right_knob_k2_fn);

		SOC_IO_Input(BUTTON_LEFT_1, BUTTON_LEFT_1, 0);
		SOC_IO_Input(BUTTON_LEFT_2, BUTTON_LEFT_2, 0);
		SOC_IO_Input(BUTTON_RIGHT_1, BUTTON_RIGHT_1, 0);
		SOC_IO_Input(BUTTON_RIGHT_2, BUTTON_RIGHT_2, 0);

		SOC_IO_ISR_Add(BUTTON_LEFT_1, IRQF_TRIGGER_RISING , irq_left_knob_k3, NULL);
		SOC_IO_ISR_Add(BUTTON_LEFT_2, IRQF_TRIGGER_FALLING, irq_left_knob_k4, NULL);
		SOC_IO_ISR_Add(BUTTON_RIGHT_1, IRQF_TRIGGER_RISING, irq_right_knob_k1, NULL);
		SOC_IO_ISR_Add(BUTTON_RIGHT_2, IRQF_TRIGGER_FALLING, irq_right_knob_k2, NULL);
		
	}
}
int thread_button_init(void *data)
{
	button_init();
	return 0;
}








static int __init button_dev_init(void)
{
	printk(KERN_WARNING "chdrv_init\n");
	printk(KERN_CRIT "hello_button\n");

	CREATE_KTHREAD(thread_button_init, NULL);

	return 0;


}

static void __exit button_dev_exit(void)
{
	printk("chdrv_exit\n");



}

//EXPORT_SYMBOL(button_suspend);
//EXPORT_SYMBOL(button_resume);
module_init(button_dev_init);
module_exit(button_dev_exit);



MODULE_AUTHOR("fly, <fly@gmail.com>");
MODULE_DESCRIPTION("Devices Driver");
MODULE_LICENSE("GPL");





