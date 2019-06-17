 
/***************************************************************************
* @file: audio.c
* @author:   
* @date:  5,29,2019
* @brief:  音频编码相关函数
* @attention:
***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "imp_audio.h"
#include "audio.h"
#include "video.h"
#include "CircularBuffer.h"
#include "typeport.h"
#include "fifo.h"


typedef struct
{
    int getdata_enable;
    int putdata_enable;
    FIFO_HANDLE fifo;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} TALKBACK_CONTEXT;

static TALKBACK_CONTEXT tb_ctx;  //对讲控制结构




int audio_send_Aframe_to_cirbuf(void *buf,E_AENC_STANDARD enc_type);
static int AENC_G711_init(void);
static int AENC_G711_encode_one_frame(void * buf_pcm,int len);
static int AENC_G711_exit(void);
int talkback_start(int getmode, int putmode);
static int talkback_ao_stop(void);
int talkback_init(void);
int talkback_exit(void);






/**********************************************************************************
* 								AI/AO 部分代码  
***********************************************************************************/
#define AI_AO_PORTION

/*******************************************************************************
*@ Description    :音频输入音量设置
*@ Input          :<chnVol>音频输入音量  [-30 ~ 120]
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_set_AI_volume(int chnVol)
{
	int ret;	
	/* Step 5: Set audio channel volume. */
	ret = IMP_AI_SetVol(AI_DEVICE_ID, AI_CHANNEL_ID, chnVol);
	if(ret != 0) {
		ERROR_LOG(" Set AI volume failed\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_AI_GetVol(AI_DEVICE_ID, AI_CHANNEL_ID, &chnVol);
	if(ret != 0) {
		ERROR_LOG("get AI volume failed\n");
		return HLE_RET_ERROR;
	}
	DEBUG_LOG("Get AI Volume : %d\n", chnVol);

	return HLE_RET_OK;
}

/*******************************************************************************
*@ Description    :音频输入增益设置
*@ Input          :<aigain>音频输入增益 [0,31]
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_set_AI_Gain(int aigain)
{
	int ret;
	//音频输入增益设置
	ret = IMP_AI_SetGain(AI_DEVICE_ID, AI_CHANNEL_ID, aigain);
	if(ret != 0) {
		ERROR_LOG("Set AI Gain failed\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_AI_GetGain(AI_DEVICE_ID, AI_CHANNEL_ID, &aigain);
	if(ret != 0) {
		ERROR_LOG("Get AI Gain failed\n");
		return HLE_RET_ERROR;
	}
	DEBUG_LOG("AI GetGain gain : %d\n", aigain);

	return HLE_RET_OK;
}

/*******************************************************************************
*@ Description    :设置音频输出（speaker）通道音量
*@ Input          :<chnVol>音频输出音量 [-30,120]
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_set_AO_volume(int chnVol)
{
	int ret;
	/* Step 4: Set audio channel volume. */
	ret = IMP_AO_SetVol(AO_DEVICE_ID, AO_CHANNEL_ID, chnVol);
	if (ret != 0) {
		ERROR_LOG( "IMP_AO_SetVol failed\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_AO_GetVol(AO_DEVICE_ID, AO_CHANNEL_ID, &chnVol);
	if (ret != 0) {
		ERROR_LOG( "Audio Play get volume failed\n");
		return HLE_RET_ERROR;
	}
	DEBUG_LOG( "Audio Out GetVol    vol:%d\n", chnVol);

	return HLE_RET_OK;
}

/*******************************************************************************
*@ Description    :设置音频输出（speaker）增益
*@ Input          :<aogain>音频输出增益 [0,31]
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_set_AO_Gain(int aogain)
{
	int ret;
	ret = IMP_AO_SetGain(AO_DEVICE_ID, AO_CHANNEL_ID, aogain);
	if (ret != 0) {
		ERROR_LOG( "Audio Record Set Gain failed\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_AO_GetGain(AO_DEVICE_ID, AO_CHANNEL_ID, &aogain);
	if (ret != 0) {
		ERROR_LOG( "Audio Record Get Gain failed\n");
		return HLE_RET_ERROR;
	}
	DEBUG_LOG( "Audio Out GetGain    gain : %d\n", aogain);

	return HLE_RET_OK;
	
}



/*******************************************************************************
*@ Description    :AI初始化
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_in_init(void)
{
	int ret;
	/* Step 1: set public attribute of AI device. */
	IMPAudioIOAttr attr;
	attr.samplerate = AUDIO_SAMPLE_RATE_16000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 640;
	attr.chnCnt = 1;
	ret = IMP_AI_SetPubAttr(AI_DEVICE_ID, &attr);
	if(ret != 0) {
		ERROR_LOG("set ai %d attr err: %d\n", AI_DEVICE_ID, ret);
		return HLE_RET_ERROR;
	}

	memset(&attr, 0x0, sizeof(attr));
	ret = IMP_AI_GetPubAttr(AI_DEVICE_ID, &attr);
	if(ret != 0) {
		ERROR_LOG("get ai %d attr err: %d\n", AI_DEVICE_ID, ret);
		return HLE_RET_ERROR;
	}
	
	printf("-------AI Info--------------------------------------\n");
	printf( "Audio In GetPubAttr samplerate : %d\n", attr.samplerate);
	printf( "Audio In GetPubAttr   bitwidth : %d\n", attr.bitwidth);
	printf( "Audio In GetPubAttr  soundmode : %d\n", attr.soundmode);
	printf( "Audio In GetPubAttr     frmNum : %d\n", attr.frmNum);
	printf( "Audio In GetPubAttr  numPerFrm : %d\n", attr.numPerFrm);
	printf( "Audio In GetPubAttr     chnCnt : %d\n", attr.chnCnt);
	printf("-----------------------------------------------------\n");

	/* Step 2: enable AI device. */
	ret = IMP_AI_Enable(AI_DEVICE_ID);
	if(ret != 0) {
		ERROR_LOG( "Enable ai %d err\n", AI_DEVICE_ID);
		return HLE_RET_ERROR;
	}

	/* Step 3: set audio channel attribute of AI device. */
	IMPAudioIChnParam chnParam;
	chnParam.usrFrmDepth = 20;
	ret = IMP_AI_SetChnParam(AI_DEVICE_ID, AI_CHANNEL_ID, &chnParam);
	if(ret != 0) {
		ERROR_LOG( "set ai %d channel %d attr err: %d\n", AI_DEVICE_ID, AI_CHANNEL_ID, ret);
		return HLE_RET_ERROR;
	}

	memset(&chnParam, 0x0, sizeof(chnParam));
	ret = IMP_AI_GetChnParam(AI_DEVICE_ID, AI_CHANNEL_ID, &chnParam);
	if(ret != 0) {
		ERROR_LOG( "get ai %d channel %d attr err: %d\n", AI_DEVICE_ID, AI_CHANNEL_ID, ret);
		return HLE_RET_ERROR;
	}
	DEBUG_LOG( "Audio In GetChnParam usrFrmDepth : %d\n", chnParam.usrFrmDepth);

	/* Step 4: enable AI channel. */
	ret = IMP_AI_EnableChn(AI_DEVICE_ID, AI_CHANNEL_ID);
	if(ret != 0) {
		ERROR_LOG("AI enable channel failed\n");
		return HLE_RET_ERROR;
	}
	
	/*---#音频音量设置------------------------------------------------------------*/
	audio_set_AI_volume(60);
	audio_set_AI_Gain(15);

	/*---#回音消除------------------------------------------------------------*/
	/* 回音消除的参数文件位于/etc/webrtc_profile.ini 配置文件中.*/
#if 0
	ret = IMP_AI_EnableAec(AI_DEVICE_ID, AI_CHANNEL_ID, 0, 0);
	if(ret != 0) {
		ERROR_LOG("Audio enable aec failed\n");
		return HLE_RET_ERROR;
	}
#endif

	return HLE_RET_OK; 
	
}

/*******************************************************************************
*@ Description    :AI反初始化
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_in_exit(void)
{
	int ret;
	/* Disable Aec */
	ret = IMP_AI_DisableAec(AI_DEVICE_ID, AI_CHANNEL_ID);
	if(ret != 0) {
		ERROR_LOG("Audio disable aec failed\n");
		return HLE_RET_ERROR;
	}

	/* Step 9: disable the audio channel. */
	ret = IMP_AI_DisableChn(AI_DEVICE_ID, AI_CHANNEL_ID);
	if(ret != 0) {
		ERROR_LOG("Audio channel disable error\n");
		return HLE_RET_ERROR;
	}

	/* Step 10: disable the audio devices. */
	ret = IMP_AI_Disable(AI_DEVICE_ID);
	if(ret != 0) {
		ERROR_LOG("Audio device disable error\n");
		return HLE_RET_ERROR;
	}
	return HLE_RET_OK;
	
}



/*******************************************************************************
*@ Description    :AO初始化
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_out_init(void)
{
	int ret;
	/* Step 1: set public attribute of AO device. */
	IMPAudioIOAttr attr;
	attr.samplerate = AUDIO_SAMPLE_RATE_16000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 640;
	attr.chnCnt = 1;
	ret = IMP_AO_SetPubAttr(AO_DEVICE_ID, &attr);
	if (ret != 0) {
		ERROR_LOG( "IMP_AO_SetPubAttr err\n");
		return HLE_RET_ERROR;
	}

	memset(&attr, 0x0, sizeof(attr));
	ret = IMP_AO_GetPubAttr(AO_DEVICE_ID, &attr);
	if (ret != 0) {
		ERROR_LOG( "IMP_AO_GetPubAttr error!\n");
		return HLE_RET_ERROR;
	}

	printf("-------AO Info--------------------------------------\n");
	printf( "Audio Out GetPubAttr samplerate:%d\n", attr.samplerate);
	printf( "Audio Out GetPubAttr   bitwidth:%d\n", attr.bitwidth);
	printf( "Audio Out GetPubAttr  soundmode:%d\n", attr.soundmode);
	printf( "Audio Out GetPubAttr     frmNum:%d\n", attr.frmNum);
	printf( "Audio Out GetPubAttr  numPerFrm:%d\n", attr.numPerFrm);
	printf( "Audio Out GetPubAttr     chnCnt:%d\n", attr.chnCnt);
	printf("-------------------------------------------------------\n");

	/* Step 2: enable AO device. */
	ret = IMP_AO_Enable(AO_DEVICE_ID);
	if (ret != 0) {
		ERROR_LOG( "IMP_AO_Enable error!\n");
		return HLE_RET_ERROR;
	}

	/* Step 3: enable AO channel. */
//	ret = talkback_ao_start();
//	if (ret != 0) {
//		ERROR_LOG( "talkback_ao_start error!\n");
//		return HLE_RET_ERROR;
//	}

	//talkback_start(0,1); //打开喇叭播放模式
	
	audio_set_AO_volume(80);
	audio_set_AO_Gain(18);	

	return HLE_RET_OK;

}

/*******************************************************************************
*@ Description    :AO反初始化
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_out_exit(void)
{
	int ret;
	ret = IMP_AO_FlushChnBuf(AO_DEVICE_ID, AO_CHANNEL_ID);
	if (ret != 0) {
		ERROR_LOG( "IMP_AO_FlushChnBuf error\n");
		return HLE_RET_ERROR;
	}
	
	/* Step 6: disable the audio channel. */
	ret = talkback_ao_stop();
	if (ret != 0) {
		ERROR_LOG( "talkback_ao_stop error\n");
		return HLE_RET_ERROR;
	}

	/* Step 7: disable the audio devices. */
	ret = IMP_AO_Disable(AO_DEVICE_ID);
	if (ret != 0) {
		ERROR_LOG( "IMP_AO_Disable error\n");
		return HLE_RET_ERROR;
	}
	return HLE_RET_OK;
	
}




/*******************************************************************************
*@ Description    :获取Audio编码帧，线程响应函数
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
#define DEBUG_WRITE_PCM_TO_FILE 	0	  //DEBUG
#define RECORD_PCM_FRAME_NUM 		200   //DEBUG
void* audio_get_PCM_frame_func(void*args)
{
	pthread_detach(pthread_self());

	int ret;
	IMPAudioFrame frm;

	#if DEBUG_WRITE_PCM_TO_FILE
	int record_num = 0;
	FILE *record_file = fopen("/tmp/audio_pcm.pcm", "wb");
	if(record_file == HLE_RET_ERROR) {
		ERROR_LOG("fopen /tmp/audio_pcm.pcm failed\n");
		return HLE_RET_ERROR;
	}
	#endif

	while(1 /*以后要用条件进行控制*/)
	{
		/* Step 6: get audio record frame. */
		ret = IMP_AI_PollingFrame(AI_DEVICE_ID, AI_CHANNEL_ID, 1000);
		if (ret != 0 ) {
			ERROR_LOG("Audio Polling Frame Data error\n");
			sleep(1);
			continue;
		}
		
		ret = IMP_AI_GetFrame(AI_DEVICE_ID, AI_CHANNEL_ID, &frm, BLOCK);
		if(ret != 0) {
			ERROR_LOG("Audio Get Frame Data error\n");
			sleep(1);
			continue;
		}
		
		#if DEBUG_WRITE_PCM_TO_FILE	//debug /* Step 7: Save the recording data to a file. */
			fwrite(frm.virAddr, 1, frm.len, record_file);
		#else	//直接放入循环缓存buffer
			//audio_send_Aframe_to_cirbuf(&frm,AENC_STD_ADPCM); //注意 PCM/AAC/g711 只能放入一种格式的帧到循环buffer
		#endif
	
		//输送到AENC编码
		AENC_G711_encode_one_frame(frm.virAddr,frm.len);
	
		//AI to AO ， MIC数据输出到喇叭 //DEBUG
		audio_send_PCM_stream_to_speacker(frm.virAddr,frm.len);

		
		/* Step 8: release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(AI_DEVICE_ID, AI_CHANNEL_ID, &frm);
		if(ret != 0) {
			ERROR_LOG("Audio release frame data error\n");
			goto ERR;
		}

		#if DEBUG_WRITE_PCM_TO_FILE
		if(++record_num >= RECORD_PCM_FRAME_NUM)
		{
			ERROR_LOG("********************Audio  PCM record success*************************!\n");
			break;
		}
		#endif
			
	
		
	}
		
ERR:
	#if DEBUG_WRITE_PCM_TO_FILE
	fclose(record_file);
	#endif

	pthread_exit(0) ;

}

/*******************************************************************************
*@ Description    :创建“获取PCM编码帧”线程
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_get_PCM_frame_task(void)
{
	pthread_t tid;
	int ret = pthread_create(&tid, NULL, audio_get_PCM_frame_func, NULL);
	if (ret < 0) {
		ERROR_LOG( "Create audio_get_PCM_frame_func fsiled!\n");
		return HLE_RET_ERROR;
	}
			
	return HLE_RET_OK;
}

/*******************************************************************************
*@ Description    :发送PCM音频数据到AO
*@ Input          :<buf>数据指针
					<size>数据大小
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_send_PCM_stream_to_speacker(void *buf,int size)
{
	if(NULL == buf || size <= 0)
	{
		ERROR_LOG("Illegal parameter!\n");
		return HLE_RET_EINVAL;
	}
	
	int ret;
	/* Step 5: send frame data. */
	IMPAudioFrame frm;
	frm.virAddr = (uint32_t *)buf;
	frm.len = size;
	ret = IMP_AO_SendFrame(AO_DEVICE_ID, AO_CHANNEL_ID, &frm, BLOCK);
	if (ret != 0) {
		ERROR_LOG("send Frame Data error\n");
		return HLE_RET_ERROR;
	}

	IMPAudioOChnState play_status;
	ret = IMP_AO_QueryChnStat(AO_DEVICE_ID, AO_CHANNEL_ID, &play_status);
	if (ret != 0) {
		ERROR_LOG("IMP_AO_QueryChnStat error\n");
		return HLE_RET_ERROR;
	}

	return HLE_RET_OK;
}


/*******************************************************************************
*@ Description    :发送音频数据(PCM/AAC/g711)帧到循环缓冲区
*@ Input          :<buf>数据指针
					<size>数据大小
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :注意 PCM/AAC/g711 只能接受一种类型的编码帧，不要将不同类型的
					编码同时放入到循环缓存中去
*******************************************************************************/
static CircularBuffer_t *cirbuf_handle[FS_CHN_NUM] = {0}; 
static char* tmp_Aframe_buf = NULL;		//用于构造一帧完整的数据帧
static int  tmp_Aframe_buf_size = 0;
int audio_send_Aframe_to_cirbuf(void *buf,E_AENC_STANDARD enc_type)
{
	if(NULL == buf)
	{
		ERROR_LOG("Illegal parameter!\n");
		return HLE_RET_ERROR;
	}

	
	/*---#构造帧数据头------------------------------------------------------------*/
	char* real_data = NULL;		//帧真实数据
	int real_len = 0;			//帧真实数据长度
	FRAME_HDR head = {{0x00,0x00,0x01},0xFA};
	AFRAME_INFO info = {0};
	info.enc_type = enc_type;
	info.sample_rate = AUDIO_SR_16000;
	info.bit_width = AUDIO_BW_16;
	info.sound_mode = AUDIO_SM_MONO;
	if(enc_type == AENC_STD_ADPCM)
	{
		IMPAudioFrame * PCM_frame = (IMPAudioFrame *)buf;
		info.length = PCM_frame->len;
		info.pts_msec = (HLE_U64)(PCM_frame->timeStamp/1000); //注意是否要除以1000，SDK没有说明时间戳的单位
		real_data = (char*)PCM_frame->virAddr;
		real_len = PCM_frame->len;
	}
	else if(enc_type == AENC_STD_G711A)
	{
		IMPAudioStream *G711A_frame = (IMPAudioStream*)buf;
		info.length = G711A_frame->len;
		info.pts_msec = (HLE_U64)(G711A_frame->timeStamp/1000); //注意是否要除以1000，SDK没有说明时间戳的单位
		real_data = (char*)G711A_frame->stream;
		real_len = G711A_frame->len;
	}
	else if(enc_type == AENC_STD_AAC)
	{
		ERROR_LOG("not support AAC typpe !\n");
		return HLE_RET_EINVAL;
	}
	else
	{
		ERROR_LOG("not support AENC typpe !\n");
		return HLE_RET_EINVAL;
	}
	

	/*---#缓冲空间申请（用于构造一帧完整的数据）-----------------------------------*/
	int frame_size = sizeof(FRAME_HDR) + sizeof(AFRAME_INFO) + real_len;
	if(tmp_Aframe_buf_size < frame_size || NULL == tmp_Aframe_buf)
	{
		if(NULL == tmp_Aframe_buf)//初次申请
		{
			DEBUG_LOG("tmp_Aframe_buf init..... just onece\n");
			tmp_Aframe_buf = (char*)malloc(frame_size);
			if(NULL == tmp_Aframe_buf)
			{
				return HLE_RET_ENORESOURCE;
			}
			tmp_Aframe_buf_size = frame_size;
		}
		else //原有空间不够大，需要重新申请
		{
			DEBUG_LOG("tmp_Aframe_buf realloc......\n");
			tmp_Aframe_buf = (char*)realloc(tmp_Aframe_buf,frame_size);
			if(NULL == tmp_Aframe_buf)
			{
				return HLE_RET_ENORESOURCE;
			}
			tmp_Aframe_buf_size = frame_size;
		}
		
		memset(tmp_Aframe_buf,0,tmp_Aframe_buf_size);

	}

	
	/*---#拷贝数据------------------------------------------------------------*/
	memcpy(tmp_Aframe_buf,&head,sizeof(FRAME_HDR));
	memcpy(tmp_Aframe_buf+sizeof(FRAME_HDR),&info,sizeof(AFRAME_INFO));
	memcpy(tmp_Aframe_buf+sizeof(FRAME_HDR)+sizeof(AFRAME_INFO),real_data,real_len);

	/*---#放入循环缓冲池------------------------------------------------------------*/
	int i = 0;
	for(i = 0;i<FS_CHN_NUM;i++)
	{
		if(NULL == cirbuf_handle[i])
		{
			ERROR_LOG("cirbuf_handle[%d] not init!\n",i);
			return HLE_RET_EINVAL;
		}
		else
		{
			if(CircularBufferPutOneFrame(cirbuf_handle[i],tmp_Aframe_buf,frame_size) < 0)
			{
				ERROR_LOG("CircularBufferPutOneFrame error!\n");
				return HLE_RET_ERROR;
			}
	
		}
	
	}
	
	

	return HLE_RET_OK;
}


/*******************************************************************************
*@ Description    :音频初始化函数
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_inited = 0;
int audio_init(void)
{
	int ret;

	ret = audio_in_init();
	if(ret < 0) return HLE_RET_ERROR;
	
	ret = audio_out_init();
	if(ret < 0) return HLE_RET_ERROR;
	
	/*---#获取循环缓冲区的 handle ------------------------------------------------------------*/
	int i = 0;
	for(i = 0; i<FS_CHN_NUM;i++)
	{
		E_IMAGE_SIZE resolution = -1;
		if(0 == i)
			resolution = IMAGE_SIZE_1920x1080;
		else if(1 == i)
			resolution = IMAGE_SIZE_640x360;
		else
			resolution = -1;
		
		cirbuf_handle[i] =  CircularBufferGetHandle(resolution);
	
	}

	ret = talkback_init();
	if(ret < 0) return HLE_RET_ERROR;
	
	audio_inited = 1;
	
	return HLE_RET_OK;
}



/*******************************************************************************
*@ Description    :退出音频编码
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :需要先停止对讲 talkback_stop
*******************************************************************************/
int audio_exit(void)
{

	talkback_exit();
	
	audio_in_exit();
	audio_out_exit();

	

	/*---#释放全局变量---------------------------------------------------------*/
	memset(&cirbuf_handle,0,sizeof(cirbuf_handle));
	if(tmp_Aframe_buf)
		free(tmp_Aframe_buf);
	tmp_Aframe_buf_size = 0;
	
	audio_inited = 0;
	
	return HLE_RET_OK;
}


/**********************************************************************************
* 								AENC/ADEC  部分代码  
*	g711 + AAC 编码
***********************************************************************************/
#define AENC_PORTION

/*******************************************************************************
*@ Description    :SDK自带编码（g711/G726）初始化
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
static int AENC_G711_init(void)
{
	int ret;
	
	/* audio encode create channel. */
	IMPAudioEncChnAttr attr;
	attr.type = PT_G711A; /* Use the My method to encoder. if use the system method is attr.type = PT_G711A; */
	attr.bufSize = 20;
	ret = IMP_AENC_CreateChn(AENC_CHANNEL, &attr);
	if(ret != 0) {
		ERROR_LOG("imp audio encode create channel failed\n");
		return HLE_RET_ERROR;
	}

	return HLE_RET_OK;
}

/*******************************************************************************
*@ Description    :
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
static int AENC_G711_encode_one_frame(void * buf_pcm,int len)
{
	int ret;
	/* Send a frame to encode. */
	IMPAudioFrame frm;
	frm.virAddr = (uint32_t *)buf_pcm;
	frm.len = len;
	ret = IMP_AENC_SendFrame(AENC_CHANNEL, &frm);
	if(ret != 0) {
		ERROR_LOG("imp audio encode send frame failed\n");
		return HLE_RET_ERROR;
	}

	return HLE_RET_OK;
	
}

/*******************************************************************************
*@ Description    :“获取g711编码帧”线程响应函数
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int  DEBUG_RECORD_G711_TO_FILE = 0; //1开;0关
#define AUDIO_ENCODE_FILE "/tmp/g711_file.g711"
#define RECORD_G711_FRAME_NUM 	150
void* AENC_G711_get_frame_func(void*args)
{
	pthread_detach(pthread_self());

	int ret;
	int record_num = 0;
	FILE *file_g711 = NULL;
	
	if(DEBUG_RECORD_G711_TO_FILE)
	{
		file_g711 = fopen(AUDIO_ENCODE_FILE, "wb");
		if(file_g711 == NULL) {
			ERROR_LOG("fopen %s failed\n", AUDIO_ENCODE_FILE);
			pthread_exit(0);
		}
	}
	
	while(1/*后续要有退出条件*/) 
	{
		/* get audio encode frame. */
		IMPAudioStream stream;
		ret = IMP_AENC_PollingStream(AENC_CHANNEL, 1000);
		if (ret != 0) {
			ERROR_LOG("imp audio encode polling stream failed\n");
			sleep(1);
			continue;
		}

		ret = IMP_AENC_GetStream(AENC_CHANNEL, &stream, BLOCK);
		if(ret != 0) {
			ERROR_LOG("imp audio encode get stream failed\n");
			sleep(1);
			continue;
		}
		
		
		if(DEBUG_RECORD_G711_TO_FILE)	//保存到文件 
		{
			fwrite(stream.stream, 1, stream.len, file_g711);
		}
		else //	保存到循环缓冲区
		{
			audio_send_Aframe_to_cirbuf(&stream,AENC_STD_G711A);
		}
		
		/* release stream. */
		ret = IMP_AENC_ReleaseStream(AENC_CHANNEL, &stream);
		if(ret != 0) {
			ERROR_LOG("imp audio encode release stream failed\n");
			break;
		}

		if(DEBUG_RECORD_G711_TO_FILE && ++record_num >= RECORD_G711_FRAME_NUM)
		{
			ERROR_LOG("********************Audio G711 record success*************************!\n");
			DEBUG_RECORD_G711_TO_FILE = 0;
			break;
		}
	}
	
	if(DEBUG_RECORD_G711_TO_FILE) 
		fclose(file_g711);
	
	pthread_exit(0) ;
	
}

/*******************************************************************************
*@ Description    :创建“获取g711编码帧”线程
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int AENC_G711_get_frame_task(void)
{
	pthread_t tid;
	int ret = pthread_create(&tid, NULL, AENC_G711_get_frame_func, NULL);
	if (ret < 0) {
		ERROR_LOG( "Create AENC_G711_get_frame_task failed!\n");
		return HLE_RET_ERROR;
	}
			
	return HLE_RET_OK;

}

static int AENC_G711_exit(void)
{
	int ret;
	/* destroy the encode channel. */
	ret = IMP_AENC_DestroyChn(AENC_CHANNEL);
	if(ret != 0) {
		ERROR_LOG("imp audio encode destroy channel failed\n");
		return HLE_RET_ERROR;
	}

	return HLE_RET_OK;
}






#define TALKBACK_PORTION


static int talkback_adec_start(void)
{
	int ret;
	/* audio decoder create channel. */
	IMPAudioDecChnAttr attr;
	attr.type = PT_G711A;
	attr.bufSize = 20;
	attr.mode = ADEC_MODE_PACK;
	ret = IMP_ADEC_CreateChn(ADEC_CHANNEL, &attr);
	if(ret != 0) {
		ERROR_LOG("imp audio decoder create channel failed\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_ADEC_ClearChnBuf(ADEC_CHANNEL);
	if(ret != 0) {
		ERROR_LOG("IMP_ADEC_ClearChnBuf failed\n");
		return HLE_RET_ERROR;
	}

	return HLE_RET_OK;
}

/*******************************************************************************
*@ Description    :解码一帧g711数据,并播放
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
static int talkback_adec_G711_frame_and_play(void*buf_g711,int len)
{
	if(NULL == buf_g711 || len <= 0)
		return HLE_RET_EINVAL;
	
	int ret;
	/* Send a frame to decoder. */
	IMPAudioStream stream_in;
	stream_in.stream = (uint8_t *)buf_g711;
	stream_in.len = len;
	ret = IMP_ADEC_SendStream(ADEC_CHANNEL, &stream_in, BLOCK);
	if(ret != 0) {
		ERROR_LOG("imp audio encode send frame failed\n");
		return HLE_RET_ERROR;
	}

	/* get audio decoder frame. */
	IMPAudioStream stream_out;
	ret = IMP_ADEC_PollingStream(ADEC_CHANNEL, 1000);
	if(ret != 0) {
		ERROR_LOG("imp audio encode polling stream failed\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_ADEC_GetStream(ADEC_CHANNEL, &stream_out, BLOCK);
	if(ret != 0) {
		ERROR_LOG("imp audio decoder get stream failed\n");
		return HLE_RET_ERROR;
	}

	//发送到AO 播放
	ret = audio_send_PCM_stream_to_speacker(stream_out.stream,stream_out.len);
	if(ret != 0) {
		ERROR_LOG("audio_send_PCM_stream_to_speacker failed\n");
		return HLE_RET_ERROR;
	}
		
	/* release stream. */
	ret = IMP_ADEC_ReleaseStream(ADEC_CHANNEL, &stream_out);
	if(ret != 0) {
		ERROR_LOG("imp audio decoder release stream failed\n");
		return HLE_RET_ERROR;
	}

	return HLE_RET_OK;

}



static int talkback_adec_stop(void)
{
	int ret;
	/* destroy the decoder channel. */
	ret = IMP_ADEC_DestroyChn(ADEC_CHANNEL);
	if(ret != 0) {
		ERROR_LOG("imp audio decoder destroy channel failed\n");
		return HLE_RET_ERROR;
	}
	return HLE_RET_OK;
}


static int talkback_ao_start(void)
{
	int ret;
	/* Step 3: enable AO channel. */
	ret = IMP_AO_EnableChn(AO_DEVICE_ID, AO_CHANNEL_ID);
	if (ret != 0) {
		ERROR_LOG( "IMP_AO_EnableChn failed\n");
		return HLE_RET_ERROR;
	}
	return HLE_RET_ERROR;

}

static int talkback_ao_stop(void)
{
	int ret;
	/* Step 6: disable the audio channel. */
	ret = IMP_AO_DisableChn(AO_DEVICE_ID, AO_CHANNEL_ID);
	if (ret != 0) {
		ERROR_LOG( "IMP_AO_DisableChn error\n");
		return HLE_RET_ERROR;
	}
	return HLE_RET_ERROR;
	
}





/*******************************************************************************
*@ Description    :对讲，获取本地编码数据
*@ Input          :
*@ Output         :<data>数据的首地址(编码数据格式：g711a)
					<size>数据的长度
*@ Return         :
*@ attention      :没有数据时会阻塞
					调用顺序：
					audio_init()
					audio_get_PCM_frame_task()
					AENC_G711_get_frame_task()
					talkback_start()
					talkback_get_data()
*******************************************************************************/
int talkback_get_data(void *data, int *size)
{
	if (audio_inited == 0)
        return HLE_RET_ENOTINIT;

    pthread_mutex_lock(&tb_ctx.lock);
    if (tb_ctx.getdata_enable == 0) {
        pthread_mutex_unlock(&tb_ctx.lock);
        return HLE_RET_ENOTINIT;
    }

    while (fifo_len(tb_ctx.fifo, 0) == 0) {
        pthread_cond_wait(&tb_ctx.cond, &tb_ctx.lock);
    }

    *size = fifo_out(tb_ctx.fifo, data, *size, 0);

    pthread_mutex_unlock(&tb_ctx.lock);
    return HLE_RET_OK;

}

/*******************************************************************************
*@ Description    :对讲，数据播放到speaker
*@ Input          :<data>数据的首地址(编码数据格式：g711a)
					<size>数据的长度
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :	调用顺序：
					audio_init()
					audio_get_PCM_frame_task()
					AENC_G711_get_frame_task()
					talkback_start()
					talkback_put_data()
*******************************************************************************/
int talkback_put_data(void *data, int size)
{
	if (audio_inited == 0)
        return HLE_RET_ENOTINIT;

	int ret;
    pthread_mutex_lock(&tb_ctx.lock);
    if (tb_ctx.putdata_enable == 0) {
        pthread_mutex_unlock(&tb_ctx.lock);
        return HLE_RET_ENOTINIT;
    }
    pthread_mutex_unlock(&tb_ctx.lock);

#if 1
	ret = talkback_adec_G711_frame_and_play(data,size);
	if(ret < 0)
	{
		ERROR_LOG("talkback_adec_G711_frame_and_play failed !\n");
		return HLE_RET_ERROR;
	}
#else
    // todo
#endif

    return HLE_RET_OK;

}

int talkback_init(void)
{
	
	int ret;
	
	/*---#初始化对讲缓存区------------------------------------------------------------*/
	tb_ctx.getdata_enable = 0;
	tb_ctx.putdata_enable = 0;
	tb_ctx.fifo = fifo_malloc(TALKBACK_FIFO_SIZE);
	if (tb_ctx.fifo == NULL)
        return HLE_RET_ERROR;

    pthread_mutex_init(&tb_ctx.lock, NULL);
    pthread_cond_init(&tb_ctx.cond, NULL);
	
	/*---#初始化编解码通道------------------------------------------------------------*/
	ret = AENC_G711_init();
	if(ret < 0)
	{
		ERROR_LOG("AENC_G711_init error!\n");
		return HLE_RET_ERROR;
	}

	


	return HLE_RET_OK;

}



/*******************************************************************************
*@ Description    :开启对讲
*@ Input          :<getmode> 0:关闭MIC数据采集模式；1：打开MIC数据采集模式
					<putmode>0:关闭喇叭播放模式； 1：打开喇叭播放模式
*@ Output         :
*@ Return         :
*@ attention      :两个模式至少打开一个（不能同时为0）
					需要先初始化（audio_init）
*******************************************************************************/
int talkback_start(int getmode, int putmode)
{
	if ((audio_inited == 0) || ((getmode == 0) && (putmode == 0)))
		   return HLE_RET_ENOTINIT;
	
	   pthread_mutex_lock(&tb_ctx.lock);
	   if (putmode) //喇叭播放模式
	   {
		   if (tb_ctx.putdata_enable) {
			   pthread_mutex_unlock(&tb_ctx.lock);
			   return HLE_RET_EBUSY;
		   }
	
		   if (HLE_RET_OK != talkback_adec_start()) {
			   pthread_mutex_unlock(&tb_ctx.lock);
			   return HLE_RET_ERROR;
		   }
	
		   if (HLE_RET_OK != talkback_ao_start()) {
			   talkback_adec_stop();
			   pthread_mutex_unlock(&tb_ctx.lock);
			   return HLE_RET_ERROR;
		   }
	
		   tb_ctx.putdata_enable = 1;
	   }
	
	   if (getmode) { //MIC数据采集模式
		   if (tb_ctx.getdata_enable) {
			   pthread_mutex_unlock(&tb_ctx.lock);
			   return HLE_RET_EBUSY;
		   }
	
		   fifo_reset(tb_ctx.fifo, 0);
		   tb_ctx.getdata_enable = 1;
	   }
	
	   pthread_mutex_unlock(&tb_ctx.lock);
	   DEBUG_LOG("talkback_start success\n");
	   return HLE_RET_OK;

	
}


/*******************************************************************************
*@ Description    :停止对讲
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
int talkback_stop(void)
{
	if (audio_inited == 0)
    return HLE_RET_ENOTINIT;

    pthread_mutex_lock(&tb_ctx.lock);
    if (tb_ctx.putdata_enable != 0) {
        talkback_ao_stop();
        talkback_adec_stop();
        tb_ctx.putdata_enable = 0;
    }

    tb_ctx.getdata_enable = 0;
    pthread_mutex_unlock(&tb_ctx.lock);

    return HLE_RET_OK;
	
}


int talkback_exit(void)
{
	//int ret;
	/*---#释放对讲缓存区------------------------------------------------------------*/\
	tb_ctx.putdata_enable = 0;
    tb_ctx.getdata_enable = 0;
    fifo_free_(tb_ctx.fifo, 0);
    tb_ctx.fifo = NULL;
    pthread_mutex_destroy(&tb_ctx.lock);
    pthread_cond_destroy(&tb_ctx.cond);
	
	/*---#退出编解码通道------------------------------------------------------------*/
	AENC_G711_exit();
	
	return HLE_RET_OK;
}





























