 
/***************************************************************************
* @file: CircularBuffer.c
* @author:   
* @date:  5,22,2019
* @brief:  循环缓存池管理文件
* @attention:
***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>


#include "CircularBuffer.h"
#include "CircularBuffer_print.h"

CircularBuffer_t* CircularBuffer[3] = {0}; //循环缓冲buffer管理数组


int CircularBufferPrintStatus(CircularBuffer_t *cBuf)
{
	if(NULL == cBuf)
	{
		CBUF_ERROR_LOG("Illegal parameter!\n");
		return -1;
	}

	printf("---CircularBuffer status--------------------------------------------\n");
	printf("cBuf->bufSize = %d\n",cBuf->bufSize);
	printf("cBuf->occupiedSize = %d\n",cBuf->occupiedSize);
	printf("cBuf->writePos = %d\n",cBuf->writePos);
	printf("cBuf->FrmList_w = %d\n",cBuf->FrmList_w);
	printf("cBuf->totalFrm = %d\n",cBuf->totalFrm);
	printf("cBuf->IFrmIndex_w = %d\n",cBuf->IFrmIndex_w);
	printf("cBuf->totalIFrm = %d\n",cBuf->totalIFrm);
	printf("cBuf->circleNum = %d\n",cBuf->circleNum);
	printf("--------------------------------------------------------------------\n");

	return 0;
	
}

/*******************************************************************************
*@ Description    :向循环缓冲buffer 申请一个新的用户ID
*@ Input          :<cBuf>buffer句柄（要申请哪一个buffer的用户ID）
*@ Output         :
*@ Return         :成功：用户ID
					失败：-1;
*@ attention      :注意，不使用时需要调用函数释放
*******************************************************************************/
HLE_S32 CircularBufferRequestUserID(CircularBuffer_t *cBuf)
{
	if(NULL == cBuf)
	{
		CBUF_ERROR_LOG("Illegal parameter!\n");
		return -1;
	}
	int i = 0;
	
	pthread_mutex_lock(&cBuf->BufManageMutex);
	for(i = 0;i<MAX_USER_NUM;i++)
	{
		if(0 == cBuf->userArray[i].occupied)//该用户ID可用
		{
			cBuf->userArray[i].occupied = 1;
			cBuf->userArray[i].ReadFrmIndex = 0;
			cBuf->userArray[i].ReadCircleNum = 0;
			cBuf->userArray[i].diffpos = 0;
			cBuf->userArray[i].throwframcount = 0;
			cBuf->userArray[i].needReset = 1;
			
			pthread_mutex_unlock(&cBuf->BufManageMutex);
			return i;
		}
	}
	pthread_mutex_unlock(&cBuf->BufManageMutex);

	CBUF_ERROR_LOG("no free user ID !\n");
	return -1;
}

/*******************************************************************************
*@ Description    :释放申请的循环缓冲buffer的用户ID
*@ Input          :<cBuf>buffer句柄（要释放哪一个buffer的用户ID）
					<userid>释放的用户ID号
*@ Output         :
*@ Return         :成功：0
					失败：-1;
*@ attention      :
*******************************************************************************/
HLE_S32 CircularBufferFreeUserID(CircularBuffer_t *cBuf,HLE_S32 userid)
{
	if(NULL == cBuf || userid < 0 || userid >= MAX_USER_NUM)
	{
		CBUF_ERROR_LOG("Illegal parameter!\n");
		return -1;
	}
	cBuf->userArray[userid].occupied = 0;
	cBuf->userArray[userid].ReadFrmIndex = 0;
	cBuf->userArray[userid].ReadCircleNum = 0;
	cBuf->userArray[userid].diffpos = 0;
	cBuf->userArray[userid].throwframcount = 0;
	cBuf->userArray[userid].needReset = 0;

	return 0;
}


/*******************************************************************************
*@ Description    :复位指定消费者用户的读指针信息，跳转到离写指针所属I帧位置
*@ Input          :<cBuf>缓存池handle
					<userid> 用户id
*@ Output         :
*@ Return         :成功:返回0
					失败：返回-1
*@ attention      :注意函数内部有加锁操作
*******************************************************************************/
HLE_S32 CircularBufferResetUserInfo(CircularBuffer_t *cBuf,HLE_S32 userid)
{ 
	if(NULL == cBuf || userid >= MAX_USER_NUM) 
	{ 
		CBUF_ERROR_LOG("Illegal parameter !\n");
		return -1; 
	}

	CBUF_DEBUG_LOG("CircularBuffer %s ResetUserInfo... !\n",cBuf->resolution);
		
	pthread_mutex_lock(&cBuf->BufManageMutex); 
		
	HLE_U32 ICurIndex = cBuf->IFrmIndex[cBuf->IFrmIndex_w];//写指针所属I帧的索引值
	CBUF_DEBUG_LOG("Current user ReadFrmIndex---->frame flag = %#x\n",cBuf->FrmList[ICurIndex].flag);
	
	if(ICurIndex <= cBuf->FrmList_w)  //----I------w--------------> w所属的I帧在同一圈
	{ 	 
		//跳转到写指针的同一圈
		cBuf->userArray[userid].ReadCircleNum = cBuf->circleNum; 
	} 
	else 	//----------w---------I-----> w所属的I帧在上一圈
	{ 
		//跳转到写指针的上一圈
		cBuf->userArray[userid].ReadCircleNum = cBuf->circleNum-1; 
	} 
	cBuf->userArray[userid].ReadFrmIndex = ICurIndex; 
	cBuf->userArray[userid].occupied = 1;//重置的用户都是已经在使用中的用户，注意别赋值为0 
	cBuf->userArray[userid].diffpos = 0; 
	cBuf->userArray[userid].throwframcount = 0; 
	cBuf->userArray[userid].needReset = 0;

	CBUF_DEBUG_LOG("Current user ReadFrmIndex---->frame flag = %#x\n",cBuf->FrmList[cBuf->userArray[userid].ReadFrmIndex].flag);
	
	pthread_mutex_unlock(&cBuf->BufManageMutex);

	return 0; 
}



/*******************************************************************************
*@ Description    :循环缓冲 buf 创建
*@ Input          :<resolution> 视频帧分辨率大小
					<audioflg>有无音频帧标志。0：无 1：有
					<size> 缓冲区的大小,如使用默认大小请传0
*@ Output         :
*@ Return         :成功：buffer的指针
					失败：NULL
*@ attention      :
*******************************************************************************/
CircularBuffer_t* CircularBufferCreate(E_IMAGE_SIZE resolution, HLE_U32 audioflg , HLE_U32 bufsize)
{

	HLE_U16 totalFrm = 0;
	char * str_resolution = NULL;
	if(audioflg)//有无audio帧的buffer总帧数不一样
	{
		totalFrm = MAX_FRM_NUM;
	}
	else
	{
		totalFrm = MAX_V_F_NUM;
	}
	DEBUG_LOG("totalFrm = %d\n",totalFrm);
	
	if(bufsize == 0)//采用默认大小
	{	  
		if(IMAGE_SIZE_1920x1080 == resolution)
		{
			bufsize = BUFFER_SIZE_1920x1080;
			str_resolution = "1920*1080";
		}
//		else if(IMAGE_SIZE_960x544 == resolution)
//		{
//			bufsize = BUFFER_SIZE_960x544;
//		}
		else if(IMAGE_SIZE_640x360 == resolution)
		{
			bufsize = BUFFER_SIZE_640x360;
			str_resolution = "640*360";
		}
//		else if(IMAGE_SIZE_480x272 == resolution)
//		{
//			bufsize = BUFFER_SIZE_480x272;			
//		}
		else 
		{
			CBUF_ERROR_LOG("unknown video resolution!\n");
			return NULL;					
		}
	}
	
	CircularBuffer_t* FrameBufferPool = (CircularBuffer_t*)malloc(sizeof(CircularBuffer_t) + bufsize);
	if(NULL == FrameBufferPool)
	{
		CBUF_ERROR_LOG("CreateBufferPool: malloc failed!\n");
		return NULL;
	}
	memset(FrameBufferPool,0,sizeof(CircularBuffer_t) + bufsize);

	FrameBufferPool->resolution = (HLE_S8*)str_resolution;
	pthread_mutex_init(&FrameBufferPool->BufManageMutex, NULL);
	int i = 0;
	for(i=0;i<MAX_USER_NUM;i++)
	{
	
		FrameBufferPool->userArray[i].ReadFrmIndex = 0;
		FrameBufferPool->userArray[i].occupied = 0;
		FrameBufferPool->userArray[i].ReadCircleNum = 0;
		FrameBufferPool->userArray[i].diffpos = 0;
		FrameBufferPool->userArray[i].throwframcount = 0;
		FrameBufferPool->userArray[i].needReset = 0;
	}
	FrameBufferPool->bufStart = (HLE_U8*)FrameBufferPool + sizeof(CircularBuffer_t);
	FrameBufferPool->bufSize = bufsize;
	FrameBufferPool->occupiedSize = 0;
	FrameBufferPool->writePos = 0;
	//FrameBufferPool->readPos = 0;

	FrameBufferPool->FrmList_w = 0;
	//FrameBufferPool->FrmList_r = 0;
	FrameBufferPool->totalFrm = totalFrm; //初始化时填入期望值

	FrameBufferPool->IFrmIndex_w = 0;
	//FrameBufferPool->IFrmIndex_r = 0;
	FrameBufferPool->totalIFrm = 0;
	FrameBufferPool->circleNum = 0;
	
	
	return FrameBufferPool;
}

void CircularBufferFree(CircularBuffer_t *cBuf)
{
	if(NULL != cBuf)
	{
		cBuf->bufStart = NULL;//buf是一整块缓存，bufStart不需要释放
		free (cBuf);
		cBuf = NULL;
		
	}
}


extern int sys_get_time(HLE_SYS_TIME *sys_time, int utc, HLE_U8* wday);

/*******************************************************************************
*@ Description    :存放一帧video/audio帧到缓存
*@ Input          :<cBuf>缓存指针
					<data> 帧数据指针
					<length>帧数据长度
*@ Output         :
*@ Return         :成功：0
					失败：-1
*@ attention      :传入到该接口的帧，建议都按照之前的帧格式打包好传入
				传入的码流帧格式:
				    视频关键帧: 	 	  FRAME_HDR + IFRAME_INFO + DATA
				    视频非关键帧: FRAME_HDR + PFRAME_INFO + DATA
				    音频帧:   		  FRAME_HDR + AFRAME_INFO + DATA
				关于帧类型和帧同步标志选取:
				    前三个字节帧头同步码和H.264 NALU分割码相同，均为0x00 0x00 0x01
				    第四个字节使用了H.264中不会使用到的0xF8-0xFF范围
*******************************************************************************/
HLE_S32 CircularBufferPutOneFrame(CircularBuffer_t * cBuf,void *data,HLE_S32 length)
{
	if(NULL == cBuf || NULL == data || length <= 0)
	{
		CBUF_ERROR_LOG("Illegal parameter!\n");
		return -1;
	}
	/*---#判断帧格式是否合法------------------------------------------------------------*/
	FRAME_HDR * frame_head = (FRAME_HDR *)data;
	if(frame_head->sync_code[0] != 0x00 || frame_head->sync_code[1] != 0x00 || frame_head->sync_code[2] != 0x01 )
	{
		CBUF_ERROR_LOG("The format of frame is error!\n");
		return -1;
	}
	

	/*---#获取帧的时间信息------------------------------------------------------------*/   
	HLE_U8 flag = frame_head->type;
	IFRAME_INFO *Iframe_info = NULL;
	PFRAME_INFO *Pframe_info = NULL;
	AFRAME_INFO *Aframe_info = NULL;
	HLE_U64 PTS = 0;
	if(flag == 0xF8)//IDR（I）帧
	{
		Iframe_info = (IFRAME_INFO*)((char*)data + sizeof(FRAME_HDR));
		//vframe_rate = Iframe_info->framerate;
		PTS = Iframe_info->pts_msec;
		
	}
	else if(flag == 0xF9) // P帧
	{
		Pframe_info = (PFRAME_INFO*)((char*)data + sizeof(FRAME_HDR));
		PTS = Pframe_info->pts_msec;
	}
	else if(flag == 0xFA) //audio帧
	{
		Aframe_info = (AFRAME_INFO*)((char*)data + sizeof(FRAME_HDR));
		PTS = Aframe_info->pts_msec;
	}
	else
	{
		CBUF_ERROR_LOG("Unknown frame type!\n");
		return -1;
	}

	pthread_mutex_lock(&cBuf->BufManageMutex);
	
	/*到达缓存 buffer 尾部
	判断缓存池剩余空间是否足够放下一帧数据,不足则将剩余空间清零并跳转到
	缓存池开始位置,避免读数据时还需要进行两次拷贝
	*/
	if((cBuf->bufSize - cBuf->writePos) < length )
	{
		memset(cBuf->bufStart + cBuf->writePos , 0 ,cBuf->bufSize - cBuf->writePos);
		cBuf->totalFrm = cBuf->FrmList_w; //期望能存的总帧数和实际存下的帧数是会有一定出入的，这里需要修正
		cBuf->FrmList_w = 0;
		cBuf->circleNum += 1;
		cBuf->writePos = 0;
	}

	
	/*---#填写一帧索引信息------------------------------------------------------------*/
	HLE_SYS_TIME sys_time;
	HLE_U8 wday;
	sys_get_time(&sys_time, 1, &wday);
	cBuf->FrmList[cBuf->FrmList_w].frmStartPos = cBuf->writePos;
	cBuf->FrmList[cBuf->FrmList_w].frmLength = length;
	cBuf->FrmList[cBuf->FrmList_w].PTS = PTS;
	cBuf->FrmList[cBuf->FrmList_w].time = sys_time;
	cBuf->FrmList[cBuf->FrmList_w].flag = frame_head->type;

	/*---#拷贝帧数据------------------------------------------------------------*/
	memcpy(cBuf->bufStart + cBuf->writePos,data,length);
	cBuf->writePos += length;

	/*---#如果是I 帧则还需填充I 帧列表------------------------------------------------------------*/
	if(flag == 0xF8)
	{
	#if 0 //IFrmIndex_w后++，存在BUG，当别处在使用 IFrmIndex_w 时，IFrmIndex_w已经++了，这样定位不到I帧
		cBuf->IFrmIndex[cBuf->IFrmIndex_w] = cBuf->FrmList_w;
		//cBuf.ICurIndex = cBuf.CurFrmIndex;//当前I帧序列
		cBuf->IFrmIndex_w ++;
		if(cBuf->totalIFrm < cBuf->IFrmIndex_w)
		{
			cBuf->totalIFrm = cBuf->IFrmIndex_w;
		}
		
		if(cBuf->IFrmIndex_w >= MAX_I_F_NUM)
		{
			cBuf->IFrmIndex_w = 0;
		}
	#else  //IFrmIndex_w先++ 再更新索引，保证 IFrmIndex_w 对应的索引就是I帧，这样IFrmIndex是从下标1开始放了
		cBuf->IFrmIndex_w ++;
		if(cBuf->totalIFrm < cBuf->IFrmIndex_w)
		{
			cBuf->totalIFrm = cBuf->IFrmIndex_w;
		}
		
		if(cBuf->IFrmIndex_w >= MAX_I_F_NUM)
		{
			cBuf->IFrmIndex_w = 0;
		}
		cBuf->IFrmIndex[cBuf->IFrmIndex_w] = cBuf->FrmList_w;
		//cBuf.ICurIndex = cBuf.CurFrmIndex;//当前I帧序列
		
	#endif
	}

	/*---#修改帧索引下标------------------------------------------------------------*/
	cBuf->FrmList_w ++;
	if(cBuf->FrmList_w > cBuf->totalFrm)
	{
		cBuf->totalFrm = cBuf->FrmList_w;
	}
	 
	if(cBuf->FrmList_w >= MAX_FRM_NUM)
	{
		cBuf->totalFrm = cBuf->FrmList_w;
		cBuf->FrmList_w = 0;
		cBuf->circleNum += 1;
		cBuf->writePos = 0;
	}

	pthread_mutex_unlock(&cBuf->BufManageMutex);

	
	return 0;
	
}



/*******************************************************************************
*@ Description    :从循环buffer中获取一帧数据
*@ Input          :<userID>访问缓存buffer的用户ID号
					<cBuf>缓存buffer指针
*@ Output         :<dataOut>帧首地址
					<length帧数据长度
*@ Return         :成功：0
					失败：-1 
*@ attention      :1.内部超时时间（1S）
				    2.新申请的用户第一次读帧时内部会自动进行reset操作，
						保证第一次读帧时返回的是IDR帧。

读指针的各种情况分析：
	读指针：r
	写指针：w
	W+n:n代表w超过r的圈数。

	W + 0 时 R 情况 1，2：
		1. ----------r--------w--------------------------->
			最初始，circleNum_w == circleNum_r   &&  r < w  -------------------- 能正常读取。

		2. ------------------[r]w--------------------------->
			circleNum_w == circleNum_r   &&  r == w  -------------------- 不能正常读取（无数据）。

	W+1 时 R 没被踩的情况 3，4：
		3. -----------w+1----------r------------------------>
			circleNum_w = circleNum_r + 1   &&  r < totalFrm && r > w  ---------- 能正常读取。

		4. -------w+1--------------------------------------r>
			circleNum_w = circleNum_r + 1   &&  r >= totalFrm  ---------- r需要重绕，
			重绕后	如果 r==w 则无数据可读，如果 r < w 则能正常读。
			重绕后又恢复到 1，2的情况。

	W+1 时 R 被踩的情况 5：
		5. -------------r----w+1---------------------------->  
			circleNum_w = circleNum_r + 1   &&  r < w-------------------- r 被踩，需要重定位到 w 所属的I
			帧位置。
			重定位后又恢复到 1，2的情况。

	W+n(n>=2) 时的情况 6： 
		6. -------------r----w+n---------------------------->  
			circleNum_w >= circleNum_r + 2 
			无论 r w 相对位置如何，r 必被踩，r必须重定位到 w 所属的I帧位置。
			重定位后又恢复到 1，2的情况。

	综上所述，需要进行特殊处理的情况有：2，4，5，6
*******************************************************************************/
HLE_S32 CircularBufferReadOneFrame(int userID,CircularBuffer_t * cBuf, void **dataOut, FrameInfo_t *pFrameInfo)
{
	if((userID > MAX_USER_NUM)||NULL == cBuf ||NULL == dataOut || NULL == pFrameInfo)
	{
		CBUF_ERROR_LOG("userID(%d) cBuf(%p) dataOut(%p) pFrameInfo(%p) Illegal parameter! \n",\
						userID,cBuf,dataOut,pFrameInfo);
		return -1;
	}

	struct timeval start_read_time = {0};
	struct timeval tmp_time = {0};
	gettimeofday(&start_read_time,NULL);
	memcpy(&tmp_time,&start_read_time,sizeof(start_read_time));

	/*---#检查是否要重置userinfo，保证第一次读帧时跳转到写指针所对应的IDR帧位置-------------------------------*/
	//CBUF_DEBUG_LOG("cBuf->userArray[userID].needReset = %d\n",cBuf->userArray[userID].needReset);
	if(cBuf->userArray[userID].needReset)
		CircularBufferResetUserInfo(cBuf,userID);//注意里边带锁操作
	
READ_AGAIN:	//重新读（读指针有重定位，或者异常）
	gettimeofday(&tmp_time,NULL);
	if(tmp_time.tv_usec - start_read_time.tv_usec >= 1*1000*1000)//1s 超时(注意不要使用秒来做差)
	{
		CBUF_ERROR_LOG("Read CircularBuffer time out!\n");
		return -1;
	}
	
	pthread_mutex_lock(&cBuf->BufManageMutex);

	if(cBuf->userArray[userID].ReadCircleNum > cBuf->circleNum)//circleNum 溢出,或者异常
	{
		CBUF_ERROR_LOG("ReadCircleNum > circleNum !\n");
		cBuf->circleNum = 0;
		cBuf->userArray[userID].ReadFrmIndex = cBuf->IFrmIndex_w;//跳转需要跳到I帧上，否则会引起视频花屏
		cBuf->userArray[userID].ReadCircleNum = cBuf->circleNum;
		pthread_mutex_unlock(&cBuf->BufManageMutex);
		goto READ_AGAIN;
	}
	/*	buffer： -------r[w]--------------->
	
	1.读圈数 == 写圈数 ，读指针 == 写指针,无数据可读
	*/
	if(cBuf->userArray[userID].ReadFrmIndex == cBuf->FrmList_w &&
	   cBuf->userArray[userID].ReadCircleNum == cBuf->circleNum)
	{
		//CBUF_ERROR_LOG("No frame data to be read!\n");
		pthread_mutex_unlock(&cBuf->BufManageMutex);
		usleep(1000*100); 	//此处延时注意延时不能太短，在15帧/s的情况下 1帧时长 = 1000/15 = 66(ms)
		goto READ_AGAIN;
		//return -1;
	}

	/*	buffer： ---------w[+n]-------------r>
	
	2.写圈数 > 读圈数，读指针 == 缓存末尾。
	读指针到达 FrmList 实际有效帧的最尾巴处，需循环重绕
	*/
	if((cBuf->userArray[userID].ReadFrmIndex >= cBuf->totalFrm) &&
		(cBuf->userArray[userID].ReadCircleNum < cBuf->circleNum))
	{		
		#if 1 //DEBUG
		CBUF_DEBUG_LOG("change cycle,ReadFrmIndex = %d,TotalFrm=%d,ReadCircleNum=%d,circlenum=%d\n", 
			cBuf->userArray[userID].ReadFrmIndex,cBuf->totalFrm,
			cBuf->userArray[userID].ReadCircleNum,cBuf->circleNum);	 
		#endif
		cBuf->userArray[userID].ReadFrmIndex = 0;
		cBuf->userArray[userID].ReadCircleNum = cBuf->circleNum;//重绕后 读圈数跳转到 和写圈数一致
		
		if(cBuf->userArray[userID].ReadFrmIndex == cBuf->FrmList_w)//重绕后刚好碰见 “写指针” 索引，则无数据可读
		{
			CBUF_ERROR_LOG("No frame data to be read!\n");
			pthread_mutex_unlock(&cBuf->BufManageMutex);
			usleep(100);
			goto READ_AGAIN;
			//return -1;
		}		 
	}

	/*	buffer： -----r----w[+1]------------->
	
	3.读指针被踩: 写圈数 == 读圈数 + 1 ，读指针 < 写指针
	*/
	if((cBuf->circleNum - cBuf->userArray[userID].ReadCircleNum == 1)
	  && cBuf->FrmList[cBuf->userArray[userID].ReadFrmIndex].frmStartPos < cBuf->writePos)
	{
		//表示读的太慢,跳转到当前写的位置（保证视频的实时性）
		#if 1 //DEBUG
		CBUF_ERROR_LOG("err: data recover,ReadFrmIndex = %d,TotalFrm=%d,ReadCircleNum=%d,circlenum=%d\n", 
			cBuf->userArray[userID].ReadFrmIndex,cBuf->totalFrm,
			cBuf->userArray[userID].ReadCircleNum,cBuf->circleNum);
		#endif

		cBuf->userArray[userID].ReadFrmIndex = cBuf->IFrmIndex_w;//跳转需要跳到I帧上，否则会引起视频花屏
		cBuf->userArray[userID].ReadCircleNum = cBuf->circleNum;
		pthread_mutex_unlock(&cBuf->BufManageMutex);
		goto READ_AGAIN;
		//return -1;
	}
	
	/*数据覆盖
	4.非常极端的情况:读非常慢,写的圈数,比读的圈数大于2。 写圈数 >= 读圈数 + 2 ，读指针 >= 写指针||读指针 < 写指针
	*/
	if(cBuf->circleNum - cBuf->userArray[userID].ReadCircleNum >= 2)
	{
		//*表示读的太慢,跳转到当前写的位置（因读指针数据已经被覆盖,且需保证视频的实时性）
		#if 1
		CBUF_ERROR_LOG("err: data recover,circlenum:%d, ReadCircleNum:%d\n",
			cBuf->circleNum, cBuf->userArray[userID].ReadCircleNum);
		#endif
		cBuf->userArray[userID].ReadFrmIndex = cBuf->IFrmIndex_w;//跳转需要跳到I帧上，否则会引起视频花屏
		cBuf->userArray[userID].ReadCircleNum = cBuf->circleNum;
		pthread_mutex_unlock(&cBuf->BufManageMutex);
		goto READ_AGAIN;
		//return -1;
	}

	
	*dataOut = cBuf->bufStart + cBuf->FrmList[cBuf->userArray[userID].ReadFrmIndex].frmStartPos;
	memcpy(pFrameInfo, &(cBuf->FrmList[cBuf->userArray[userID].ReadFrmIndex]), sizeof(FrameInfo_t));
	
	cBuf->userArray[userID].ReadFrmIndex++;

	if(cBuf->userArray[userID].ReadFrmIndex < cBuf->FrmList_w &&\
		cBuf->userArray[userID].ReadCircleNum == cBuf->circleNum)
	{
		cBuf->userArray[userID].diffpos =  cBuf->FrmList_w -cBuf->userArray[userID].ReadFrmIndex;
	}
	
	else if(cBuf->userArray[userID].ReadFrmIndex >= cBuf->FrmList_w &&\
		cBuf->userArray[userID].ReadCircleNum < cBuf->circleNum)
	{
		cBuf->userArray[userID].diffpos = cBuf->totalFrm + cBuf->FrmList_w -cBuf->userArray[userID].ReadFrmIndex;
	}
		
	pthread_mutex_unlock(&cBuf->BufManageMutex);
	
   
	return 0;
}

/*******************************************************************************
*@ Description    :循环缓冲buffer初始化函数
*@ Input          :
*@ Output         :
*@ Return         :成功：0
					失败：-1
*@ attention      :不可重复调用
*******************************************************************************/
int CircularBufferInit(void)
{
	if(CircularBuffer[0] != NULL ||CircularBuffer[1] != NULL ||CircularBuffer[2] != NULL)
	{
		CBUF_ERROR_LOG("CircularBuffer already inited!\n");
		return -1;
	}
	
	memset(CircularBuffer,0,sizeof(CircularBuffer));
	
	CircularBuffer[0] =  CircularBufferCreate(IMAGE_SIZE_1920x1080,1,0);
	if(CircularBuffer[0] == NULL)
	{
		CBUF_ERROR_LOG("CircularBufferCreate failed!\n");
		return -1;
	}

	CircularBuffer[1] =  CircularBufferCreate(IMAGE_SIZE_640x360,1,0); 
	if(CircularBuffer[1] == NULL)
	{
		CBUF_ERROR_LOG("CircularBufferCreate failed!\n");
		return -1;
	}

	return 0;
	
}

/*******************************************************************************
*@ Description    :获取缓存池buffer的handle
*@ Input          :<resolution>图像的分辨率，内部将依据分辨率返回buffer句柄
*@ Output         :
*@ Return         :成功：对应循环buffer的句柄
					失败：NULL
*@ attention      :
*******************************************************************************/
CircularBuffer_t* CircularBufferGetHandle(E_IMAGE_SIZE resolution)
{
	if(resolution == IMAGE_SIZE_1920x1080)
		return CircularBuffer[0];
	else if(resolution == IMAGE_SIZE_640x360)
		return CircularBuffer[1];
	else
	{
		CBUF_ERROR_LOG("resolution not supported !\n");
		return NULL;
	}
}
/*******************************************************************************
*@ Description    :销毁循环缓冲buffer
*@ Input          :
*@ Output         :
*@ Return         :成功：对应循环buffer的句柄
					失败：NULL
*@ attention      :
*******************************************************************************/
int CircularBufferDestory(void)
{
	int i = 0;
	for(i = 0; i<3;i++)
	{
		CircularBufferFree(CircularBuffer[i]);	
	}
	return 0;
}







































