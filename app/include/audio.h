 
/***************************************************************************
* @file:audio.h 
* @author:   
* @date:  5,29,2019
* @brief:  音频编码相关函数头文件
* @attention:
***************************************************************************/
#ifndef _AUDIO_H
#define _AUDIO_H

#ifdef __cplusplus
extern "C"
{
#endif

/*---# Audio 输入输出部分------------------------------------------------------------*/
#define AI_DEVICE_ID 	1		//AI(MIC)设备号(ID)
#define AO_DEVICE_ID	0		//AO(speaker)设备号(ID)

#define AI_CHANNEL_ID 	0		//音频输入通道号(ID)
#define AO_CHANNEL_ID 	0		//音频输出通道号(ID)

/*---# Audio 编码部分------------------------------------------------------------*/
#define AENC_CHANNEL	0



/*******************************************************************************
*@ Description    :音频初始化函数
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_init(void);

/*******************************************************************************
*@ Description    :创建“获取PCM编码帧”线程
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_get_PCM_frame_task(void);

/*******************************************************************************
*@ Description    :创建“获取g711编码帧”线程
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int AENC_G711_get_frame_task(void);


/*******************************************************************************
*@ Description    :退出音频编码
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
int audio_exit(void);


	 
	 
#ifdef __cplusplus
}
#endif	 
	 
#endif






