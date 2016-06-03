#include "../../soc/lidbg_target.h"
#include <linux/interrupt.h>
#include <linux/irq.h>

#if GTP_ICS_SLOT_REPORT
#include <linux/input/mt.h>
#endif
#include <uapi/linux/i2c.h>

extern int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num);
extern struct i2c_adapter *i2c_get_adapter(int nr);
extern struct hw_version_specific g_hw_version_specific[];

int READ_I(char *buf, int len)
{

	struct i2c_adapter *adapter = i2c_get_adapter(g_hw_version_specific[0].i2c_bus_ts);
	struct i2c_msg msgs[2];
	s32 ret=-1;
	s32 retries = 0;
	
	//GTP_DEBUG_FUNC();
	
	msgs[0].flags = !I2C_M_RD;
	msgs[0].addr  = g_hw_version_specific[0].i2c_bus_addr;
	msgs[0].len   = g_hw_version_specific[0].i2c_bus_addr_len;
	msgs[0].buf   = &buf[0];
	//msgs[0].scl_rate = 300 * 1000;    // for Rockchip, etc.
	
	msgs[1].flags = I2C_M_RD;
	msgs[1].addr  = g_hw_version_specific[0].i2c_bus_addr;
	msgs[1].len   = len - g_hw_version_specific[0].i2c_bus_addr_len;
	msgs[1].buf   = &buf[g_hw_version_specific[0].i2c_bus_addr_len];
	//msgs[1].scl_rate = 300 * 1000;
	
	while(retries < 5)
	{
	    ret = i2c_transfer(adapter, msgs, 2);
	    if(ret == 2)break;
	    retries++;
	}
	if((retries >= 5))
	{
		return -1;
	}
	return ret;
}
/*s32 public_gtp_i2c_write(u8 *buf, s32 len)
{
	return 0;
}
*/
EXPORT_SYMBOL(READ_I);
