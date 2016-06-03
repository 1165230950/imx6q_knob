#include <lidbg_target.h>

s32 gtp_i2c_read( u8 *buf, s32 len)
{

	struct i2c_adapter *adapter = i2c_get_adapter(int nr)
	struct i2c_msg msgs[2];
	s32 ret=-1;
	s32 retries = 0;
	
	GTP_DEBUG_FUNC();
	
	msgs[0].flags = !I2C_M_RD;
	msgs[0].addr  = client->addr;
	msgs[0].len   = GTP_ADDR_LENGTH;
	msgs[0].buf   = &buf[0];
	//msgs[0].scl_rate = 300 * 1000;    // for Rockchip, etc.
	
	msgs[1].flags = I2C_M_RD;
	msgs[1].addr  = client->addr;
	msgs[1].len   = len - GTP_ADDR_LENGTH;
	msgs[1].buf   = &buf[GTP_ADDR_LENGTH];
	//msgs[1].scl_rate = 300 * 1000;
	
	while(retries < 5)
	{
	    ret = i2c_transfer(client->adapter, msgs, 2);
	    if(ret == 2)break;
	    retries++;
	}
	if((retries >= 5))
	{
	#if GTP_COMPATIBLE_MODE
	    struct goodix_ts_data *ts = i2c_get_clientdata(client);
	#endif
	
	#if GTP_GESTURE_WAKEUP
	    // reset chip would quit doze mode
	    if (DOZE_ENABLED == doze_status)
	    {
	        return ret;
	    }
	#endif
	    GTP_ERROR("I2C Read: 0x%04X, %d bytes failed, errcode: %d! Process reset.", (((u16)(buf[0] << 8)) | buf[1]), len-2, ret);
	#if GTP_COMPATIBLE_MODE
	    if (CHIP_TYPE_GT9F == ts->chip_type)
	    { 
	        gtp_recovery_reset(client);
	    }
	    else
	#endif
	    {
	        gtp_reset_guitar(client, 10);  
	    }
	}
	return ret;
}
s32 gtp_i2c_write(u8 *buf, s32 len)
{
	return 0;
}
