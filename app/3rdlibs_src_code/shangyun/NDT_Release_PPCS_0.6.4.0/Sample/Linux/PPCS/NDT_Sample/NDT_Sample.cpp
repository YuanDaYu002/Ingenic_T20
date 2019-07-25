// NDT_Sample.cpp : 定義主控台應用程式的進入點。
//
#if defined WINDOWS 
#include <windows.h>
#include <stdio.h>
#elif defined LINUX
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#endif

#include "NDT_Type.h"
#include "NDT_API.h"
#include "NDT_Error.h"

void mSecSleep(UINT32 ms)
{
#if defined WINDOWS
	Sleep(ms);
#elif defined LINUX
	usleep(ms * 1000);
#endif
}

typedef struct {
	CHAR ResponseBuf[1280];
	INT32 ClientHandle;
	UINT16 ResponseSize;
	CHAR Reserved[2];
} st_SendBack_Info;

//// Todo: Modify this for your own InitString
const CHAR *DefaultInitString = "EBGAEIBIKHJJGFJKEOGCFAEPHPMAHONDGJFPBKCPAJJMLFKBDBAGCJPBGOLKIKLKAJMJKFDOOFMOBECEJIMM"; 
//// Todo: Modify this for your own AES128Key 
const CHAR *DefaultAES128Key = "0123456789ABCDEF";	

void ShowUsage(CHAR *ProgramName)
{
	printf("Usage: \n  %s DID FuncCode [Repeat] [InitString] [AES128Key]\n", ProgramName);
	printf("\tDID: The DID or DID+NDTLicense: ABCD-123456-ABCDEF:PQRSTU\n");
	printf("\tFuncCode: Fuction code, refer to following function code explanation\n");
	printf("\tRepeat: Repeat times, if this parameter is empty or '0', the repeat times value is unlimited\n");
	printf("\tInitString: The InitString, if this parameter is empty, program default InitString will be used.\n");
	printf("\tAES128Key: The AES128Key, if this parameter is empty, program default AES128Key will be used.\n");

	printf("FuncCode:\n");
	printf("  [s]: SendTo Testing\n");
	printf("  [s+]: Reliable Send Testing\n");
	printf("  [r]: Recv Testing\n\n");

	printf("  [sb]: SendBack Testing\n");
	printf("  [sb+]: Reliable SendBack Testing\n");
	printf("  [rf]: RecvFrom Testing\n\n");

	printf("  [es]: Echo Server, echo any data back to sender, (Recv, Sendback, multithread, Reliable)\n");
	printf("  [ec]: Echo Client, send data to echo Server then receive data, (SendTo, RecvFrom, Reliable)\n\n");
	
	printf("  [bs]: Bi-direction Server: 2-Way data transmission Testing \n");
	printf("  [bc]: Bi-direction Client: 2-Way data transmission Testing\n\n");

	printf("  NDT_Client: all Client FuncCode <---> NDT_Device: r/sb/sb+\n");
	printf("  Client: s or s+ <---> Device: r\n");
	printf("  Client: rf      <---> Device: sb or sb+\n");
	printf("  Client: ec 	  <---> Device: es\n");
	printf("  Client: bc 	  <---> Device: bs\n\n");
	
	printf("Example:\n");
	printf("\t%s PPCS-000000-MYGXJ s\n",ProgramName);
	printf("\t%s PPCS-000000-MYGXJ s 100\n",ProgramName);
	printf("\t%s PPCS-000000-MYGXJ s 100 %s %s\n",ProgramName, DefaultInitString, DefaultAES128Key);
	printf("\t%s PPCS-000000-MYGXJ s+\n",ProgramName);
	printf("\t%s PPCS-000000-MYGXJ:CFGMLT r\n",ProgramName);

	printf("\t%s PPCS-000000-MYGXJ:CFGMLT sb\n",ProgramName);
	printf("\t%s PPCS-000000-MYGXJ:CFGMLT sb+\n",ProgramName);
	printf("\t%s PPCS-000000-MYGXJ:CFGMLT sb+ 100 %s %s\n",ProgramName, DefaultInitString, DefaultAES128Key);
	printf("\t%s PPCS-000000-MYGXJ rf 100 %s %s\n",ProgramName, DefaultInitString, DefaultAES128Key);

	printf("\t%s PPCS-000000-MYGXJ:CFGMLT es\n",ProgramName);
	printf("\t%s PPCS-000000-MYGXJ ec 100\n",ProgramName);

	printf("\t%s PPCS-000000-MYGXJ:CFGMLT bs\n",ProgramName);
	printf("\t%s PPCS-000000-MYGXJ bc 1000\n",ProgramName);
}

const char *getNDTErrorCodeInfo(int err)
{
	if (0 < err) 
	{
		return "NoError";
	}
	switch (err)
	{
	case 0: return "NDT_ERROR_NoError";
	case -1: return "NDT_ERROR_AlreadyInitialized";
	case -2: return "NDT_ERROR_NotInitialized";
	case -3: return "NDT_ERROR_TimeOut";
	case -4: return "NDT_ERROR_ScketCreateFailed";
	case -5: return "NDT_ERROR_ScketBindFailed";
	case -6: return "NDT_ERROR_HostResolveFailed";
	case -7: return "NDT_ERROR_ThreadCreateFailed";
	case -8: return "NDT_ERROR_MemoryAllocFailed";
	case -9: return "NDT_ERROR_NotEnoughBufferSize";
	case -10: return "NDT_ERROR_InvalidInitString";
	case -11: return "NDT_ERROR_InvalidAES128Key";
	case -12: return "NDT_ERROR_InvalidDataOrSize";
	case -13: return "NDT_ERROR_InvalidDID";
	case -14: return "NDT_ERROR_InvalidNDTLicense";
	case -15: return "NDT_ERROR_InvalidHandle";
	case -16: return "NDT_ERROR_ExceedMaxDeviceHandle";
	case -17: return "NDT_ERROR_ExceedMaxClientHandle";
	case -18: return "NDT_ERROR_NetworkDetectRunning";
	case -19: return "NDT_ERROR_SendToRunning";
	case -20: return "NDT_ERROR_RecvRunning";
	case -21: return "NDT_ERROR_RecvFromRunning";
	case -22: return "NDT_ERROR_SendBackRunning";
	case -23: return "NDT_ERROR_DeviceNotOnRecv";
	case -24: return "NDT_ERROR_ClientNotOnRecvFrom";
	case -25: return "NDT_ERROR_NoAckFromCS";
	case -26: return "NDT_ERROR_NoAckFromPushServer";
	case -27: return "NDT_ERROR_NoAckFromDevice";
	case -28: return "NDT_ERROR_NoAckFromClient";
	case -29: return "NDT_ERROR_NoPushServerKnowDevice";
	case -30: return "NDT_ERROR_NoPushServerKnowClient";
	case -31: return "NDT_ERROR_UserBreak";
	case -32: return "NDT_ERROR_SendToNotRunning";
	case -33: return "NDT_ERROR_RecvNotRunning";
	case -34: return "NDT_ERROR_RecvFromNotRunning";
	case -35: return "NDT_ERROR_SendBackNotRunning";
	case -36: return "NDT_ERROR_RemoteHandleClosed";
	case -99: return "NDT_ERROR_FAESupportNeeded";
	default: return "Unknow, something is wrong!";
	}
}

//// RecvFrom thread of BC
//// 0: Not Running, 1: Running, 2: About thread exit ...
CHAR gFlagStatus_thread_BC_RecvFrom = 0; 
#if defined WINDOWS
DWORD WINAPI thread_BC_RecvFrom(void* arg)
#elif defined LINUX
void *thread_BC_RecvFrom(void *arg)
#endif
{
	INT32 RetryCounter;
	INT32 RecvFromCounter;
	INT32 ret;
	CHAR Buf[1280];
	UINT16 Size;
	INT32 DeviceHandle = (INT32) ((intptr_t)arg);
	
	printf("thread_BC_RecvFrom running... DeviceHandle=%d\n", DeviceHandle);

	RecvFromCounter = 0;
	gFlagStatus_thread_BC_RecvFrom = 1;
	while (1)
	{
		RetryCounter = 0;
		memset(Buf, 0 , sizeof(Buf));
		Size = sizeof(Buf);
		RecvFromCounter += 1;
		while (RetryCounter < 3)
		{
			ret = NDT_PPCS_RecvFrom(DeviceHandle, Buf, &Size, 5000); //// 5 sec
			if (ret == NDT_ERROR_NoError) 
			{
				break;
			}
			else 
			{
				printf("NG: NDT_PPCS_RecvFrom(), ret=%d, RetryCounter=%d, DeviceHandle=%d [%s]\n", ret, RetryCounter, DeviceHandle, Buf);
				if(ret == NDT_ERROR_InvalidHandle)
					RetryCounter = 3;
			}
			RetryCounter++;
		}
		if (RetryCounter >= 3)
		{
			printf("**** ERROR!!! **** RecvFrom Failed for DeviceHandle = %d, Return Code = %d\n", DeviceHandle, ret);
			break;
		}
		if (RecvFromCounter != atoi(Buf))
		{
			printf("**** ERROR!! **** DeviceHandle=%d, Recv=[%s], Sould be=%06d\n",DeviceHandle, Buf, RecvFromCounter);
			break;
		}
		else
		{
			printf("DeviceHandle=%d, Recv=[%s]\n",DeviceHandle, Buf);
		}
	}
	gFlagStatus_thread_BC_RecvFrom = 2;
	NDT_PPCS_CloseHandle(DeviceHandle);
	printf("thread_BC_RecvFrom exit... DeviceHandle=%d\n", DeviceHandle);
#if defined WINDOWS
	return 0;
#elif defined LINUX
	pthread_exit(0);
#endif
}

//// SendBack thread of BS
#if defined WINDOWS
DWORD WINAPI thread_BS_SendBack(void* arg)
#elif defined LINUX
void *thread_BS_SendBack(void *arg)
#endif
{
	INT32 RetryCounter;
	INT32 SendBackCounter;
	INT32 ret;
	CHAR Buf[1280];
	INT32 ClientHandle = (INT32) ((intptr_t)arg);
	printf("thread_BS_SendBack running... ClientHandle=%d\n", ClientHandle);

	mSecSleep(100);
	SendBackCounter = 0;
	while (1)
	{
		RetryCounter = 0;
		SendBackCounter++;
		memset(Buf, 0 , sizeof(Buf));
		sprintf(Buf, "%06d",SendBackCounter);
		while (RetryCounter < 3)
		{
			RetryCounter++;
			ret = NDT_PPCS_SendBack(ClientHandle, Buf, (UINT16)strlen(Buf), 5000);
			if (ret == NDT_ERROR_NoError) 
				break;
			else 
			{
				//printf("NG: NDT_PPCS_SendBack(), ret=%d, RetryCounter=%d, ClientHandle=%d [%s]\n", ret, RetryCounter, ClientHandle, Buf);
				if (ret == NDT_ERROR_InvalidHandle)
					RetryCounter = 3;
				mSecSleep(50);
			}	
		}
		if (RetryCounter >= 3)
		{
			printf("**** ERROR!!! **** SendBack Failed for ClientHandle = %d, Return Code = %d [%s]\n", ClientHandle, ret, Buf);
			break;
		}
	}
	NDT_PPCS_CloseHandle(ClientHandle);
#if defined WINDOWS
	return 0;
#elif defined LINUX
	pthread_exit(0);
#endif
}

//// SendBack thread of ES
#if defined WINDOWS
DWORD WINAPI thread_ES_SendBack(void* arg)
#elif defined LINUX
void *thread_ES_SendBack(void *arg)
#endif
{
	INT32 RetryCounter = 0;
	INT32 ret;
	st_SendBack_Info *pSB_Info;
	pSB_Info = (st_SendBack_Info *)arg;
	printf("thread_ES_SendBack running... ClientHandle=%d, [%s]\n", pSB_Info->ClientHandle, pSB_Info->ResponseBuf);

	mSecSleep(100); 
	while (RetryCounter < 3)
	{
		RetryCounter++;
		ret = NDT_PPCS_SendBack(pSB_Info->ClientHandle, pSB_Info->ResponseBuf, pSB_Info->ResponseSize, 5000);
		if (ret == NDT_ERROR_NoError) 
			break;
		else 
		{
			printf("NG: NDT_PPCS_SendBack(), ret=%d, RetryCounter=%d, ClientHandle=%d [%s]\n", ret, RetryCounter, pSB_Info->ClientHandle, pSB_Info->ResponseBuf);
			if(ret == NDT_ERROR_SendBackRunning)
				mSecSleep(1000);
			if(ret == NDT_ERROR_InvalidHandle)
				RetryCounter = 3;
		}	
	}
	if (RetryCounter >= 3)
	{
		printf("**** ERROR!!! **** SendBack Failed for ClientHandle = %d, Return Code = %d [%s]\n", pSB_Info->ClientHandle, ret, pSB_Info->ResponseBuf);
	}
	else
	{
		printf("thread_ES_SendBack Exit. ClientHandle = %d, [%s]\n",pSB_Info->ClientHandle, pSB_Info->ResponseBuf);
	}
	NDT_PPCS_CloseHandle(pSB_Info->ClientHandle);
	free(pSB_Info);
#if defined WINDOWS
	return 0;
#elif defined LINUX
	pthread_exit(0);
#endif
}

INT32 main(INT32 argc, CHAR* argv[])
{
	UINT32 APIVersion;
	CHAR *API_Description = NDT_PPCS_GetAPIVersion(&APIVersion);
	printf("NDT API Version: %d.%d.%d.%d \nAPI Description:%s\n", 
								(APIVersion & 0xFF000000) >> 24, 
								(APIVersion & 0x00FF0000) >> 16, 
								(APIVersion & 0x0000FF00) >> 8, 
								(APIVersion & 0x000000FF) >> 0, 
								API_Description);
	printf("---------------------------------------------\n");							
	if (argc < 3 || argc > 6)
	{
		ShowUsage(argv[0]);
		return 0;
	}
	
	INT32 SendToMode = 1;  // for SendTo()
	//// SendToMode = 0 : Sending via Both LAN and Internet
	//// SendToMode = 1 : Sending via Internet only
	//// SendToMode = 2 : Sending via LAN only 
	
	INT32 Option = 0;  // for Recv()
	//// Option = 0 --> Recvive command both from LAN and Internet
	//// Option = 1 --> Only receive command from Client of LAN

	
	UINT32 RepeatTimes = 0xFFFFFFFF;
	UINT32 Counter, SuccessCounter;
	CHAR DID[64] = {0};
	CHAR InitString[256]  = {0};
	CHAR AES128Key[20]  = {0};
	//UINT16 Port = rand() % 10000 + 10000; //The working UDP Port Number, 0 --> decided by system.
	UINT16 Port = 0;
	INT32 ret, ClientHandle = -1, DeviceHandle = -1;
	INT32 BSRecvCounter[16];

	//// Get AES128Key, InitString
	if (argc == 6)
	{
		strcpy(AES128Key, argv[5]);
		strcpy(InitString, argv[4]);
	}
	else
	{
		strcpy(AES128Key, DefaultAES128Key);
		strcpy(InitString, DefaultInitString);
	}

	//// Get RepeatTimes
	if (argc == 3)
		RepeatTimes = 0xFFFFFFFF;
	else if ((atoi(argv[3]) == 0) && (argv[3][0] == '0'))
		RepeatTimes = 0xFFFFFFFF;
	else
		RepeatTimes = (UINT32)atoi(argv[3]);
	
	strncpy(DID, argv[1], sizeof(DID));

	// 1. Initialize
	if ((strcmp(argv[2], "r") == 0) || 
	   (strcmp(argv[2], "sb") == 0) ||
	   (strcmp(argv[2], "sb+")  == 0) ||
	   (strcmp(argv[2], "es")  == 0) || 
	   (strcmp(argv[2], "bs")  == 0) )
	{
		//printf("NDT_PPCS_Initialize(%s, %d, %s, %s)...\n", InitString, Port, DID, AES128Key);
		ret = NDT_PPCS_Initialize(InitString, Port, DID, AES128Key);// Device
		printf("NDT_PPCS_Initialize ret = %d\n",ret);
		//printf("InitString=%s\n",InitString);
	}
	else if ((strcmp(argv[2], "s") == 0) || 
			(strcmp(argv[2], "s+") == 0) || 
			(strcmp(argv[2], "rf") == 0) || 
			(strcmp(argv[2], "ec")  == 0) ||
			(strcmp(argv[2], "bc")  == 0))
	{
		//printf("NDT_PPCS_Initialize(%s, %d, NULL, %s)", InitString, Port, AES128Key);
		ret = NDT_PPCS_Initialize(InitString, Port, NULL, AES128Key);// Client
		printf("NDT_PPCS_Initialize ret = %d\n", ret);
	}
	else
	{
		ShowUsage(argv[0]);
		return 0;
	}
	if (NDT_ERROR_NoError != ret && NDT_ERROR_AlreadyInitialized != ret)
		return 0;
	
	// 2. Network Detect
	st_NDT_NetInfo NetInfo;
	for (int i = 0; i < 3; i++)
	{
		ret = NDT_PPCS_NetworkDetect(&NetInfo, 3000); //// wait for 3 sec 
		printf("NDT_PPCS_NetworkDetect done! ret=%d\n", ret);
		printf("------------- NetInfo: --------------\n");
		printf("My Lan IP:Port=%s:%d\n", NetInfo.LanIP, NetInfo.LanPort);
		printf("My Wan IP:Port=%s:%d\n", NetInfo.WanIP, NetInfo.WanPort);
		printf("Server Hello Ack: %s\n", 1 == NetInfo.bServerHelloAck ? "Yes":"No");
		printf("-------------------------------------\n");
		if (0 == NetInfo.bServerHelloAck)
		{
			printf("*** Warning!! CS didn't response!!\n");
			mSecSleep(1000);
			continue;
		}
		else break;
	}
	if (0 == NetInfo.bServerHelloAck) 
		return 0;
	
	// 3. Do Job
	Counter = 0;
	SuccessCounter = 0;
	while (Counter < RepeatTimes)
	{
		Counter++;
		// Client: "s" or "s+" <--> Device: "r"
		if ((strcmp(argv[2], "s") == 0) || (strcmp(argv[2], "s+") == 0))
		{
			CHAR Buf[1280] = {0};
			sprintf(Buf, "Hello NDT!! Counter=%06d", Counter);
			while(1)
			{
				ret = NDT_PPCS_SendToByServer(DID, Buf, (UINT16)strlen(Buf), SendToMode, InitString, AES128Key);
				if (ret < 0)
				{
					printf("**** NG: NDT_PPCS_SendToByServer(), ret = %d, [%s]\n", ret, Buf);
					if (argv[2][1] == '+') //// Need Reliably Sent 
						continue;
				}
				else
				{
					DeviceHandle = ret;
					printf("%d, NDT_PPCS_SendToByServer() OK, DeviceHandle = %d, [%s]\n", Counter, DeviceHandle, Buf);
				}
				break;
			}
			if ((SuccessCounter == RepeatTimes) && (DeviceHandle >= 0))
				printf("NDT_PPCS_CloseHandle ret=%d\n", NDT_PPCS_CloseHandle(DeviceHandle));  // Call CloseHandle() if you don't wanna call any more SendTo() for this DID 
			else
				mSecSleep(10);
		}
		else if (strcmp(argv[2], "r") == 0)
		{
			CHAR Buf[1280] = {0};
			UINT16 Size = sizeof(Buf);
			memset(Buf, 0 , Size);
			ret = NDT_PPCS_Recv(Buf, &Size, 60000, Option);
			if (ret >= 0)
			{
				ClientHandle = ret;
				SuccessCounter ++;
				printf("%d, NDT_PPCS_Recv() OK, ClientHandle = %d, Size =%d, [%s]\n",SuccessCounter, ClientHandle, Size, Buf);
			}
			else
			{
				printf("**** NG: NDT_PPCS_Recv() ret = %d\n", ret);
			}
			if ((SuccessCounter == RepeatTimes) && (ClientHandle >= 0))
				printf("NDT_PPCS_CloseHandle ret=%d\n", NDT_PPCS_CloseHandle(ClientHandle));// Call CloseHandle() if sendback() is not required for this Client. 
		}
		else if (strcmp(argv[2], "rf") == 0)// Client: "rf" <--> Device: "sb" or "sb+"
		{
			CHAR Buf[1280] = {0};
			UINT16 Size = sizeof(Buf);
			//// call SendTo first
			if (Counter == 1)
			{
				memset(Buf, 0 , Size);
				sprintf(Buf, "Hello NDT!! Counter=%06d", Counter);
				while (1)
				{
					ret = NDT_PPCS_SendToByServer(DID, Buf, (UINT16)strlen(Buf), SendToMode, InitString, AES128Key);
					if (ret < 0)
					{
						printf("**** NG: Failed to Get DeviceHandle, NDT_PPCS_SendToByServer ret=%d\n", ret);
						continue;
					}
					DeviceHandle = ret;
					break;
				}
			}
			memset(Buf, 0 , Size);
			ret = NDT_PPCS_RecvFrom(DeviceHandle, Buf, &Size, 600000);
			if (ret == NDT_ERROR_NoError)
			{
				SuccessCounter ++;
				printf("%d, NDT_PPCS_RecvFrom() ret = %d, Size =%d, [%s]\n",SuccessCounter, ret, Size, Buf);
			}
			else
			{
				printf("**** NG: NDT_PPCS_RecvFrom() ret = %d \n", ret);
			}
			if ((SuccessCounter == RepeatTimes) && (DeviceHandle >= 0))
				printf("NDT_PPCS_CloseHandle ret=%d\n", NDT_PPCS_CloseHandle(DeviceHandle));	// Call CloseHandle() if RecvFrom() is not required for this Device. 
		}
		else if ((strcmp(argv[2], "sb+") == 0) || (strcmp(argv[2], "sb") == 0))
		{
			CHAR Buf[1280] = {0};
			UINT16 Size = sizeof(Buf);
			if (1 == Counter)
			{
				memset(Buf, 0 , Size);
				//printf("%d, NDT_PPCS_Recv ...\n", Counter);
				ret = NDT_PPCS_Recv(Buf, &Size, 0, Option);
				if (ret < 0)
				{
					printf("**** NG: Failed to Get ClientHandle, NDT_PPCS_Recv ret=%d\n", ret);
					return 0;
				}
				ClientHandle = ret;
				printf("%d, NDT_PPCS_Recv: Data:[%s], Size: %u byte\n", Counter, Buf, (unsigned)strlen(Buf));
			}
			//mSecSleep(70000);
			mSecSleep(100);
			while (1)
			{
				CHAR Buf[1280] = {0};
				sprintf(Buf, "Hello NDT!! Counter=%06d", Counter);
				ret = NDT_PPCS_SendBack(ClientHandle, Buf, (UINT16)strlen(Buf), 5000);
				if (0 > ret)
				{
					printf("%d, **** NG: NDT_PPCS_SendBack(), ret=%d, [%s]\n", Counter, ret, getNDTErrorCodeInfo(ret));
					if (NDT_ERROR_RemoteHandleClosed == ret)
						RepeatTimes = Counter;
					else if (argv[2][2] == '+') //// Need Reliably SentBack 
						continue;
				}
				else 
					printf("%d, NDT_PPCS_SendBack() OK! ret=%d, [%s]\n", Counter, ret, Buf);
				break;
			}
			if ((Counter == RepeatTimes) && (ClientHandle >= 0))
				printf("NDT_PPCS_CloseHandle(%d) ret=%d\n", ClientHandle, NDT_PPCS_CloseHandle(ClientHandle));  // Call CloseHandle() if you don't wanna call any more SendBack() for this Client    
		}
		else if (strcmp( argv[2], "es") == 0) // Client: "ec" <---> Device: "es"
		{
#if defined WINDOWS
			HANDLE hthread_ES_SendBack;
#elif defined LINUX
			pthread_t hthread_ES_SendBack;
#endif
			CHAR Buf[1280] = {0};
			UINT16 Size = sizeof(Buf);
			st_SendBack_Info *pSB_Info = (st_SendBack_Info *)malloc(sizeof(st_SendBack_Info));
			if (!pSB_Info)
			{
				printf("**** ERROR!! **** Malloc failed!\n");
				return 0;
			}
			else memset(pSB_Info, 0, sizeof(st_SendBack_Info));
			
			printf("%d, NDT_PPCS_Recv ...\n", Counter);
			ret = NDT_PPCS_Recv(Buf, &Size, 0, Option);
			
			//// Check if successfully Recv a Command
			if (ret < 0)
			{
				printf("**** ERROR !! Failed to Get ClientHandle, NDT_PPCS_Recv ret=%d ****\n", ret);
				if (ret == NDT_ERROR_ExceedMaxClientHandle) //// Sleep 1 sec, if ExceedMaxClientHandle
					mSecSleep(1000);
				continue;
			}
			ClientHandle = ret;
			
			//// Prepare SB_Info 
			pSB_Info->ClientHandle = ClientHandle;
			memset(pSB_Info->ResponseBuf, 0, sizeof(pSB_Info->ResponseBuf));
			memcpy(pSB_Info->ResponseBuf, Buf, Size);
			pSB_Info->ResponseSize = Size;
			//// Create SendBack thread
#ifdef WINDOWS
			hthread_ES_SendBack = CreateThread(NULL, 0, thread_ES_SendBack, (void *) pSB_Info, 0, NULL);
			if (hthread_ES_SendBack) CloseHandle(hthread_ES_SendBack);
#elif defined LINUX
			pthread_create(&hthread_ES_SendBack, NULL, &thread_ES_SendBack, (void *) pSB_Info);
			if (hthread_ES_SendBack) pthread_detach(hthread_ES_SendBack);
#endif
		}
		else if (strcmp(argv[2], "ec") == 0)
		{
			CHAR Buf_SendTo[1280] = {0};
			CHAR Buf_RecvFrom[1280] = {0};
			INT32 RetryCounter = 0;
			CHAR FlagWaitRemoteClose = 0;
			sprintf(Buf_SendTo, "Hello NDT!! Counter=%06d", SuccessCounter);
			while (RetryCounter < 3)
			{
				RetryCounter++;
				printf("%d, NDT_PPCS_SendToByServer: Size:%d byte, Data:[%s] ...\n", Counter, (UINT16)strlen(Buf_SendTo), Buf_SendTo);
				
				ret = NDT_PPCS_SendToByServer(DID, Buf_SendTo, (UINT16)strlen(Buf_SendTo), SendToMode, InitString, AES128Key);
				if (ret >= 0)
				{
					DeviceHandle = ret;
					break;
				}
				else if (ret == NDT_ERROR_DeviceNotOnRecv) //// it is because ES is busy
					RetryCounter = 0;
				else
				{
					printf("NG: NDT_PPCS_SendToByServer ret=%d, RetryCounter=%d\n", ret, RetryCounter);
					continue;
				}
			}
			if (RetryCounter >= 3)
			{
				printf("Failed to SendTo()!!\n");
				return 0;
			}
			RetryCounter = 0;
			FlagWaitRemoteClose = 0;
			
			while (RetryCounter < 3)
			{
				RetryCounter++;
				UINT16 Size = sizeof(Buf_RecvFrom);
				memset(Buf_RecvFrom, 0 , Size);
				//printf("FlagWaitRemoteClose=%d\n",FlagWaitRemoteClose);
				if (FlagWaitRemoteClose == 0)
				{
					ret = NDT_PPCS_RecvFrom(DeviceHandle, Buf_RecvFrom, &Size, 10*1000); //  Wait 10 Sec
				}
				else
				{	
					CHAR Buf_NULL[4];
					UINT16 Size_NULL = sizeof(Buf_NULL);
					ret = NDT_PPCS_RecvFrom(DeviceHandle, Buf_NULL, &Size_NULL, 1*1000); //  Wait 1 Sec
				}
				
				if (ret == NDT_ERROR_NoError)
				{
					printf("%d, NDT_PPCS_RecvFrom() OK! Size=%d byte, Data:[%s]\n", SuccessCounter, Size, Buf_RecvFrom);
					FlagWaitRemoteClose = 1;
					
					if (strcmp(Buf_SendTo, Buf_RecvFrom) == 0)
					{
						SuccessCounter ++;
						//printf("%d, Successfully: [%s]\n",Counter, Buf_RecvFrom);
					}
					else
					{
						printf("**** ERROR !! Received Data not match!! ****\n");
						printf("  Send: %s\n", Buf_SendTo);
						printf("  Recv: %s\n", Buf_RecvFrom);
						return 0;
					}
					setbuf(stdout, NULL);
					mSecSleep(50);
					RetryCounter = -5; //// allow more retry times
					continue;
				}
				else if (ret == NDT_ERROR_RemoteHandleClosed)
				{
					RetryCounter = 0;
					break;
				}
				else 
				{
					if ((FlagWaitRemoteClose != 1) || (ret != NDT_ERROR_TimeOut))  //// We are waiting for Remote close, so no need to show timeout
						printf("NG: NDT_PPCS_RecvFrom(), ret = %d\n", ret);
					mSecSleep(10);
					continue;
				}				
			}
			if (RetryCounter >= 3)
			{
				printf("**** ERROR!!! **** RecfFrom() Failed for DeviceHandle = %d, Return Code = %d\n", DeviceHandle, ret);
				return 0;
			}
			mSecSleep(100);	
		}
		else if (strcmp(argv[2], "bs") == 0) // Client: "bc" <---> Device: "bs"
		{
#if defined WINDOWS
			HANDLE hthread_BS_SendBack;
#elif defined LINUX
			pthread_t hthread_BS_SendBack;
#endif
			CHAR Buf[1280] = {0};
			UINT16 Size = sizeof(Buf);
			ret = NDT_PPCS_Recv(Buf, &Size, 0, Option);
			
			//// Check if successfully Recv a Command
			if (ret < 0)
			{
				printf("**** ERROR !! Failed to Get ClientHandle, NDT_PPCS_Recv ret=%d ****\n", ret);
				if (ret == NDT_ERROR_ExceedMaxClientHandle) //// Sleep 1 sec, if ExceedMaxClientHandle
					mSecSleep(1000);
				continue;
			}
			ClientHandle = ret;
			if (atoi(Buf) == 0) //// 0 is new client
			{
				BSRecvCounter[ClientHandle] = 0;
				//// Create SendBack thread
#ifdef WINDOWS
				hthread_BS_SendBack = CreateThread(NULL, 0, thread_BS_SendBack, (void *)((intptr_t)ClientHandle) , 0, NULL);
				if (hthread_BS_SendBack) CloseHandle(hthread_BS_SendBack);
#elif defined LINUX
				pthread_create(&hthread_BS_SendBack, NULL, &thread_BS_SendBack, (void *)((intptr_t)ClientHandle));
				if (hthread_BS_SendBack) pthread_detach(hthread_BS_SendBack);
#endif
			}
			else
			{	
				BSRecvCounter[ClientHandle]++;
				if (BSRecvCounter[ClientHandle] != atoi(Buf))
				{
					printf("**** ERROR!! **** ClientHandle=%d, %d, Sould be=%06d\n",ClientHandle, atoi(Buf), BSRecvCounter[ClientHandle]);
					NDT_PPCS_CloseHandle(ClientHandle);
				}
				else
				{
					//printf("ClientHandle=%d, %06d\n",ClientHandle, atoi(Buf));
				}
			}
			
			//// for debug print
			if (Counter % 100 == 0)
			{
				INT32 i;
				for (i=0; i < 16; i++)
					printf("%06d ",BSRecvCounter[i]);
				printf("\n");
			}
		}
		else if (strcmp(argv[2], "bc") == 0)
		{
#if defined WINDOWS
			HANDLE hthread_BC_RecvFrom;
#elif defined LINUX
			pthread_t hthread_BC_RecvFrom;
#endif
			CHAR Buf_SendTo[1280] = {0};
			INT32 RetryCounter = 0;
			sprintf(Buf_SendTo, "%06d", SuccessCounter);
			while (RetryCounter < 3)
			{
				RetryCounter++;
				if (gFlagStatus_thread_BC_RecvFrom == 2)
					break;
				ret = NDT_PPCS_SendToByServer(DID, Buf_SendTo, (UINT16)strlen(Buf_SendTo), SendToMode, InitString, AES128Key);
				if (ret >= 0)
				{
					DeviceHandle = ret;
					break;
				}
				else if (ret == NDT_ERROR_DeviceNotOnRecv) //// it is because ES is busy
					RetryCounter = 0;
				else
				{
					printf("***NG: NDT_PPCS_SendToByServer RetryCounter=%d, ret=%d, [%s]\n", RetryCounter, ret, getNDTErrorCodeInfo(ret));
					continue;
				}
			} // while
			if (RetryCounter >= 3)
			{
				printf("Failed to SendTo()!!\n");
				break;
			}
			else if (gFlagStatus_thread_BC_RecvFrom == 2)
			{
				printf("Complete!!\n");
				break;
			}
			else SuccessCounter++;
			
			if (gFlagStatus_thread_BC_RecvFrom == 0)
			{
#ifdef WINDOWS
				hthread_BC_RecvFrom = CreateThread(NULL, 0, thread_BC_RecvFrom, (void *)((intptr_t)DeviceHandle), 0, NULL);
				if (hthread_BC_RecvFrom) CloseHandle(hthread_BC_RecvFrom);
#elif defined LINUX
				pthread_create(&hthread_BC_RecvFrom, NULL, &thread_BC_RecvFrom, (void *)((intptr_t)DeviceHandle));
				if (hthread_BC_RecvFrom) pthread_detach(hthread_BC_RecvFrom);
#endif
				while (gFlagStatus_thread_BC_RecvFrom == 0)
					mSecSleep(10);
			}
			mSecSleep(10);
		}
	}
	INT32 AckTime, NumberDH, NumberCH;
	NDT_PPCS_StatusCheck(&AckTime, &NumberDH, &NumberCH);
	printf("Last Ack from Server is %d Sec before, NumberDH=%d, NumberCH=%d\n", AckTime, NumberDH, NumberCH);
	mSecSleep(2.5 * 1000); 
	NDT_PPCS_DeInitialize();
	printf("DeInitialize done.\n");
	return 0;
}

