 
/***************************************************************************
* @file: rtsp_main.c
* @author:   
* @date:  7,3,2019
* @brief:  rtsp 主业务文件
* @attention:
***************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "rtsp_stream.h"
#include "typeport.h"
#include "encoder.h"
#include "CircularBuffer.h"

/*******************************************************************************
*@ Description    :rtsp的服务端线程
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :该线程是rtsp的服务线程，死循环，一旦开启，不会自动退出的
*******************************************************************************/
void* live555MediaServer_task(void*args)
{
	printf("==========RTSP SERVER START SUCCESS !!===============\n");
	pthread_detach(pthread_self());
	
	system("/system/bin/live555MediaServer"); //该命令会阻塞，是死循环
	
	printf("==========RTSP SERVER EXIT SUCCESS !!===============\n");
	pthread_exit(0);
	return NULL;
}

/*******************************************************************************
*@ Description    :rtsp 主业务入口函数
*@ Input          :
*@ Output         :
*@ Return         :成功：死循环 ；失败返回-1;
*@ attention      :不可重复调用！！！
*******************************************************************************/
void* rtsp_main(void*args)
{
	pthread_detach(pthread_self());
	printf("****** start rtsp server**********************************************\n");

	int ret = 0;
	unsigned int skip_len = 0;
    unsigned int frame_len = 0;
	FRAME_HDR *header  = NULL;	//帧头指针
	char* frame_data = NULL;	//帧(整体)数据指针
	FrameInfo_t  frame_info;

	unsigned int A_count = 0;
    unsigned int V_count = 0;

	struct timeval audio_time_start = {0};
	struct timeval video_time_start = {0};
	struct timeval audio_time_current = {0};
	struct timeval video_time_current = {0};


	/*---#rtsp初始化------------------------------------------------------------*/
	CRTSPStream_init();

	/*---#启动rtsp服务程序------------------------------------------------------*/
	pthread_t tid;
	if (pthread_create(&tid, NULL, live555MediaServer_task, NULL) < 0) 
	{
		ERROR_LOG( "Create live555MediaServer_task failed!\n");
		pthread_exit(0);
	}

	
	// system("/system/bin/live555MediaServer"); //这种方法调用会阻塞在这，错误

	//CRTSPStream_SendH264File("E:\\测试视频\\test.264");

	//--从循环缓冲区获取数据帧--------------------------------------
	CircularBuffer_t* buffer_handle = CircularBufferGetHandle(IMAGE_SIZE_1920x1080);
	if(NULL == buffer_handle)
	{
		ERROR_LOG("CircularBufferGetHandle failed !\n");
		return NULL;
	}

	int user_id = CircularBufferRequestUserID(buffer_handle);
	if(user_id < 0)
	{
		ERROR_LOG("CircularBufferRequestUserID failed !\n");
		return NULL;
	}
	
WAIT_START:
	if(CRTSPStream_start_send()) //阻塞函数
	{
		printf("[rtsp]:=======start send frame========\n");
	}
	
	while(1)  //保证第一帧是视频关键帧
    {
        printf("[rtsp]:search to IDR frame...\n");
        CircularBufferReadOneFrame(user_id,buffer_handle,(void*)&frame_data,&frame_info);
        header = (FRAME_HDR *) frame_data;
        if(header->type != 0xF8)//寻找IDR帧
        {
            printf("[rtsp]:continue find IDR frame...");
            continue;
        }
        else
        {
            printf("[rtsp]:IDR frame finded*******!\n");
            break;
        }       
    }

	
	gettimeofday(&audio_time_start,NULL);
	gettimeofday(&video_time_start,NULL);
	
	while(1) 
   {
				
        //找到IDR帧，开始录制
        gettimeofday(&audio_time_current,NULL);
        if(audio_time_current.tv_sec - audio_time_start.tv_sec >=1)//1s 打印一次
        {
            printf("[rtsp]:count Aframe = %d\n",A_count);
            memcpy(&audio_time_start,&audio_time_current,sizeof(struct timeval));
            A_count = 0;
        }
		
        gettimeofday(&video_time_current,NULL);
        if(video_time_current.tv_sec - video_time_start.tv_sec >=1)//1s 打印一次
        {
            printf("[rtsp]:count Vframe = %d\n",V_count);
            memcpy(&video_time_start,&video_time_current,sizeof(struct timeval));
            V_count = 0;
        }
        
         if (header->type == 0xFA)//0xFA-音频帧
        {
            A_count ++;
            #if 0
                skip_len = sizeof (FRAME_HDR) + sizeof (AFRAME_INFO);
                frame_len = frame_info.frmLength - skip_len; 
                AFRAME_INFO * A_info = (AFRAME_INFO*)(frame_data + sizeof(FRAME_HDR));
                // printf("[rtsp]:Audio pts : %lld\n",A_info->pts_msec);
                //暂时不支持发送audio帧
            
            #endif

        }
        else if(header->type == 0xF8)   //0xF8-视频关键帧 
        {
            V_count ++;
            printf("[rtsp]:I frame ========\n");
            skip_len = sizeof (FRAME_HDR) + sizeof (IFRAME_INFO);
            frame_len = frame_info.frmLength - skip_len;    
            //IFRAME_INFO * V_info = (IFRAME_INFO*)(frame_data + sizeof(FRAME_HDR));
        	
			ret = CRTSPStream_SendH264Data((unsigned char*)frame_data + skip_len,frame_len);
        }
        else if(header->type == 0xF9)//0xF9-视频非关键帧
        {
            V_count ++;
            //printf("[rtsp]:P frame\n");
            skip_len = sizeof (FRAME_HDR) + sizeof (PFRAME_INFO);
            frame_len = frame_info.frmLength - skip_len;    
           // PFRAME_INFO * V_info = (PFRAME_INFO*)(frame_data + sizeof(FRAME_HDR));
        
			ret = CRTSPStream_SendH264Data((unsigned char*)frame_data + skip_len,frame_len);
        }
        else
        {
            ERROR_LOG("[rtsp]:unknown frame type!\n");
			//usleep(1000*100);
        }

		if(2 == ret)//rtsp客户端关闭
		{
			ERROR_LOG("remotr rtsp client call close!\n");
			goto WAIT_START;
		}
		else if(ret <= 0)//失败
		{
			ERROR_LOG("CRTSPStream_SendH264Data failed!\n");
		}

        CircularBufferReadOneFrame(user_id,buffer_handle,(void*)&frame_data,&frame_info);
        header = (FRAME_HDR *)frame_data;

            
    }
	 
	
	printf("****** rtsp server exit **********************************************\n");
	
	CircularBufferFreeUserID(buffer_handle,user_id);
	CRTSPStream_exit();
	pthread_exit(0);
	return NULL;
}

/*******************************************************************************
*@ Description    :创建rtsp主业务线程
*@ Input          :
*@ Output         :
*@ Return         :成功：0 ；失败：-1
*@ attention      :不可重复调用！！！
*******************************************************************************/
int start_rtsp_main_task(void)
{
	pthread_t tid;
	int ret = pthread_create(&tid, NULL, rtsp_main, NULL);
	if (ret < 0) {
		ERROR_LOG( "Create rtsp_main failed!\n");
		return -1;
	}
			
	return 0;

}













