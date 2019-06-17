 
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
/*---# Audio 解码部分------------------------------------------------------------*/
#define ADEC_CHANNEL	0

/*---#对讲部分------------------------------------------------------------*/
#define TALKBACK_FIFO_SIZE		(8*1024)	//对讲缓冲区大小


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
*@ Description    :发送PCM音频数据到AO
*@ Input          :<buf>数据指针
					<size>数据大小
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_send_PCM_stream_to_speacker(void *buf,int size);





/*******************************************************************************
*@ Description    :开启对讲
*@ Input          :<getmode> 0:关闭MIC数据采集模式；1：打开MIC数据采集模式
					<putmode>0:关闭喇叭播放模式； 1：打开喇叭播放模式
*@ Output         :
*@ Return         :
*@ attention      :两个模式至少打开一个（不能同时为0）
					需要先初始化（audio_init）
*******************************************************************************/
int talkback_start(int getmode, int putmode);


/*******************************************************************************
*@ Description    :对讲，获取本地编码数据
*@ Input          :
*@ Output         :<data>数据的首地址(编码数据格式：g711a)
					<size>数据的长度
*@ Return         :
*@ attention      :没有数据时会阻塞
					调用顺序：
					audio_init()
					audio_get_PCM_frame_task()
					AENC_G711_get_frame_task()
					talkback_start()
					talkback_get_data()
*******************************************************************************/
int talkback_get_data(void *data, int *size);


/*******************************************************************************
*@ Description    :对讲，数据播放到speaker
*@ Input          :<data>数据的首地址(编码数据格式：g711a)
					<size>数据的长度
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :	调用顺序：
					audio_init()
					audio_get_PCM_frame_task()
					AENC_G711_get_frame_task()
					talkback_start()
					talkback_put_data()
*******************************************************************************/
int talkback_put_data(void *data, int size);

/*******************************************************************************
*@ Description    :停止对讲
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
int talkback_stop(void); 



/*******************************************************************************
*@ Description    :退出音频编码
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :需要先停止对讲 talkback_stop
*******************************************************************************/
int audio_exit(void);


	 
#ifdef __cplusplus
}
#endif	 
	 
#endif








