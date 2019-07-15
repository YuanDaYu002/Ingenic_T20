 /***************************************************************************
* @file:main.c 
* @author:   
* @date:  5,29,2019
* @brief:  工程总入口文件
* @attention:
***************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include<sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>


#include "video.h"
#include "audio.h"
#include "osd.h"
#include "encoder.h"
#include "CircularBuffer.h"
#include "fmp4_interface.h"



extern int get_image_size(E_IMAGE_SIZE image_size, int *width, int *height);

/*******************************************************************************
*@ Description    :将循环缓存区的数据帧存储成H264文件
*@ Input          :<image_size> 要录制的码流对应的分辨率
					<time_len> 要录制时长
					<file_name> 保存文件的文件名(带绝对路径)
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int save_h264_file_from_cirbuf(E_IMAGE_SIZE image_size,int time_len,char*file_name)
{
	if(time_len <= 0 || NULL == file_name)
	{
		ERROR_LOG("Illegal parameter!\n");
		return HLE_RET_EINVAL;
	}
		
	int userID,ret;
	CircularBuffer_t*handle = CircularBufferGetHandle(image_size);
	if(NULL == handle)
	{
		ERROR_LOG("CircularBufferGetHandle failed!\n");
		return HLE_RET_ERROR;
	}
	
	userID = CircularBufferRequestUserID(handle);
	if(userID < 0)
	{
		ERROR_LOG("CircularBufferRequestUserID failed!\n");
		return HLE_RET_ERROR;
	}

	/*---#打开文件------------------------------------------------------------*/
	int fd = open(file_name,O_RDWR|O_CREAT|O_TRUNC,0755);
	if(fd < 0)
	{
		ERROR_LOG("open file(%s) failed!\n",file_name);
		goto ERR;
	}

	/*---#获取帧数据------------------------------------------------------------*/
	void * data = NULL;
	FrameInfo_t info = {0};
	struct timeval start_time;
	struct timeval current_time;
	int width,height;
	get_image_size(image_size,&width,&height);
	
	gettimeofday(&start_time,NULL);
	memcpy(&current_time,&start_time,sizeof(struct timeval));

	int count_frame_num = 0;
	char* frame_type = NULL;
	/*注意采用系统时间去控制录制时长并不很准确,因为循环缓冲buffer
	在读第一帧时会自动跳转到读指针所对应的IDR帧上，可能多读[0,gop-1]个帧数*/
	while(current_time.tv_sec - start_time.tv_sec < time_len)
	{
		data = NULL;
		memset(&info,0,sizeof(info));
		gettimeofday(&current_time,NULL);

		ret = CircularBufferReadOneFrame(userID,handle,&data,&info);
		if(ret < 0)
		{
			ERROR_LOG("CircularBufferReadOneFrame error!\n");
			continue;
		}

		if(info.flag == 0xF8)
			frame_type = "I";
		else if(info.flag == 0xF9)
			frame_type = "P";
		else if(info.flag == 0xFA)
			frame_type = "A";
		else
			continue;
		
		/*---#-写入到文件-----------------------------------------------------------*/
		#if 1
		int skip_len = 0;
		FRAME_HDR *header = (FRAME_HDR *)data;
            if (header->sync_code[0] != 0 || header->sync_code[1] != 0|| header->sync_code[2] != 1) 
           {
                printf("------BUG------\n");
                continue;
            }

            if (header->type == 0xF8)  //0xF8-视频关键帧
			{
                skip_len = sizeof (FRAME_HDR) + sizeof (IFRAME_INFO);
            }
            else if (header->type == 0xF9) //0xF9-视频非关键帧 
			{
                skip_len = sizeof (FRAME_HDR) + sizeof (PFRAME_INFO);
            }
            else if (header->type == 0xFA)//0xFA-音频帧
			{
                 skip_len = sizeof (FRAME_HDR) + sizeof (AFRAME_INFO);
            }
            else 
			{
                printf("------BUG------\n");
                continue;
            }
			
		
		#endif

		 if (header->type == 0xF8)  //0xF8-视频关键帧
		 {
		 	char* IDR = (char*)data + skip_len;
			DEBUG_LOG("IDR[]= %x %x %x %x %x\n",IDR[0],IDR[1],IDR[2],IDR[3],IDR[4]);
		 }
		ret = write(fd,data + skip_len,info.frmLength - skip_len);
		if(ret != info.frmLength- skip_len)
		{
			ERROR_LOG("write file(%s) failed!\n",file_name);
			goto ERR;
		}
		count_frame_num++;
		printf("record frame from circularbuffer...image_size[%dx%d] count_frame_num = %d frame_type = %s\n",
					width,height,count_frame_num,frame_type);
			
	}


	CircularBufferFreeUserID(handle,userID);
	close(fd);
	return HLE_RET_OK;
ERR:
	CircularBufferFreeUserID(handle,userID);
	if(fd > 0)
		close(fd);
	
	return HLE_RET_ERROR;

}


// 本机大端返回1，小端返回0
int my_checkCPUendian(void)
{
       union{
                unsigned long int i;
                unsigned char s[4];
              }c;

       c.i = 0x12345678;
	   printf("c.s[0] = %#x\n",c.s[0]);
       return (0x12 == c.s[0]);
}



extern int fmp4_record(fmp4_out_info_t* info);
extern int ts_record(void **out_buf,int* out_len,int recode_time);
int main(int argc,char*argv[])
{
	int ret;
	/*---#缓存池初始化------------------------------------------------------------*/
	CircularBufferInit();

	/*---#编码系统初始化----------------------------------------------------------*/
	ret = encoder_system_init();
	if(ret < 0) return -1;

	/*---#启动编码业务------------------------------------------------------------*/
	ret= encoder_system_start();
	if(ret < 0) goto ERR;

	
	/*---#获取抓拍图像，保存到文件------------------------------------------------*/
	sleep(2); //开机后不要太快抓拍，图像还没稳定下来，可能抓到黑屏
	ret = jpeg_get_one_snap(0);
	if(ret < 0) goto ERR;
	
	ret = jpeg_get_one_snap(1);
	if(ret < 0) goto ERR;
	
	/*---#从循环缓存池获取编码帧(H264 + AAC/PCM)，直接保存到文件-----------------------------------*/
#if 0
	ret = save_h264_file_from_cirbuf(IMAGE_SIZE_640x360,8,"/tmp/cirbuffer_chn_1.h264");	
	if(ret < 0) goto ERR;
	
	ret = save_h264_file_from_cirbuf(IMAGE_SIZE_1920x1080,8,"/tmp/cirbuffer_chn_0.h264");
	if(ret < 0) goto ERR;
#endif

	//播放g711文件 test
#if 0	//该种方式不能正常解码播放
	sleep(3);
	char buf[1024] = {0};
	FILE* fd = fopen("/system/bin/g711_file.g711","r");

	DEBUG_LOG("********************fopen file success!****************\n");
	while(1)
	{
		ret = fread(buf, 1, 1024, fd);
		if(ret > 0)
		{
			DEBUG_LOG("playing file.....ret(%d)\n",ret);
			ret = ADEC_G711_decode_frame_and_play(buf,ret);
			if(ret < 0)
			{
				ERROR_LOG("ADEC_G711_decode_frame_and_play error!\n");
				continue;
			}
		}
		else if(0 == ret)
		{
			DEBUG_LOG("*********************play g7ll file finished !*********************\n");
			break;
		}
		else
		{
			ERROR_LOG("read file error!\n");
		}
			
	}

	fclose(fd);
	
#endif 


	/*---#测试fmp4文件录制
	因为牵涉到HLS协议不兼容 MP4 + M3U8 ，后续再调试------------------------------*/
#if 0
	fmp4_out_info_t fmp4_info = {0};
	#if 0
	fmp4_info.file_mode.file_name = "/tmp/t20_fmp4_1920x1080_.mp4";
	#else
	fmp4_info.buf_mode.buf_start = (char*)malloc(1024*1024*5);
	if(NULL == fmp4_info.buf_mode.buf_start)
	{
		ERROR_LOG("malloc failed!\n");
		return -1;
	}
	memset(fmp4_info.buf_mode.buf_start,0,1024*1024*5);
	fmp4_info.buf_mode.w_offset = 0;
	fmp4_info.buf_mode.buf_size = 1024*1024*5;
	
	#endif
	fmp4_info.recode_time = 8; //录制的 s 数
	if(fmp4_record(&fmp4_info) < 0)
	{
		ERROR_LOG("fmp4 record failed!\n");
		
	}
#endif
	/*---#------------------------------------------------------------*/


	/*---#录TS视频------------------------------------------------------------*/
#if 0
	void* out_buf = NULL;
    int out_len = 0;
	if(ts_record(&out_buf,&out_len,15) < 0)
	{
		ERROR_LOG("ts_record failed!\n");
		return -1;
	}

	int fd = open("/tmp/T20_ts_main.ts",O_RDWR|O_CREAT|O_TRUNC,0755);
	if(fd < 0)
	{
		ERROR_LOG("open /tmp/T20_ts_main.ts failed!\n");
		return -1;
	}

	ret = write(fd,out_buf,out_len);
	if(ret != out_len)
	{
		ERROR_LOG("write file failed!\n");
		close(fd);
		return -1;
	}

	close(fd);
	free(out_buf);
#endif
	/*---#------------------------------------------------------------*/
	
	extern int start_rtsp_main_task(void);		
	start_rtsp_main_task();

	
	DEBUG_LOG("--------checkCPUendian ret (%d)\n",my_checkCPUendian());
	
	int i= 10;//延迟多少秒自动退出（DEBUG）
	while(1)
	{
		sleep(1);
		//printf("...\n");
		i--;
	}



ERR:
	encoder_system_exit();
	CircularBufferDestory();
	
	return 0;
}


































































































