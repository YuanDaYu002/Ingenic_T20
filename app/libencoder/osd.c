 
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
#include <pthread.h>

#include "imp_osd.h"
#include "osd.h"
#include "video.h"
#include "typeport.h"
#include "surface_scaler.h"
#include "ziku.h"


/*time OSD 字符串的长度 (20 个字符,包括"\0")*/
#define TIME_OSD_STR_LEN        20


/*---#限定整个时间戳OSD区域的范围------------------------------------------------------------*/
#define OSD_TIME_REGION_START_X		10		//OSD区域起始点坐标X
#define OSD_TIME_REGION_START_Y		10		//OSD区域起始点坐标Y
#define OSD_TIME_REGION_WIDTH		600		//OSD区域的宽度（1920*1080）
#define OSD_TIME_REGION_HEIGHT		70		//OSD区域的高度（1920*1080）

#define OSD_TIME_REGION_START_X_SEC	10		//OSD区域起始点坐标X
#define OSD_TIME_REGION_START_Y_SEC	10		//OSD区域起始点坐标Y
#define OSD_TIME_REGION_WIDTH_SEC	300		//OSD区域的宽度（640*360）
#define OSD_TIME_REGION_HEIGHT_SEC	30		//OSD区域的宽度（640*360）

/*rate OSD 字符串长度(16 个字符,包括"\0")*/
#define RATE_OSD_STR_LEN       20
/*---#限定整个码率OSD区域的范围------------------------------------------------------------*/
#define OSD_BITRATE_REGION_START_X		10		//OSD区域起始点坐标X
#define OSD_BITRATE_REGION_START_Y		80		//OSD区域起始点坐标Y
#define OSD_BITRATE_REGION_WIDTH		600		//OSD区域的宽度（1920*1080）
#define OSD_BITRATE_REGION_HEIGHT		70		//OSD区域的高度（1920*1080）

#define OSD_BITRATE_REGION_START_X_SEC	10		//OSD区域起始点坐标X
#define OSD_BITRATE_REGION_START_Y_SEC	40		//OSD区域起始点坐标Y
#define OSD_BITRATE_REGION_WIDTH_SEC	300		//OSD区域的宽度（640*360）
#define OSD_BITRATE_REGION_HEIGHT_SEC	30		//OSD区域的宽度（640*360）




extern struct chn_conf chn[FS_CHN_NUM];

int grpNum[FS_CHN_NUM] = {0,1}; 				//OSD 组编号数组	
IMPRgnHandle prHander_timestamp[FS_CHN_NUM];	//OSD 时间戳句柄数组
IMPRgnHandle prHander_bitrate[FS_CHN_NUM];		//OSD 码率句柄数组


#define OSD_TYPE_NUM    3  	//OSD的总数目，详细如下:
#define TIME_OSD_INDEX  0	//时间OSD
#define USER_OSD_INDEX  1	//用户OSD
#define RATE_OSD_INDEX  2	//码率OSD

/*---#OSD的BMP数据参数数组------------------------------------------------------------*/
static OSD_BITMAP_ATTR OSD_bmp_attrs[FS_CHN_NUM][OSD_TYPE_NUM];

/*---#OSD区域参数数组------------------------------------------------------------*/
static OSD_REGION_ATTR OSD_region_attrs[FS_CHN_NUM][OSD_TYPE_NUM]; 


#define RGB24_TO_RGB15(c) (HLE_U16)((((((c)>>16)&0xFF)>>3)<<10) | (((((c)>>8)&0xff)>>3)<<5) | (((c)&0XFF)>>3))

/*******************************************************************************
*@ Description    :将点阵（1个像素 1 bit，单色）数据，映射成
					BMP位图（1个像素 2 byte RGB555，彩色）数据
*@ Input          :<osd_attr> OSD参数信息（包含点阵数据）
*@ Output         :<bitmap> BMP位图数据
*@ Return         :
*@ attention      :
*******************************************************************************/
static void draw_bmp(OSD_BITMAP_ATTR *osd_attr, HLE_U16 *bitmap)
{
    HLE_U32 i, j;
    
    //OSD BMP数据的总字节数
    int matrix_len = (osd_attr->width / 8) * osd_attr->height;
    //OSD 数据
    HLE_U8 *matrix = osd_attr->raster;
	HLE_U16 fg_color = RGB24_TO_RGB15(osd_attr->fg_color) | 0x8000;//第15位置1（RGB15，0 ~ 14，第15位空余）
    HLE_U16 bg_color = RGB24_TO_RGB15(osd_attr->bg_color);
	
	//DEBUG_LOG("osd_attr->fg_color(%x) osd_attr->bg_color(%x)\n",osd_attr->fg_color,osd_attr->bg_color);

    /*将点阵（1个像素 1 bit）数据，映射成BMP位图（1个像素 2 byte RGB555）数据*/
    for (i = 0; i < matrix_len; i++) 
    {
        //HLE_U16 clr = need_reverse_osd(osd_attr, &vfi, i, pY) ? rv_color : fg_color;
        for (j = 0; j < 8; j++) //一次处理1字节
        {
            if (*matrix & (0x01 << (7 - j))) //从7 ~ 0位依次判断是否为1
            {
                *(bitmap++) = fg_color; //为 1 则BMP数据对应该字节使用前景色
            }
            else {
                *(bitmap++) = bg_color; //为 0 则BMP数据对应该字节使用背景色
            }
        }

        matrix++;
    }

}


static HLE_S32 date_string_format = DDF_YYYYMMDD;

void encoder_set_date_format(E_DATE_DISPLAY_FMT date_fmt)
{
    date_string_format = date_fmt;
}


void get_date_string(char *time_str)
{
    time_t time_cur;
    time(&time_cur);

    struct tm time_date;
    localtime_r(&time_cur, &time_date);

    HLE_S32 year = 1900 + time_date.tm_year;
    HLE_S32 mon = 1 + time_date.tm_mon;
    HLE_S32 mday = time_date.tm_mday;
    HLE_S32 hour = time_date.tm_hour;
    HLE_S32 min = time_date.tm_min;
    HLE_S32 sec = time_date.tm_sec;

    switch (date_string_format) {
    case DDF_YYYYMMDD:
    {
        snprintf(time_str, TIME_OSD_STR_LEN, "%04d-%02d-%02d %02d:%02d:%02d",
                 year, mon, mday, hour, min, sec);
        break;
    }
    case DDF_MMDDYYYY:
    {
        snprintf(time_str, TIME_OSD_STR_LEN, "%02d/%02d/%04d %02d:%02d:%02d",
                 mon, mday, year, hour, min, sec);
        break;
    }
    case DDF_DDMMYYYY:
    {
        snprintf(time_str, TIME_OSD_STR_LEN, "%02d/%02d/%04d %02d:%02d:%02d",
                 mday, mon, year, hour, min, sec);
        break;
    }
    default:
        DEBUG_LOG("Unsupport date string format\n");
        break;
    }
}


/*******************************************************************************
*@ Description    :将BMP数据叠加到OSD区域上
*@ Input          :
*@ Output         :
*@ Return         :成功：0；失败：-1
*@ attention      :需要确保BMP图片区间大小不超出OSD区域大小，否则多出部分会被丢弃
*******************************************************************************/
int attach_BMP_data_to_OSD_region(OSD_BITMAP_ATTR* bmp_attr,void*bmp_data,OSD_REGION_ATTR* OSD_region)
{
	if(NULL == bmp_attr || NULL == bmp_data ||NULL == OSD_region)
	{
		ERROR_LOG("Illegal parameter!\n");
		return -1;
	}
	
//	DEBUG_LOG("OSD_region->OSD_data_buf_size = %d\n",OSD_region->OSD_data_buf_size);
//	DEBUG_LOG("OSD_region->region_width = %d\n",OSD_region->region_width);
//	DEBUG_LOG("OSD_region->region_hegiht = %d\n",OSD_region->region_hegiht);
//	DEBUG_LOG("OSD_region->start_x = %d\n",OSD_region->start_x);
//	DEBUG_LOG("OSD_region->start_y = %d\n",OSD_region->start_y);
//	DEBUG_LOG("bmp_attr->width(%d)\n",bmp_attr->width);
//	DEBUG_LOG("bmp_attr->height(%d)\n",bmp_attr->height);
	
	int i = 0;
	int OSD_width_bytes = OSD_region->region_width*sizeof(HLE_U16);//GRB555
	int bmp_width_bytes = bmp_attr->width * sizeof(HLE_U16);
	
	memset(OSD_region->OSD_data_buf,0,OSD_width_bytes*OSD_region->region_hegiht);
	
	if(bmp_attr->width <= OSD_region->region_width && 
		bmp_attr->height <= OSD_region->region_hegiht)//bmp数据宽高都没超出OSD范围
	{
		for(i = 0 ; i < bmp_attr->height ; i++)
		{
			if((i+1)*OSD_width_bytes >= OSD_region->OSD_data_buf_size)
			{
				ERROR_LOG("overflow!!! (i+1)*OSD_width_bytes(%d) OSD_data_buf_size(%d)\n",(i+1)*OSD_width_bytes,OSD_region->OSD_data_buf_size);
				return -1;
			}
			memcpy((char*)OSD_region->OSD_data_buf + i*OSD_width_bytes,
					(char*)bmp_data + i*bmp_width_bytes,bmp_width_bytes);
		}
	}
	else if(bmp_attr->width > OSD_region->region_width && 
			bmp_attr->height <= OSD_region->region_hegiht)//bmp数据宽超出OSD区域范围,高没超出
	{
		for(i = 0 ; i < bmp_attr->height ; i++)
		{
			memcpy((char*)OSD_region->OSD_data_buf + i*OSD_width_bytes,
					(char*)bmp_data + i*bmp_width_bytes,OSD_width_bytes);
		}
		
	}
	else if(bmp_attr->width <= OSD_region->region_width && 
			bmp_attr->height > OSD_region->region_hegiht)//bmp数据宽没超出OSD区域范围,高超出
	{
		for(i = 0 ; i < OSD_region->region_hegiht ; i++)
		{
			memcpy((char*)OSD_region->OSD_data_buf + i*OSD_width_bytes,
					(char*)bmp_data + i*bmp_width_bytes,bmp_width_bytes);
		}
	}
	else //bmp数据宽高都超出OSD区域范围
	{
			
		for(i = 0;i<OSD_region->region_hegiht;i++)
		{
			memcpy((char*)OSD_region->OSD_data_buf + i*OSD_width_bytes,
					(char*)bmp_data + i*bmp_width_bytes,OSD_width_bytes);
		}		
		
	}
	return 0;
	
}



/*******************************************************************************
*@ Description    :重新调整OSD区域大小
*@ Input          :<OSD_handle> OSD的句柄
					<osd_letter_num>OSD字母个数
					<osd_weight> OSD的宽
					<osd_height> OSD的高
					<pData>OSD数据
*@ Output         :
*@ Return         :成功：0 ； 失败： -1
*@ attention      :
*******************************************************************************/
int config_OSD_region(IMPRgnHandle OSD_handle,int osd_letter_num,
								int osd_width,int osd_height,void*pData)
{
	if(NULL == pData)
	{
		ERROR_LOG("Illegal parameter !\n");
		return -1;
	}

	printf("----------------------------------into pos 005!\n");
	/*---#设置OSD区域属性------------------------------------------------------------*/
	IMPOSDRgnAttr rAttrFont;
	memset(&rAttrFont, 0, sizeof(IMPOSDRgnAttr));
	rAttrFont.type = OSD_REG_PIC;

	//OSD区域的起始坐标
	rAttrFont.rect.p0.x = 10;
	rAttrFont.rect.p0.y = 10;
	//OSD区域的结束坐标
	rAttrFont.rect.p1.x = rAttrFont.rect.p0.x + osd_letter_num * osd_width- 1;   //p0 is start，and p1 well be epual p0+width(or heigth)-1
	rAttrFont.rect.p1.y = rAttrFont.rect.p0.y + osd_height - 1;

	rAttrFont.fmt = PIX_FMT_RGB555LE;
	//rAttrFont.fmt = PIX_FMT_BGRA;

	rAttrFont.data.picData.pData = pData;
	printf("----------------------------------into pos 006!\n");

	int ret = IMP_OSD_SetRgnAttr(OSD_handle, &rAttrFont);
	if (ret < 0) 
	{
		ERROR_LOG( "IMP_OSD_SetRgnAttr TimeStamp[%d] error !\n",OSD_handle);
		return -1;
	}
	printf("----------------------------------into pos 007!\n");
	return 0;

}

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
	
	ret = zk_init();
	if(ret < 0)
	{
		ERROR_LOG("zk_init failed!\n");
		return HLE_RET_ERROR;
	}

	for (i = 0; i <  FS_CHN_NUM; i++)
	{
		/*---#时间戳OSD的bmp数据参数初始化------------------------------------------------------------*/
		OSD_bmp_attrs[i][TIME_OSD_INDEX].enable = 0;
		OSD_bmp_attrs[i][TIME_OSD_INDEX].width = 0 ;
		OSD_bmp_attrs[i][TIME_OSD_INDEX].height = 0;
		OSD_bmp_attrs[i][TIME_OSD_INDEX].fg_color = 0xffffffff; //白色
		OSD_bmp_attrs[i][TIME_OSD_INDEX].bg_color = 0x00;
		OSD_bmp_attrs[i][TIME_OSD_INDEX].raster = NULL;
		
		/*---#码率OSD的bmp数据参数初始化------------------------------------------------------------*/
		OSD_bmp_attrs[i][RATE_OSD_INDEX].enable = 0;
		OSD_bmp_attrs[i][RATE_OSD_INDEX].width = 0 ;
		OSD_bmp_attrs[i][RATE_OSD_INDEX].height = 0;
		OSD_bmp_attrs[i][RATE_OSD_INDEX].fg_color = 0xffffffff; //白色
		OSD_bmp_attrs[i][RATE_OSD_INDEX].bg_color = 0x00;
		OSD_bmp_attrs[i][RATE_OSD_INDEX].raster = NULL;

		/*---#时间戳OSD区域及缓存初始化------------------------------------------------------------*/
		OSD_region_attrs[i][TIME_OSD_INDEX].start_x = OSD_TIME_REGION_START_X;
		OSD_region_attrs[i][TIME_OSD_INDEX].start_y = OSD_TIME_REGION_START_Y;
		if(0 == i)
		{
			OSD_region_attrs[i][TIME_OSD_INDEX].region_width = OSD_TIME_REGION_WIDTH;
			OSD_region_attrs[i][TIME_OSD_INDEX].region_hegiht = OSD_TIME_REGION_HEIGHT;
			OSD_region_attrs[i][TIME_OSD_INDEX].OSD_data_buf_size = OSD_TIME_REGION_WIDTH * OSD_TIME_REGION_HEIGHT * sizeof(HLE_U16);//RGB555
			OSD_region_attrs[i][TIME_OSD_INDEX].OSD_data_buf = (void*)malloc(OSD_region_attrs[i][TIME_OSD_INDEX].OSD_data_buf_size);
			if(NULL == OSD_region_attrs[i][TIME_OSD_INDEX].OSD_data_buf)
			{
				ERROR_LOG("malloc failed!\n");
				return HLE_RET_ERROR;
			}
		}
		else
		{
			OSD_region_attrs[i][TIME_OSD_INDEX].region_width = OSD_TIME_REGION_WIDTH_SEC;
			OSD_region_attrs[i][TIME_OSD_INDEX].region_hegiht = OSD_TIME_REGION_HEIGHT_SEC;
			OSD_region_attrs[i][TIME_OSD_INDEX].OSD_data_buf_size = OSD_TIME_REGION_WIDTH_SEC * OSD_TIME_REGION_HEIGHT_SEC * sizeof(HLE_U16);//RGB555
			OSD_region_attrs[i][TIME_OSD_INDEX].OSD_data_buf = (void*)malloc(OSD_region_attrs[i][TIME_OSD_INDEX].OSD_data_buf_size);
			if(NULL == OSD_region_attrs[i][TIME_OSD_INDEX].OSD_data_buf)
			{
				ERROR_LOG("malloc failed!\n");
				return HLE_RET_ERROR;
			}

		}

		
		/*---#码率OSD区域及缓存初始化------------------------------------------------------------*/
		OSD_region_attrs[i][RATE_OSD_INDEX].start_x = OSD_BITRATE_REGION_START_X;
		OSD_region_attrs[i][RATE_OSD_INDEX].start_y = OSD_BITRATE_REGION_START_Y;
		if(0 == i)
		{
			OSD_region_attrs[i][RATE_OSD_INDEX].region_width = OSD_BITRATE_REGION_WIDTH;
			OSD_region_attrs[i][RATE_OSD_INDEX].region_hegiht = OSD_BITRATE_REGION_HEIGHT;
			OSD_region_attrs[i][RATE_OSD_INDEX].OSD_data_buf_size = OSD_BITRATE_REGION_WIDTH * OSD_BITRATE_REGION_HEIGHT  * sizeof(HLE_U16);//RGB555
			OSD_region_attrs[i][RATE_OSD_INDEX].OSD_data_buf = (void*)malloc(OSD_region_attrs[i][RATE_OSD_INDEX].OSD_data_buf_size);
			if(NULL == OSD_region_attrs[i][RATE_OSD_INDEX].OSD_data_buf)
			{
				ERROR_LOG("malloc failed!\n");
				return HLE_RET_ERROR;
			}
		}
		else
		{
			OSD_region_attrs[i][RATE_OSD_INDEX].region_width = OSD_BITRATE_REGION_WIDTH_SEC;
			OSD_region_attrs[i][RATE_OSD_INDEX].region_hegiht = OSD_BITRATE_REGION_HEIGHT_SEC;
			OSD_region_attrs[i][RATE_OSD_INDEX].OSD_data_buf_size = OSD_BITRATE_REGION_WIDTH_SEC * OSD_BITRATE_REGION_HEIGHT_SEC * sizeof(HLE_U16);
			OSD_region_attrs[i][RATE_OSD_INDEX].OSD_data_buf = (void*)malloc(OSD_region_attrs[i][RATE_OSD_INDEX].OSD_data_buf_size * sizeof(HLE_U16));//RGB555
			if(NULL == OSD_region_attrs[i][RATE_OSD_INDEX].OSD_data_buf)
			{
				ERROR_LOG("malloc failed!\n");
				return HLE_RET_ERROR;
			}

		}
	
		/*---#创建OSD用户组 + 创建并注册OSD区域------------------------------------------------------------*/
		if (IMP_OSD_CreateGroup(grpNum[i]) < 0) {
			ERROR_LOG("IMP_OSD_CreateGroup(%d) error !\n",i);
			return HLE_RET_ERROR;
		}

		prHander_timestamp[i] = IMP_OSD_CreateRgn(NULL);
		if (prHander_timestamp[i] == INVHANDLE) {
			ERROR_LOG("IMP_OSD_CreateRgn TimeStamp[%d] error !\n",i);
			return HLE_RET_ERROR;
		}

		prHander_bitrate[i] = IMP_OSD_CreateRgn(NULL);
		if (prHander_bitrate[i] == INVHANDLE) {
			ERROR_LOG("IMP_OSD_CreateRgn Bitrate[%d] error !\n",i);
			return HLE_RET_ERROR;
		}
	
		ret = IMP_OSD_RegisterRgn(prHander_timestamp[i], grpNum[i], NULL);
		if (ret < 0) {
			ERROR_LOG("IVS IMP_OSD_RegisterRgn[%d] failed\n",i);
			return HLE_RET_ERROR;
		}
		
		ret = IMP_OSD_RegisterRgn(prHander_bitrate[i], grpNum[i], NULL);
		if (ret < 0) {
			ERROR_LOG("IVS IMP_OSD_RegisterRgn[%d] failed\n",i);
			return HLE_RET_ERROR;
		}
		
		/*---#时间戳OSD区域属性设置------------------------------------------------------------*/
		IMPOSDRgnAttr rAttrFont;
		memset(&rAttrFont, 0, sizeof(IMPOSDRgnAttr));
		rAttrFont.type = OSD_REG_PIC;
		if(0 == i)//主分辨率右下角点坐标
		{
			rAttrFont.rect.p0.x = OSD_TIME_REGION_START_X;
			rAttrFont.rect.p0.y = OSD_TIME_REGION_START_Y;
			rAttrFont.rect.p1.x = rAttrFont.rect.p0.x + OSD_TIME_REGION_WIDTH- 1;   //p0 is start，and p1 well be epual p0+width(or heigth)-1
			rAttrFont.rect.p1.y = rAttrFont.rect.p0.y + OSD_TIME_REGION_HEIGHT - 1;
		}
		else //次分辨率右下角点坐标
		{
			rAttrFont.rect.p0.x = OSD_TIME_REGION_START_X_SEC;
			rAttrFont.rect.p0.y = OSD_TIME_REGION_START_Y_SEC;
			rAttrFont.rect.p1.x = rAttrFont.rect.p0.x + OSD_TIME_REGION_WIDTH_SEC- 1;   //p0 is start，and p1 well be epual p0+width(or heigth)-1
			rAttrFont.rect.p1.y = rAttrFont.rect.p0.y + OSD_TIME_REGION_HEIGHT_SEC - 1;
		}
		rAttrFont.fmt = PIX_FMT_RGB555LE;
		rAttrFont.data.picData.pData = NULL; //实际数据由时间戳实时更新线程来填充，此处初始化置NULL
		ret = IMP_OSD_SetRgnAttr(prHander_timestamp[i], &rAttrFont);
		if (ret < 0) {
			ERROR_LOG( "IMP_OSD_SetRgnAttr TimeStamp[%d] error !\n",i);
			return HLE_RET_ERROR;
		}

		/*---#码率OSD区域属性设置------------------------------------------------------------*/
		//IMPOSDRgnAttr rAttrFont;
		memset(&rAttrFont, 0, sizeof(IMPOSDRgnAttr));
		rAttrFont.type = OSD_REG_PIC;
		if(0 == i)//主分辨率右下角点坐标
		{
			rAttrFont.rect.p0.x = OSD_BITRATE_REGION_START_X;
			rAttrFont.rect.p0.y = OSD_BITRATE_REGION_START_Y;
			rAttrFont.rect.p1.x = rAttrFont.rect.p0.x + OSD_BITRATE_REGION_WIDTH- 1;   //p0 is start，and p1 well be epual p0+width(or heigth)-1
			rAttrFont.rect.p1.y = rAttrFont.rect.p0.y + OSD_BITRATE_REGION_HEIGHT - 1;
		}
		else //次分辨率右下角点坐标
		{
			rAttrFont.rect.p0.x = OSD_BITRATE_REGION_START_X_SEC;
			rAttrFont.rect.p0.y = OSD_BITRATE_REGION_START_Y_SEC;
			rAttrFont.rect.p1.x = rAttrFont.rect.p0.x + OSD_BITRATE_REGION_WIDTH_SEC- 1;   //p0 is start，and p1 well be epual p0+width(or heigth)-1
			rAttrFont.rect.p1.y = rAttrFont.rect.p0.y + OSD_BITRATE_REGION_HEIGHT_SEC - 1;
		}
		rAttrFont.fmt = PIX_FMT_RGB555LE;
		rAttrFont.data.picData.pData = NULL; //实际数据由时间戳实时更新线程来填充，此处初始化置NULL
		ret = IMP_OSD_SetRgnAttr(prHander_bitrate[i], &rAttrFont);
		if (ret < 0) {
			ERROR_LOG( "IMP_OSD_SetRgnAttr Bitrate[%d] error !\n",i);
			return HLE_RET_ERROR;
		}



		/*---#开始OSD组区域的显示--------------------------------------------------------------------*/
		ret = IMP_OSD_Start(grpNum[i]);
		if (ret < 0) {
			ERROR_LOG( "IMP_OSD_Start grop[%d] error !\n",i);
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

		int i = 0;
		for(i=0;i<FS_CHN_NUM;i++)
		{
			if(OSD_region_attrs[i][TIME_OSD_INDEX].OSD_data_buf)
			{
				free(OSD_region_attrs[i][TIME_OSD_INDEX].OSD_data_buf); 
				OSD_region_attrs[i][TIME_OSD_INDEX].OSD_data_buf = NULL;
			}
				
		}
		
	
	}




	return HLE_RET_OK;

}

/*******************************************************************************
*@ Description    :打开OSD(时间戳)显示开关
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
static int osd_timestamp_show(void)
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
*@ Description    :打开OSD(码率)显示开关
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
static int osd_bitrate_show(void)
{
	int ret,i;
	for (i = 0; i <  FS_CHN_NUM; i++)
	{
		ret = IMP_OSD_ShowRgn(prHander_bitrate[i], grpNum[i], 1);
		if (ret != 0) {
			ERROR_LOG( "IMP_OSD_ShowRgn(%d) bitrate error\n",i);
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
	 
	int ret,i;
	HLE_SURFACE org_sfc;
	char *org_bmp = NULL;
	
	/*generate time*/
	//char DateStr[40];
	HLE_U8 time_str[TIME_OSD_STR_LEN];
	
	unsigned int k = 0;	
	IMPOSDRgnAttrData rAttrData;
	
	ret = osd_timestamp_show();
	if (ret < 0) {
		ERROR_LOG( "OSD show error\n");
		pthread_exit(0);
	}

	for(i = 0;i<FS_CHN_NUM;i++)
	{
		OSD_bmp_attrs[i][TIME_OSD_INDEX].enable = 1;	//使能时间戳OSD
	}
	
	while(1) //可以加入OSD开关条件
	{
			#if 0
				time(&currTime);
				currDate = localtime(&currTime);
				memset(DateStr, 0, 40);
				strftime(DateStr, 40, "%Y-%m-%d %I:%M:%S", currDate);//将时间转换成%Y-%m-%d %I:%M:%S格式
			#else
        		get_date_string((char *) time_str);
			#endif
			
			
			/*---#更新OSD区域数据------------------------------------------------------------*/
			for (k = 0; k <  FS_CHN_NUM; k++)
			{
				
				OSD_matrix_info_t ret_info = {0};
				int enc_w,enc_h;
				OSD_BITMAP_ATTR *osd_bmp_attr = &OSD_bmp_attrs[k][TIME_OSD_INDEX];
				if (osd_bmp_attr->enable == 0) 
	            {
	                continue;
	            }
			
				if(0 == k)
				{
					enc_w = 1920;
					enc_h = 1080;
				}
				else
				{
					enc_w = 640;
					enc_h = 360;
				}

  		        if(str2matrix_vector((HLE_U8*)&time_str, &ret_info,enc_w,enc_h) < 0)
		        {
		            ERROR_LOG("call str2matrix_vector error !\n");
		            continue;
		        }

				/*修正宽高度参数，矢量字体宽高度不是固定的值，需要动态获取*/
                osd_bmp_attr->raster = ret_info.matrix_data;
                osd_bmp_attr->width = ret_info.width;
                osd_bmp_attr->height = ret_info.height;
				
				/*---创建新的 surface ，即BMP图片数据缓存空间------*/
                org_sfc.u32Width = osd_bmp_attr->width;
                org_sfc.u32Height = osd_bmp_attr->height;

				org_bmp = create_surface(&org_sfc); 
                if (org_bmp == NULL) 
                {
                    ERROR_LOG("create_surface org_bmp failed!\n");
                    free(ret_info.matrix_data);
                    continue;
                }
				draw_bmp(osd_bmp_attr, (HLE_U16 *) org_bmp);
						
				attach_BMP_data_to_OSD_region(osd_bmp_attr,org_bmp,&OSD_region_attrs[k][TIME_OSD_INDEX]);

				//printf("org_sfc.u32Width(%d) org_sfc.u32Height(%d)\n",org_sfc.u32Width,org_sfc.u32Height);
		 
				rAttrData.picData.pData = OSD_region_attrs[k][TIME_OSD_INDEX].OSD_data_buf;
				IMP_OSD_UpdateRgnAttrData(prHander_timestamp[k], &rAttrData);
				
				free(ret_info.matrix_data);
				destroy_surface(org_bmp);
				
				
			}
	
			sleep(1); //时间戳1s更新一次
	}

	pthread_exit(0);
}

/*******************************************************************************
*@ Description    :创建时间戳OSD更新线程
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

/*******************************************************************************
*@ Description    :OSD(码率)更新函数
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
static void *bitrate_update_func(void *args)
{
	pthread_detach(pthread_self());
	 
	int ret,i;
	HLE_SURFACE org_sfc;
	char *org_bmp = NULL;
	
	/*generate time*/
	//char DateStr[40];
	HLE_U8 bitrate_str[RATE_OSD_STR_LEN];
	
	unsigned int k = 0;	
	IMPOSDRgnAttrData rAttrData;
	
	ret = osd_bitrate_show();
	if (ret < 0) {
		ERROR_LOG( "OSD show error\n");
		pthread_exit(0);
	}

	for(i = 0;i<FS_CHN_NUM;i++)
	{
		OSD_bmp_attrs[i][RATE_OSD_INDEX].enable = 1;	//使能波特率OSD
	}
	
	while(1) //可以加入OSD开关条件
	{
			
			
			/*---#更新OSD区域数据------------------------------------------------------------*/
			for (k = 0; k <  FS_CHN_NUM; k++)
			{
				unsigned int bitrate = 0; 
				unsigned int framerate = 0;
				get_H264_bitrate(k,&bitrate,&framerate);
				snprintf((char*)&bitrate_str, RATE_OSD_STR_LEN,"%2dfps,%5dkbps",framerate,bitrate);
					
				OSD_matrix_info_t ret_info = {0};
				int enc_w,enc_h;
				OSD_BITMAP_ATTR *osd_bmp_attr = &OSD_bmp_attrs[k][RATE_OSD_INDEX];
				if (osd_bmp_attr->enable == 0) 
	            {
	                continue;
	            }
			
				if(0 == k)
				{
					enc_w = 1920;
					enc_h = 1080;
				}
				else
				{
					enc_w = 640;
					enc_h = 360;
				}

  		        if(str2matrix_vector((HLE_U8*)&bitrate_str, &ret_info,enc_w,enc_h) < 0)
		        {
		            ERROR_LOG("call str2matrix_vector error !\n");
		            continue;
		        }

				/*修正宽高度参数，矢量字体宽高度不是固定的值，需要动态获取*/
                osd_bmp_attr->raster = ret_info.matrix_data;
                osd_bmp_attr->width = ret_info.width;
                osd_bmp_attr->height = ret_info.height;
				
				/*---创建新的 surface ，即BMP图片数据缓存空间------*/
                org_sfc.u32Width = osd_bmp_attr->width;
                org_sfc.u32Height = osd_bmp_attr->height;

				org_bmp = create_surface(&org_sfc); 
                if (org_bmp == NULL) 
                {
                    ERROR_LOG("create_surface org_bmp failed!\n");
                    free(ret_info.matrix_data);
                    continue;
                }
				draw_bmp(osd_bmp_attr, (HLE_U16 *) org_bmp);
						
				attach_BMP_data_to_OSD_region(osd_bmp_attr,org_bmp,&OSD_region_attrs[k][RATE_OSD_INDEX]);

				//printf("org_sfc.u32Width(%d) org_sfc.u32Height(%d)\n",org_sfc.u32Width,org_sfc.u32Height);
		 
				rAttrData.picData.pData = OSD_region_attrs[k][RATE_OSD_INDEX].OSD_data_buf;
				IMP_OSD_UpdateRgnAttrData(prHander_bitrate[k], &rAttrData);
				
				free(ret_info.matrix_data);
				destroy_surface(org_bmp);
				
				
			}
	
			sleep(1); //时间戳1s更新一次
	}

	pthread_exit(0);
}


/*******************************************************************************
*@ Description    :创建码率OSD更新线程
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int bitrate_update_task(void)
{
	int ret;

	/*Create OSD bgramap update thread */
	pthread_t tid;
	ret = pthread_create(&tid, NULL, bitrate_update_func, NULL);
	if (ret) {
		ERROR_LOG("bitrate_update_func create error\n");
		return HLE_RET_ERROR;
	}

	return HLE_RET_OK;
}



#ifdef __cplusplus
}
#endif







