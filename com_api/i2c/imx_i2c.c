#include "../../soc/lidbg_target.h"
#include "../../drivers/goodix_ts/gt9xx.h"
#include <linux/interrupt.h>
#include <linux/irq.h>

#if GTP_ICS_SLOT_REPORT
#include <linux/input/mt.h>
#endif
#include <linux/i2c.h>
#include <uapi/linux/i2c.h>

extern struct hw_version_specific g_hw_version_specific[];

int soc_i2c_read(char *buf, int len)
{
	struct i2c_adapter *adapter = i2c_get_adapter(TS_I2C_BUS);
	if(!adapter){
		printk("i2c get adapter failed\n");
		return -1;
	}
	struct i2c_msg msgs[2];
	s32 ret=-1;
	s32 retries = 0;
	
	GTP_DEBUG_FUNC();
	
	msgs[0].flags = !I2C_M_RD;
	msgs[0].addr  = ADDR;
	msgs[0].len   = ADDR_LEN;
	msgs[0].buf   = &buf[0];
	//msgs[0].scl_rate = 300 * 1000;    // for Rockchip, etc.
	
	msgs[1].flags = I2C_M_RD;
	msgs[1].addr  = ADDR;
	msgs[1].len   = len - ADDR_LEN;
	msgs[1].buf   = &buf[ADDR_LEN];
	//msgs[1].scl_rate = 300 * 1000;
	
	while(retries < 5)
	{
	    ret = i2c_transfer(adapter, msgs, 2);
	    if(ret == 2)break;
	    retries++;
	}
	if((retries >= 5))
	{
		GTP_ERROR("I2C Read: 0x%04X, %d bytes failed, errcode: %d! Process reset.", (((u16)(buf[0] << 8)) | buf[1]), len-2, ret);
		return -1;
	}
	return ret;
}
int soc_i2c_write(char *buf, int len)
{
	struct i2c_adapter *adapter = i2c_get_adapter(TS_I2C_BUS);
	if(!adapter){
		printk("i2c get adapter failed\n");
		return -1;
	}
	struct i2c_msg msg;
	s32 ret = -1;
	s32 retries = 0;
	
	GTP_DEBUG_FUNC();
	
	msg.flags = !I2C_M_RD;
	msg.addr  = ADDR;
	msg.len   = len;
	msg.buf   = buf;
	
	while(retries < 5)
	{
	    ret = i2c_transfer(adapter, &msg, 1);
	    if (ret == 1)break;
	    retries++;
	}
	if((retries >= 5))
	{
		GTP_ERROR("I2C Write: 0x%04X, %d bytes failed, errcode: %d! Process reset.", (((u16)(buf[0] << 8)) | buf[1]), len-2, ret);
	}
	return ret;
}

EXPORT_SYMBOL(soc_i2c_read);
EXPORT_SYMBOL(soc_i2c_write);
