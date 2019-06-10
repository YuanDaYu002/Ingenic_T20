 
/***************************************************************************
* @file:osd.h 
* @author:   
* @date:  5,31,2019
* @brief: OSD操作相关函数
* @attention:
***************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef SUPPORT_RGB555LE
#include "bgramapinfo_rgb555le.h"
#else
#include "bgramapinfo.h"
#endif

#include "imp_osd.h"
#include "osd.h"
#include "video.h"
#include "typeport.h"


#define OSD_LETTER_NUM 20

extern struct chn_conf chn[FS_CHN_NUM];

int grpNum[2] = {0,1}; 		//OSD 组编号数组	
IMPRgnHandle prHander_timestamp[2];//OSD 时间戳句柄数组

/*******************************************************************************
*@ Description    :OSD初始化函数
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int osd_init(void)
{
	int ret,i;

	for (i = 0; i <  FS_CHN_NUM; i++)
	{
		if (IMP_OSD_CreateGroup(grpNum[i]) < 0) {
			ERROR_LOG("IMP_OSD_CreateGroup(%d) error !\n",i);
			return HLE_RET_ERROR;
		}

		prHander_timestamp[i] = IMP_OSD_CreateRgn(NULL);
		if (prHander_timestamp[i] == INVHANDLE) {
			ERROR_LOG("IMP_OSD_CreateRgn TimeStamp[%d] error !\n",i);
			return HLE_RET_ERROR;
		}
	
		ret = IMP_OSD_RegisterRgn(prHander_timestamp[i], grpNum[i], NULL);
		if (ret < 0) {
			ERROR_LOG("IVS IMP_OSD_RegisterRgn[%d] failed\n",i);
			return HLE_RET_ERROR;
		}

		/*---#设置时间戳OSD区域属性------------------------------------------------------------*/
		IMPOSDRgnAttr rAttrFont;
		memset(&rAttrFont, 0, sizeof(IMPOSDRgnAttr));
		rAttrFont.type = OSD_REG_PIC;
		rAttrFont.rect.p0.x = 10;
		rAttrFont.rect.p0.y = 10;
		if(0 == i)//主分辨率右下角点坐标
		{
			rAttrFont.rect.p1.x = rAttrFont.rect.p0.x + 20 * OSD_REGION_WIDTH- 1;   //p0 is start，and p1 well be epual p0+width(or heigth)-1
			rAttrFont.rect.p1.y = rAttrFont.rect.p0.y + OSD_REGION_HEIGHT - 1;
		}
		else //次分辨率右下角点坐标
		{
			rAttrFont.rect.p1.x = rAttrFont.rect.p0.x + 20 * OSD_REGION_WIDTH- 1;   //p0 is start，and p1 well be epual p0+width(or heigth)-1
			rAttrFont.rect.p1.y = rAttrFont.rect.p0.y + OSD_REGION_HEIGHT - 1;
		}

#ifdef SUPPORT_RGB555LE
		rAttrFont.fmt = PIX_FMT_RGB555LE;
#else
		rAttrFont.fmt = PIX_FMT_BGRA;
#endif
		rAttrFont.data.picData.pData = NULL; //实际数据由时间戳实时更新线程来填充，此处初始化置NULL
		ret = IMP_OSD_SetRgnAttr(prHander_timestamp[i], &rAttrFont);
		if (ret < 0) {
			ERROR_LOG( "IMP_OSD_SetRgnAttr TimeStamp[%d] error !\n",i);
			return HLE_RET_ERROR;
		}

		/*---#开始OSD组区域的显示--------------------------------------------------------------------*/
		ret = IMP_OSD_Start(grpNum[i]);
		if (ret < 0) {
			ERROR_LOG( "IMP_OSD_Start TimeStamp[%d] error !\n",i);
			return HLE_RET_ERROR;
		}
		
			
	}


	return HLE_RET_OK;	

}

/*******************************************************************************
*@ Description    :OSD退出
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int osd_exit(void)
{
	int ret,i;
	for (i = 0; i <  FS_CHN_NUM; i++)
	{
		/* Step.b UnBind */


		ret = IMP_OSD_ShowRgn(prHander_timestamp[i], grpNum[i], 0);
		if (ret < 0) {
			ERROR_LOG("IMP_OSD_ShowRgn close timeStamp error\n");
		}

		ret = IMP_OSD_UnRegisterRgn(prHander_timestamp[i], grpNum[i]);
		if (ret < 0) {
			ERROR_LOG("IMP_OSD_UnRegisterRgn timeStamp error\n");
		}

		IMP_OSD_DestroyRgn(prHander_timestamp[i]);


		ret = IMP_OSD_DestroyGroup(grpNum[i]);
		if (ret < 0) {
			ERROR_LOG("IMP_OSD_DestroyGroup(0) error\n");
			return HLE_RET_ERROR;
		}
	
	}




	return HLE_RET_OK;

}

/*******************************************************************************
*@ Description    :打开OSD显示开关
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
static int osd_show(void)
{
	int ret,i;
	for (i = 0; i <  FS_CHN_NUM; i++)
	{
		ret = IMP_OSD_ShowRgn(prHander_timestamp[i], grpNum[i], 1);
		if (ret != 0) {
			ERROR_LOG( "IMP_OSD_ShowRgn(%d) timeStamp error\n",i);
			return HLE_RET_ERROR;
		}
	
	}

	return HLE_RET_OK;
}

/*******************************************************************************
*@ Description    :OSD(时间戳)更新函数
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
static void *timestamp_update_func(void *args)
{
	pthread_detach(pthread_self());
	 
	int ret;

	/*generate time*/
	char DateStr[40];
	time_t currTime;
	struct tm *currDate;
	unsigned i = 0, j = 0, k = 0;
	void *dateData = NULL;
	//uint32_t *data = NULL;
	uint32_t *timeStampData[2] = {0}; //OSD字库数据指针
	
	IMPOSDRgnAttrData rAttrData;

#ifdef SUPPORT_RGB555LE
	timeStampData[0] = (uint32_t *)malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint16_t));
	timeStampData[1] = (uint32_t *)malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint16_t));
#else
	timeStampData[0] = (uint32_t *)malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint32_t));
	timeStampData[1] = (uint32_t *)malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint32_t));
#endif

	if (timeStampData[0] == NULL || timeStampData[1] == NULL) 
	{
		ERROR_LOG( "malloc timeStampData error\n");
		if(timeStampData[0]) free(timeStampData[0]);
		if(timeStampData[1]) free(timeStampData[1]);
		pthread_exit(0);
	}
	

	ret = osd_show();
	if (ret < 0) {
		ERROR_LOG( "OSD show error\n");
		pthread_exit(0);
	}

	while(1) //可以加入OSD开关条件
	{
			int penpos_t = 0;
			int fontadv = 0;

			time(&currTime);
			currDate = localtime(&currTime);
			memset(DateStr, 0, 40);
			strftime(DateStr, 40, "%Y-%m-%d %I:%M:%S", currDate);
			for (i = 0; i < OSD_LETTER_NUM; i++) 
			{
				switch(DateStr[i]) 
				{
					case '0' ... '9':
						dateData = (void *)gBgramap[DateStr[i] - '0'].pdata;
						fontadv = gBgramap[DateStr[i] - '0'].width;
						penpos_t += gBgramap[DateStr[i] - '0'].width;
						break;
					case '-':
						dateData = (void *)gBgramap[10].pdata;
						fontadv = gBgramap[10].width;
						penpos_t += gBgramap[10].width;
						break;
					case ' ':
						dateData = (void *)gBgramap[11].pdata;
						fontadv = gBgramap[11].width;
						penpos_t += gBgramap[11].width;
						break;
					case ':':
						dateData = (void *)gBgramap[12].pdata;
						fontadv = gBgramap[12].width;
						penpos_t += gBgramap[12].width;
						break;
					default:
						break;
				}

				/*---#目前的字库点阵每个字符的宽度都是16字节的，适合1920*1080分辨率的视频，
				后续需要兼容次分辨率的视频，再做该进！！！！----------------------------------------*/	
				for (k = 0; k <  FS_CHN_NUM; k++)
				{
					#ifdef SUPPORT_RGB555LE
						for (j = 0; j < OSD_REGION_HEIGHT; j++) {//拷贝一个字符点阵的数据
							memcpy((void *)((uint16_t *)timeStampData[k] + j*OSD_LETTER_NUM*OSD_REGION_WIDTH + penpos_t),
									(void *)((uint16_t *)dateData + j*fontadv), fontadv*sizeof(uint16_t));
						}
					#else
						for (j = 0; j < OSD_REGION_HEIGHT; j++) {//拷贝一个字符点阵的数据
							memcpy((void *)((uint32_t *)timeStampData[k] + j*OSD_LETTER_NUM*OSD_REGION_WIDTH + penpos_t),
									(void *)((uint32_t *)dateData + j*fontadv), fontadv*sizeof(uint32_t));
						}

					#endif
					
				}


			}

			/*---#更新OSD区域数据------------------------------------------------------------*/
			for (k = 0; k <  FS_CHN_NUM; k++)
			{
				rAttrData.picData.pData = timeStampData[k];
				IMP_OSD_UpdateRgnAttrData(prHander_timestamp[k], &rAttrData);
			}
			
			sleep(1); //时间戳1s更新一次
	}

	pthread_exit(0);
}

/*******************************************************************************
*@ Description    :创建OSD时间戳更新线程
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int timestamp_update_task(void)
{
	int ret;

	/*Create OSD bgramap update thread */
	pthread_t tid;
	ret = pthread_create(&tid, NULL, timestamp_update_func, NULL);
	if (ret) {
		ERROR_LOG("timestamp_update_func create error\n");
		return HLE_RET_ERROR;
	}

	return HLE_RET_OK;
}



#ifdef __cplusplus
}
#endif







