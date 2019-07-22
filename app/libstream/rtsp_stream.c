 
/***************************************************************************
* @file: rtsp_stream.c
* @author:   
* @date:  7,3,2019
* @brief:  通过live555实现H264 RTSP直播
* @attention:
***************************************************************************/
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

#include <sys/types.h>
#include <sys/shm.h>


//#include <sys/types.h>
//#include <sys/stat.h>
#include <string.h>
//#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>


#include "rtsp_stream.h"


//#define FIFO_NAME    "/tmp/H264_fifo"	//需要和live555里边定义的保持一致
//#define BUFFERSIZE   PIPE_BUF

//共享内存相关参数
#define SHM_FLAG_ID		13
#define SHM_SIZE		1024*300

int 	shmid;		//共享内存的 id
void *	shm_buf;	//共享内存的起始地址

//进程同步相关参数
#define SEM_MUTEX_R 	"rtsp_mutex_r" 
#define SEM_MUTEX_W 	"rtsp_mutex_w" 

//有名信号灯部分相关参数
sem_t* semr;
sem_t* semw;


int CRTSPStream_exit(void)
{
	sem_unlink(SEM_MUTEX_R);
	sem_unlink(SEM_MUTEX_W);

	if(NULL != shm_buf)
	{
		shmdt(shm_buf);//共享内存分离
		shm_buf = NULL;
	}

	shmctl(shmid, IPC_RMID, NULL);//对共享内存的控制或者处理；IPC_RMID:删除对象,删除创建的共享内存
	
	return true;
}


/*******************************************************************************
*@ Description    :RTSP（client端）初始化
*@ Input          :
*@ Output         :
*@ Return         :成功：true（1）；失败：false（0）
*@ attention      :
*******************************************************************************/
char CRTSPStream_init(void)
{
	/*---#有名信号灯初始化------------------------------------------------------------*/
	semr = sem_open(SEM_MUTEX_R, O_CREAT | O_RDWR , 0666, 0);
	if (semr == SEM_FAILED)
	{
		printf("[MEDIA SERVER] error sem_open semr failed errno=%d\n",errno);
		goto ERR;
	}

	semw = sem_open(SEM_MUTEX_W, O_CREAT | O_RDWR, 0666, 0); 
	if (semw == SEM_FAILED)
	{
		printf("[MEDIA SERVER] error sem_open semw failed errno=%d\n",errno);
		goto ERR;
	}

	/*---#共享内存初始化------------------------------------------------------------*/
	key_t key;
	key = ftok("/tmp", SHM_FLAG_ID);
	if (key < 0)
	{
		printf("[MEDIA SERVER] error ftok failed\n");
		goto ERR;
	}
	
	shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);//创建共享内存,IPC_CREAT | 0666为权限设置，//类似open（）函数的权限位一般这样写就行
	if (shmid < 0)
	{
		printf("[MEDIA SERVER] error shmget failed\n");
		goto ERR;
	}

	shm_buf = shmat(shmid, NULL, 0);//共享内存映射。NULL表示系统自动分配；0：表共享内存可读写
	if (NULL == shm_buf)
	{
		printf("[MEDIA SERVER] error shmat failed\n");
		shmctl(shmid, IPC_RMID, NULL);//对共享内存的控制或者处理；IPC_RMID:删除对象，删除创建的共享内存
		goto ERR;
	}
	
	return true;
ERR:
	CRTSPStream_exit();
	return false;
}



/*******************************************************************************
*@ Description    :发送一个.264文件
*@ Input          :<pFileName> H264 文件名
*@ Output         :
*@ Return         :成功：true（1）；失败：false（0）
*@ attention      :
*******************************************************************************/
char CRTSPStream_SendH264File(const char *pFileName)
{
	if(pFileName == NULL)
	{
		return false;
	}
	FILE *fp = fopen(pFileName, "rb");  
	if(!fp)  
	{  
		printf("[RTSPStream] error:open file %s failed!",pFileName);
	}  
	fseek(fp, 0, SEEK_SET);

	unsigned char *buffer  = (unsigned char*)malloc(FILEBUFSIZE);
	if(NULL == buffer)
	{
		fclose(fp);
		printf("[RTSPStream] error:malloc failed!\n");
		return false;
	}
	
	int pos = 0;
	while(1)
	{
		int readlen = fread(buffer+pos, sizeof(unsigned char), FILEBUFSIZE-pos, fp);

		if(readlen<=0)
		{
			break;
		}

		readlen+=pos;

		int writelen = CRTSPStream_SendH264Data(buffer,readlen);
		if(writelen<=0)
		{
			break;
		}
		memcpy(buffer,buffer+writelen,readlen-writelen);
		pos = readlen-writelen;

		mSleep(25);
	}
	
	fclose(fp);
	free(buffer);
	return true;
}

/*******************************************************************************
*@ Description    :发送H264数据帧
*@ Input          :<data> 帧数据
					<size>数据大小
*@ Output         :
*@ Return         :成功：true（1）；失败：false（0）； rtsp客户端离线：2
*@ attention      :
*******************************************************************************/
static int closed_flag = 0;
int CRTSPStream_SendH264Data(const unsigned char *data,unsigned int size)
{
	if(data == NULL)
	{
		return false;
	}
	if(size + 4 > SHM_SIZE)
	{
		printf("[RTSPStream] error:CRTSPStream_SendH264Data failed! data size(%d) is too long!\n",size);
		return false;
	}

	while(sem_trywait(semw) < 0)
	{
		closed_flag++;
		if(closed_flag > 5000/10) //时长达2s，返回“客户端会话关闭”状态
		{
			closed_flag = 0;
			return 2;
		}
		usleep(1000*10); //10ms
	}
	
	closed_flag = 0;
	
	//统一在头4个字节填充数据长度信息
	//printf("[MEDIA client]******** GetFrameData can write !********");
	memcpy(shm_buf,&size,4);
	//char*tmp_buf=(char*)shm_buf;
	//printf("[send]shm_buf[]=%d %d %d %d %d\n",tmp_buf[0],tmp_buf[1],tmp_buf[2],tmp_buf[3],tmp_buf[4]);
	memcpy(shm_buf+4,data,size);
	
	sem_post(semr); 
		
	
	return true;
}

/*******************************************************************************
*@ Description    :判断RTSP是否开始发送数据帧
*@ Input          :
*@ Output         :
*@ Return         : 1
*@ attention      :阻塞函数，只有当客户端接入时才会返回
*******************************************************************************/
int CRTSPStream_start_send(void)
{
	sem_wait(semw); //等待可写（阻塞）
	sem_post(semw); //恢复可写状态
	return 1;
}
















