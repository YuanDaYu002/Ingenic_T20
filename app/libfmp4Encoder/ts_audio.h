 
/***************************************************************************
* @file:ts_audio.h 
* @author:   
* @date:  4,16,2019
* @brief:  
* @attention:
***************************************************************************/
#ifndef _TS_AUDIO_H
#define _TS_AUDIO_H

#include "ts_interface.h"

#define ADTS_LENGTH 	7  //7:adts长度7字节，无CRC校验。（有CRC校验为9字节）
typedef struct _adts_fix_header_t
{
	unsigned int 		syncword:12; 				//帧同步标识一个帧的开始，固定为0xFFF
	unsigned int		ID:1;  	  					//MPEG 标示符。0表示MPEG-4，1表示MPEG-2
	unsigned int		layer:2;    				//固定为'00'
	unsigned int		protection_absent:1; 		//标识是否进行误码校验。0表示有CRC校验，1表示没有CRC校验
	
	unsigned int		profile:2;  				//标识使用哪个级别的AAC(值再减1)。1: AAC Main 2:AAC LC (Low Complexity) 3:AAC SSR (Scalable Sample Rate) 4:AAC LTP (Long Term Prediction) 
	unsigned int		sampling_frequency_index:4;	//标识使用的采样率的下标（0x8             : 16000HZ /0x5 ：32000HZ  ）
	unsigned int		private_bit:1;  			//私有位，编码时设置为0，解码时忽略
	unsigned int		channel_configuration:3;	//标识声道数 
	unsigned int		original_copy:1; 			//编码时设置为0，解码时忽略 
	unsigned int		home:1;  					//编码时设置为0，解码时忽略 
	/*---# 注意，多余了 4*8-28 = 4bit位------------------------------------------------------------*/
	
}adts_fix_header_t;


typedef struct _adts_variable_header_t   
{   
	 unsigned int		copyright_identification_bit:1; 	//编码时设置为0，解码时忽略
	 unsigned int		copyright_identification_start:1; 	//编码时设置为0，解码时忽略 
	 unsigned int		frame_length:13; 					//ADTS帧长度包括ADTS长度和AAC声音数据长度的和。即 aac_frame_length = (protection_absent == 0 ? 9 : 7) + audio_data_length 
	 unsigned int		adts_buffer_fullness:11;  			//固定为0x7FF。表示是码率可变的码流
	 unsigned int		number_of_raw_data_blocks_in_frame:2; //表示当前帧有number_of_raw_data_blocks_in_frame + 1 个原始帧(一个AAC原始帧包含一段时间内1024个采样及相关数据)。
	 /*---# 注意，多余了 4*8-28 = 4bit位------------------------------------------------------------*/
}adts_variable_header_t;

void TS_Audio_init(ts_audio_init_t* info );
int TS_Audio_Encode(void*frame,int frame_len,char**out_buf,int* out_buf_len);
void TS_audio_exit(void);
void ts_audio_global_variable_reset(void);




#endif
