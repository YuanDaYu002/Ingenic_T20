 
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
#include "imp_audio.h"
#include "audio.h"
#include "typeport.h"

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
	ret = IMP_AI_SetVol(AUDIO_DEVICE_ID, AI_CHANNEL_ID, chnVol);
	if(ret != 0) {
		ERROR_LOG(" Set AI volume failed\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_AI_GetVol(AUDIO_DEVICE_ID, AI_CHANNEL_ID, &chnVol);
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
	ret = IMP_AI_SetGain(AUDIO_DEVICE_ID, AI_CHANNEL_ID, aigain);
	if(ret != 0) {
		ERROR_LOG("Set AI Gain failed\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_AI_GetGain(AUDIO_DEVICE_ID, AI_CHANNEL_ID, &aigain);
	if(ret != 0) {
		ERROR_LOG("Get AI Gain failed\n");
		return HLE_RET_ERROR;
	}
	DEBUG_LOG("AI GetGain gain : %d\n", aigain);

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
	int ret;

	/* Step 1: set public attribute of AI device. */
	IMPAudioIOAttr attr;
	attr.samplerate = AUDIO_SAMPLE_RATE_8000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 320;
	attr.chnCnt = 1;
	ret = IMP_AI_SetPubAttr(AUDIO_DEVICE_ID, &attr);
	if(ret != 0) {
		ERROR_LOG("set ai %d attr err: %d\n", AUDIO_DEVICE_ID, ret);
		return HLE_RET_ERROR;
	}

	memset(&attr, 0x0, sizeof(attr));
	ret = IMP_AI_GetPubAttr(AUDIO_DEVICE_ID, &attr);
	if(ret != 0) {
		ERROR_LOG("get ai %d attr err: %d\n", AUDIO_DEVICE_ID, ret);
		return HLE_RET_ERROR;
	}
	
	printf("-------Audio Info--------------------------------------\n");
	printf( "Audio In GetPubAttr samplerate : %d\n", attr.samplerate);
	printf( "Audio In GetPubAttr   bitwidth : %d\n", attr.bitwidth);
	printf( "Audio In GetPubAttr  soundmode : %d\n", attr.soundmode);
	printf( "Audio In GetPubAttr     frmNum : %d\n", attr.frmNum);
	printf( "Audio In GetPubAttr  numPerFrm : %d\n", attr.numPerFrm);
	printf( "Audio In GetPubAttr     chnCnt : %d\n", attr.chnCnt);
	printf("-------------------------------------------------------\n");

	/* Step 2: enable AI device. */
	ret = IMP_AI_Enable(AUDIO_DEVICE_ID);
	if(ret != 0) {
		ERROR_LOG( "Enable ai %d err\n", AUDIO_DEVICE_ID);
		return HLE_RET_ERROR;
	}

	/* Step 3: set audio channel attribute of AI device. */
	IMPAudioIChnParam chnParam;
	chnParam.usrFrmDepth = 20;
	ret = IMP_AI_SetChnParam(AUDIO_DEVICE_ID, AI_CHANNEL_ID, &chnParam);
	if(ret != 0) {
		ERROR_LOG( "set ai %d channel %d attr err: %d\n", AUDIO_DEVICE_ID, AI_CHANNEL_ID, ret);
		return HLE_RET_ERROR;
	}

	memset(&chnParam, 0x0, sizeof(chnParam));
	ret = IMP_AI_GetChnParam(AUDIO_DEVICE_ID, AI_CHANNEL_ID, &chnParam);
	if(ret != 0) {
		ERROR_LOG( "get ai %d channel %d attr err: %d\n", AUDIO_DEVICE_ID, AI_CHANNEL_ID, ret);
		return HLE_RET_ERROR;
	}
	DEBUG_LOG( "Audio In GetChnParam usrFrmDepth : %d\n", chnParam.usrFrmDepth);

	/* Step 4: enable AI channel. */
	ret = IMP_AI_EnableChn(AUDIO_DEVICE_ID, AI_CHANNEL_ID);
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
	ret = IMP_AI_EnableAec(AUDIO_DEVICE_ID, AI_CHANNEL_ID, 0, 0);
	if(ret != 0) {
		ERROR_LOG("Audio enable aec failed\n");
		return HLE_RET_ERROR;
	}
#endif
	
		
	return HLE_RET_OK;
}

/*******************************************************************************
*@ Description    :获取Audio编码帧，线程响应函数
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
#define DEBUG_WRITE_TO_FILE 1	  //DEBUG
#define IMP_AUDIO_RECORD_NUM 100  //DEBUG
void* audio_get_PCM_stream_func(void*args)
{
	pthread_detach(pthread_self());

	int ret;
	int record_num = 0;
	IMPAudioFrame frm;
#if DEBUG_WRITE_TO_FILE
	FILE *record_file = fopen("/tmp/audio_pcm.pcm", "wb");
	if(record_file == NULL) {
		ERROR_LOG("fopen /tmp/audio_pcm.pcm failed\n");
		return NULL;
	}
#endif
	while(1 /*以后要用条件进行控制*/)
	{
		/* Step 6: get audio record frame. */
		ret = IMP_AI_PollingFrame(AUDIO_DEVICE_ID, AI_CHANNEL_ID, 1000);
		if (ret != 0 ) {
			ERROR_LOG("Audio Polling Frame Data error\n");
			sleep(1);
			continue;
		}
		
		ret = IMP_AI_GetFrame(AUDIO_DEVICE_ID, AI_CHANNEL_ID, &frm, BLOCK);
		if(ret != 0) {
			ERROR_LOG("Audio Get Frame Data error\n");
			sleep(1);
			continue;
		}
		
#if DEBUG_WRITE_TO_FILE	//debug 直接保存到文件
		/* Step 7: Save the recording data to a file. */
		fwrite(frm.virAddr, 1, frm.len, record_file);
#else	//直接放入循环缓存buffer

#endif

		/* Step 8: release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(AUDIO_DEVICE_ID, AI_CHANNEL_ID, &frm);
		if(ret != 0) {
			ERROR_LOG("Audio release frame data error\n");
			goto ERR;
		}

		if(++record_num >= IMP_AUDIO_RECORD_NUM)
		{
			ERROR_LOG("********************Audio  PCM record success*************************!\n");
			break;
		}
			
	
		
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
*@ Description    :退出音频编码
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK ；失败 ： 错误码
*@ attention      :
*******************************************************************************/
int audio_exit(void)
{
	int ret;
	/* Disable Aec */
	ret = IMP_AI_DisableAec(AUDIO_DEVICE_ID, AI_CHANNEL_ID);
	if(ret != 0) {
		ERROR_LOG("Audio disable aec failed\n");
		return HLE_RET_ERROR;
	}

	/* Step 9: disable the audio channel. */
	ret = IMP_AI_DisableChn(AUDIO_DEVICE_ID, AI_CHANNEL_ID);
	if(ret != 0) {
		ERROR_LOG("Audio channel disable error\n");
		return HLE_RET_ERROR;
	}

	/* Step 10: disable the audio devices. */
	ret = IMP_AI_Disable(AUDIO_DEVICE_ID);
	if(ret != 0) {
		ERROR_LOG("Audio device disable error\n");
		return HLE_RET_ERROR;
	}
	
	return HLE_RET_OK;
}















