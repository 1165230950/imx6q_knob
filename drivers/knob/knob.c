#include "../../soc/lidbg_target.h"
#include "../../com_api/gpio/gpio.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/kfifo.h>
#include <linux/slab.h>

#define FIFO_SIZE (512)

extern int gpio_get_value(unsigned gpio);
extern bool SOC_IO_Input(u32 group, u32 index, u32 pull);
extern bool SOC_IO_ISR_Enable(u32 irq);
extern bool SOC_IO_ISR_Add(u32 irq, u32  interrupt_type, pinterrupt_isr func, void *dev);


extern struct input_dev *input_allocate_device(void);


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


static int k1_val;
static int k2_val;
static int k3_val;
static int k4_val;

static unsigned int count;
static void work_right_knob_k1_fn(struct work_struct *work)
{
	printk("enter k1 work\n");
/*
	if(k1_val)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k1_val);
	}
	if(k1_val == 0)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k1_val);
	}
*/	

}

static void work_right_knob_k2_fn(struct work_struct *work)
{

	printk("enter k2 work\n");
/*

	if(k2_val)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k2_val);
	}
	if(k2_val == 0)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k2_val);
	}
*/

}

static void work_left_knob_k3_fn(struct work_struct *work)
{
	printk("enter k3 work\n");
/*
	if(k3_val)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k3_val);
	}

	if(k3_val == 0)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k3_val);
	}
*/	
}

static void work_left_knob_k4_fn(struct work_struct *work)
{
	printk("enter k4 work\n");
/*
	if(k4_val)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k4_val);
	}
	if(k4_val == 0)
	{
		printk(KERN_CRIT "FUNC:%s LINE:%d val:%d \n",__func__,__LINE__, k4_val);
	}
*/	
}


irqreturn_t irq_right_knob_k1(int irq, void *dev_id)
{
	k1_val = SOC_IO_Input(BUTTON_LEFT_2, BUTTON_LEFT_2, 0);
	if(work_pending(&work_left_knob_k3))
		schedule_work(&work_left_knob_k3);
	
	
	return IRQ_HANDLED;

}
irqreturn_t irq_right_knob_k2(int irq, void *dev_id)
{
	k2_val = SOC_IO_Input(BUTTON_LEFT_1, BUTTON_LEFT_1, 0 /*GPIO_CFG_PULL_UP*/);
	if(work_pending(&work_left_knob_k4))
		schedule_work(&work_left_knob_k4);
	return IRQ_HANDLED;
}




irqreturn_t irq_left_knob_k3(int irq, void *dev_id)
{
	k3_val = SOC_IO_Input(BUTTON_RIGHT_2, BUTTON_RIGHT_2, 0 /*GPIO_CFG_PULL_UP*/);
	if(work_pending(&work_right_knob_k1))
		schedule_work(&work_right_knob_k1);	
	return IRQ_HANDLED;
}
irqreturn_t irq_left_knob_k4(int irq, void *dev_id)
{
	k4_val = SOC_IO_Input(BUTTON_RIGHT_1, BUTTON_RIGHT_1, 0 /*GPIO_CFG_PULL_UP*/);
	if(work_pending(&work_right_knob_k1))
		schedule_work(&work_right_knob_k1);
	return IRQ_HANDLED;
}
void button_init(void)
{
	int ret;
	int button_en = 1;
	char *buffer = (char *)kmalloc(FIFO_SIZE, GFP_KERNEL);
	struct __kfifo knob_fifo;
	__kfifo_init(&knob_fifo,  buffer, FIFO_SIZE, 1);

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

		SOC_IO_ISR_Add(BUTTON_LEFT_1, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, irq_left_knob_k3, NULL);
		SOC_IO_ISR_Add(BUTTON_LEFT_2, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, irq_left_knob_k4, NULL);
		SOC_IO_ISR_Add(BUTTON_RIGHT_1, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, irq_right_knob_k1, NULL);
		SOC_IO_ISR_Add(BUTTON_RIGHT_2, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, irq_right_knob_k2, NULL);
		
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
	printk(KERN_CRIT "hello_knob\n");

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





