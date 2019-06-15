 
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

int audio_send_frame_to_cirbuf(IMPAudioFrame *buf);


/*******************************************************************************
*@ Description    :音频输入音量设置
*@ Input          :<chnVol>音频输入音量  [-30 ~ 120]
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int set_AI_volume(int chnVol)
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
int set_AI_Gain(int aigain)
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
int set_AO_volume(int chnVol)
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
int set_AO_Gain(int aogain)
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
	set_AI_volume(60);
	set_AI_Gain(15);

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
	ret = IMP_AO_EnableChn(AO_DEVICE_ID, AO_CHANNEL_ID);
	if (ret != 0) {
		ERROR_LOG( "IMP_AO_EnableChn failed\n");
		return HLE_RET_ERROR;
	}

	set_AO_volume(80);
	set_AO_Gain(18);	

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
	ret = IMP_AO_DisableChn(AO_DEVICE_ID, AO_CHANNEL_ID);
	if (ret != 0) {
		ERROR_LOG( "IMP_AO_DisableChn error\n");
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
#define DEBUG_WRITE_TO_FILE 0	  //DEBUG
#define IMP_AUDIO_RECORD_NUM 200  //DEBUG
void* audio_get_PCM_stream_func(void*args)
{
	pthread_detach(pthread_self());

	int ret;
	IMPAudioFrame frm;
#if DEBUG_WRITE_TO_FILE
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
		
#if DEBUG_WRITE_TO_FILE	//debug /* Step 7: Save the recording data to a file. */
		fwrite(frm.virAddr, 1, frm.len, record_file);
#else	//直接放入循环缓存buffer
		audio_send_frame_to_cirbuf(&frm);
#endif

	
		//AI to  AO MIC数据输出到喇叭
		audio_send_PCM_stream_to_speacker(frm.virAddr,frm.len);

		
		/* Step 8: release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(AI_DEVICE_ID, AI_CHANNEL_ID, &frm);
		if(ret != 0) {
			ERROR_LOG("Audio release frame data error\n");
			goto ERR;
		}

#if DEBUG_WRITE_TO_FILE
		if(++record_num >= IMP_AUDIO_RECORD_NUM)
		{
			ERROR_LOG("********************Audio  PCM record success*************************!\n");
			break;
		}
#endif
			
	
		
	}
		
ERR:
#if DEBUG_WRITE_TO_FILE
	fclose(record_file);
#endif
	pthread_exit(0) ;

}

/*******************************************************************************
*@ Description    :创建获取音频码流的线程
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_get_PCM_stream_task(void)
{
	pthread_t tid;
	int ret = pthread_create(&tid, NULL, audio_get_PCM_stream_func, NULL);
	if (ret < 0) {
		ERROR_LOG( "Create audio_get_PCM_stream_func fsiled!\n");
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
*@ Description    :发送音频数据(PCM/AAC)帧到循环缓冲区
*@ Input          :<buf>数据指针
					<size>数据大小
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
static CircularBuffer_t *cirbuf_handle[FS_CHN_NUM] = {0}; 
static char* tmp_Aframe_buf = NULL;		//用于构造一帧完整的数据帧
static int  tmp_Aframe_buf_size = 0;
int audio_send_frame_to_cirbuf(IMPAudioFrame *buf)
{
	if(NULL == buf)
	{
		ERROR_LOG("Illegal parameter!\n");
		return HLE_RET_ERROR;
	}

	
	/*---#构造帧数据头------------------------------------------------------------*/
	FRAME_HDR head = {{0x00,0x00,0x01},0xFA};
	AFRAME_INFO info = {0};
	info.enc_type = AENC_STD_ADPCM;
	info.sample_rate = AUDIO_SR_16000;
	info.bit_width = AUDIO_BW_16;
	info.sound_mode = AUDIO_SM_MONO;
	info.length = buf->len;
	info.pts_msec = buf->timeStamp; //注意是否要除以1000，SDK没有说明时间戳的单位

	/*---#缓冲空间申请（用于构造一帧完整的数据）-----------------------------------*/
	int frame_size = sizeof(FRAME_HDR) + sizeof(AFRAME_INFO) + buf->len;
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
	memcpy(tmp_Aframe_buf+sizeof(FRAME_HDR)+sizeof(AFRAME_INFO),buf->virAddr,buf->len);

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
int audio_init(void)
{
	//int ret;

	audio_in_init();
	audio_out_init();
	
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

	
	return HLE_RET_OK;
}



/*******************************************************************************
*@ Description    :退出音频编码
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_exit(void)
{

	audio_in_exit();
	audio_out_exit();

	/*---#释放全局变量---------------------------------------------------------*/
	memset(&cirbuf_handle,0,sizeof(cirbuf_handle));
	if(tmp_Aframe_buf)
		free(tmp_Aframe_buf);
	tmp_Aframe_buf_size = 0;
		
	
	return HLE_RET_OK;
}

























