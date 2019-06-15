 
/***************************************************************************
* @file:encoder.h 
* @author:   
* @date:  5,29,2019
* @brief:  编码相关函数头文件
* @attention:
***************************************************************************/
#ifndef _ENCODER_H
#define _ENCODER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "typeport.h"

#define MAX_STREAM_PER_CHN  4

//编码标准枚举

typedef enum
{
    VENC_STD_H264,
    VENC_STD_JEPG,
    VENC_STD_H265,

    VENC_STD_NR,
} E_VENC_STANDARD;

// 分辨率枚举

typedef enum
{
    IMAGE_SIZE_1920x1080, 
  	IMAGE_SIZE_960x544,	//该分辨率在T20的次码流中支持不到（不使用）
    IMAGE_SIZE_640x360,
    IMAGE_SIZE_480x272,

    IMAGE_SIZE_NR
} E_IMAGE_SIZE;

typedef enum
{
    AENC_STD_G711A,
    AENC_STD_ADPCM,
    AENC_STD_MP3,
    AENC_STD_AAC,

    AENC_STD_NR,
} E_AENC_STANDARD;

typedef enum
{
    AUDIO_SR_8000, /* 8K samplerate*/
    AUDIO_SR_16000,
    AUDIO_SR_44100,

    AUDIO_SR_NR,
} E_AUDIO_SAMPLE_RATE;

typedef enum
{
    AUDIO_BW_16, /* 16bit width*/

    AUDIO_BW_NR,
} E_AUDIO_BIT_WIDTH;

typedef enum
{
    AUDIO_SM_MONO, /*mono*/

    AUDIO_SM_NR,
} E_AUDIO_SOUND_MODE;


//码流能力集描述结构体

typedef struct
{
    HLE_U32 enc_std_mask; /*所支持的编码标准，位图掩码: 每个bit代表一种编码标准，具体见E_VENC_STANDARD*/
    HLE_U32 img_size_mask; /*所支持的编码分辨率，位图掩码: 每个bit代表一种图像分辨率，具体见E_IMAGE_SIZE*/
    HLE_U32 max_framerate; /*编码支持的最大帧率*/
    HLE_U32 brc_mask; /*编码支持的码率控制方式，位图掩码：bit0-CBR, bit1-VBR，对应的位为1表示支持*/
    HLE_U32 max_bitrate; /*最大码率，单位Kbps*/
} STREAM_CAP;

//编码能力集描述结构体

typedef struct
{
    int stream_count; /*支持的码流数目，最大MAX_STREAM_PER_CHN个码流*/
    STREAM_CAP streams[MAX_STREAM_PER_CHN];
} ENC_CAPABILITY;


//编码属性信息结构体

typedef struct
{
    HLE_U8 enc_standard; /*编码标准，具体取值见E_VENC_STANDARD*/
    HLE_U8 img_size; /*图像大小，具体取值见E_IMAGE_SIZE定义*/
    HLE_U8 framerate; /*帧率设置，1-30*/
    HLE_U8 bitratectrl; /*码流类型，CBR为0，VBR为1*/
    HLE_U8 quality; /*画质设置[0, 4]，最清晰为0，最差为4*/
    HLE_U8 hasaudio; /*是否包含音频，0不包含、1为包含音频*/
    HLE_U16 bitrate; /*码流设置，单位Kbps*/
} ENC_STREAM_ATTR;


/*
码流帧格式:
    视频关键帧: FRAME_HDR + IFRAME_INFO + DATA
    视频非关键帧: FRAME_HDR + PFRAME_INFO + DATA
    音频帧: FRAME_HDR + AFRAME_INFO + DATA
关于帧类型和帧同步标志选取:
    前三个字节帧头同步码和H.264 NALU分割码相同，均为0x00 0x00 0x01
    第四个字节使用了H.264中不会使用到的0xF8-0xFF范围
 */


//音视频帧头标志

typedef struct
{
    HLE_U8 sync_code[3]; /*帧头同步码，固定为0x00,0x00,0x01*/
    HLE_U8 type; /*帧类型, 0xF8-视频关键帧，0xF9-视频非关键帧，0xFA-音频帧*/
} FRAME_HDR;

//关键帧描述信息

typedef struct
{
    HLE_U8 enc_std; //编码标准，具体见E_VENC_STANDARD
    HLE_U8 framerate; //帧率
    HLE_U16 reserved;
    HLE_U16 pic_width;
    HLE_U16 pic_height;
    HLE_SYS_TIME rtc_time; //当前帧时间戳，精确到秒，非关键帧时间戳需根据帧率来计算
    HLE_U32 length;		//实际帧数据长度
    HLE_U64 pts_msec; //毫秒级时间戳，一直累加，溢出后自动回绕
} IFRAME_INFO;

//非关键帧描述信息

typedef struct
{
    HLE_U64 pts_msec; //毫秒级时间戳，一直累加，溢出后自动回绕
    HLE_U32 length;
} PFRAME_INFO;

//音频帧描述信息

typedef struct
{
    HLE_U8 enc_type; //音频编码类型，具体见 E_AENC_STANDARD
    HLE_U8 sample_rate; //采样频率，具体见 E_AUDIO_SAMPLE_RATE
    HLE_U8 bit_width; //采样位宽，具体见 E_AUDIO_BIT_WIDTH
    HLE_U8 sound_mode; //单声道还是立体声，具体见 E_AUDIO_SOUND_MODE
    HLE_U32 length;
    HLE_U64 pts_msec;
} AFRAME_INFO;

//码流包描述结构体，包内有且只有一帧数据

typedef struct _tag_ENC_STREAM_PACK
{
    int channel; //通道号
    int stream_index; //码流编号
    int length; //包内有效数据长度
    HLE_U8 *data; //包缓冲首地址
} ENC_STREAM_PACK;

#define MAX_ROI_REGION_NUM  4

typedef struct
{
    HLE_U32 enable; //是否使能。0-不使能，其他参数忽略，1-使能
    HLE_S32 level; //编码roi区域的清晰度等级[0, 4], 0为最清晰
    HLE_U16 left; //矩形区域坐标为相对REL_COORD_WIDTH X REL_COORD_HEIGHT坐标系大小
    HLE_U16 top;
    HLE_U16 right;
    HLE_U16 bottom;
} ENC_ROI_ATTR;

//JPEG 编码描述结构体

typedef struct
{
    HLE_U32 img_size; //图像大小，具体取值见E_IMAGE_SIZE定义
    HLE_U32 level; //图片清晰等级[0, 4],0为最清晰
} ENC_JPEG_ATTR;

//JPEG 能力描述结构体

typedef struct
{
    HLE_U32 img_size_mask; //所支持的编码分辨率，位图掩码: 每个bit代表一种图像分辨率，具体见E_IMAGE_SIZE
} JPEG_CAPABILITY;


	 	 
/*******************************************************************************
*@ Description    :编码系统初始化
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :内部初始化的顺序不可随意变动，修改时需要注意
*******************************************************************************/
int encoder_system_init(void);

/*******************************************************************************
*@ Description    :开始编码系统业务线程
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int encoder_system_start(void);


/*******************************************************************************
*@ Description    :编码系统退出
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :内部退出的先后顺序不能随意改变，修改时需要注意
*******************************************************************************/
int encoder_system_exit(void);

#ifdef __cplusplus
}
#endif	 
	 
#endif







