 
/***************************************************************************
* @file:ts_video.h 
* @author:   
* @date:  4,16,2019
* @brief:  
* @attention:
***************************************************************************/
#ifndef _TS_VIDEO_H
#define _TS_VIDEO_H
#include "ts_interface.h"

typedef struct _TS_SPS_PPS_info_t
{
	char*	SPS;		//指向保存的SPS NAL数据
	int 	SPS_len;	//SPS的数据长度
	char*	PPS;		//指向保存的PPS NAL数据
	int 	PPS_len;	//PPS的数据长度
}TS_PPS_SPS_info_t;

void TS_video_init(ts_video_init_t* info );
int TS_Video_Encode(void*frame,int frame_len,char**out_buf,int* out_buf_len,int* is_key_frame);
void TS_video_exit(void);
void TS_video_global_variable_reset(void);






#endif


