 
/***************************************************************************
* @file: rtsp_stream.h
* @author:   
* @date:  7,3,2019
* @brief:  通过live555实现H264 RTSP直播
* @attention:
***************************************************************************/
#pragma once

#include <stdio.h>
#include <pthread.h>

typedef unsigned int SOCKET;
typedef pthread_t    ThreadHandle;
#define mSleep(ms)   usleep(ms*1000)

#ifndef false 
#define false  0
#endif

#ifndef true 
#define true  1
#endif




#define FILEBUFSIZE (1024 * 1024) 

// 初始化
char CRTSPStream_init(void);
// 退出
int CRTSPStream_exit(void);

// 发送H264文件
char CRTSPStream_SendH264File(const char *pFileName);
int CRTSPStream_SendH264Data(const unsigned char *data,unsigned int size);

/*******************************************************************************
*@ Description    :判断RTSP是否开始发送数据帧
*@ Input          :
*@ Output         :
*@ Return         : 1
*@ attention      :阻塞函数，只有当客户端接入时才会返回
*******************************************************************************/
int CRTSPStream_start_send(void);




