 
/***************************************************************************
* @file: ts_video.c
* @author:   
* @date:  4,16,2019
* @brief:  
* @attention:
***************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "ts_video.h"
#include "ts_print.h"
#include "my_inet.h"
#include "buf.h"


 /*---#---------------------------------------------------------------------*/
 /*---#-video部分-----------------------------------------------------------*/
#define 	SPS_NALU  0
#define		PPS_NALU  1
#define		I_NALU    2
#define		P_NALU    3
#define		SET_NALU  4


 int VFrameType(unsigned char* naluData)
 {
	 int index = -1; 
	  
	 if(naluData[0]==0 && naluData[1]==0 && naluData[2]==0 && naluData[3]==1 && naluData[4]==0x67)
	 {
		 index = SPS_NALU;
		 //printf("---SPS_NALU---\n");
	 }
	 else if(naluData[0]==0 && naluData[1]==0 && naluData[2]==0 && naluData[3]==1 && naluData[4]==0x68)
	 {
		 index = PPS_NALU;
		 //printf("---PPS_NALU---\n");
	 }
	 else if(naluData[0]==0 && naluData[1]==0 && naluData[2]==0 && naluData[3]==1 && naluData[4]==0x65)
	 {
		 index = I_NALU;
		 //printf("---I_NALU---\n");
	 }
	 else if(naluData[0]==0 && naluData[1]==0 && naluData[2]==0 && naluData[3]==1 && (naluData[4]==0x61||naluData[4]==0x41))
	 {
		 index = P_NALU;
		//printf("---P_NALU---\n");
	 }
	 else if(naluData[0]==0 && naluData[1]==0 && naluData[2]==0 && naluData[3]==1 && naluData[4]==0x6)
	 {
		 index = SET_NALU;
		 //printf("---SET_NALU---\n");
	 }
	 else
	 {
		 TS_ERROR_LOG("---unknow NALU !!---naluData[] = %#x %#x %#x %#x %#x\n",naluData[0],naluData[1],naluData[2],
		 																	naluData[3],naluData[4]);
	 }
 
	 return index;
 }


 
 TS_PPS_SPS_info_t TS_PPS_SPS_info = {0};
 int TS_set_sps(char* data,int len)
 {
	 if(NULL == data || len <= 0 ||TS_PPS_SPS_info.SPS != NULL)
		 return -1;
	 
	 TS_PPS_SPS_info.SPS = (char*)malloc(len);
	 if(NULL == TS_PPS_SPS_info.SPS)
	 {
		 TS_ERROR_LOG("malloc failed\n");
		 return -1;
	 }
	 memcpy(TS_PPS_SPS_info.SPS,data,len);
	 TS_PPS_SPS_info.SPS_len = len;
	 return 0;	 
 }
 
 int TS_get_sps(char**data,int *len)
 {
	 if(NULL == data || NULL == len)
		 return -1;
	 if(TS_PPS_SPS_info.SPS == NULL)
	 {
		 TS_ERROR_LOG("SPS not inited !\n");
		 return -1;
	 }
 
	 *data = TS_PPS_SPS_info.SPS;
	 *len = TS_PPS_SPS_info.SPS_len;
	 return 0;
	 
 }
 int TS_set_pps(char* data,int len)
 {
	 if(NULL == data || len <= 0 || TS_PPS_SPS_info.PPS != NULL)
		 return -1;
	 
	 TS_PPS_SPS_info.PPS = (char*)malloc(len);
	 if(NULL == TS_PPS_SPS_info.PPS)
	 {
		 TS_ERROR_LOG("malloc failed\n");
		 return -1;
	 }
	 memcpy(TS_PPS_SPS_info.PPS,data,len);
	 TS_PPS_SPS_info.PPS_len = len;
	 return 0;
 }
 
 int TS_get_pps( char**data, int *len)
 {
	 if(NULL == data || NULL == len)
		 return -1;
 
	 if(TS_PPS_SPS_info.PPS == NULL)
	 {
		 TS_ERROR_LOG("PPS not inited !\n");
		 return -1;
	 }
 
	 *data = TS_PPS_SPS_info.PPS;
	 *len = TS_PPS_SPS_info.PPS_len;
	 return 0;
 }
 
 void TS_free_SPS_PPS_info(void)
 {
	 if(NULL != TS_PPS_SPS_info.PPS)
	 {
		 free(TS_PPS_SPS_info.PPS);
		 TS_PPS_SPS_info.PPS = NULL;
	 }
 
	 if(NULL != TS_PPS_SPS_info.SPS)
	 {
		 free(TS_PPS_SPS_info.SPS);
		 TS_PPS_SPS_info.SPS = NULL;
	 }
	 
 }
 
 static int I_start_offset = 0; //IDR帧中 I NALU的偏移量
 //注意，返回的是包含0x0001同步码的起始首位置
 int TS_get_I_start_offset(void)
 {
	 if(I_start_offset <= 0)
		 return -1;
	 
	 return I_start_offset;  
 }
 
 int TS_set_I_start_offset(unsigned int offset)
 {
	 I_start_offset = offset;
	 return 0;	 
 }
 

/*******************************************************************************
*@ Description    :初始化 SPS PPS SEI 等信息
*@ Input          :
*@ Output         :
*@ Return         :成功：0 失败：-1
*@ attention      :
*******************************************************************************/
 static int SPS_PPS_init_done = 0;   //标记 SPS PPS 信息是否已经初始化 1：初始化 0：未初始化
 int TS_Init_SPS_PPS(void *IDR_frame , unsigned int frame_length)
 {
	 if(SPS_PPS_init_done)
	 {
		 TS_ERROR_LOG("SPS PPS already inited!\n");
		 return -1;
	 }
	 if(NULL == IDR_frame)
	 {
		 TS_ERROR_LOG("Illegal parameter !\n");
		 return -1;
	 }
	 
	 char *data = IDR_frame;
	 char *sps_start = IDR_frame;
	 char *pps_start = NULL;
	 char *sei_start = NULL;
	 char *I_start = NULL;
 
 
	 //寻找 PPS SEI I NALU 的起始位置
	 int i = 0;
	 for(i = 0;i < frame_length;i++)
	 {
		 if(data[0]==0 && data[1]==0 && data[2]==0 && data[3]==1 && data[4]==0x68) // PPS NALU	 
			 pps_start = data;
		 
		 if(data[0]==0 && data[1]==0 && data[2]==0 && data[3]==1 && data[4]==0x06) // SEI NALU	 
			 sei_start = data;
		 
		 if(data[0]==0 && data[1]==0 && data[2]==0 && data[3]==1 && data[4]==0x65) // I NALU 
			 I_start = data;
		 
		 if(pps_start != NULL && /*sei_start != NULL &&*/ I_start != NULL)
			 break;
		 
		 data = (char*)IDR_frame + i;
		 
	 }
	 
	 if(i >= frame_length)
	 {
		 TS_ERROR_LOG("find SPS PPS SEI I NALU failed!\n ");
		 return -1;
	 }
	 //注意此次设置进去的NALU 去掉了起始头 0x 0001
	 if(TS_set_sps(sps_start + 4, pps_start - sps_start - 4) < 0)	//4：NALU 的起始码 0x 0001
	 {	 
		 TS_ERROR_LOG("set_sps failed !\n");
		 return -1;
	 }

	 int pps_len = (sei_start == NULL)?(I_start - pps_start - 4):(sei_start - pps_start - 4);
	 if(TS_set_pps(pps_start + 4, pps_len) < 0)
	 {
		 TS_ERROR_LOG("set_pps failed !\n");
		 return -1;
	 }
	 SPS_PPS_init_done = 1;
	 TS_set_I_start_offset(I_start - (char*)IDR_frame);
	 
	 return I_start_offset;
 
 }
 

 ts_video_init_t ts_video_init_info = {0};
 void TS_video_init(ts_video_init_t* info )
 {
	memset(&ts_video_init_info,0,sizeof(ts_video_init_info));
	ts_video_init_info.frame_rate = info->frame_rate;
	TS_DEBUG_LOG("ts_video_init_info.frame_rate = %d\n",ts_video_init_info.frame_rate);
 }
 
/*******************************************************************************
*@ Description    :接收一帧 video 帧数据，加上AUD头返回
*@ Input          :<frame> 输入帧数据
					<frame_len>输入帧数据长度
*@ Output         :<out_buf>输帧buf地址
					<out_buf_len>输出帧buf的长度
					<is_key_frame>0:非关键帧 1：关键帧
*@ Return         :
*@ attention      :需要保证输入的第一帧video帧是关键帧
*******************************************************************************/
static int first_Vframe_is_IDR = 0; //标记第一帧 h264 帧是否为关键帧
buf_t video_frame_buf = {0};
int TS_Video_Encode(void*frame,int frame_len,char**out_buf,int* out_buf_len,int* is_key_frame)
{
	char AUD[6]={0,0,0,1,9,240}; //240:0xF0
 	int frame_type = VFrameType((unsigned char*)frame);
	
	/*---#获取第一帧关键帧数据时需要进行的初始化操作-------------------------------*/
	if(0 == first_Vframe_is_IDR )
	{
		if(frame_type == SPS_NALU)//标记传入的第一帧数据帧是关键帧
		{
			first_Vframe_is_IDR = 1;
			if(0 == SPS_PPS_init_done) //初始化 SPS PPS 部分的数据
			{
				if(TS_Init_SPS_PPS(frame,frame_len) < 0)
				{
					TS_ERROR_LOG("TS_Init_SPS_PPS failed !\n");
					return -1;
				}
			}
		}
		else //传入的第一帧数据帧不是关键帧，出错返回
		{
			TS_ERROR_LOG("The first video frame need input IDR frame!\n");
			return -1;
		}
	}

	/*---#申请帧缓冲buf ------------------------------------------------------------*/	
	int ret = 0;
	int need_buf_len = frame_len + sizeof(AUD) /*+ TS头长度*/ ; 
	if(NULL == video_frame_buf.buf)//初次进入
	{
		ret = init_buf(&video_frame_buf,need_buf_len);
		if(ret < 0)
		{
			TS_ERROR_LOG("init_buf failed !\n");
			return -1;
		}
	}
	
	if(need_buf_len > video_frame_buf.buf_size)//原本的缓存空间不够
	{
		ret = realloc_buf(&video_frame_buf,need_buf_len);
		if(ret < 0)
		{
			TS_ERROR_LOG("realloc_buf failed !\n");
			return -1;
		}		
	}

	reset_buf(&video_frame_buf);//在写数据前统一进行一次重置操作
	/*---#构造成 TS帧数据------------------------------------------------------------*/
	*is_key_frame = 0;
	if(frame_type == SPS_NALU)//关键帧 ，SPS NALU 会在最前边（SPS + PPS + SEI + I帧数据）
	{
		printf("IDR frame---\n");
		*is_key_frame = 1;
		#if 0 
		/*---放 AUD 头（分割器,6字节，兼容 IOS）---*/
		write_buf(&video_frame_buf,AUD,sizeof(AUD));

		/*---写帧数据部分(原始数据已经包含了 同步码 SPS PPS SEI I等数据)-------------------------*/
		write_buf(&video_frame_buf,frame,frame_len); 
		
		#else //需要去掉SEI的数据
		/*---# 裁减掉 前边带同步码的 SPS + PPS + SEI 数据，重新构造帧头数据---*/
		frame = (char*)frame + TS_get_I_start_offset() + 4;//4代表同步码
		frame_len = frame_len - TS_get_I_start_offset() - 4;
		TS_DEBUG_LOG("TS_get_I_start_offset = %d\n",TS_get_I_start_offset());
		
		/*---放 AUD 头（分割器,6字节，兼容 IOS）---*/
		char AUD[6]={0,0,0,1,9,240}; //240:0xF0
		
		TS_DEBUG_LOG("into write video_frame_buf \n");
		write_buf(&video_frame_buf,AUD,sizeof(AUD));
		
		/*---放 SPS + PPS 数据--------------------*/
		char 	sync_code[4] = {0x0,0x0,0x0,0x1}; //同步码
		char*	sps_data;
		int 	sps_len;
		TS_get_sps(&sps_data,&sps_len);
		
		TS_DEBUG_LOG("into write video_frame_buf \n");
		write_buf(&video_frame_buf,sync_code,4);
		TS_DEBUG_LOG("into write video_frame_buf sps_len(%d)\n",sps_len);
		write_buf(&video_frame_buf,sps_data,sps_len);

		char*	pps_data;
		int 	pps_len;
		TS_get_pps(&pps_data,&pps_len);
		
		TS_DEBUG_LOG("into write video_frame_buf \n");
		write_buf(&video_frame_buf,sync_code,4);
		TS_DEBUG_LOG("into write video_frame_buf \n");
		write_buf(&video_frame_buf,pps_data,pps_len);
		#if 0
		/*---放入帧数据（需要去掉同步码，将其填充为4字节的长度信息）-------------------------*/
        // TS_DEBUG_LOG("current actual frame len (%d)\n",frame_len-4);
        unsigned int data_len = t_htonl(frame_len-4);
		write_buf(&video_frame_buf,&data_len,4);
		frame = (char*)frame + 4;
		frame_len -= 4;
		#endif
		
		TS_DEBUG_LOG("into write video_frame_buf \n");
		write_buf(&video_frame_buf,sync_code,4);
		TS_DEBUG_LOG("into write video_frame_buf \n");
		write_buf(&video_frame_buf,frame,frame_len); //写帧真实数据部分
		
		#endif
       
	}
	else if(frame_type == P_NALU )
	{
		//printf("P frame---\n");
		/*---放 AUD 头（分割器,6字节，兼容 IOS）---*/
		TS_DEBUG_LOG("into write video_frame_buf \n");
		write_buf(&video_frame_buf,AUD,sizeof(AUD));

		/*---写帧数据部分(原始数据已经包含了 同步码 SPS PPS SEI I等数据)-------------------------*/
		TS_DEBUG_LOG("into write video_frame_buf \n");
		write_buf(&video_frame_buf,frame,frame_len); 
	}
	else //是已经去掉过同步码的帧数据
	{
		//hisi编码的数据都带同步码，此处暂不实现
		TS_ERROR_LOG("video frame not have synchronous code,not support!\n");
		return -1;
	}

	*out_buf = video_frame_buf.buf;
	*out_buf_len = (int)(video_frame_buf.w_pos - video_frame_buf.buf);//need_buf_len;
	 return 0;
 }


void TS_video_exit(void)
{
	TS_free_SPS_PPS_info();
	free_buf(&video_frame_buf);

}

void TS_video_global_variable_reset(void)
{
	
	I_start_offset = 0;
	SPS_PPS_init_done = 0;
	memset(&ts_video_init_info,0,sizeof(ts_video_init_info));
	first_Vframe_is_IDR = 0;
}






