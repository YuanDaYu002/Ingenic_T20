 /***************************************************************************
* @file:aac.c 
* @author:   
* @date:  6,19,2019
* @brief:  
* @attention:AAC编码相关接口函数
***************************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#include "aac.h"
#include "typeport.h"

 /*======================================================================================
								 AAC编码部分
 采样率：16KHZ
 每帧样本数：n (160 、320 、 480 ) 
 帧率：16000/n
 =======================================================================================*/

 static unsigned int send_PCM_count = 0; //统计接收（发送）的PCM帧数（在一段时间内）
 
 int debug_count = 0;
 static unsigned int PCM_residue = 0; //PCM帧在 aac_config.PCMBuffer没能放下的部分数据大小
 static unsigned char PCM_residue_buf[PCM_FRAME_SIZE] = {0};//PCM帧在 aac_config.PCMBuffer没能放下的部分数据(备份)
 static unsigned int get_PCM_count = 0; //统计接收端编码一帧AAC数据收到的PCM帧数。

 
 aac_config_t aac_config = {0};
 typedef struct _audFrm_t
 {
	 AUDIO_FRAME_S	 audFrm;	 //发送给AAC编码线程的原始音频数据帧信息
	 pthread_mutex_t audFrm_mut;
	 pthread_cond_t  audFrm_cnd;
 
 }audFrm_t;
 audFrm_t audFrm_flg = {{0}};
 
 /*---# DEBUG 记录AAC编码数据到本地文件------------------------------------------------------------*/
#define AAC_FILE_BUF_SIZE (1024*40)
 typedef struct _AAC_record_file_t
 {
	 int				 AAC_file_fd;
	 unsigned char* 	 AAC_file_buf;
	 unsigned char* 	 AAC_file_buf_Wpos;//写指针位置
	 int				 recod_exit;//写完文件后标记退出
 }AAC_record_file_t;
 AAC_record_file_t AAC_file = {0};
#define switch_record_AAC  0 	//1：打开总控制开关 0：关闭



 
 /*******************************************************************************
 *@ Description    :初始化AAC编码
 *@ Input		   :
 *@ Output		   :
 *@ Return		   :成功：配置好的AAC参数指针（是个全局变量的地址，不需要释放）
					 失败：NULL
 *@ attention	   :
 *******************************************************************************/
 aac_config_t* AAC_encode_init(void)
 {
 
	 /*-DEBUG 用于记录AAC文件------------------------------------------*/
#if switch_record_AAC
	 AAC_file.AAC_file_fd = open("/tmp/aac_get.AAC", O_CREAT | O_WRONLY | O_TRUNC, 0664);
	 if(AAC_file.AAC_file_fd < 0)
	 {
		 ERROR_LOG("open file failed !\n");
		 return NULL;
	 }
 
	 AAC_file.AAC_file_buf = (unsigned char*)malloc(AAC_FILE_BUF_SIZE);
	 if(NULL == AAC_file.AAC_file_buf)
	 {
			 ERROR_LOG("malloc buf failed !\n");
			 return NULL;
	 }
	 memset(AAC_file.AAC_file_buf,0,AAC_FILE_BUF_SIZE);
	 AAC_file.AAC_file_buf_Wpos = AAC_file.AAC_file_buf;
#endif
	 /*----------------------------------------------------------------*/
	 
	 int ret;
	 
	aac_config.SampleRate = PCM_SAMPLE_RATE; //16KHZ
	aac_config.Channels = 1;
	aac_config.PCMBitSize = 16;
	//aac_config.PCMBuffer_size = PCM_BUF_SIZE; //BUG
	aac_config.InputSamples = 0;		 //注意该参数是后边faac编码接口的传入样本数量，并不是要我们输入一个样本数值
	aac_config.MaxOutputBytes = 0;//8192;	 //同上; 
 
	 /*用于faac 编码 凑PCM样本数的缓存空间初始化*/
	 aac_config.EncHandle =  faacEncOpen(aac_config.SampleRate, \
										 aac_config.Channels, \
										 &aac_config.InputSamples, \
										 &aac_config.MaxOutputBytes);
	 if(aac_config.EncHandle == NULL)
	 {
		 DEBUG_LOG("failed to call faacEncOpen()\n");
		 return NULL;
	 }
	 
	 aac_config.PCMBuffer_size =  aac_config.InputSamples * (aac_config.PCMBitSize / 8);//PCM_BUF_SIZE;
	 DEBUG_LOG("InputSamples(%ld) MaxOutputBytes(%ld) PCMBuffer_size(%d)\n",\
				aac_config.InputSamples,aac_config.MaxOutputBytes,aac_config.PCMBuffer_size);
	 aac_config.PCMBuffer = (unsigned char*)malloc(aac_config.PCMBuffer_size * sizeof(unsigned char));
	 aac_config.AACBuffer = (unsigned char*)malloc(aac_config.MaxOutputBytes * sizeof(unsigned char));
	 memset(aac_config.PCMBuffer, 0, aac_config.PCMBuffer_size);
	 memset(aac_config.AACBuffer, 0, aac_config.MaxOutputBytes);
	
	 
	 //获取当前配置参数
	  faacEncConfigurationPtr pConfiguration = faacEncGetCurrentConfiguration(aac_config.EncHandle);
 
	 //调整配置参数
	#if 1 
		 pConfiguration->inputFormat = FAAC_INPUT_16BIT;
		 pConfiguration->outputFormat = 0; /*0 - raw; 1 - ADTS 本地播放请选择 1 */  
		 pConfiguration->bitRate = PCM_SAMPLE_RATE;  //库内部默认为64000
		 pConfiguration->useTns = 0;
		 pConfiguration->allowMidside = 1;
		 pConfiguration->shortctl = SHORTCTL_NORMAL;
		 pConfiguration->aacObjectType = LOW;
		 pConfiguration->mpegVersion = MPEG4;//MPEG2
		 //pConfiguration->useLfe = 1;
	#endif  
 
	 //重新设置回去
	 ret = faacEncSetConfiguration(aac_config.EncHandle,pConfiguration);
	 if(ret != 1)
	 {
		 ERROR_LOG("failed to call faacEncSetConfiguration()\n");
		 return NULL;
	 }
 
 
 
	 pthread_mutex_init(&audFrm_flg.audFrm_mut, NULL);
	 pthread_cond_init(&audFrm_flg.audFrm_cnd, NULL);

//该部分已经在 audio_get_PCM_frame_func 线程做了
//创建AAC编码客户端线程（获取audio源数据并发送给AAC编码server线程）
//	 pthread_t tid;
//	 lRunAiAenc = 1;
//	 ret = pthread_create(&tid, NULL,ai_AAC_proc, NULL);
//	 if (ret) {
//		 ERROR_LOG("pthread_create fail: %d\n", ret);
//		 return NULL;
//	 }
//	 pthread_detach(tid);
		 
 
	 
	 DEBUG_LOG("AAC_encode_init success!\n");
 
	 /*---------获取解码参数---------------------------------------------------------*/
#if 1
	 unsigned char* pDSI = NULL;
	 unsigned long int ulDsiSize = 0;
	 unsigned long int nSampleRate = 0;
	 if (0 == faacEncGetDecoderSpecificInfo(aac_config.EncHandle, &pDSI, &ulDsiSize))
	 {
		 //unsigned int nBit = 16;
		 unsigned int nChannels = (pDSI[1]&0x7F)>>3;
		 int ints32SampleRateIndex = ((pDSI[0]&0x7)<<1)|(pDSI[1]>>7);
		 switch (ints32SampleRateIndex)
		 {
		 case 0x0:
			 nSampleRate = 96000;
			 break;
		 case 0x1:
			 nSampleRate = 88200;
			 break;
		 case 0x2:
			 nSampleRate = 64000;
			 break;
		 case 0x3:
			 nSampleRate = 48000;
			 break;
		 case 0x4:
			 nSampleRate = 44100;
			 break;
		 case 0x5:
			 nSampleRate = 32000;
			 break;
		 case 0x6:
			 nSampleRate = 24000;
			 break;
		 case 0x7:
			 nSampleRate = 22050;
			 break;
		 case 0x8:
			 nSampleRate = 16000;
			 break;
		 case 0x9:
			 nSampleRate = 2000;
			 break;
		 case 0xa:
			 nSampleRate = 11025;
			 break;
		 case 0xb:
			 nSampleRate = 8000;
			 break;
		 default:
			 nSampleRate = 44100;
			 break;
		 }
 
		 printf("\n-----faacEncGetDecoderSpecificInfo------\n");
		 printf("nChannels	:%d\n",nChannels);
		 printf("nSampleRate:%ld\n",nSampleRate);
		 printf("----------------------------------------\n\n");
	 }
	 else
	 {
		 ERROR_LOG("faacEncGetDecoderSpecificInfo failed!\n");
	 }
#endif
	 /*------------------------------------------------------------------------*/
	 
	 return &aac_config;
 }
 
 
 /*******************************************************************************
 *@ Description    :编码一帧AAC帧
 *@ Input		   :<inputBuffer> 输入的PCM数据buf
					 <samplesInput>输入的PCM数据所包含采样的样本数
 *@ Output		   :<outputBuffer>编码后的AAC数据指针
					 （全局缓冲空间，单次调用不需要释放，AAC编码退出统一释放）
					 <bufferSize>编码后的AAC数据大小
 *@ Return		   :成功：返回编码成AAC帧的字节数（大于0）
					 失败： 小于等于0的数
 *@ attention	   :
 *******************************************************************************/
 int AAC_encode(int * inputBuffer,
					 unsigned int samplesInput,
					 unsigned char *outputBuffer,
					 unsigned int bufferSize)
 {
	 if(NULL == aac_config.EncHandle)
	 {
		 ERROR_LOG("AAC encoder not init ....\n");
		 return -1;
	 }
	 
	 return faacEncEncode(aac_config.EncHandle,inputBuffer,samplesInput,outputBuffer,bufferSize);
 }
 
  
 /*******************************************************************************
 *@ Description    :发送一帧audio数据给AAC编码线程编码
 *@ Input		   :<pstFrm >音频帧结构体指针
 *@ Output		   :
 *@ Return		   : 成功 ： 0
					 失败： -1
 *@ attention	   :
 *******************************************************************************/
 int AAC_AENC_SendFrame( const AUDIO_FRAME_S *pstFrm)
 {
	 if(NULL == pstFrm)
	 {
		 ERROR_LOG("pstFrm is NULL!\n");
		 return -1;
	 }
	 
	 pthread_mutex_lock(&audFrm_flg.audFrm_mut); 
		 memcpy(&audFrm_flg.audFrm,pstFrm,sizeof(AUDIO_FRAME_S));
		 send_PCM_count++;	 
		 pthread_cond_signal(&audFrm_flg.audFrm_cnd);
	 pthread_mutex_unlock(&audFrm_flg.audFrm_mut);
	 
	 return 0;
 }
 
 /*******************************************************************************
 *@ Description    :获取一帧编码后的AAC数据（编码业务在该函数内进行）
 *@ Input		   :
 *@ Output		   :<pstStream>(输出)编码后AAC数据存放buf的首地址
 *@ Return		   :成功： AAC编码帧的实际长度
					 失败：-1
 *@ attention	   :
 *******************************************************************************/

 int AAC_AENC_getFrame(AUDIO_STREAM_S *pstStream)
 {
	 if(NULL == pstStream)
	 {
		 ERROR_LOG("AAC_server_fd not init !\n");
		 return -1;
	 }
	 int ret = -1;
	 unsigned int out_len = 0;
	 AUDIO_FRAME_S audFrm_bak; //PCM帧描述信息(备份)
	 //unsigned char PCM_buf[PCM_FRAME_SIZE] = {0};//PCM帧实际数据(备份)
	 unsigned int PCMBuffer_offset = 0;
		 
 AAC_OUT_LEN_IS_0:
	 
	 pthread_mutex_lock(&audFrm_flg.audFrm_mut);
		 pthread_cond_wait(&audFrm_flg.audFrm_cnd, &audFrm_flg.audFrm_mut);
		 
		 memcpy(&audFrm_bak,&audFrm_flg.audFrm,sizeof(AUDIO_FRAME_S)); //备份PCM帧描述信息
		 //memcpy(&PCM_buf,audFrm_flg.audFrm.pVirAddr[0],audFrm_flg.audFrm.u32Len); //备份PCM帧实际数据
		 get_PCM_count ++;
		 //进行AAC编码
		 debug_count ++;
 
			 //需要进行凑数据处理
			 if(PCM_residue > 0)//上次循环还有数据没放完，先放上次剩余的部分
			 {
				 memcpy(aac_config.PCMBuffer + PCMBuffer_offset,PCM_residue_buf,PCM_residue); 
				 PCMBuffer_offset += PCM_residue;
				 PCM_residue = 0;
			 }
			 
			 if(aac_config.PCMBuffer_size - PCMBuffer_offset > 0)//缓冲区原始PCM数据没满，凑数据
			 {
			 
				 if(aac_config.PCMBuffer_size - PCMBuffer_offset >= audFrm_flg.audFrm.len)//能直接放入当前PCM帧
				 {
					 memcpy(aac_config.PCMBuffer + PCMBuffer_offset,audFrm_flg.audFrm.virAddr,audFrm_flg.audFrm.len); 
					 PCMBuffer_offset += audFrm_flg.audFrm.len;
				 }
				 else //原始PCM数据没凑满,但不能直接放入当前PCM帧
				 {
					 unsigned int PCMBuffer_residue = aac_config.PCMBuffer_size - PCMBuffer_offset;//剩余空间大小
					 memcpy(aac_config.PCMBuffer + PCMBuffer_offset,audFrm_flg.audFrm.virAddr,PCMBuffer_residue); //只放入一部分
					 PCMBuffer_offset += PCMBuffer_residue;
					 PCM_residue = audFrm_flg.audFrm.len - PCMBuffer_residue;//PCM帧剩余的部分大小记录
					 memcpy(PCM_residue_buf,(unsigned char*)audFrm_flg.audFrm.virAddr + PCMBuffer_residue,PCM_residue);//剩余部分进行数据备份
				 }
				 
				 
			 
			 }
			 //DEBUG_LOG("aac_config.PCMBuffer_size(%d) PCMBuffer_offset(%d)\n",aac_config.PCMBuffer_size,PCMBuffer_offset);
			 if(aac_config.PCMBuffer_size - PCMBuffer_offset == 0)//缓冲区原始PCM数据满,进行AAC编码
			 {
				 
				 unsigned int nInputSamples = (aac_config.PCMBuffer_size / (aac_config.PCMBitSize / 8));
				 //DEBUG_LOG("AAC_encode need PCM samples (%d) actual input PCM samples (%d)\n",aac_config.InputSamples,nInputSamples);
				 ret = AAC_encode((int *)aac_config.PCMBuffer,nInputSamples,aac_config.AACBuffer,aac_config.MaxOutputBytes);
				 if(ret < 0)
				 {
					 ERROR_LOG("AAC_encode failed !\n");
					 return -1;
				 }
				 out_len = ret;
				 //printf("send_PCM_count = %d	get_PCM_count = %d\n",send_PCM_count,get_PCM_count);
				 get_PCM_count = 0;
				 send_PCM_count = 0;//统计编码一帧AAC实际凑了多少帧pcm数据
				 
			 }
			 else if(aac_config.PCMBuffer_size - PCMBuffer_offset < 0)//凑数据越界了，异常！！！
			 {
				 ERROR_LOG("aac_config.PCMBuffer overflow!!\n");
				 return -1;
			 }
			 else //缓存没满，继续接受下一帧PCM数据
			 {
				 pthread_mutex_unlock(&audFrm_flg.audFrm_mut);
				 //DEBUG_LOG("Lack PCM data---waite the next input frame... \n");
				 goto AAC_OUT_LEN_IS_0;
			 }
		 
		 if(debug_count >= 10)
		 {
			 //DEBUG_LOG("encode AAC frame size(%d) out_len(%d)\n",ret,out_len);
			 debug_count = 0;
		 }
 
 
		 //填充返回参数
		 pstStream->stream = (unsigned char*)aac_config.AACBuffer;
		 pstStream->phyAddr = 0;
		 pstStream->len = out_len;
 // 	 printf("AAC out_len(%d)\n",out_len);
		 pstStream->timeStamp = audFrm_bak.timeStamp;
		 pstStream->seq = 0;  //该变量如有问题后续修改
	 
	 pthread_mutex_unlock(&audFrm_flg.audFrm_mut);
 
		 /*--DEBUG 将文件写到缓存buf----------------------------------------*/
		#if switch_record_AAC
			 if(!AAC_file.recod_exit)//记录AAC编码文件还没有退出
			 {
				 if(AAC_file.AAC_file_buf_Wpos + out_len > AAC_file.AAC_file_buf + AAC_FILE_BUF_SIZE)//写满缓存了再统一写入文件
				 {
					 ret = write(AAC_file.AAC_file_fd,AAC_file.AAC_file_buf, AAC_file.AAC_file_buf_Wpos - AAC_file.AAC_file_buf);
					 if(ret < 0)
					 {
						 ERROR_LOG("write error!\n");
						 return -1;
					 }
					 close(AAC_file.AAC_file_fd);
					 AAC_file.recod_exit = 1; //写满缓存空间就退出
					 ERROR_LOG("=============AAC write file success!==============\n\n\n");
					 
					 //return -1;
				 }
				 else
				 {
					 memcpy(AAC_file.AAC_file_buf_Wpos,aac_config.AACBuffer,out_len);
					 AAC_file.AAC_file_buf_Wpos += out_len;
				 }
			 }
			 
		#endif
		 /*----------------------------------------------------------------*/
 
	 return ret;
 }
 
 
 /*******************************************************************************
 *@ Description    :退出AAC编码
 *@ Input		   :
 *@ Output		   :
 *@ Return		   :成功：0
					 失败：-1
 *@ attention	   :
 *******************************************************************************/
 int  AAC_encode_exit(void)
 {
 
	  if(aac_config.EncHandle)
	 {	
		 faacEncClose(aac_config.EncHandle);  
		 aac_config.EncHandle = NULL;  
	 }
	 else
	 {
		 ERROR_LOG("AAC_encode_exit failed !\n");
		 return -1;
	 }
 
 
	#if switch_record_AAC
		 close(AAC_file.AAC_file_fd);
	#endif
	 
	 free(aac_config.AACBuffer);
	 
	 DEBUG_LOG("AAC_encode_exit success !\n");
	  return 0;
 }



 










