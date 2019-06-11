/***************************************************************************
* @file:video.h 
* @author:   
* @date:  5,29,2019
* @brief:  视频编码相关函数头文件
* @attention:
***************************************************************************/
#ifndef _VIDEO_H
#define _VIDEO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "imp_framesource.h"

/*---#sensor源帧率（VI）------------------------------------------------------------*/
#define SENSOR_FRAME_RATE_NUM	30	
#define SENSOR_FRAME_RATE_DEN	1

/*---#视频编码通道帧率（VENC）------------------------------------------------------*/
#define VENC_FRAME_RATE_NUM		15
#define VENC_FRAME_RATE_DEN		1

/*---#sensor型号参数----------------------------------------------------------------*/
#define SENSOR_JXF22  			
#if defined SENSOR_JXF22
#define SENSOR_NAME				"sc2235"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR			0x30  //0x40
#define SENSOR_WIDTH			1920  //视频主分辨率（和sensor属性一致）
#define SENSOR_HEIGHT			1080
#define CHN0_EN                 1
#define CHN1_EN                 1
#define CROP_EN					1
#endif

#define SENSOR_WIDTH_SECOND		640 //480  //640	 //视频次分辨率(不能超过max_width = 800 max_height = 800)
#define SENSOR_HEIGHT_SECOND	360 //272  //360

#define NR_FRAMES_TO_SAVE		200
#define STREAM_BUFFER_SIZE		(1 * 1024 * 1024)

#define ENC_H264_CHANNEL		0
#define ENC_JPEG_CHANNEL		1

#define STREAM_FILE_PATH_PREFIX		"/tmp"
#define SNAP_FILE_PATH_PREFIX		"/tmp"

#define OSD_REGION_WIDTH		16
#define OSD_REGION_HEIGHT		34
#define OSD_REGION_WIDTH_SEC	8
#define OSD_REGION_HEIGHT_SEC   18


#define SLEEP_TIME				1

#define FS_CHN_NUM			2  //MIN 1,MAX 2
#define IVS_CHN_ID          1

#define CH0_INDEX  			0	//主通道
#define CH1_INDEX  			1	//次通道
#define CHN_ENABLE 			1
#define CHN_DISABLE			0

/*#define SUPPORT_RGB555LE*/

struct chn_conf{
	unsigned int index;//0 for main channel ,1 for second channel
	unsigned int enable;
	IMPFSChnAttr fs_chn_attr;
	IMPCell framesource_chn;
	IMPCell imp_encoder;
	IMPCell osdcell;
};

#define  CHN_NUM  ARRAY_SIZE(chn)


/*******************************************************************************
*@ Description    :视频编码初始化函数
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int video_init(void);


/*******************************************************************************
*@ Description    :创建线程获取编码视频帧
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int video_get_h264_stream_task(void);

/*******************************************************************************
*@ Description    :抓拍一帧jpeg图像
*@ Input          :<index>对应视频的编码通道[0,1]
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :1.内部会自动换算成视频通道对应的抓拍通道
					2.抓拍一帧jpeg图像后会自动停止接收抓拍图像
*******************************************************************************/
int jpeg_get_one_snap(unsigned int index);


/*******************************************************************************
*@ Description    :退出视频编码
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :1.调用该函数前需要先调用video_stop函数
					2.抓拍jpg图片也会连带退出（因为抓拍和视频编码是绑定在同一通道上的）
*******************************************************************************/
int video_exit(void);


#ifdef __cplusplus
}
#endif	 


#endif






