 
/***************************************************************************
* @file: CircularBuffer.h 
* @author:   
* @date:  5,18,2019
* @brief:  循环缓冲池头文件
* @attention:
***************************************************************************/
#ifndef _CIRCULAR_BUFFER_H
#define _CIRCULAR_BUFFER_H

#include "typeport.h"
#include "encoder.h"

#ifndef false 
#define false  0
#endif

#ifndef true 
#define true  1
#endif



#define BUFFER_SIZE_1920x1080 	1024*1024*4		/*1920*1080 分辨率的缓冲buf大小*/
#define BUFFER_SIZE_960x544 	1024*1024*1		/*960*544 分辨率的缓冲buf大小*/
#define BUFFER_SIZE_640x360 	1024*1024*1		/*640*360 分辨率的缓冲buf大小*/
#define BUFFER_SIZE_480x272 	1024*1024/4		/*480*272 分辨率的缓冲buf大小*/



#define RECODE_TIME		40		//最大缓存音视频时长（秒）
#define MAX_I_F_NUM		(RECODE_TIME / 2)  //最大I帧数量（gop:30帧; 帧率:15帧/s ; I帧:2s/I帧） 	
#define MAX_A_F_NUM		(RECODE_TIME * 15)	//最大的audio帧数量（audio: 15帧/s , aac帧）
#define MAX_V_F_NUM		(RECODE_TIME * 15) //最大的 video帧数量（video：15帧/s ）
#define MAX_FRM_NUM     (MAX_A_F_NUM + MAX_V_F_NUM)

#define MAX_USER_NUM	6	//缓存池支持的最大同时 “读” 用户个数

//用户读缓存的信息结构（支持多个用户同时读缓存，用户只能读）
typedef struct _UserInfo_t
{
	HLE_U16			occupied;				/*0:该用户空闲，1:该用户已经被占用*/
	HLE_U16			ReadFrmIndex;			/*此用户对帧缓冲池访问所用的帧索引*/
	HLE_U32			ReadCircleNum;			/*此用户对帧缓冲池的访问圈数，初始时等于帧缓冲池中的circlenum*/
	HLE_U32			diffpos;				/*读指针和写指针位置差值，单位为帧*/
	HLE_U32 		throwframcount;			/*从开始计数丢帧的个数*/
	HLE_U32			needReset;				/*读信息需要重置（申请用户ID后，首次读时数据时需要将读信息重置到离写指针所对应的IDR帧位置）*/
}UserInfo_t;


//一个 video/audio 帧在缓冲池的信息描述结构体
typedef struct  _FrameInfo_t
{
	HLE_U32			frmStartPos;	/*此帧在buffer中的偏移*/
	HLE_U32			frmLength;  	/*此帧的有效数据长度*/
	HLE_U64			PTS;			/*如果是视频帧，则为此帧的时间戳*/
	HLE_SYS_TIME	time;			/*产生此帧的时间*/
	HLE_U8			flag;			/*帧类型, 0xF8-视频关键帧，0xF9-视频非关键帧，0xFA-音频帧*/  
	HLE_U8			reserve[3];
	
}FrameInfo_t;

/*循环缓冲区管理结构*/
typedef struct _CircularBuffer_t 
{
	HLE_S8			*resolution;			/*当前循环buf所对应的视频分辨率(字符串格式：1920*1080 )*/
	pthread_mutex_t BufManageMutex;			/*读写锁*/
	UserInfo_t		userArray[MAX_USER_NUM];/*用户信息描述数组*/
	HLE_U8			*bufStart;				/*媒体数据buf 起始地址*/
	HLE_U32			bufSize;				/*buf 空间大小*/
	HLE_U32			occupiedSize;			/*实际已使用空间大小*/
	HLE_U32 		writePos;				/*写指针偏移*/

	FrameInfo_t		FrmList[MAX_FRM_NUM];	/*buf 中存储的帧列表信息*/
	HLE_U16		 	FrmList_w;				/*写指针，在 FrmList 中的下标*/		
	HLE_U16		 	totalFrm;				/*总帧数（buffer一圈实际存下的总帧数）*/
	
	HLE_U32 		IFrmIndex[MAX_I_F_NUM];	/*buf中I帧在 FrmList 数组中的下标信息*/
	HLE_U16			IFrmIndex_w;			/*写指针所属的I帧,在 IFrmIndex 中的索引*/
	HLE_U16			totalIFrm;				/*当前buffer中总的有效的I帧数目*/
	HLE_U32			circleNum;				/*写buf覆盖的圈数*/
	
}CircularBuffer_t;




/*******************************************************************************
*@ Description    :循环缓冲buffer初始化函数
*@ Input          :
*@ Output         :
*@ Return         :成功：0
					失败：-1
*@ attention      :不可重复调用
*******************************************************************************/
int CircularBufferInit(void);

/*******************************************************************************
*@ Description    :获取缓存池buffer的handle
*@ Input          :<resolution>图像的分辨率，内部将依据分辨率返回buffer句柄
*@ Output         :
*@ Return         :成功：对应循环buffer的句柄
					失败：NULL
*@ attention      :
*******************************************************************************/
CircularBuffer_t* CircularBufferGetHandle(E_IMAGE_SIZE resolution);



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
HLE_S32 CircularBufferPutOneFrame(CircularBuffer_t * cBuf,void *data,HLE_S32 length);


/*******************************************************************************
*@ Description    :向循环缓冲buffer 申请一个新的用户ID
*@ Input          :<cBuf>buffer句柄（要申请哪一个buffer的用户ID）
*@ Output         :
*@ Return         :成功：用户ID
					失败：-1;
*@ attention      :注意，不使用时需要调用函数释放
*******************************************************************************/
HLE_S32 CircularBufferRequestUserID(CircularBuffer_t *cBuf);


/*******************************************************************************
*@ Description    :释放申请的循环缓冲buffer的用户ID
*@ Input          :<cBuf>buffer句柄（要释放哪一个buffer的用户ID）
					<userid>释放的用户ID号
*@ Output         :
*@ Return         :成功：0
					失败：-1;
*@ attention      :
*******************************************************************************/
HLE_S32 CircularBufferFreeUserID(CircularBuffer_t *cBuf,HLE_S32 userid);


/*******************************************************************************
*@ Description    :从循环buffer中获取一帧数据
*@ Input          :<userID>访问缓存buffer的用户ID号
					<cBuf>缓存buffer指针
*@ Output         :<dataOut>帧首地址
					<length>帧数据长度
*@ Return         :成功：0
					失败：-1 
*@ attention      :内部超时时间（1S）
*******************************************************************************/
HLE_S32 CircularBufferReadOneFrame(int userID,CircularBuffer_t * cBuf, void **dataOut, FrameInfo_t *pFrameInfo);


/*******************************************************************************
*@ Description    :销毁循环缓冲buffer
*@ Input          :
*@ Output         :
*@ Return         :成功：对应循环buffer的句柄
					失败：NULL
*@ attention      :
*******************************************************************************/
int CircularBufferDestory(void);





#endif










