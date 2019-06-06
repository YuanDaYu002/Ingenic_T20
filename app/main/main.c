 /***************************************************************************
* @file:main.c 
* @author:   
* @date:  5,29,2019
* @brief:  工程总入口文件
* @attention:
***************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <unistd.h>

#include "video.h"
#include "osd.h"
#include "encoder.h"


int main(int argc,char*argv[])
{
	encoder_system_init();
	
	/*---#创建OSD实时更新线程-----------------------------------------------------*/
	timestamp_update_task();
	/*---#创建h264实时编码帧获取线程----------------------------------------------*/
	video_get_h264_stream_task();
	/*---#获取抓拍图像------------------------------------------------------------*/
	jpeg_get_one_snap(0);
	jpeg_get_one_snap(1);

	sleep(2);

	encoder_system_exit();
	return 0;
}

#ifdef __cplusplus
}
#endif





