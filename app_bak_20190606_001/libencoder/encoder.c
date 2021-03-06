 
/***************************************************************************
* @file:encoder.c 
* @author:   
* @date:  5,29,2019
* @brief:  编码相关函数
* @attention:
***************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>

#include "imp_isp.h"
#include "imp_system.h"
#include "video.h"
#include "osd.h"
#include "typeport.h"


extern struct chn_conf chn[FS_CHN_NUM];

/*******************************************************************************
*@ Description    :IMP系统初始化
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
IMPSensorInfo sensor_info;
int T20_system_init(void)
{
	int ret = 0;
	memset(&sensor_info, 0, sizeof(IMPSensorInfo));
	memcpy(sensor_info.name, SENSOR_NAME, sizeof(SENSOR_NAME));
	sensor_info.cbus_type = SENSOR_CUBS_TYPE;
	memcpy(sensor_info.i2c.type, SENSOR_NAME, sizeof(SENSOR_NAME));
	sensor_info.i2c.addr = SENSOR_I2C_ADDR;

	DEBUG_LOG("sample_system_init start...\n");

	ret = IMP_ISP_Open();
	if(ret < 0){
		ERROR_LOG("failed to open ISP\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_ISP_AddSensor(&sensor_info);
	if(ret < 0){
		ERROR_LOG( "failed to AddSensor\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_ISP_EnableSensor();
	if(ret < 0){
		ERROR_LOG( "failed to EnableSensor\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_System_Init();
	if(ret < 0){
		ERROR_LOG( "IMP_System_Init failed\n");
		return HLE_RET_ERROR;
	}

	/* enable turning, to debug graphics */
	ret = IMP_ISP_EnableTuning();
	if(ret < 0){
		ERROR_LOG( "IMP_ISP_EnableTuning failed\n");
		return HLE_RET_ERROR;
	}

    ret = IMP_ISP_Tuning_SetSensorFPS(SENSOR_FRAME_RATE_NUM, SENSOR_FRAME_RATE_DEN);
    if (ret < 0){
        ERROR_LOG( "failed to set sensor fps\n");
        return HLE_RET_ERROR;
    }

	DEBUG_LOG("ImpSystemInit success\n");

	return HLE_RET_OK;
}

/*******************************************************************************
*@ Description    :退出IMP系统
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
int T20_system_exit(void)
{
	int ret = 0;

	DEBUG_LOG("sample_system_exit start...\n");

	IMP_System_Exit();

	ret = IMP_ISP_DisableSensor();
	if(ret < 0){
		ERROR_LOG("failed to EnableSensor\n");
		return -1;
	}

	ret = IMP_ISP_DelSensor(&sensor_info);
	if(ret < 0){
		ERROR_LOG("failed to AddSensor\n");
		return -1;
	}

	ret = IMP_ISP_DisableTuning();
	if(ret < 0){
		ERROR_LOG("IMP_ISP_DisableTuning failed\n");
		return -1;
	}

	if(IMP_ISP_Close()){
		ERROR_LOG("failed to open ISP\n");
		return -1;
	}

	DEBUG_LOG( " sample_system_exit success\n");

	return 0;
}




/*******************************************************************************
*@ Description    :编码系统初始化
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :内部初始化的顺序不可随意变动，修改时需要注意
*******************************************************************************/
int encoder_system_init(void)
{
	int ret,i;
	
	ret = T20_system_init();
	if (ret < 0) {
		ERROR_LOG("jpeg init failed\n");
		return -1;
	}
		
	ret = video_init();
	if (ret < 0) {
		ERROR_LOG("jpeg init failed\n");
		return -1;
	}
	

	ret = osd_init();
	if (ret < 0) {
		ERROR_LOG("OSD init failed\n");
		return -1;
	}

	for (i = 0; i <  FS_CHN_NUM; i++)
	{
		/*---#Bind 数据源通道--->OSD叠加--->编码通道------------------------------------------------------------*/
		ret = IMP_System_Bind(&chn[i].framesource_chn, &chn[i].osdcell);
		if (ret < 0) {
			ERROR_LOG("Bind FrameSource channel[%d] and OSD[%d] failed\n",i,i);
			return -1;
		}

		ret = IMP_System_Bind(&chn[i].osdcell, &chn[i].imp_encoder);
		if (ret < 0) {
			ERROR_LOG("Bind OSD[%d] and Encoder[%d] failed! \n",i,i);
			return -1;
		}
	

	}

	
	return 0;
	
}

/*******************************************************************************
*@ Description    :编码系统退出
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :内部退出的先后顺序不能随意改变，修改时需要注意
*******************************************************************************/
int encoder_system_exit(void)
{
	int ret,i;
	for (i = 0; i <  FS_CHN_NUM; i++) 
	{
		/* UnBind */
		ret = IMP_System_UnBind(&chn[i].osdcell, &chn[i].imp_encoder);
		if (ret < 0) {
			ERROR_LOG( "UnBind OSD and Encoder failed\n");
			return HLE_RET_ERROR;
		}

		ret = IMP_System_UnBind(&chn[i].framesource_chn, &chn[i].osdcell);
		if (ret < 0) {
			ERROR_LOG( "UnBind FrameSource and OSD failed\n");
			return HLE_RET_ERROR;
		}
	
	}

	osd_exit();//需要先退出OSD,再退出video(venc)
	video_exit();
	
	T20_system_exit();
	
	return HLE_RET_OK;
	
}

#ifdef __cplusplus
}
#endif





