 
/***************************************************************************
* @file:移动侦测模块 
* @author:   
* @date:  7,24,2019
* @brief:  
* @attention:
***************************************************************************/

#ifndef MOVE_H
#define MOVE_H

#include "imp_common.h"

#define SAD_MODE_SIZE 8  //sadMode为0时表示检测区域大小8*8，这个宏定义为8;

#define IVS_GROUP_NUM	0			//智能算法组编号
#define IVS_SRC_CHN_ID  CH1_INDEX	//智能算法使用的视频源编号

#define IVS_MOVE_CHN_NUM	3		//移动侦测的通道号




/*---#用户设置的检测区域描述结构----------------------------------------------*/
#define USER_MAX_DETECT_REGION_NUM	5	//支持用户设置的检测区间最大个数

typedef struct 
{
	unsigned int enable;		//该区域是否有效：0：无效，1：有效
	unsigned int start_x;		//起点坐标X
	unsigned int start_y;		//起点坐标y
	unsigned int end_x;			//结束点坐标X
	unsigned int end_y;			//结束点坐标y
}region_t;

typedef struct 
{
	unsigned int		valid_num;			//有效区间的个数
	region_t region[USER_MAX_DETECT_REGION_NUM];  //检测区域数组
}user_detect_region_t;




/*******************************************************************************
*@ Description    :初始化移动侦测
*@ Input          :
*@ Output         :
*@ Return         :成功：返回0 ； 失败：返回-1
*@ attention      :
*******************************************************************************/
int ivs_move_init(void);

/*******************************************************************************
*@ Description    :启动移动侦测主线程
*@ Input          :
*@ Output         :
*@ Return         :成功：返回0 ； 失败：返回-1
*@ attention      :
*******************************************************************************/
int start_move_main_thread(void);

/*******************************************************************************
*@ Description    :设置移动侦测的灵敏度
*@ Input          :<sensor>灵敏度【对正常摄像机范围是0-4，对全景摄像机范围是0-8】
*@ Output         :
*@ Return         :成功：返回0 ； 失败：返回-1
*@ attention      :
*******************************************************************************/
int ivs_set_sense(int sensor);

/*******************************************************************************
*@ Description    :退出移动侦测
*@ Input          : 
*@ Output         :
*@ Return         :成功：返回0 ； 失败：返回-1
*@ attention      :
*******************************************************************************/
int ivs_move_exit(void);



#endif


