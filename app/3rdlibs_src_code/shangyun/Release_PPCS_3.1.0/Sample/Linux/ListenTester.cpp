//// ListenTester.cpp 
//// Author: Zheng.B.C
////	To test PPCS Listen to a specified Platform
////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <stdint.h>  // uintptr_t

#if defined(WIN32DLL) || defined(WINDOWS)
//#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <direct.h>
#elif defined(LINUX)
#include <unistd.h> 
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h> 
#include <netinet/in.h>
#include <netdb.h> 
#include <net/if.h>
#include <sched.h>
#include <stdarg.h>
#include <dirent.h>
#include <arpa/inet.h> 
#endif

#include "PPCS_API.h"

#define 	PKT_TEST

#if defined(WIN32DLL) || defined(WINDOWS)
typedef	 	DWORD					my_threadid;
typedef		HANDLE	 				my_Thread_mutex_t;
#define		my_Mutex_Lock(mutex) 	WaitForSingleObject(mutex, INFINITE) 
#define		my_Mutex_UnLock(mutex) 	ReleaseMutex(mutex) 
#define		my_Mutex_Close(mutex) 	CloseHandle(mutex) 
#define		my_SocketClose(skt)		closesocket(skt)
#define		my_Thread_exit(a)  		return(a)
#elif defined(LINUX)
typedef 	pthread_t				my_threadid;
typedef		pthread_mutex_t  		my_Thread_mutex_t;
#define		my_Mutex_Lock(mutex)	pthread_mutex_lock(&mutex)
#define		my_Mutex_UnLock(mutex)	pthread_mutex_unlock(&mutex)
#define		my_Mutex_Close(mutex) 	pthread_mutex_destroy(&mutex)
#define		my_SocketClose(skt)		close(skt)
#define		my_Thread_exit(a)		pthread_exit(a) 		
#endif
typedef uintptr_t UINTp;

// show info 开关 -> 终端打印调试信息
static int g_ST_INFO_OPEN = 1;
// debug log 开关 -> 输出到本地log文件
static int g_DEBUG_LOG_FILE_OPEN = 0;
const char LogFileName[] = "./ListenTester.log";

#define CH_CMD				0
#define CH_DATA				1
#define SERVER_NUM			3	// WakeUp Server Number
#define SIZE_DID 			30	// Device ID Size
#define SIZE_APILICENSE 	24	// APILicense Size
#define SIZE_INITSTRING 	256	// InitString Size
#define SIZE_WAKEUP_KEY 	17	// WakeUp Key Size

//// liteOS TCP packet Send to Device: 
////#define WAKEUP_CODE {0x98,0x3b,0x16,0xf8,0xf3,0x9c}

int gSessionID = -99;
// LoginStatus_Check Thread exit flags: 0->keep running,1->exit			 
char gThread_Exit = 1;		
// LoginStatus_Check Thread Running flags: thread exit->0, thread running->1	
char gThread_bRunning = 0;		

// define for Read/Write test mode
#define TEST_WRITE_SIZE 		1004  // (251 * 4)
#define TOTAL_WRITE_SIZE 		(4*1024*TEST_WRITE_SIZE)
#define TEST_NUMBER_OF_CHANNEL 	8
typedef struct 
{
	unsigned long TotalSize_Read;
	unsigned long TotalSize_Write;
	unsigned int Tick_Used_Read;
	unsigned int Tick_Used_Write;
} st_RW_Test_Info;
st_RW_Test_Info g_RW_Test_Info[TEST_NUMBER_OF_CHANNEL];

typedef struct 
{
	int Year;
	int Mon;
	int Day;
	int Week;
	int Hour;
	int Min;
	int Sec;
	int mSec;
	unsigned long TimeTick_mSec;
} st_Time_Info;
#define ST_TIME_USED	(int)(TimeEnd.TimeTick_mSec-TimeBegin.TimeTick_mSec)

void st_info(const char *format, ...) 
{
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	if (1 == g_DEBUG_LOG_FILE_OPEN) 
	{
		FILE *pFile = fopen(LogFileName, "a");
		if (!pFile)
		{
			fprintf(stderr, "Error: Can not Open %s file!\n", LogFileName);
			return ;
		}
		va_list ap;
		va_start(ap, format);
		vfprintf(pFile, format, ap);
		va_end(ap);
		fclose(pFile);
	}
}

void st_debug(const char *format, ...) 
{
	if (1 == g_ST_INFO_OPEN) 
	{
		va_list ap;
		va_start(ap, format);
		vfprintf(stderr, format, ap);
		va_end(ap);
	}
	if (1 == g_DEBUG_LOG_FILE_OPEN) 
	{
		FILE *pFile = fopen(LogFileName, "a");
		if (!pFile)
		{
			fprintf(stderr, "Error: Can not Open %s file!\n", LogFileName);
			return ;
		}
		va_list ap;
		va_start(ap, format);
		vfprintf(pFile, format, ap);
		va_end(ap);
		fclose(pFile);
	}
}

void my_GetCurrentTime(st_Time_Info *Time)
{
#if defined(WINDOWS) || defined(WIN32DLL)
	SYSTEMTIME mTime = {0};
	GetLocalTime(&mTime);
	Time->Year = mTime.wYear;
	Time->Mon = mTime.wMonth;
	Time->Day = mTime.wDay;
	Time->Week = mTime.wDayOfWeek;
	Time->Hour = mTime.wHour;
	Time->Min = mTime.wMinute;
	Time->Sec = mTime.wSecond;
	Time->mSec = mTime.wMilliseconds;
	Time->TimeTick_mSec = GetTickCount();
#elif defined(LINUX)
	struct timeval mTime;
	int ret = gettimeofday(&mTime, NULL);
	if (0 != ret)
	{
		st_info("gettimeofday failed!! errno=%d\n", errno);
		memset(Time, 0, sizeof(st_Time_Info));
		return ;
	}
	//struct tm *ptm = localtime((const time_t *)&mTime.tv_sec); 
	struct tm st_tm = {0};
	struct tm *ptm = localtime_r((const time_t *)&mTime.tv_sec, &st_tm); 
	if (!ptm)
	{
		st_info("localtime_r failed!!\n");
		memset(Time, 0, sizeof(st_Time_Info));
		Time->TimeTick_mSec = mTime.tv_sec*1000 + mTime.tv_usec/1000;
	}
	else
	{
		Time->Year = st_tm.tm_year+1900;
		Time->Mon = st_tm.tm_mon+1;
		Time->Day = st_tm.tm_mday;
		Time->Week = st_tm.tm_wday;
		Time->Hour = st_tm.tm_hour;
		Time->Min = st_tm.tm_min;
		Time->Sec = st_tm.tm_sec;
		Time->mSec = (int)(mTime.tv_usec/1000);
		Time->TimeTick_mSec = mTime.tv_sec*1000 + mTime.tv_usec/1000;
	}
#endif
}

unsigned long getTickCount(void)
{
#if defined(WIN32DLL) || defined(WINDOWS) 
	return GetTickCount();
#elif defined(LINUX)
	struct timeval current;
	gettimeofday(&current, NULL);
	return current.tv_sec*1000 + current.tv_usec/1000;
#endif
}

void mSleep(UINT32 ms)
{
#if defined(WIN32DLL) || defined(WINDOWS)
	Sleep(ms);
#elif defined LINUX
	usleep(ms * 1000);
#endif
}

void error(const char *msg) 
{
	st_info(msg);
    perror(msg);
    exit(0);
}

const char *getP2PErrorCodeInfo(int err)
{
    if (0 < err) 
	{
		return "NoError";
	}
    switch (err)
    {
        case 0: return "ERROR_P2P_SUCCESSFUL";
        case -1: return "ERROR_P2P_NOT_INITIALIZED";
        case -2: return "ERROR_P2P_ALREADY_INITIALIZED";
        case -3: return "ERROR_P2P_TIME_OUT";
        case -4: return "ERROR_P2P_INVALID_ID";
        case -5: return "ERROR_P2P_INVALID_PARAMETER";
        case -6: return "ERROR_P2P_DEVICE_NOT_ONLINE";
        case -7: return "ERROR_P2P_FAIL_TO_RESOLVE_NAME";
        case -8: return "ERROR_P2P_INVALID_PREFIX";
        case -9: return "ERROR_P2P_ID_OUT_OF_DATE";
        case -10: return "ERROR_P2P_NO_RELAY_SERVER_AVAILABLE";
        case -11: return "ERROR_P2P_INVALID_SESSION_HANDLE";
        case -12: return "ERROR_P2P_SESSION_CLOSED_REMOTE";
        case -13: return "ERROR_P2P_SESSION_CLOSED_TIMEOUT";
        case -14: return "ERROR_P2P_SESSION_CLOSED_CALLED";
        case -15: return "ERROR_P2P_REMOTE_SITE_BUFFER_FULL";
        case -16: return "ERROR_P2P_USER_LISTEN_BREAK";
        case -17: return "ERROR_P2P_MAX_SESSION";
        case -18: return "ERROR_P2P_UDP_PORT_BIND_FAILED";
        case -19: return "ERROR_P2P_USER_CONNECT_BREAK";
        case -20: return "ERROR_P2P_SESSION_CLOSED_INSUFFICIENT_MEMORY";
        case -21: return "ERROR_P2P_INVALID_APILICENSE";
        case -22: return "ERROR_P2P_FAIL_TO_CREATE_THREAD";
        default: return "Unknown, something is wrong!";
    }
}

void showErrorInfo(int ret)
{
	if (0 <= ret)
	{
		return ;
	}		
	switch (ret)
	{
	case ERROR_PPCS_NOT_INITIALIZED:
		st_info("API didn't initialized\n"); 
		break;		
	case ERROR_PPCS_TIME_OUT:
		st_info("Listen time out, No client connect me !!\n"); 
		break;			
	case ERROR_PPCS_INVALID_ID:
		st_info("Invalid Device ID !!\n"); 
		break;
	case ERROR_PPCS_INVALID_PREFIX:
		st_info("Prefix of Device ID is not accepted by Server !!\n"); 
		break;
	case ERROR_PPCS_UDP_PORT_BIND_FAILED:
		st_info("The specified UDP port can not be binded !!\n"); 
		break;
	case ERROR_PPCS_MAX_SESSION:
		st_info("Exceed max session !!\n"); 
		break;
	case ERROR_PPCS_USER_LISTEN_BREAK:
		st_info("Listen break is called !!\n"); 
		break;
	case ERROR_PPCS_INVALID_APILICENSE:
		st_info("API License code is not correct !!\n");
		break;
	case ERROR_PPCS_FAIL_TO_CREATE_THREAD:
		st_info("Fail tO Create Thread !!\n");
		break;
	default: st_info("%s\n", getP2PErrorCodeInfo(ret));
		break;
	}
}

void showNetwork(st_PPCS_NetInfo NetInfo)
{
	st_info("-------------- NetInfo: -------------------\n");
	st_info("Internet Reachable     : %s\n", (NetInfo.bFlagInternet == 1) ? "YES":"NO");
	st_info("P2P Server IP resolved : %s\n", (NetInfo.bFlagHostResolved == 1) ? "YES":"NO");
	st_info("P2P Server Hello Ack   : %s\n", (NetInfo.bFlagServerHello == 1) ? "YES":"NO");
	switch(NetInfo.NAT_Type)
	{
	case 0: st_info("Local NAT Type         : Unknow\n"); break;
	case 1: st_info("Local NAT Type         : IP-Restricted Cone\n"); break;
	case 2: st_info("Local NAT Type         : Port-Restricted Cone\n"); break;
	case 3: st_info("Local NAT Type         : Symmetric\n"); break;
	}
	st_info("My Wan IP : %s\n", NetInfo.MyWanIP);
	st_info("My Lan IP : %s\n", NetInfo.MyLanIP);
	st_info("-------------------------------------------\n");
}

int iPN_StringEnc(const char *keystr, const char *src, char *dest, unsigned int maxsize)
{
	int Key[17] = {0};
	unsigned int i;
	unsigned int s, v;
	if (maxsize < strlen(src) * 2 + 3)
	{
		return -1;
	}
	for (i = 0 ; i < 16; i++)
	{
		Key[i] = keystr[i];
	}
	srand((unsigned int)time(NULL));
	s = abs(rand() % 256);
	memset(dest, 0, maxsize);
	dest[0] = 'A' + ((s & 0xF0) >> 4);
	dest[1] = 'a' + (s & 0x0F);
	for (i = 0; i < strlen(src); i++)
	{
		v = s ^ Key[(i + s * (s % 23))% 16] ^ src[i];
		dest[2 * i + 2] = 'A' + ((v & 0xF0) >> 4);
		dest[2 * i + 3] = 'a' + (v & 0x0F);
		s = v;
	}
	return 0;
}

int iPN_StringDnc(const char *keystr, const char *src, char *dest, unsigned int maxsize)
{
	int Key[17] = {0};
	unsigned int i;
	unsigned int s, v;
	if ((maxsize < strlen(src) / 2) || (strlen(src) % 2 == 1))
	{
		return -1;
	}
	for (i = 0 ; i < 16; i++)
	{
		Key[i] = keystr[i];
	}	
	memset(dest, 0, maxsize);
	s = ((src[0] - 'A') << 4) + (src[1] - 'a');
	for (i = 0; i < strlen(src) / 2 - 1; i++)
	{
		v = ((src[i * 2 + 2] - 'A') << 4) + (src[i * 2 + 3] - 'a');
		dest[i] = v ^ Key[(i + s * (s % 23))% 16] ^ s;
		if (dest[i] > 127 || dest[i] < 32) 
		{
			return -1; // not a valid character string
		}	
		s = v;
	}
	return 0;
}

#if defined(WIN32DLL) || defined(WINDOWS)
DWORD WINAPI Thread_LoginStatus_Check(void *arg)
#elif defined(LINUX)
void *Thread_LoginStatus_Check(void *arg)
#endif
{
#if defined(LINUX)
	pthread_detach(pthread_self());
#endif	
	gThread_bRunning = 1;
	st_debug("Thread_LoginStatus_Check start...\n");
	int i = 0;
	char LoginStatus = 0;
	char StatusFlags = -99;
	while (0 == gThread_Exit)
	{
		printf("I am runing!!\n");
		mSleep(1000);
		
		if (++i % 60 != 10) 
		{
			continue;
		}			
		if (0 == gThread_Exit)
		{
			if (ERROR_PPCS_SUCCESSFUL == PPCS_LoginStatus_Check(&LoginStatus))
			{
				if (StatusFlags != LoginStatus)
				{
					StatusFlags = LoginStatus;
					if (1 == LoginStatus) 
					{
						st_info("Got Server Response!!\n");
					}						
					else 
					{
						st_info("No Server Response!!!\n");
					}						
				}
			}
		}			
	}
	gThread_bRunning = 0;
	st_debug("Thread_LoginStatus_Check exit!\n");
	my_Thread_exit(0);
}

void CreateThread_LoginStatus_Check()
{
	gThread_Exit = 0;
#if defined(WIN32DLL) || defined(WINDOWS)	
	HANDLE hthread_LoginStatus_Check = CreateThread(NULL, 0, Thread_LoginStatus_Check, NULL, 0, NULL);
	if (NULL == hthread_LoginStatus_Check) 
	{
		error("create Thread LoginStatus_Check failed");
	}		
	else 
	{
		CloseHandle(hthread_LoginStatus_Check);
	}		
#elif defined(LINUX)
	pthread_t threadID_LoginStatus_Check;
	int err = pthread_create(&threadID_LoginStatus_Check, NULL, &Thread_LoginStatus_Check, NULL);
	if (0 != err) 
	{
		error("create Thread LoginStatus_Check failed");
	}	
#endif
}

#ifdef WIN32DLL
DWORD WINAPI ThreadWrite(void* arg)
#elif defined(LINUX)
void *ThreadWrite(void *arg)
#endif
{	
	INT32 Channel = (INT32)((UINTp)arg);
	if (0 > Channel || 7 < Channel)
	{
		st_info("ThreadWrite - Channel=%d !!\n", Channel);
		my_Thread_exit(0);
	}
	UCHAR *Buffer = (UCHAR *)malloc(TEST_WRITE_SIZE+1);
	if (!Buffer) 
	{
		st_info("ThreadWrite Channel %d - Malloc failed!!\n", Channel);
		my_Thread_exit(0);
	}
	for (INT32 i = 0 ; i < TEST_WRITE_SIZE; i++) 
	{
		Buffer[i] = i%251; //0~250
	}
	Buffer[TEST_WRITE_SIZE] = '\0';
	st_info("ThreadWrite Channel %d running... \n", Channel);
	
	INT32 ret = 0;
	INT32 Check_ret = 0;
	ULONG TotalSize = 0;
	UINT32 WriteSize = 0;
	UINT32 tick = getTickCount();
	while (ERROR_PPCS_SUCCESSFUL == (Check_ret = PPCS_Check_Buffer(gSessionID, Channel, &WriteSize, NULL)))
	{
		if ((WriteSize < 256*1024) && (TotalSize < TOTAL_WRITE_SIZE))
		{
			ret = PPCS_Write(gSessionID, Channel, (CHAR*)Buffer, TEST_WRITE_SIZE);
			if (0 > ret)
			{
				if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret)
				{
					st_info("ThreadWrite CH=%d, ret=%d, Session Closed TimeOUT!!\n", Channel, ret);
				}				
				else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret)
				{
					st_info("ThreadWrite CH=%d, ret=%d, Session Remote Close!!\n", Channel, ret);
				}				
				else 
				{
					st_info("ThreadWrite CH=%d, ret=%d %s\n", ret, getP2PErrorCodeInfo(ret));
				}				
				continue;
			}
			TotalSize += ret; // PPCS_Write return ret >=0: Number of byte wirten.
		}
		//When PPCS_Check_Buffer return WriteSize equals 0, all the data in this channel is sent out
		else if (0 == WriteSize) 
		{
			break;
		}			
		else 
		{
			mSleep(2);
		}			
	}
	tick = getTickCount() - tick;
	g_RW_Test_Info[Channel].Tick_Used_Write = tick;
	g_RW_Test_Info[Channel].TotalSize_Write = TotalSize;
		
	if (Buffer) free(Buffer);
	st_debug("ThreadWrite Channel %d Exit. TotalSize: %lu Byte (%.2f MByte), Time:%3d.%03d sec, %.3f KByte/sec\n", Channel, TotalSize, (double)TotalSize/(1024*1024), tick/1000, tick%1000, (0==tick)?0:((double)TotalSize/tick));
	
	my_Thread_exit(0);
}

#ifdef WIN32DLL
DWORD WINAPI ThreadRead(void* arg)
#elif defined(LINUX)
void *ThreadRead(void *arg)
#endif
{
	INT32 Channel = (INT32)((UINTp)arg);
	if (0 > Channel || 7 < Channel)
	{
		st_info("ThreadRead - Channel=%d !!\n", Channel);
		my_Thread_exit(0);
	}
	ULONG TotalSize = 0;
	int timeout_ms = 200;
	st_info("ThreadRead  Channel %d running... \n", Channel);
	
	UINT32 tick = getTickCount();
	while (1)
	{
		UCHAR zz = 0;
		INT32 ReadSize = 1;
		INT32 ret = PPCS_Read(gSessionID, Channel, (CHAR*)&zz, &ReadSize, timeout_ms);
		//st_info("PPCS_Read ret=%d, CH=%d, ReadSize=%d Byte, TotalSize=%lu Byte, zz=%d\n", ret, Channel, ReadSize, TotalSize, zz);
		
		if ((ret < 0) && (ret != ERROR_PPCS_TIME_OUT))
		{
			if (TOTAL_WRITE_SIZE == TotalSize) 
			{
				break;
			}		
			st_info("PPCS_Read ret=%d, CH=%d, ReadSize=%d Byte, TotalSize=%lu Byte\n", ret, Channel, ReadSize, TotalSize);
			break;
		}
		if (ReadSize && (TotalSize%251) != zz)//TotalSize%251: 0~250, zz: 0~250
		{
			st_info("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ ret=%d, CH=%d Error!! ReadSize=%d, TotalSize=%lu, zz=%d\n", ret, Channel, ReadSize, TotalSize, zz);
			break;
		} 
		else if (TotalSize%(1*1024*1024) == 1*1024*1024-1) 
		{	
			st_info("%d", Channel);
			setbuf(stdout, NULL);
		}
		TotalSize += ReadSize;
		if (TOTAL_WRITE_SIZE == TotalSize) 
		{
			break;
		}			
	}
	tick = getTickCount() - tick;
	g_RW_Test_Info[Channel].Tick_Used_Read = tick;
	g_RW_Test_Info[Channel].TotalSize_Read = TotalSize;
	
	st_debug("ThreadRead  Channel %d Exit - TotalSize: %lu Byte (%.2f MByte), Time:%3d.%03d sec, %.3f KByte/sec\n", Channel, TotalSize, (double)TotalSize/(1024*1024), tick/1000, tick%1000, (0==tick)?0:((double)TotalSize/tick));
	
	my_Thread_exit(0);
}

int RW_Test()
{	
#ifdef WIN32DLL
	HANDLE hThreadWrite[TEST_NUMBER_OF_CHANNEL];
	HANDLE hThreadRead[TEST_NUMBER_OF_CHANNEL];
#elif defined(LINUX)
	pthread_t ThreadWriteID[TEST_NUMBER_OF_CHANNEL];
	pthread_t ThreadReadID[TEST_NUMBER_OF_CHANNEL];
#endif
	
	memset(g_RW_Test_Info, 0, sizeof(g_RW_Test_Info));
	for (INT32 i = 0; i < TEST_NUMBER_OF_CHANNEL; i++)
	{
#ifdef WIN32DLL
		hThreadWrite[i] = CreateThread(NULL, 0, ThreadWrite, (void *)i, 0, NULL);
		if (NULL == hThreadWrite[i]) 
		{
			error("create ThreadWrite failed");
		}			
		hThreadRead[i] = CreateThread(NULL, 0, ThreadRead, (void *)i, 0, NULL);
		if (NULL == hThreadRead[i]) 
		{
			error("create ThreadRead failed");
		}			
#elif defined(LINUX)
		if (0 != pthread_create(&ThreadWriteID[i], NULL, &ThreadWrite, (void *)i)) 
		{
			error("create ThreadWrite failed");
		}			
		if (0 != pthread_create(&ThreadReadID[i], NULL, &ThreadRead, (void *)i)) 
		{
			error("create ThreadRead failed");
		}			
#endif
		mSleep(10);
	}
	
	for (INT32 i = 0; i < TEST_NUMBER_OF_CHANNEL; i++)
	{	
#ifdef WIN32DLL
		WaitForSingleObject(hThreadRead[i], INFINITE);
		CloseHandle(hThreadRead[i]);
		
		WaitForSingleObject(hThreadWrite[i], INFINITE);
		CloseHandle(hThreadWrite[i]);
#elif defined(LINUX)
		pthread_join(ThreadReadID[i], NULL);
		pthread_join(ThreadWriteID[i], NULL);
#endif
	}
	printf("\n");
	
	// show transmission information for each channel
	UINT32 tick = 0;
	ULONG TotalSize = 0;
	float speed_Write = 0;
    float speed_Read = 0;
    float Total_Speed_Read = 0;
    float Total_Speed_Write = 0;
	for (int i = 0; i < TEST_NUMBER_OF_CHANNEL; i++)
	{
		// read info
		tick = g_RW_Test_Info[i].Tick_Used_Read;
		TotalSize = g_RW_Test_Info[i].TotalSize_Read;
		speed_Read = (0==tick)?0:((double)TotalSize/tick);
		Total_Speed_Read += speed_Read;
		st_info("ThreadRead  Channel %d Done - TotalSize: %lu Byte (%.2f MByte), Time:%3d.%03d sec, %.3f KByte/sec\n", i, TotalSize, (double)TotalSize/(1024*1024), tick/1000, tick%1000, speed_Read);
		
		// write info
		tick = g_RW_Test_Info[i].Tick_Used_Write;
		TotalSize = g_RW_Test_Info[i].TotalSize_Write;
		speed_Write = (0==tick)?0:((double)TotalSize/tick);
		Total_Speed_Write += speed_Write; 
		st_info("ThreadWrite Channel %d Done - TotalSize: %lu Byte (%.2f MByte), Time:%3d.%03d sec, %.3f KByte/sec\n", i, TotalSize, (double)TotalSize/(1024*1024), tick/1000, tick%1000, speed_Write);
		
		setbuf(stdout, NULL);
	}
	st_info("Total_Speed_Read  %.3f KByte/sec\n", Total_Speed_Read);
    st_info("Total_Speed_Write %.3f KByte/sec\n", Total_Speed_Write);
	
	return 0;
}

int ft_Test()
{
	const char fileName[] = "1.7z";
	FILE *fp = fopen(fileName, "rb");
	if (!fp)
	{
		st_info("***Error: failed to open file: %s\n", fileName);
		return -1;
	}
	
	INT32 ret = -1;
	UINT32 wsize = 0;
	unsigned long SizeCounter = 0;
	unsigned int tick = getTickCount();
	while (!feof(fp))
	{				
		ret = PPCS_Check_Buffer(gSessionID, CH_DATA, &wsize, NULL);
		if (0 > ret)
		{
			st_info("PPCS_Check_Buffer ret=%d %s\n", ret, getP2PErrorCodeInfo(ret));
			break;
		}
		if (wsize > 256*1024) 
		{
			mSleep(5);
			continue;
		}
		if (wsize > 128*1024) 
		{
			mSleep(2);
			continue;
		}
		
		CHAR buf[1024];
		memset(buf, 0, sizeof(buf));
		INT32 DataSize = (INT32)fread(buf, 1, sizeof(buf), fp);
		if (0 == DataSize) 
		{
			st_info("ft_Test - fread failed: file DataSize=0, No Data!!\n");
			continue;
		}
		
		ret = PPCS_Write(gSessionID, CH_DATA, buf, DataSize);
		if (0 > ret)
		{
			if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret)
			{
				st_info("Session Closed TimeOUT!!\n");
			}				
			else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret)
			{
				st_info("Session Remote Close!!\n");
			}				
			else 
			{
				st_info("PPCS_Write ret=%d %s\n", ret, getP2PErrorCodeInfo(ret));
			}				
			break;
		}
		SizeCounter += ret;
		if (SizeCounter % (1024*1024) == 0) 
		{
			st_info("*");
			setbuf(stdout, NULL);
		}	
	}
	while (ERROR_PPCS_SUCCESSFUL == PPCS_Check_Buffer(gSessionID, CH_DATA, &wsize, NULL))
	{
		//st_info("gSessionID=%d, CH=%d, wsize=%d\n", gSessionID, CH_DATA, wsize);
		//setbuf(stdout, NULL);
		if (0 == wsize) 
		{
			break;
		} 			
		else 
		{
			mSleep(1);
		}			
	}
	tick = getTickCount() - tick;
	
	st_info("\nFile Transfer done!! Send Size=%lu Byte, Time:%3d.%03d sec, %4lu KByte/sec\n", SizeCounter, tick/1000,tick%1000, (0==tick)?0:(SizeCounter/tick));
	
	fclose(fp);
	return 0;	
}

#ifdef PKT_TEST
int pkt_Test()
{
	for (INT32 i = 0 ; i < 5000; i++)
	{
		char PktBuf[1024];
		memset(PktBuf, (UCHAR)(i % 100), sizeof(PktBuf));// data: 0~99
		
		int ret = PPCS_PktSend(gSessionID, CH_DATA, PktBuf, sizeof(PktBuf));
		
		st_debug("PPCS_PktSend: ret=%d, session=%d, channel=%d, data=%d..., size=%lu\n", ret, gSessionID, CH_DATA, PktBuf[0], sizeof(PktBuf));
		
		if (0 > ret)
		{
			if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret)
			{
				st_info("Session Closed TimeOUT!!\n");
			}				
			else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret)
			{
				st_info("Session Remote Close!!\n");
			}			
			else 
			{
				st_info("PPCS_PktSend ret=%d %s\n", ret, getP2PErrorCodeInfo(ret));
			}				
			break;
		}
		if (i%100 == 99) 
		{
			st_info("----->Send %d packets. (1 packets=%u Byte)\n", i+1, (unsigned)sizeof(PktBuf));
		}		
		mSleep(5); 
	}
	return 0;
}
#endif

int Call_SendCMD(const int skt, const char *CMD, const unsigned short SizeOfCMD, struct sockaddr_in Addr, const int index)
{
	if (0 > skt)
	{
		st_debug("Invalid skt!!\n");
		return skt;
	}
	if (!CMD || 0 == SizeOfCMD)
	{
		st_debug("Invalid CMD!!\n");
		return -1;
	}
	//char dest[30];
	//memset(dest, 0, sizeof(dest));
	//st_debug("write CMD to %s ...", inet_ntop(Addr.sin_family, (char *)&Addr.sin_addr.s_addr, dest, sizeof(dest)));
	st_debug("write CMD to %s ...", inet_ntoa(Addr.sin_addr));

#if defined(WINDOWS) || defined(WIN32DLL)
	int size = send(skt, CMD, SizeOfCMD, 0);
#else	
	int size = write(skt, CMD, SizeOfCMD);
#endif
	if (size <= 0) 
	{
		perror("ERROR, writing to socket ");
		my_SocketClose(skt);
		st_debug("close socket: %d\n", skt);
	}
	else 
	{
		st_info("#");
		setbuf(stdout, NULL);
		st_debug("write data(%u Byte):%s\n", size, CMD);
	}
	return size;
}

int Call_P2P_Listen(const char *DID, const char *APILicense, int Repeat)
{
	if (1 == Repeat) 
	{
		st_info("PPCS_Listen('%s', 600, 0, 1, '%s')...\n", DID, APILicense);
	}	
	st_Time_Info TimeBegin, TimeEnd;
	my_GetCurrentTime(&TimeBegin);	
	gSessionID = PPCS_Listen(DID, 600, 0, 1, APILicense);
	my_GetCurrentTime(&TimeEnd);
	
	st_info("-%02d-", Repeat);
	if (gSessionID < 0)
	{
		st_info("Listen failed (%d): ", gSessionID);
		showErrorInfo(gSessionID);
		return gSessionID;
	}
	// Success!! gSessionID>=0
	int ret = -1;
	st_PPCS_Session Sinfo;	
	if (ERROR_PPCS_SUCCESSFUL == (ret = PPCS_Check(gSessionID, &Sinfo)))
	{
		st_info("RemoteAddr=%s:%d, Mode=%s, Time=%d.%03d (Sec)\n", 
				inet_ntoa(Sinfo.RemoteAddr.sin_addr), 
				ntohs(Sinfo.RemoteAddr.sin_port), 
				(Sinfo.bMode == 0)? "P2P":"RLY",
				ST_TIME_USED/1000,
				ST_TIME_USED%1000);
	}
	else // connect success, but remote session closed
	{
		st_info("RemoteAddr=Unknown (remote closed), Mode=Unknown, Time=%d.%03d (Sec)\n", ST_TIME_USED/1000, ST_TIME_USED%1000);
		PPCS_Close(gSessionID);
		st_debug("-%02d-PPCS_Close(%d).\n", Repeat, gSessionID);
		return ret;
	}
	
	// Read Mode from Client
	char Mode = -1;
	int ReadSize = 1;
	ret = PPCS_Read(gSessionID, CH_CMD, (CHAR*)&Mode, &ReadSize, 2000);
	if (0 > ret)
	{
		if (ERROR_PPCS_TIME_OUT == ret) 
		{
			printf("fail to read Test Mode!!\n");
		}	
		else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret) 
		{
			printf("Remote site call close!!\n");
		}
		else 
		{
			st_debug("PPCS_Read: Channel=%d, ret=%d\n", CH_CMD, ret);
		}
		PPCS_Close(gSessionID);
		st_debug("-%02d-PPCS_Close(%d)\n", Repeat, gSessionID);
		return ret;
	}
	if (-1 != Mode) 
	{
		gThread_Exit = 1; // Exit the LoginStatus_Check thread
	}
	st_debug("Test Mode=%d, gSessionID=%d\n", Mode, gSessionID);
	
	// Select the test options according to the Mode
	switch (Mode)
	{
	case 0: ft_Test(); break;	// File transfer test
	case 1: RW_Test(); break;	// Bidirectional read write test
	case 2: 
	{
#ifdef PKT_TEST
		pkt_Test(); 			// PktRecv/PktSend test
#endif
		break;	
	}
	default: st_info("the Mode(%d) Unknown!!\n", Mode); 
		break;
	}
	mSleep(50);
	PPCS_Close(gSessionID);
	st_debug("-%02d-PPCS_Close(%d)\n", Repeat, gSessionID);
	return gSessionID;
}

INT32 main(INT32 argc, CHAR **argv)
{	
	// 1. get P2P API Version
	UINT32 APIVersion = PPCS_GetAPIVersion();
	st_info("P2P API Version: %d.%d.%d.%d\n",
							(APIVersion & 0xFF000000)>>24, 
							(APIVersion & 0x00FF0000)>>16, 
							(APIVersion & 0x0000FF00)>>8, 
							(APIVersion & 0x000000FF)>>0);
	if (4 > argc || 9 < argc)
	{
		printf("Usage: %s DID APILicense InitString [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n",argv[0]);
		printf("   Or: %s DID APILicense:CRCKey InitString [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n\n",argv[0]);
		printf("\tDID: This is Device ID, such as: ABCD-123456-ABCDEF\n");
		printf("\tAPILicense: If you set up CRCKey, this parameter must enter APILicense:CRCKey, if not, you only need to enter APILicense.\n");
		printf("\tInitString: The encrypted string of the server IP address or the domain name.\n");
		printf("\t[Repeat]: The number of repeated calls to PPCS_Listen.\n");
		printf("\t[WakeupKey]: Wakeup Server String Key.\n");
		printf("\t[IP1],[IP2],[IP3]: Three Wakeup Server IP or domain name, You need to enter at least one IP.\n\n");
		
		const char *SampleInitString =  "EBGAEIBIKHJJGFJKEOGCFAEPHPMAHONDGJFPBKCPAJJMLFKBDBAGCJPBGOLKIKLKAJMJKFDOOFMOBECEJIMM";
		
		printf("Example:\n");
		printf("\t%s ABCD-123456-ABCDEF ABCDEF %s\n", argv[0], SampleInitString);
		printf("\t%s ABCD-123456-ABCDEF ABCDEF:ABC123 %s 10\n", argv[0], SampleInitString);
		printf("\t%s ABCD-123456-ABCDEF ABCDEF:ABC123 %s 10 0123456789ABCDEF 127.0.0.1\n", argv[0], SampleInitString);
		printf("\t%s ABCD-123456-ABCDEF ABCDEF:ABC123 %s 10 0123456789ABCDEF 127.0.0.1 127.0.0.1\n", argv[0], SampleInitString);
		printf("\t%s ABCD-123456-ABCDEF ABCDEF:ABC123 %s 10 0123456789ABCDEF 127.0.0.1 127.0.0.1 127.0.0.1\n", argv[0], SampleInitString);
		return 0;
	}
	else if (6 == argc)
	{
		printf("Please enter three wake up server IP or domain name:[IP1] [IP2] [IP3]\n");
		printf("Usage: %s DID APILicense InitString [Repeat] [WakeupKey] [IP1] [IP2] [IP3]\n",argv[0]);
		return 0;
	}
	
	char DID[SIZE_DID];
	char APILicense[SIZE_APILICENSE];
	char InitString[SIZE_INITSTRING];
	char WakeupKey[SIZE_WAKEUP_KEY];
	int Repeat = 1;
	unsigned char NumberOfServer = 0;
	const int TCP_Port = 12306;
	char dest[30];
	struct sockaddr_in serveraddr[SERVER_NUM];
	
	memset(DID, 0, sizeof(DID));
	memset(APILicense, 0, sizeof(APILicense));
	memset(InitString, 0, sizeof(InitString));
	memset(WakeupKey, 0, sizeof(WakeupKey));
	memset(dest, 0, sizeof(dest));
	memset(serveraddr, 0, sizeof(serveraddr));
	
	memcpy(DID, argv[1], strlen(argv[1]));
	memcpy(APILicense, argv[2], strlen(argv[2]));
	memcpy(InitString, argv[3], strlen(argv[3]));
	if (5 <= argc) Repeat = atoi(argv[4]);
	if (6 < argc) memcpy(WakeupKey, argv[5], strlen(argv[5]));	
	if (7 <= argc)
	{
		int count = 0;
		NumberOfServer = argc - 6;
		for (int i = 0; i < SERVER_NUM; i++)
		{
			serveraddr[i].sin_family = AF_INET;
			serveraddr[i].sin_port = htons(TCP_Port);
			serveraddr[i].sin_addr.s_addr = inet_addr("127.0.0.1");
		}
		
		st_debug("gethostbyname...\n");
		for (int i = 0; i < NumberOfServer; i++)
		{
			// gethostbyname: get the server's DNS entry
			struct hostent *Host = gethostbyname(argv[6+i]);
			if (!Host)
			{
				fprintf(stderr, "ERROR, no such host as %s\n", argv[6+i]);
				perror("gethostbyname failed");
				count++;
			}
			else
			{
				// build the server's Internet address
				serveraddr[i].sin_family = Host->h_addrtype;
				serveraddr[i].sin_port = htons(TCP_Port);
				serveraddr[i].sin_addr.s_addr = *((unsigned int*)Host->h_addr_list[0]);
				//bcopy((char *)Host.h_addr, (char *)&serveraddr[i].sin_addr.s_addr, Host.h_length);
			}
		}
		if (NumberOfServer == count) 
		{
			error("ERROR, gethostbyname failed");
		}
		for (int i = 0; i < SERVER_NUM; i++)
		{
			//st_debug("Host[%d]:%s\n", i, inet_ntop(serveraddr[i].sin_family, (char *)&serveraddr[i].sin_addr.s_addr, dest, sizeof(dest)));
			st_debug("Host[%d]:%s\n", i, inet_ntoa(serveraddr[i].sin_addr));
		}
	}
	
	st_debug("DID = %s\n", DID);
	st_debug("APILicense = %s\n", APILicense);
	st_debug("InitString = %s\n", InitString);
	st_debug("Repeat = %d\n", Repeat);
	if (6 < argc) st_debug("WakeupKey = %s\n", WakeupKey);
	if (6 < argc) st_debug("NumberOfServer = %d\n\n", NumberOfServer);
	
	if (7 <= argc)
	{
#if defined(WINDOWS) || defined(WIN32DLL)
		WSADATA wsaData;
		WORD socketVersion = MAKEWORD(2, 2);
		if (WSAStartup(socketVersion, &wsaData) != 0)
		{
			st_info("Init socket dll error! errno=%d (%s)\n", errno, strerror(errno));
			return 0;
		}
#endif		
	}
	
	// 2. P2P Initialize
	st_debug("PPCS_Initialize(%s) ...\n", InitString);
	INT32 ret = PPCS_Initialize((char *)InitString);
	st_debug("PPCS_Initialize done! ret=%d\n", ret);
	if (ERROR_PPCS_SUCCESSFUL != ret && ERROR_PPCS_ALREADY_INITIALIZED != ret)
	{
		st_info("PPCS_Initialize failed!! ret=%d: %s\n", ret, getP2PErrorCodeInfo(ret));
		return 0;
	}
	
	// 3. Network Detect
	st_PPCS_NetInfo NetInfo;
	ret = PPCS_NetworkDetect(&NetInfo, 0);
	if (0 > ret) 
	{
		st_info("PPCS_NetworkDetect failed: ret=%d\n", ret);
	}
	showNetwork(NetInfo);
	
	char CMD[64];
	unsigned short SizeOfCMD = 0;
	if (7 <= argc)
	{
		// 加密DID，设备与唤醒服务器建立TCP连接后第一次发封包必须要发送加密的DID
		if (0 > iPN_StringEnc(WakeupKey, DID, CMD, sizeof(CMD))) 
		{
			st_info("StringEncode failed.\n");
			ret = PPCS_DeInitialize();
			st_debug("PPCS_DeInitialize() done!\n");
			return 0;
		}
		SizeOfCMD = strlen(CMD);
		st_debug("[%s] %u Byte -> [%s] %u Byte\n", DID, (unsigned)strlen(DID), CMD, SizeOfCMD);
	}
	
	// 4. do job
	int skt = -1;
	unsigned short i = 0;
	unsigned short index = 0;
	int size_W = 0;
	int size_R = 0;
	char readBuf[1024];
	st_Time_Info TimeBegin, TimeEnd;
	
	const int Total_Times = Repeat;
	Repeat = 1;
	while (Repeat <= Total_Times)
	{
		st_debug("----------------------tryCount=%02d\n", Repeat);
		if (7 <= argc) // wakeup test
		{
			if (0 > skt)
			{
				srand((UINT32)time(NULL));
				i = abs(rand() % NumberOfServer);
				for (int x = 0; x < NumberOfServer; x++)
				{ 
					if (0 > skt)
					{
						st_debug("Create TCP Socket...\n");
						skt = socket(AF_INET, SOCK_STREAM, 0);
						if (0 > skt) error("ERROR, create TCP socket ");
					}
								
					i = (i + 1) % NumberOfServer;
					memset(dest, 0, sizeof(dest));
					//st_debug("index=%d-connecting to Wakeup_Server-%d %s:%d, skt: %d ...\n", i, i, inet_ntop(serveraddr[i].sin_family, (char *)&serveraddr[i].sin_addr.s_addr, dest, sizeof(dest)), TCP_Port, skt);
					
					const char *pAddr = inet_ntoa(serveraddr[i].sin_addr);
					memcpy(dest, pAddr, strlen(pAddr));
					st_debug("%s:%d connecting ...\n", dest, TCP_Port);
					my_GetCurrentTime(&TimeBegin);
					
					if (0 > connect(skt, (struct sockaddr *)&serveraddr[i], sizeof(struct sockaddr)))
					{
						st_info("%s - ", dest);
						setbuf(stdout, NULL);
						perror("ERROR, connecting ");
						my_SocketClose(skt);
						st_debug("close socket: %d\n", skt);
						skt = -1;
						continue;
					}
					else 
					{
						st_info("%s  connected ", dest);
						index = i;
						break;
					}
				} // for 
				if (0 > skt) 
				{
					error("ERROR, connect failed ");
				}					
			} // if (0 > skt)
			
			if (0 >= (size_W = Call_SendCMD(skt, CMD, SizeOfCMD, serveraddr[index], index)))
			{
				skt = -1;
				continue;
			}
			setbuf(stdout, NULL);
			gThread_Exit = 1; // Exit the LoginStatus_Check thread
			unsigned int timeout_ms = 10*1000;//10 sec
			struct timeval timeout;
			i = 0;
			while (0 <= skt)
			{
				fd_set readfds;
				FD_ZERO(&readfds);
				FD_SET(skt, &readfds);
				timeout.tv_sec = (timeout_ms-(timeout_ms%1000))/1000;
				timeout.tv_usec = (timeout_ms%1000)*1000;
				
				// Enter select, wait for wakeup 
				st_debug("Enter select, Waiting to be wakeup ...\n");
				int activity = select(skt+1, &readfds, NULL , NULL , &timeout);
				
				switch (activity)
				{
				case 0: // time out
					{
						st_debug("\n-------------------(select timeout!)\n");
						printf(".");
						setbuf(stdout, NULL);
						if (0 == ++i%6)
						{
							if (0 >= (size_W = Call_SendCMD(skt, CMD, SizeOfCMD, serveraddr[index], index)))
							{
								skt = -1;
								break;
							}
						}					
						continue;
					}
				case -1://select error
					st_debug("\n-------------------(select return -1, errno=%d)\n", errno);
					perror("ERROR, select ");
					if (EINTR != errno)
					{
						my_SocketClose(skt);
						st_debug("close skt: %d\n", skt);
						skt = -1;
						exit(EXIT_FAILURE);
					}
					break;
				default:
					if (FD_ISSET(skt, &readfds)) 
					{
						memset(readBuf, 0, sizeof(readBuf));
						st_debug("\nrecv data ...\n");						
#if defined(WIN32DLL) || defined(WINDOWS)	
						size_R = recv(skt, readBuf, sizeof(readBuf), 0);
#else						
						size_R = read(skt, readBuf, sizeof(readBuf));
#endif
						if (0 >= size_R) 
						{
							st_info("\nTCP read failed(%d)\n", size_R);
							perror("ERROR, recv to socket");
						} 
						else 
						{
							my_GetCurrentTime(&TimeEnd);
							st_info("Wakeup[%d.%03d (sec)]\n", ST_TIME_USED/1000, ST_TIME_USED%1000);
							st_debug("\n------Got Wakeup Cmd(%d Byte):", size_R);;
							for(int i = 0; i < size_R; i++) 
							{
								st_debug("0x%02X%c", readBuf[i]&0xFF, (i==size_R-1)?'\n':' ');
							}
						}
						my_SocketClose(skt);
						st_debug("close skt: %d\n", skt);
						skt = -1;
						break;
					}
				} // switch
				
				if (0 >= size_W) break; 
				if (0 <= size_R) break; 
			} // while (1)
			if (0 < size_R)
			{
				if (0 == gThread_bRunning) 
				{
					CreateThread_LoginStatus_Check();
				}					
				Call_P2P_Listen(DID, APILicense, Repeat);
				Repeat++;
			}
		} // if (7 <= argc)
		else 
		{
			if (0 == gThread_bRunning) 
			{
				CreateThread_LoginStatus_Check();
			}				
			Call_P2P_Listen(DID, APILicense, Repeat);
			Repeat++;
		}
	} // while (Repeat < Total_Times) 
	
	gThread_Exit = 1; // Exit the LoginStatus_Check thread
	
	ret = PPCS_DeInitialize();
	st_debug("PPCS_DeInitialize() done!\n");
	
#if defined(WIN32DLL) || defined(WINDOWS)	
	if (7 <= argc) WSACleanup();
	//printf("Please press any key to exit... ");
	//getchar();
#endif

	return 0;
}