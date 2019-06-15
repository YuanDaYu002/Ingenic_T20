 
/***************************************************************************
* @file:encoder.c 
* @author:   
* @date:  5,29,2019
* @brief:  编码相关函数
* @attention:
***************************************************************************/

#include <string.h>

#include "imp_isp.h"
#include "imp_log.h"
#include "imp_system.h"
#include "video.h"
#include "osd.h"
#include "audio.h"
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
#define ENABLE_OSD  1	//使能OSD （总开关）
int encoder_system_init(void)
{
	int ret,i;
	
	ret = T20_system_init();
	if (ret < 0) {
		ERROR_LOG("T20_system_init\n");
		return -1;
	}


	ret = video_init();
	if (ret < 0) {
		ERROR_LOG("video_init\n");
		return -1;
	}
	
#if ENABLE_OSD  //OSD初始化部分
	ret = osd_init();
	if (ret < 0) {
		ERROR_LOG("OSD init failed\n");
		return -1;
	}
#endif 
	
	for (i = 0; i <  FS_CHN_NUM; i++)
	{
	#if ENABLE_OSD  //使能OSD初始化的情况
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
	#else  //DEBUG
		ret = IMP_System_Bind(&chn[i].framesource_chn, &chn[i].imp_encoder);
		if (ret < 0) {
			ERROR_LOG("Bind Encoder[%d] and framesource[%d] failed! \n",i,i);
			return -1;
		}
		
	#endif 
		
	}

	/*---#Audio初始化------------------------------------------------------------*/
	ret = audio_init();
	if(ret < 0) 
	{
		ERROR_LOG("audio_init failed !\n");
		return -1;
	}
	
	return 0;
	
}

/*******************************************************************************
*@ Description    :启动编码系统业务线程
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int encoder_system_start(void)
{
	int ret;
	/*---#创建audio编码接收线程---------------------------------------------------*/
	ret = audio_get_PCM_frame_task();
	if(ret < 0) 
	{
		ERROR_LOG("create audio_get_PCM_frame_task failed !\n");
		return HLE_RET_ERROR;
	}

	ret = AENC_G711_get_frame_task();
	if(ret < 0) 
	{
		ERROR_LOG("create AENC_G711_get_frame_task failed !\n");
		return HLE_RET_ERROR;
	}
	
	/*---#创建OSD实时更新线程-----------------------------------------------------*/
	ret = timestamp_update_task();
	if(ret < 0) 
	{
		ERROR_LOG("create timestamp_update_task failed !\n");
		return HLE_RET_ERROR;
	}

	/*---#创建h264实时编码帧获取线程----------------------------------------------*/
	ret = video_get_h264_frame_task();
	if(ret < 0) 
	{
		ERROR_LOG("create video_get_h264_frame_task failed !\n");
		return HLE_RET_ERROR;
	}

	return HLE_RET_OK;
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
	#if ENABLE_OSD
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
	#else
		ret = IMP_System_UnBind(&chn[i].framesource_chn, &chn[i].imp_encoder);
		if (ret < 0) {
			ERROR_LOG("UnBind Encoder[%d] and framesource[%d] failed! \n",i,i);
			return -1;
		}
		
	#endif
	
	}

	audio_exit();
#if ENABLE_OSD
	osd_exit();//需要先退出OSD,再退出video(venc)
#endif
	video_exit();
	
	
	T20_system_exit();
	
	return HLE_RET_OK;
	
}


















