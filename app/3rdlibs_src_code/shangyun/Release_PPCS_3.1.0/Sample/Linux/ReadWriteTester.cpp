//// ReadWriteTester.cpp 
//// Author: Zheng.B.C
////	To test PPCS connection with a specified DID device, from an Internet Host
////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <stdint.h>  // uintptr_t

#ifdef LINUX
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
#if defined(WIN32DLL) || defined(WINDOWS)
#include <windows.h>
#include <direct.h>
#endif

#include "PPCS_API.h"
#include "PPCS_Error.h"

#define 	PKT_TEST

#if defined(WIN32DLL) || defined(WINDOWS)
typedef	 	DWORD					my_threadid;
typedef		HANDLE	 				my_Thread_mutex_t;
#define		my_Mutex_Lock(mutex) 	WaitForSingleObject(mutex, INFINITE) 
#define		my_Mutex_UnLock(mutex) 	ReleaseMutex(mutex) 
#define		my_Thread_exit(a)  		return(a)
#elif defined(LINUX) 
typedef 	pthread_t				my_threadid;
typedef		pthread_mutex_t  		my_Thread_mutex_t;
#define		my_Mutex_Lock(mutex)	pthread_mutex_lock(&mutex)
#define		my_Mutex_UnLock(mutex)	pthread_mutex_unlock(&mutex)
#define		my_Thread_exit(a)		pthread_exit(a) 		
#endif

typedef uintptr_t UINTp;

// show info 开关 -> 终端打印调试信息
static int g_ST_INFO_OPEN = 0;
// debug log 开关 -> 输出到本地log文件
static int g_DEBUG_LOG_FILE_OPEN = 0;
const char *LogFileName = "./ReadWriteTester.log";

#define CH_CMD				0
#define CH_DATA				1
#define SIZE_DID 			30
#define SIZE_INITSTRING 	256
int gSessionID = -99;

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

#define ST_TIME_USED	(int)(TimeEnd.TimeTick_mSec-TimeBegin.TimeTick_mSec)
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

void my_GetCurrentTime(st_Time_Info *Time)
{
#if  defined(WINDOWS) || defined(WIN32DLL)
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
		printf("gettimeofday failed!! errno=%d\n", errno);
		memset(Time, 0, sizeof(st_Time_Info));
		return ;
	}
	//struct tm *ptm = localtime((const time_t *)&mTime.tv_sec); 
	struct tm st_tm = {0};
	struct tm *ptm = localtime_r((const time_t *)&mTime.tv_sec, &st_tm); 
	if (!ptm)
	{
		printf("localtime_r failed!!\n");
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

void st_info(const char *format, ...) 
{
	//if (1 == g_ST_INFO_OPEN) 
	//{
		va_list ap;
		va_start(ap, format);
		vfprintf(stderr, format, ap);
		va_end(ap);
	//}
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
		return "NoError";
    switch (err)
    {
	case 0: return "ERROR_PPCS_SUCCESSFUL";
	case -1: return "ERROR_PPCS_NOT_INITIALIZED";
	case -2: return "ERROR_PPCS_ALREADY_INITIALIZED";
	case -3: return "ERROR_PPCS_TIME_OUT";
	case -4: return "ERROR_PPCS_INVALID_ID";
	case -5: return "ERROR_PPCS_INVALID_PARAMETER";
	case -6: return "ERROR_PPCS_DEVICE_NOT_ONLINE";
	case -7: return "ERROR_PPCS_FAIL_TO_RESOLVE_NAME";
	case -8: return "ERROR_PPCS_INVALID_PREFIX";
	case -9: return "ERROR_PPCS_ID_OUT_OF_DATE";
	case -10: return "ERROR_PPCS_NO_RELAY_SERVER_AVAILABLE";
	case -11: return "ERROR_PPCS_INVALID_SESSION_HANDLE";
	case -12: return "ERROR_PPCS_SESSION_CLOSED_REMOTE";
	case -13: return "ERROR_PPCS_SESSION_CLOSED_TIMEOUT";
	case -14: return "ERROR_PPCS_SESSION_CLOSED_CALLED";
	case -15: return "ERROR_PPCS_REMOTE_SITE_BUFFER_FULL";
	case -16: return "ERROR_PPCS_USER_LISTEN_BREAK";
	case -17: return "ERROR_PPCS_MAX_SESSION";
	case -18: return "ERROR_PPCS_UDP_PORT_BIND_FAILED";
	case -19: return "ERROR_PPCS_USER_CONNECT_BREAK";
	case -20: return "ERROR_PPCS_SESSION_CLOSED_INSUFFICIENT_MEMORY";
	case -21: return "ERROR_PPCS_INVALID_APILICENSE";
	case -22: return "ERROR_PPCS_FAIL_TO_CREATE_THREAD";
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
		st_info("Connection time out, probably the device is off line now\n"); 
		break;			
	case ERROR_PPCS_INVALID_ID:
		st_info("Invalid Device ID\n"); 
		break;
	case ERROR_PPCS_INVALID_PREFIX:
		st_info("Prefix of Device ID is not accepted by Server\n"); 
		break;
	case ERROR_PPCS_DEVICE_NOT_ONLINE:
		st_info("Device is not on line now, nor login in the past 5 minutes\n"); 
		break;
	case ERROR_PPCS_NO_RELAY_SERVER_AVAILABLE:
		st_info("No relay server can provide service\n"); 
		break;
	case ERROR_PPCS_SESSION_CLOSED_REMOTE:
		st_info("Session close remote.\n"); 
		break;
	case ERROR_PPCS_SESSION_CLOSED_TIMEOUT:
		st_info("Session close timeout.\n"); 
		break;
	case ERROR_PPCS_MAX_SESSION:
		st_info("Exceed max Session\n"); 
		break;
	case ERROR_PPCS_UDP_PORT_BIND_FAILED:
		st_info("The specified UDP port can not be binded\n"); 
		break;
	case ERROR_PPCS_USER_CONNECT_BREAK:
		st_info("connect break is called\n"); 
		break;
	default: st_info("%s\n", getP2PErrorCodeInfo(ret));
		break;
	} // switch	
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
		
		if (ret < 0 && ERROR_PPCS_TIME_OUT != ret)
		{
			if (TOTAL_WRITE_SIZE == TotalSize) break;	
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
		if (TOTAL_WRITE_SIZE == TotalSize) break;
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
	
	// Creating a read-write test thread 
	for (INT32 i = 0; i < TEST_NUMBER_OF_CHANNEL; i++)
	{
#ifdef WIN32DLL
		hThreadWrite[i] = CreateThread(NULL, 0, ThreadWrite, (void *)i, 0, NULL);
		if (NULL == hThreadWrite[i]) 
			error("create ThreadWrite failed");
		hThreadRead[i] = CreateThread(NULL, 0, ThreadRead, (void *)i, 0, NULL);
		if (NULL == hThreadRead[i]) 
			error("create ThreadRead failed");
#elif defined(LINUX)
		if (0 != pthread_create(&ThreadWriteID[i], NULL, &ThreadWrite, (void *)i)) 
			error("create ThreadWrite failed");
		if (0 != pthread_create(&ThreadReadID[i], NULL, &ThreadRead, (void *)i)) 
			error("create ThreadRead failed");
#endif
		//mSleep(10);
	}
	
	// Wait for thread to Exit
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
	// Open File
	char fileName[] = "2.7z";
	FILE *fp = fopen(fileName, "wb");
	if (!fp)
	{
		st_info("***Error: failed to open file: %s\n", fileName);
		return -1;
	}
	
	// read data from remote
	unsigned long SizeCounter = 0;
	unsigned int tick = getTickCount();
	while (1)
	{
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		INT32 SizeToRead = sizeof(buf);			
		
		int ret = PPCS_Read(gSessionID, CH_DATA, buf, &SizeToRead, 0xFFFFFFFF);
		
		SizeCounter += SizeToRead;
		// write data to file
		fwrite(buf, SizeToRead, 1, fp);
		
		if (ret < 0)
		{
			if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret)
				st_info("Session Closed TimeOUT!!\n");
			else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret)
				st_info("Session Remote Close!!\n");
			else
				st_info("PPCS_Read: session=%d, channel=%d, SizeToRead=%d, ret=%d %s\n", gSessionID, CH_DATA, SizeToRead, ret, getP2PErrorCodeInfo(ret));
			break;
		}
				
		if (SizeCounter % (1024*1024) == 0) 
		{
			//st_info("%lu MB\r", SizeCounter/(1024*1024));
			st_info(".");
			setbuf(stdout, NULL);
		}
	}
	tick = getTickCount() - tick;
	
	st_info("File Transfer done!! Read Size= %lu Byte, Time:%3d.%03d sec, %4lu KByte/sec\n", SizeCounter, tick/1000, tick%1000, (0==tick)?0:(SizeCounter/tick));
	
	fclose(fp);
	return 0;
}

#ifdef  PKT_TEST
int pkt_Test()
{
	int Counter = 0;
	CHAR ExpectValue = 0; // Expect Value
	st_info("PPCS_PktRecv ...\n\n");
	while (1)
	{
		CHAR PktBuf[1024];
		INT32 PktSize = sizeof(PktBuf);
		memset(PktBuf, 0, PktSize);
		
		int ret = PPCS_PktRecv(gSessionID, CH_DATA, PktBuf, &PktSize, 0xFFFFFFF);
		if (0 > ret)
		{
			if (ERROR_PPCS_SESSION_CLOSED_TIMEOUT == ret)
				st_info("Session Closed TimeOUT!!\n");
			else if (ERROR_PPCS_SESSION_CLOSED_REMOTE == ret)
				st_info("Session Remote Close!!\n");
			else 
				st_info("PPCS_PktRecv: session=%d, channel=%d, PktSize=%d, ret=%d: %s\n", gSessionID, CH_DATA, PktSize, ret, getP2PErrorCodeInfo(ret));
			break;
		}
		
		if (PktSize != 1024) //// we send 1024 Byte packet
			st_info("Packet size error!! PktSize=%d, should be 1024\n", PktSize);
		else 
			st_debug("PPCS_PktRecv: ret=%d, session=%d, channel=%d, data=%d..., size=%d\n", ret, gSessionID, CH_DATA, PktBuf[0], PktSize);
		
		setbuf(stdout, NULL);
		if (ExpectValue != PktBuf[0]) 
		{
			st_info("Packet Lost Detect!! Value = %d (should be %d)\n", PktBuf[0], ExpectValue); 
			ExpectValue = (PktBuf[0] + 1) % 100;
		}
		else ExpectValue = (ExpectValue + 1) % 100;
		
		if (Counter % 100 == 99)
			st_info("Recv %d packets. (1 packets=%u Byte)\n", Counter+1, (unsigned)sizeof(PktBuf));
		fflush(stdout);
		Counter++;
	}
	return 0;
}
#endif

INT32 main(INT32 argc, CHAR **argv)
{	
	// 1. get P2P API Version
	UINT32 APIVersion = PPCS_GetAPIVersion();
	st_info("P2P API Version: %d.%d.%d.%d\n",
				(APIVersion & 0xFF000000)>>24, 
				(APIVersion & 0x00FF0000)>>16, 
				(APIVersion & 0x0000FF00)>>8, 
				(APIVersion & 0x000000FF)>>0);
	if (4 != argc)
	{
		printf("Usage: %s Mode DID InitString\n", argv[0]);
		printf("\tMode: 0->File transfer test.\n");
		printf("\t      1->Bidirectional Read Write test.\n");
#ifdef  PKT_TEST		
		printf("\t      2->PktRecv/PktSend test.\n");
#endif
		return 0;
	}
	
	char DID[SIZE_DID];
	char InitString[SIZE_INITSTRING];
	memset(DID, 0, sizeof(DID));
	memset(InitString, 0, sizeof(InitString));
	
	char Mode = (char)atoi(argv[1]);
	if (0 > Mode || 2 < Mode) 
	{
		printf("No this Mode:%d!\n", Mode);
		return 0;
	}
	memcpy(DID, argv[2], strlen(argv[2]));
	memcpy(InitString, argv[3], strlen(argv[3]));
	st_debug("Mode = %d\n", Mode);
	st_debug("DID = %s\n", DID);
	st_debug("InitString = %s\n", InitString);
	
	// 2. P2P Initialize
	st_debug("PPCS_Initialize(%s) ...\n", InitString);
	INT32 ret = PPCS_Initialize((char *)InitString);
	st_debug("PPCS_Initialize done! ret=%d\n", ret);
	if (ERROR_PPCS_SUCCESSFUL != ret && ERROR_PPCS_ALREADY_INITIALIZED != ret)
	{
		printf("PPCS_Initialize failed!! ret=%d: %s\n", ret, getP2PErrorCodeInfo(ret));
		return 0;
	}
	
	// 3. Network Detect
	st_PPCS_NetInfo NetInfo;
	ret = PPCS_NetworkDetect(&NetInfo, 0);
	if (0 > ret) 
	{
		st_info("PPCS_NetworkDetect failed: ret=%d\n", ret);
	}	
	st_info("-------------- NetInfo: -------------------\n");
	st_info("Internet Reachable     : %s\n", (NetInfo.bFlagInternet == 1) ? "YES":"NO");
	st_info("P2P Server IP resolved : %s\n", (NetInfo.bFlagHostResolved == 1) ? "YES":"NO");
	st_info("P2P Server Hello Ack   : %s\n", (NetInfo.bFlagServerHello == 1) ? "YES":"NO");
	switch (NetInfo.NAT_Type)
	{
	case 0: st_info("Local NAT Type         : Unknow\n"); break;
	case 1: st_info("Local NAT Type         : IP-Restricted Cone\n"); break;
	case 2: st_info("Local NAT Type         : Port-Restricted Cone\n"); break;
	case 3: st_info("Local NAT Type         : Symmetric\n"); break;
	}
	st_info("My Wan IP : %s\n", NetInfo.MyWanIP);
	st_info("My Lan IP : %s\n", NetInfo.MyLanIP);
	st_info("-------------------------------------------\n");
	
	// 4. Connect to Device. 
	st_info("PPCS_Connect(%s, 0x7E, 0)...\n", DID);
	gSessionID = PPCS_Connect(DID, 0x7E, 0);
	
	if (0 > gSessionID)
	{
		st_info("Connect failed: %d [%s]\n", gSessionID, getP2PErrorCodeInfo(gSessionID));
		ret = PPCS_DeInitialize();
		st_debug("PPCS_DeInitialize() done!!\n");
		return 0;
	}
	else
	{
		st_info("Connect Success!! gSessionID= %d.\n", gSessionID);
		st_PPCS_Session Sinfo;	
		if (ERROR_PPCS_SUCCESSFUL == PPCS_Check(gSessionID, &Sinfo))
		{
			st_info("----------- Session Ready: -%s -----------\n", (Sinfo.bMode ==0)? "P2P":"RLY");
			st_info("Socket : %d\n", Sinfo.Skt);
			st_info("Remote Addr : %s:%d\n", inet_ntoa(Sinfo.RemoteAddr.sin_addr),ntohs(Sinfo.RemoteAddr.sin_port));
			st_info("My Lan Addr : %s:%d\n", inet_ntoa(Sinfo.MyLocalAddr.sin_addr),ntohs(Sinfo.MyLocalAddr.sin_port));
			st_info("My Wan Addr : %s:%d\n", inet_ntoa(Sinfo.MyWanAddr.sin_addr),ntohs(Sinfo.MyWanAddr.sin_port));
			st_info("Connection time : %d second before\n", Sinfo.ConnectTime);
			st_info("DID : %s\n", Sinfo.DID);
			st_info("I am %s\n", (Sinfo.bCorD == 0)? "Client":"Device");
			st_info("Connection mode: %s\n", (Sinfo.bMode ==0)? "P2P":"RLY");
			st_info("---------- End of Session info : ----------\n");
		}
	}
	
	// 5. do job ...
	// Tell device the test mode 
	ret = PPCS_Write(gSessionID, CH_CMD, (char*)&Mode, sizeof(Mode));
	//printf("PPCS_Write ret=%d, CH=%d, Data=%d, Size=%d Byte.\n", ret, CH_CMD, Mode, sizeof(Mode));
	if (0 > ret)
	{
		st_info("PPCS_Write: gSessionID=%d, CH=%d, ret=%d [%s]\n", gSessionID, CH_CMD, ret, getP2PErrorCodeInfo(ret));
	}
	else // Select the test options according to the Mode
	{		
		switch (Mode)
		{
		case 0: ft_Test(); break;	// File transfer test
		case 1: RW_Test(); break;	// Bidirectional read write test
		case 2: 
		{
#ifdef  PKT_TEST
			pkt_Test(); 	// PktRecv/PktSend test
#endif
			break;
		}
		default: break;
		}
	}
	
	ret = PPCS_Close(gSessionID);
	st_debug("PPCS_Close(%d) done!! ret=%d\n", gSessionID, ret);
	
	ret = PPCS_DeInitialize();
	st_debug("PPCS_DeInitialize() done!!\n");
	
#if defined(WIN32DLL) || defined(WINDOWS)
	//printf("Please press any key to exit... ");
	//getchar();
#endif	

	return 0;
}