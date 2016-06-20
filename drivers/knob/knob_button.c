#include "../../soc/lidbg_target.h"
#include "../../com_api/gpio/gpio.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kfifo.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#include <uapi/linux/input.h>
#include <linux/bitops.h>


#define knob_kfifo_init(kfifo, buffer, size, esize) __kfifo_init(kfifo, buffer, size, esize)
#define knob_kfifo_in(fifo, buf, len) __kfifo_in(fifo, buf, len)
#define knob_kfifo_out(fifo, buf, len) __kfifo_out(fifo, buf, len)

extern int gpio_get_value(unsigned gpio);
extern bool SOC_IO_Input(u32 group, u32 index, u32 pull);
extern bool SOC_IO_ISR_Enable(u32 irq);
extern bool SOC_IO_ISR_Add(u32 irq, u32  interrupt_type, pinterrupt_isr func, void *dev);
extern void free_irq(unsigned int irq, void * dev_id);


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

static struct work_struct work_left_button1;
static struct work_struct work_left_button2;
static struct work_struct work_right_button1;
static struct work_struct work_right_button2;
static int val;

static dev_t knob_num;
static struct cdev knob_dev;
static struct class *imx6q_knob_class;
static struct device *imx6q_knob_device;
static unsigned int imx6q_knob_major;

#define KNOB_SIZE 8
static struct __kfifo knob_fifo_data;
static char *knob_fifo_buffer;
static char knob_value;
static spinlock_t knob_lock;

static wait_queue_head_t knob_queue;
static struct semaphore knob_sem;
static unsigned int knob_flags = 0;

static struct input_dev *knob_input_dev;

static void work_left_button1_fn(struct work_struct *work)
{
	char knob_data = 0x1<<0;
	spin_lock(&knob_lock);
	if(knob_kfifo_in(&knob_fifo_data, &knob_data, 1) == sizeof(knob_data)) knob_flags ++;
	spin_unlock(&knob_lock);
	wake_up_interruptible(&knob_queue);
//	printk(KERN_CRIT "FUNC:%s LINE:%d right knob tune reverse\n",__func__,__LINE__);

}

static void work_left_button2_fn(struct work_struct *work)
{
	char knob_data = 0x1<<1;
	spin_lock(&knob_lock);
	if(knob_kfifo_in(&knob_fifo_data, &knob_data, 1) == sizeof(knob_data)) knob_flags ++;
	spin_unlock(&knob_lock);
	wake_up_interruptible(&knob_queue);
//		printk(KERN_CRIT "FUNC:%s LINE:%d right knob tune veering\n",__func__,__LINE__);
}

static void work_right_button1_fn(struct work_struct *work)
{
	char knob_data = 0x1<<2;
	spin_lock(&knob_lock);
	if(knob_kfifo_in(&knob_fifo_data, &knob_data, 1) == sizeof(knob_data)) knob_flags ++;
	spin_unlock(&knob_lock);
	wake_up_interruptible(&knob_queue);
//		printk(KERN_CRIT "FUNC:%s LINE:%d left knob vol veering\n",__func__,__LINE__);
}

static void work_right_button2_fn(struct work_struct *work)
{
	char knob_data = 0x1<<3;
	spin_lock(&knob_lock);
	if(knob_kfifo_in(&knob_fifo_data, &knob_data, 1) == sizeof(knob_data)) knob_flags ++;
	spin_unlock(&knob_lock);
	wake_up_interruptible(&knob_queue);
//		printk(KERN_CRIT "FUNC:%s LINE:%d left knob vol reverse\n",__func__,__LINE__);
}


irqreturn_t irq_left_button1(int irq, void *dev_id)
{
	val = SOC_IO_Input(BUTTON_LEFT_2, BUTTON_LEFT_2, 0);
	if(val)
		knob_input_dev->key[BIT_WORD(BTN_0)] = 0;
	else
		knob_input_dev->key[BIT_WORD(BTN_0)] = 1;
	knob_input_dev->keybit[BIT_WORD(BTN_0)] = BIT_MASK(BTN_0);
	input_report_key(knob_input_dev, BTN_0, val);
	input_sync(knob_input_dev);
	
	if(val){
		if(!work_pending(&work_left_button1))
			schedule_work(&work_left_button1);
	}else {
		if(!work_pending(&work_left_button2))
			schedule_work(&work_left_button2);
	}
	return IRQ_HANDLED;

}
irqreturn_t irq_left_button2(int irq, void *dev_id)
{
	val = SOC_IO_Input(BUTTON_LEFT_1, BUTTON_LEFT_1, 0 /*GPIO_CFG_PULL_UP*/);
	if(val)
		knob_input_dev->key[BIT_WORD(BTN_1)] = 0;
	else
		knob_input_dev->key[BIT_WORD(BTN_1)] = 2;
	knob_input_dev->keybit[BIT_WORD(BTN_1)] = BIT_MASK(BTN_1);
	input_report_key(knob_input_dev, BTN_1, val);
	input_sync(knob_input_dev);
	if(val){
		if(!work_pending(&work_left_button1))
			schedule_work(&work_left_button1);
	}else {
		if(!work_pending(&work_left_button2))
			schedule_work(&work_left_button2);
	}
	return IRQ_HANDLED;
}




irqreturn_t irq_right_button1(int irq, void *dev_id)
{
	val = SOC_IO_Input(BUTTON_RIGHT_2, BUTTON_RIGHT_2, 0 /*GPIO_CFG_PULL_UP*/);
	if(val)
		knob_input_dev->key[BIT_WORD(BTN_2)] = 0;
	else
		knob_input_dev->key[BIT_WORD(BTN_2)] = 4;
	knob_input_dev->keybit[BIT_WORD(BTN_2)] = BIT_MASK(BTN_2);
	input_report_key(knob_input_dev, BTN_2, val);
	input_sync(knob_input_dev);

	if(val){
		if(!work_pending(&work_right_button1))
			schedule_work(&work_right_button1);
	}else {
		if(!work_pending(&work_right_button2))
			schedule_work(&work_right_button2);
	}
	return IRQ_HANDLED;
}
irqreturn_t irq_right_button2(int irq, void *dev_id)
{
	val = SOC_IO_Input(BUTTON_RIGHT_1, BUTTON_RIGHT_1, 0 /*GPIO_CFG_PULL_UP*/);
	if(val)
		knob_input_dev->key[BIT_WORD(BTN_3)] = 0;
	else
		knob_input_dev->key[BIT_WORD(BTN_3)] = 8;
	knob_input_dev->keybit[BIT_WORD(BTN_3)] = BIT_MASK(BTN_3);
	input_report_key(knob_input_dev, BTN_3, val);
	input_sync(knob_input_dev);
	if(val){
		if(!work_pending(&work_right_button1))
			schedule_work(&work_right_button1);
	}else{
		if(!work_pending(&work_right_button2))
			schedule_work(&work_right_button2);
	}
	
	return IRQ_HANDLED;
}

static long knob_ioctl(struct file * filep, unsigned int cmd, unsigned long arg)
{
	int ret;
	int *value = (int *)arg;
	if(wait_event_interruptible(knob_queue, knob_flags != 0)){
		printk("wait failed\n");
		return -1;
	}
	spin_lock(&knob_lock);
	if(ret = knob_kfifo_out(&knob_fifo_data, &knob_value, 1) == sizeof(knob_value)) knob_flags --;
	spin_unlock(&knob_lock);
	if(ret != sizeof(knob_value)){
		return -1;
	}
	*value = ((int)knob_value)&0xff;
	knob_value = 0x0;
	return 0;
}

static int knob_probe(void)
{
	int button_en = 1, ret;

	printk(KERN_CRIT "button_init\n");

	knob_fifo_buffer = (char *)kmalloc(KNOB_SIZE, GFP_KERNEL);
	if(!knob_fifo_buffer){
		printk("fifo buffer kmalloc failed\n");
		return -1;
	}
	ret = knob_kfifo_init(&knob_fifo_data, knob_fifo_buffer, KNOB_SIZE, 1);
	if(ret < 0){
		printk("kfifo init failed ret = %d\n", ret);
		return ret;
	}
	spin_lock_init(&knob_lock);
	sema_init(&knob_sem, 1);
	init_waitqueue_head(&knob_queue);
	
	if(button_en)
	{
		printk("init work\n");
		INIT_WORK(&work_left_button1, work_left_button1_fn);
		INIT_WORK(&work_left_button2, work_left_button2_fn);

		INIT_WORK(&work_right_button1, work_right_button1_fn);
		INIT_WORK(&work_right_button2, work_right_button2_fn);
		printk("request gpio\n");
		SOC_IO_Input(BUTTON_LEFT_1, BUTTON_LEFT_1, 0);
		SOC_IO_Input(BUTTON_LEFT_2, BUTTON_LEFT_2, 0);
		SOC_IO_Input(BUTTON_RIGHT_1, BUTTON_RIGHT_1, 0);
		SOC_IO_Input(BUTTON_RIGHT_2, BUTTON_RIGHT_2, 0);
		printk("request irq\n");
		SOC_IO_ISR_Add(BUTTON_LEFT_1, IRQF_TRIGGER_RISING, irq_left_button1, "hdj");
		SOC_IO_ISR_Add(BUTTON_LEFT_2, IRQF_TRIGGER_FALLING, irq_left_button2, "hdj");
		SOC_IO_ISR_Add(BUTTON_RIGHT_1, IRQF_TRIGGER_RISING, irq_right_button1, "hdj");
		SOC_IO_ISR_Add(BUTTON_RIGHT_2, IRQF_TRIGGER_FALLING, irq_right_button2, "hdj");
		
	}
}

static struct file_operations knob_fops = {
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= knob_ioctl,
};

static int __init button_dev_init(void)
{
	printk(KERN_WARNING "chdrv_init\n");
	printk(KERN_CRIT "hello_button\n");

	int ret;
	printk(KERN_WARNING "chdrv_init\n");
	printk(KERN_CRIT "hello_knob\n");
	knob_probe();
	printk("startk register device\n");
	ret = alloc_chrdev_region(&knob_num, imx6q_knob_major, 1, "knob_device");
	if(ret < 0){
		printk("can not register region\n");
		return ret;
	}
	cdev_init(&knob_dev, &knob_fops);
	ret = cdev_add(&knob_dev, knob_num, 1);
	if(ret < 0){
		printk("cdev add failed\n");
		goto err_cdev_add;
	}
	imx6q_knob_class = class_create(THIS_MODULE, "knob_class");
	if(imx6q_knob_class == NULL){
		printk("knob class create failed\n");
		ret = -EBUSY;
		goto err_class_create;
	}
	imx6q_knob_device = device_create(imx6q_knob_class, NULL, knob_num, NULL, "knob_drv");
	if(imx6q_knob_device == NULL){
		printk("knob device cread failed\n");
		ret = -EBUSY;
		goto err_device_create;
	}
	printk("knob driver init ok !\n");
	printk("register input subsystem\n");
	knob_input_dev = input_allocate_device();
	if(!knob_input_dev){
		printk("knob_input_dev allocate failed\n");
		ret = -1;
		goto err_device_create;
	}
	knob_input_dev->evbit[BIT_WORD(EV_KEY)] = BIT_MASK(EV_KEY);
	if(input_register_device(knob_input_dev))
	{
		printk("knob input device register failed\n");
		ret = -1;
		goto err_input_dev;
	}
	printk("input subsystem register ok !\n");

	return 0;
err_input_dev:
	input_free_device(knob_input_dev);

err_device_create:
	class_destroy(imx6q_knob_class);
	
err_class_create:
	cdev_del(&knob_dev);
	
err_cdev_add:
	unregister_chrdev_region(knob_num, 1);
	return ret;

}

static void __exit button_dev_exit(void)
{
	printk("chdrv_exit\n");
	device_destroy(imx6q_knob_class, knob_num);
	class_destroy(imx6q_knob_class);
	cdev_del(&knob_dev);
	unregister_chrdev_region(knob_num, 1);
/*	
	disable_irq_nosync(262);
	disable_irq_nosync(263);
	disable_irq_nosync(264);
	disable_irq_nosync(265);
*/
	free_irq(262, "hdj");
	free_irq(263, "hdj");
	free_irq(264, "hdj");
	free_irq(265, "hdj");
	
	gpio_free(BUTTON_LEFT_1);
	gpio_free(BUTTON_LEFT_2);
	gpio_free(BUTTON_RIGHT_1);
	gpio_free(BUTTON_RIGHT_2);
}

//EXPORT_SYMBOL(button_suspend);
//EXPORT_SYMBOL(button_resume);
module_init(button_dev_init);
module_exit(button_dev_exit);



MODULE_AUTHOR("fly, <fly@gmail.com>");
MODULE_DESCRIPTION("Devices Driver");
MODULE_LICENSE("GPL");





