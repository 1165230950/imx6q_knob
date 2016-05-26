#ifndef __GPIO_H__
#define __GPIO_H__

#include <linux/interrupt.h>

typedef irqreturn_t (*pinterrupt_isr)(int irq,void *dev_id);


struct io_int_config
{
	__u32 ext_int_num;
	unsigned long irqflags;
	pinterrupt_isr pisr;
	void *dev;
};


struct io_status
{
	u32 gpio;
	u32 direction;
	u32 pull;
	bool force_reconfig;
	bool out_mod;
	u32 suspend_mod;
};

enum
{
	GPIO_CFG_INPUT,
	GPIO_CFG_OUTPUT,
};

enum
{
	GPIO_CFG_NO_PULL,
	GPIO_CFG_PULL_UP,
	GPIO_CFG_PULL_DOWN,
};

#endif

