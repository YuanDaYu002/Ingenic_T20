 /***************************************************************************
* @file:main.c 
* @author:   
* @date:  5,29,2019
* @brief:  工程总入口文件
* @attention:
***************************************************************************/
#include <stdio.h>
#include <unistd.h>

#include "video.h"
#include "osd.h"
#include "encoder.h"


int main(int argc,char*argv[])
{
	int ret;
	ret = encoder_system_init();
	if(ret < 0) return -1;
	
	/*---#创建OSD实时更新线程-----------------------------------------------------*/
#if 1
	ret = timestamp_update_task();
	if(ret < 0) goto ERR;
#endif 
	/*---#创建h264实时编码帧获取线程----------------------------------------------*/
	ret = video_get_h264_stream_task();
	if(ret < 0) goto ERR;
	/*---#获取抓拍图像------------------------------------------------------------*/
#if 1
	sleep(2); //开机后不要太快抓拍，图像还没稳定下来，可能抓到黑屏
	ret = jpeg_get_one_snap(0);
	if(ret < 0) goto ERR;
	
	ret = jpeg_get_one_snap(1);
	if(ret < 0) goto ERR;
#endif

	int i= 30;//延迟多少秒自动退出
	while(i-- > 0)
	{
		sleep(2);
	}
		

ERR:
	encoder_system_exit();
	return 0;
}

















