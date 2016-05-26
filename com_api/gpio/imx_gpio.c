#include "gpio.h"
#include <linux/gpio.h>
#include <linux/interrupt.h>

#define IO_LOG_NUM 200
#define GPIO_TO_INT(x) gpio_to_irq(x)

static struct io_status io_config[IO_LOG_NUM];

void soc_irq_enable(unsigned int irq)
{
    enable_irq(irq);
}

void soc_irq_disable(unsigned int irq)
{
    disable_irq(irq);

}

int soc_io_irq(struct io_int_config *pio_int_config)
{
    if (request_irq(pio_int_config->ext_int_num, pio_int_config->pisr, pio_int_config->irqflags , "lidbg_irq", pio_int_config->dev ))
    {
        printk(KERN_CRIT "request_irq err!\n");
        return 0;
    }
    return 1;
}

int soc_io_output(u32 group, u32 index, bool status)
{
    gpio_direction_output(index, status);
    gpio_set_value(index, status);
    io_config[index].out_mod = status;
    return 1;
}

bool soc_io_input( u32 index)
{
    gpio_direction_input(index);
    return gpio_get_value(index);
}

int soc_io_suspend(void)
{
    int i;
    for( i = 0; i < IO_LOG_NUM; i++)
        if(io_config[i].gpio != 0)
        {
            gpio_direction_input(io_config[i].gpio);
        }
    return 0;
}

int soc_io_resume(void)
{
    int i;
    for(i = 0; i  < IO_LOG_NUM; i++)
        if(io_config[i].gpio != 0)
        {
            if(io_config[i].direction == GPIO_CFG_OUTPUT)
                soc_io_output(0, io_config[i].gpio, io_config[i].out_mod);
            else
            {
                gpio_direction_input(io_config[i].gpio);
            }
        }
    return 0;
}


void soc_io_init(void)
{
    memset(io_config, 0, sizeof(struct io_status)*IO_LOG_NUM);
}


int soc_io_suspend_config(u32 index, u32 direction, u32 pull)
{
    return -1;
}

int soc_io_config(u32 index, u32 direction, u32 pull, bool force_reconfig)
{
    bool is_first_init = 0;
    is_first_init = (io_config[index].gpio == 0) ? 1 : 0;
    if(force_reconfig == 1)
        printk(KERN_CRIT "soc_io_config:force_reconfig %d\n" , index);

    if(!is_first_init && (force_reconfig == 0))
    {
        return 1;
    }
    else
    {
        int err;

        if (!gpio_is_valid(index))
            return 0;


        printk(KERN_CRIT "gpio_request:index %d\n" , index);

        err = gpio_request(index, "lidbg_io");
        if (err)
        {
            printk(KERN_CRIT "\n\nerr: gpio request failed1!!!!!!\n\n\n");
            gpio_free(index);
            err = gpio_request(index, "lidbg_io");
            printk(KERN_CRIT "\n\nerr: gpio request failed2!!!!!!\n\n\n");
        }


        if(direction == GPIO_CFG_INPUT)
        {
            err = gpio_direction_input(index);
            if (err)
            {
                printk(KERN_CRIT "gpio_direction_set failed\n");
                goto free_gpio;
            }
        }
        io_config[index].gpio = index;
        io_config[index].direction = direction;
        io_config[index].pull = pull;
        io_config[index].suspend_mod = 0;
        return 1;

free_gpio:
        if (gpio_is_valid(index))
            gpio_free(index);
        return 0;
    }
}




//再封装一层做irq处理
bool SOC_IO_ISR_Add(u32 irq, u32  interrupt_type, pinterrupt_isr func, void *dev)
{
    bool ret = 0;
    struct io_int_config io_int_config1;

    io_int_config1.ext_int_num = GPIO_TO_INT(irq);
    io_int_config1.irqflags = interrupt_type;
    io_int_config1.pisr = func;
    io_int_config1.dev = dev;

    printk(KERN_CRIT "ext_int_num:%d \n", irq);

    ret =  soc_io_irq(&io_int_config1);
    return ret;
}
EXPORT_SYMBOL(SOC_IO_ISR_Add);

bool SOC_IO_Input(u32 group, u32 index, u32 pull)
{
    soc_io_config( index,  GPIO_CFG_INPUT, pull/*GPIO_CFG_NO_PULL*/, 0);
    return soc_io_input(index);
}
EXPORT_SYMBOL(SOC_IO_Input);

bool SOC_IO_ISR_Enable(u32 irq)
{
    soc_irq_enable(GPIO_TO_INT(irq));
    return 1;
}
EXPORT_SYMBOL(SOC_IO_ISR_Enable);


