
/***************************************************************************
* @file:osd.h 
* @author:   
* @date:  5,31,2019
* @brief: OSD操作相关函数头文件 
* @attention:
***************************************************************************/
#ifndef _OSD_H
#define _OSD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "typeport.h"

//osd区域描述结构
typedef struct
{
	int start_x;		//起始坐标x坐标（图片区域左上角为起点（0,0））
	int start_y;		//起始坐标y坐标
	int region_width;	//区域的宽
	int region_hegiht;	//区域的高
	void* OSD_data_buf;	//OSD数据buf
	int  OSD_data_buf_size;
}OSD_REGION_ATTR;

//osd 要附加的bmp数据描述结构
typedef struct
{
    HLE_U32 enable; // 是否显示。0-不显示，其他参数忽略，1-显示
//    HLE_U16 x; //x, y 坐标为相对REL_COORD_WIDTH X REL_COORD_HEIGHT坐标系大小
//    HLE_U16 y;
    HLE_U16 width; //OSD点阵宽度，必须为8对其
    HLE_U16 height; //OSD点阵高度
    HLE_U32 fg_color; //前景色，16进制表示为0xAARRGGBB，A(Alpha)表示透明度。
    HLE_U32 bg_color; //背景色，同样是ARGB8888格式

    //需要叠加的OSD单色点阵数据。0表示该点无效，1表示该点有效。每个字节包含8个象素的数据，
    //最左边的象素在最高位，紧接的下一个字节对应右边8个象素，直到一行结束，接下来是下一行象素的点阵数据。
    HLE_U8 *raster;
} OSD_BITMAP_ATTR;



/*******************************************************************************
*@ Description    :OSD初始化函数
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int osd_init(void);
/*******************************************************************************
*@ Description    :OSD退出
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int osd_exit(void);

/*******************************************************************************
*@ Description    :创建OSD时间戳更新线程
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int timestamp_update_task(void);

/*******************************************************************************
*@ Description    :创建码率OSD更新线程
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int bitrate_update_task(void);




#ifdef __cplusplus
}
#endif	 


#endif 


