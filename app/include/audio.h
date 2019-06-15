 
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


#define AI_DEVICE_ID 	1		//AI(MIC)设备号(ID)
#define AO_DEVICE_ID	0		//AO(speaker)设备号(ID)

#define AI_CHANNEL_ID 	0		//音频输入通道号(ID)
#define AO_CHANNEL_ID 	0		//音频输出通道号(ID)



/*******************************************************************************
*@ Description    :音频初始化函数
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_init(void);

/*******************************************************************************
*@ Description    :创建获取音频码流的线程
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_get_PCM_stream_task(void);

/*******************************************************************************
*@ Description    :发送PCM音频数据到AO
*@ Input          :<buf>数据指针
					<size>数据大小
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_send_PCM_stream_to_speacker(void *buf,int size);

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





