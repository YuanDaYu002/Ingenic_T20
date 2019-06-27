 
/***************************************************************************
* @file:ts_audio.h 
* @author:   
* @date:  4,16,2019
* @brief:  
* @attention:
***************************************************************************/
#include <string.h>

#include "ts_audio.h"
#include "ts_print.h"
#include "buf.h"
#include "my_inet.h"


/*---#--audio部分----------------------------------------------------------*/
/*---#---------------------------------------------------------------------*/
/*---#-------------------------55 ~ 28---------------------------27 ~ 0---------------------------------------*/	 
/*---#---------------fix_header(28 bit)------------|---------variable_header(28 bit)-------------------------*/
/*---#------------F---F----F----1----6----0----4---|--0----0----0----1----F----F----C------------------------*/
/*---#----------1111 1111 1111 0001 0110 0000 0100 | 0000 0000 0000 0001 1111 1111 1100----------------------*/ 
/*---#----------AAAA AAAA AAAA BCCD EEFF FFGH HHIJ | KLMM MMMM MMMM MMMO OOOO OOOO OOPP (QQQQQQQQ QQQQQQQQ)--*/
/*A:固定为0xFFF		B:MPEG-4  C:固定为'00'  	D:没有CRC校验  		E:1(2-1):AAC LC (Low Complexity) F:8:16000HZ
  G:私有位，编码时设置为0，解码时忽略	H:声道数1 I:0 J:0
  K:0 L:0 	M:包括adts头在内的音频数据总长度	O:0x7FF码率可变的码流	 			P:0:每帧都只包含一帧AAC帧
*/
#define ADTS_BASE_MASK 	0xFFF16040001FFC  //0xFFF1604 0001 FFC  (0001部分是变化的)

//adts_fix_header_t adts_fix_header = {0};
//long long int adts_fix_header = 0;
//adts_variable_header_t adts_variable_header = {0};
//long long int adts_variable_header = 0;
long long int adts_header = 0;

extern void print_array(unsigned char* box_name,unsigned char*start,unsigned int length);


/*******************************************************************************
*@ Description    :TS音频切片初始化
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
ts_audio_init_t ts_audio_init_info= {0};
int Aframe_duration = 0;//基于视频 timescale（90000）的时长下， 一帧audio帧时长
void TS_Audio_init(ts_audio_init_t* info )
{
	memset(&ts_audio_init_info,0,sizeof(ts_audio_init_info));
	memcpy(&ts_audio_init_info,info,sizeof(ts_audio_init_t));

	//基于视频 timescale（90000）的时长下， 一帧audio帧时长：(1024/ts_audio_init_info.sample_rate)*90000
	Aframe_duration = 1024*90000/ts_audio_init_info.sample_rate; 
			
	/*---#主要初始化 fix_header 部分 ------------------------------------------------------------*/
	adts_header = ADTS_BASE_MASK;
	long long int 	ID = info->ID;//0
	long long int  	profile = info->profile;
	long long int  	sampling_frequency_index = info->sampling_frequency_index;
	/*---#---------------------------------------清零----------------------------置位-------------*/
	adts_header = (adts_header & ~((long long int)0x01<<43)) | ((ID & 0x01)<<43);
	adts_header = (adts_header & ~((long long int)0x03<<38)) | ((profile & 0x03)<<38);
	adts_header = (adts_header & ~((long long int)0x0F<<34)) | ((sampling_frequency_index & 0x0F)<<34);

	//print_array("init adts_fix_header:",(char*)&adts_fix_header,sizeof(adts_fix_header));
	//print_array("init adts_variable_header:",(char*)&adts_variable_header,sizeof(adts_variable_header));
	
	
}

/*******************************************************************************
*@ Description    :接收一帧audio帧数据
*@ Input          :
*@ Output         :<out_buf>输帧buf地址
					<out_buf_len>输出帧buf的长度
*@ Return         :成功：0 失败:-1
*@ attention      :
*******************************************************************************/
buf_t audio_frame_buf = {0};
int TS_Audio_Encode(void*frame,int frame_len,char**out_buf,int* out_buf_len)
{
	//修正长度信息
	long long int frame_length = (long long int)(frame_len + ADTS_LENGTH);  
	adts_header = (adts_header & ~((long long int)0x1FFF << 13)) | ((frame_length & 0x1FFF)<<13);//0xFFF16040001FFC
	//print_array("adts_header:",(char*)&adts_header,sizeof(adts_header));

	//8*8 = 64 bit位，adts只有28+28 = 56 bit位，还需往前边偏移 64-56=8bit位
	//int multiple_len = sizeof(adts_header) - ADTS_LENGTH; 

	/*---#内存分配------------------------------------------------------------*/
	int ret = 0;
	int buf_len = frame_len + ADTS_LENGTH;
	if(NULL == audio_frame_buf.buf)//初次进入
	{
		ret = init_buf(&audio_frame_buf, buf_len);
		if(ret < 0)
		{
			TS_ERROR_LOG("init_buf failed !\n");
			return -1;
		}
	}

	if(buf_len > audio_frame_buf.buf_size)//原本的缓存空间不够
	{
		ret = realloc_buf(&audio_frame_buf, buf_len);
		if(ret < 0)
		{
			TS_ERROR_LOG("realloc_buf failed !\n");
			return -1;
		}
	}

	reset_buf(&audio_frame_buf);//在写数据前统一进行一次重置操作
	/*---#构添加adts头部------------------------------------------------------------*/
	/*拷贝头部，1为那8bit位多余的部分,小端机器 0x00FFF16040001FFC 低位在低字节，我们要舍弃高位的1字节
		转换成大端字节序时，需要舍弃的是低位1字节
	*/
	//print_array("ADTS:",(char*)&adts_header,sizeof(adts_header));
	long long int final_adts_header  = t_htonll(adts_header);
	//print_array("final t_htonll(adts_header):",(char*)&final_adts_header,sizeof(final_adts_header));

	TS_DEBUG_LOG("into write audio_frame_buf \n");
	write_buf(&audio_frame_buf, (char*)&final_adts_header + 1, sizeof(final_adts_header)-1/*ADTS_LENGTH*/);
	TS_DEBUG_LOG("into write audio_frame_buf \n");
	write_buf(&audio_frame_buf, frame, frame_len);//拷贝数据 加的是 ADTS_LENGTH
	//print_array("recode audio:",frame,10);
	
	*out_buf = audio_frame_buf.buf;
	*out_buf_len = (int)(audio_frame_buf.w_pos - audio_frame_buf.buf);

	return 0;
}


void TS_audio_exit(void)
{
	free_buf(&audio_frame_buf);
}


void ts_audio_global_variable_reset(void)
{
	//adts_fix_header = 0;
	//adts_variable_header = 0; 
	adts_header = 0;
	memset(&audio_frame_buf,0,sizeof(audio_frame_buf));
	memset(&ts_audio_init_info,0,sizeof(ts_audio_init_info));
}







