 
/***************************************************************************
* @file:ts.h 
* @author:   
* @date:  4,15,2019
* @brief:  
* @attention:
***************************************************************************/
//this is values for codec member of track_t
#define MPEG_AUDIO_L3  0x04
#define MPEG_AUDIO_L2  0x03
#define AAC_AUDIO		0x0F
#define H264_VIDEO		0x1B

typedef struct _ts_media_track_t{
	int 	codec;				/*编解码类型：0x0F：为音频trak;
											  0x1B：为视频trak;
											  0：啥都不是
								*/
	int 	n_frames;			//目前该trak的帧数目
	int 	bitrate;
	//float*	pts;
	//float*	dts;
	int*	pts;
	int*	dts;
	int 	repeat_for_every_segment;
	int*	flags;			//关键帧标记
	int 	sample_rate;	//采样率
	int 	n_ch;			//声道数
	int 	sample_size; 	//PCM Sample size for audio
	int 	data_start_offset;
	int 	type;
} ts_track_media_t;

typedef struct _ts_media_stats_t{
	int 		n_tracks;
	ts_track_media_t  track[2];
} ts_media_stats_t;


//一个TS文件对应源音视频 帧数据的描述结构
typedef struct ts_track_data_t
{
	int 	n_frames; 		//帧总数(实际放入的)
	int 	first_frame; 	//TS分片对应帧区间的开始位置（下标）
	char* 	buffer;			//帧数据BUF的起始位置
	char*	buffer_w_pos;	//帧数据BUF的写指针
	char*	buffer_r_pos;	//帧数据BUF的读指针
	int		buffer_size;	//帧数据BUF的总大小
	int*	size;			//帧的大小信息的指针（理解为数组的指针）
	int* 	offset;			//帧的偏移量信息的指针（理解为数组的指针）（相较于 buffer/TS文件 开始位置 ）
	int 	frames_written; //记录已经写入到TS文件的（源音视）频帧数
	int 	data_start_offset;
	int 	cc;				//mpeg2 ts continuity counter （TS 包/帧的数量统计）
} ts_track_data_t;

typedef struct ts_media_data_t
{
	int 				n_tracks;
	ts_track_data_t 	track[2];
}ts_media_data_t;

void ts_global_variable_reset(void);



