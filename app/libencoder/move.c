 
 
/***************************************************************************
* @file:移动侦测部分逻辑 
* @author:   
* @date:  7,24,2019
* @brief:  
* @attention:
***************************************************************************/

#include <string.h>
#include <unistd.h>
#include <pthread.h>



#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <imp/imp_ivs.h>
#include <imp/imp_ivs_base_move.h>




#include "video.h"
#include "typeport.h"
#include "move.h"

IMPCell ivs_cell = {DEV_ID_IVS, IVS_GROUP_NUM, 0};


extern struct chn_conf chn[];


#if 1
/*---#用户设置的检测区域部分----------------------------------------------*/

static user_detect_region_t user_detect_region = {0};
pthread_mutex_t user_detect_region_mut;

//映射数组，将用户设置的区域映射到与移动检测结果同等规格的数组里，用0/1标识有效区域
static unsigned char maping_array[SENSOR_WIDTH_SECOND/SAD_MODE_SIZE][SENSOR_HEIGHT_SECOND/SAD_MODE_SIZE] = {{0}};
pthread_mutex_t maping_array_mut;



/*******************************************************************************
*@ Description    :设置检测区域参数（所有的区域）
*@ Input          :<region> 用户设置的检测区域信息
*@ Output         :
*@ Return         :成功：返回0 ； 失败：返回-1
*@ attention      :
*******************************************************************************/
int set_user_detect_region(user_detect_region_t*region)
{
	if(NULL == region)
	{
		ERROR_LOG("Illegal parameter!\n");
		return -1;
	}
	
	pthread_mutex_lock(&user_detect_region_mut);
	memcpy(&user_detect_region,region,sizeof(user_detect_region_t));	
	pthread_mutex_unlock(&user_detect_region_mut);
	return 0;
}

/*******************************************************************************
*@ Description    :获取检测区域参数（所有的区域）
*@ Input          :
*@ Output         :<region> 检测区域信息参数
*@ Return         :成功：返回0 ； 失败：返回-1
*@ attention      :
*******************************************************************************/
int get_user_detect_region(user_detect_region_t*region)
{
	if(NULL == region)
	{
		ERROR_LOG("Illegal parameter!\n");
		return -1;
	}
	pthread_mutex_lock(&user_detect_region_mut);
	memcpy(region,&user_detect_region,sizeof(user_detect_region_t));
	pthread_mutex_unlock(&user_detect_region_mut);
	return 0;
	
}

/*******************************************************************************
*@ Description    :添加一个新区域到检测区域数组（一个元素）
*@ Input          :<region>要添加的区域坐标信息
*@ Output         :
*@ Return         :成功：返回0 ； 失败：返回-1 ；数量已超限：-2 
*@ attention      :最多添加 USER_MAX_DETECT_REGION_NUM 个区域
*******************************************************************************/
int add_one_detect_region(region_t*region)
{
	if(NULL == region)
	{
		ERROR_LOG("Illegal parameter!\n");
		return -1;
	}

	int i = 0;
	pthread_mutex_lock(&user_detect_region_mut);
	for(i = 0;i<USER_MAX_DETECT_REGION_NUM;i++)
	{
		if(0 == user_detect_region.region[i].enable)//将数据保存到未被使用的元素内
		{
			user_detect_region.region[i].enable = 1;
			user_detect_region.region[i].start_x = region->start_x;
			user_detect_region.region[i].start_y = region->start_y;
			user_detect_region.region[i].end_x = region->end_x;
			user_detect_region.region[i].end_y = region->end_y;
			user_detect_region.valid_num ++;
			pthread_mutex_unlock(&user_detect_region_mut);
			return 0;
		}
	}
	
	/*---#用户设置的区域太多，超出限制--------------------------------------------------*/
	ERROR_LOG("Too much region !!\n");
	pthread_mutex_unlock(&user_detect_region_mut);
	return -2;
	
}

/*******************************************************************************
*@ Description    :删除检测区域数组的一个区域（一个元素）
*@ Input          :<region>要添加的区域坐标信息
*@ Output         :
*@ Return         :成功：返回0 ； 失败：返回-1 ；没有对应的区域：-2
*@ attention      :
*******************************************************************************/
int del_one_detect_region(region_t*region)
{
	if(NULL == region)
	{
		ERROR_LOG("Illegal parameter!\n");
		return -1;
	}

	int i = 0;
	int flag = 0; //标记找到区域
	pthread_mutex_lock(&user_detect_region_mut);
	for(i = 0;i<USER_MAX_DETECT_REGION_NUM;i++)
	{
		if((region->start_x == user_detect_region.region[i].start_x)&&
			(region->start_y == user_detect_region.region[i].start_y)&&
			(region->end_x == user_detect_region.region[i].end_x) &&
			(region->end_y == user_detect_region.region[i].end_y))
		{
			flag ++;
			//user_detect_region.region[i].enable = 0;
			memset(&user_detect_region.region[i],0,sizeof(region_t));
			user_detect_region.valid_num --;
		}
	}

	pthread_mutex_unlock(&user_detect_region_mut);

	if(0 == flag)//没有找到匹配的区域
		return -2;
	else
		return 0;
	
}



/*******************************************************************************
*@ Description    :刷新"映射数组"，
					将“用户设置的区域” 与 “算法的检测结果数据位置” 对应上
*@ Input          :<region>用户设置的检测区域信 息
*@ Output         :
*@ Return         :
*@ attention      :每次用户设置的区间有更新都需要调用此接口更新“映射数组”
*******************************************************************************/
static int refresh_maping_array(user_detect_region_t*region)
{
	if(NULL == region)
	{
		ERROR_LOG("Illegal parameter!\n");
		return -1;
	}
	
	if(0 == region->valid_num)//没有有效区间，直接返回
	{
		return 0;
	}

	pthread_mutex_lock(&maping_array_mut);
	
	memset(maping_array,0,sizeof(maping_array));
	int i = 0,j = 0,k = 0;
	unsigned int pic_width = SENSOR_WIDTH_SECOND/SAD_MODE_SIZE;
	unsigned int pic_height = SENSOR_HEIGHT_SECOND/SAD_MODE_SIZE;

	
	for(i = 1;i<=pic_width;i++)//按照8*8的宏块遍历整个图片
	{
		for(j = 1;j<=pic_height;j++)
		{
				for(k=0;k<USER_MAX_DETECT_REGION_NUM;k++)
				{
					if(1 == region->region[k].enable)
					{
						if((i*SAD_MODE_SIZE >= region->region[k].start_x)&&
							((i-1)*SAD_MODE_SIZE <= region->region[k].end_x)&&
							(j*SAD_MODE_SIZE >= region->region[k].start_y)&&
							((j-1)*SAD_MODE_SIZE <= region->region[k].end_y))
						{
							maping_array[i][j] = 1;//8*8的宏块与用户设置的区间有重叠，则对应的位置标志位1
						}
					}
					
				}
				
		}
	}

	pthread_mutex_unlock(&maping_array_mut);
	
	return 0;
	
}

/*******************************************************************************
*@ Description    :获取映射数组
*@ Input          :
*@ Output         :<array_data>数组（映射数组）的数据指针
					<data_len>数据长度
*@ Return         :返回 0
*@ attention      :
*******************************************************************************/
int get_maping_array(unsigned char**array_data,unsigned int* data_len)
{
	static unsigned  char tmp_array[SENSOR_WIDTH_SECOND/SAD_MODE_SIZE][SENSOR_HEIGHT_SECOND/SAD_MODE_SIZE] = {{0}};

	pthread_mutex_lock(&maping_array_mut);

	memcpy(tmp_array,maping_array,sizeof(tmp_array));
	pthread_mutex_unlock(&maping_array_mut);

	*array_data = (unsigned char*)&tmp_array;
	*data_len = sizeof(tmp_array);

	return 0;
} 



/*******************************************************************************
*@ Description    :对算法检测结果进行区域过滤
*@ Input          :<result>移动侦测算法检测的结果
*@ Output         :
*@ Return         :成功：
						区域内无移动：返回0；
						区域内有移动：返回大于0的数；
				   失败：返回-1
*@ attention      :
*******************************************************************************/
int filter_move_results(IMP_IVS_BaseMoveOutput*result)
{
	if(NULL == result)
	{
		ERROR_LOG("Illegal parameter!\n");
		return -1;
	}

	unsigned int i = 0;
	unsigned int move_count = 0;//标记有移动的宏块个数（在用户设置的检测范围内）
	unsigned char*maping_array_data = NULL;
	unsigned int data_len = 0;
	user_detect_region_t region;
	
	get_user_detect_region(&region);
	
	if(0 == region.valid_num)//没有设置有效区间,全屏统计
	{
		for(i = 0;i<result->datalen;i++)
		{
			if(result->data[i] > 0)
			{
				move_count ++;
			}
		}
	}
	else //有设置有效区间,只统计区域内部分
	{
		get_maping_array(&maping_array_data,&data_len);

		if(result->datalen != data_len)//算法检测的结果数据长度和映射数组长度不匹配！！640*360下值应为3600（个8*8宏块）
		{
			ERROR_LOG("result data length != maping_array_length!\n");
			return -1;
		}

		for(i = 0;i<data_len;i++)
		{
			if(maping_array_data[i] && result->data[i])
			{
				move_count ++;
			}
		}
	
	}

	return move_count;
		
	
}

#endif

/*******************************************************************************
*@ Description    :初始化移动侦测
*@ Input          : 
*@ Output         :
*@ Return         :成功：返回0 ； 失败：返回-1
*@ attention      :
*******************************************************************************/
int ivs_move_init(void)
{
	int ret = 0;

	pthread_mutex_init(&user_detect_region_mut, NULL);
	pthread_mutex_init(&maping_array_mut, NULL);
	

	ret = IMP_IVS_CreateGroup(IVS_GROUP_NUM);
	if (ret < 0) {
		ERROR_LOG("IMP_IVS_CreateGroup(%d) failed\n", IVS_GROUP_NUM);
		return -1;
	}

//	/* Bind framesource channel.1-output.1 to IVS group */
//	/**
//	 * FS.0 ----------------> Encoder.0(Main stream)
//	 * FS.1 ----(output.0)--> Encoder.1(Second stream)
//	 *       |
//	 *       \--(output.1)--> IVS
//	 */
//	IMPCell *fs_for_ivs_cell = &chn[IVS_SRC_CHN_ID].framesource_chn; 
//	ret = IMP_System_Bind(fs_for_ivs_cell, &ivs_cell);
//	if (ret < 0) {
//		ERROR_LOG("Bind FrameSource channel.%d output.%d and ivs%d failed\n",
//					IVS_SRC_CHN_ID,fs_for_ivs_cell->outputID,IVS_GROUP_NUM);
//		return -1;
//	}
	
	return 0;
}

/*******************************************************************************
*@ Description    :退出移动侦测
*@ Input          :
*@ Output         :
*@ Return         :成功：返回0 ； 失败：返回-1
*@ attention      :
*******************************************************************************/
int ivs_move_exit(void)
{
	int ret = 0;

	/* Exit sequence as follow */

	/* Step.13 UnBind */
	ret = IMP_System_UnBind(&chn[IVS_SRC_CHN_ID].framesource_chn, &ivs_cell);
	if (ret < 0) {
		ERROR_LOG("UnBind FrameSource channel%d and ivs%d failed\n", IVS_SRC_CHN_ID,IVS_MOVE_CHN_NUM);
		return -1;
	}

	ret = IMP_IVS_DestroyGroup(IVS_GROUP_NUM);
	if (ret < 0) {
		ERROR_LOG("IMP_IVS_DestroyGroup(%d) failed\n", IVS_GROUP_NUM);
		return -1;
	}

	pthread_mutex_destroy(&user_detect_region_mut);
	pthread_mutex_destroy(&maping_array_mut);
	
	return 0;
}

/*******************************************************************************
*@ Description    :开始移动侦测
*@ Input          :<grp_num>智能算法组编号
					<chn_num>移动侦测的通道号
*@ Output         :<interface> ivs模块，算法的通用接口
*@ Return         :成功：返回0 ； 失败：返回-1
*@ attention      :
*******************************************************************************/
int ivs_move_start(int grp_num, int chn_num, IMPIVSInterface **interface)
{
	int ret = 0;
	IMP_IVS_BaseMoveParam param;

	memset(&param, 0, sizeof(IMP_IVS_BaseMoveParam));
	param.skipFrameCnt = 0;
	param.referenceNum = 1;
	param.sadMode = 0;
	param.sense = 3;
	param.frameInfo.width = SENSOR_WIDTH_SECOND;
	param.frameInfo.height = SENSOR_HEIGHT_SECOND;

	*interface = IMP_IVS_CreateBaseMoveInterface(&param);
	if (*interface == NULL) {
		ERROR_LOG("IMP_IVS_CreateGroup(%d) failed\n", grp_num);
		return -1;
	}

	ret = IMP_IVS_CreateChn(chn_num, *interface);
	if (ret < 0) {
		ERROR_LOG("IMP_IVS_CreateChn(%d) failed\n", chn_num);
		return -1;
	}

	ret = IMP_IVS_RegisterChn(grp_num, chn_num);
	if (ret < 0) {
		ERROR_LOG("IMP_IVS_RegisterChn(%d, %d) failed\n", grp_num, chn_num);
		return -1;
	}

	ret = IMP_IVS_StartRecvPic(chn_num);
	if (ret < 0) {
		ERROR_LOG("IMP_IVS_StartRecvPic(%d) failed\n", chn_num);
		return -1;
	}

	return 0;
}

/*******************************************************************************
*@ Description    :停止移动侦测
*@ Input          :<chn_num>移动侦测的通道号
					<interface> ivs模块，算法的通用接口
*@ Output         :
*@ Return         :成功：返回0 ； 失败：返回-1
*@ attention      :
*******************************************************************************/
int ivs_move_stop(int chn_num, IMPIVSInterface *interface)
{
	int ret = 0;

	ret = IMP_IVS_StopRecvPic(chn_num);
	if (ret < 0) {
		ERROR_LOG("IMP_IVS_StopRecvPic(%d) failed\n", chn_num);
		return -1;
	}
	sleep(1);

	ret = IMP_IVS_UnRegisterChn(chn_num);
	if (ret < 0) {
		ERROR_LOG("IMP_IVS_UnRegisterChn(%d) failed\n", chn_num);
		return -1;
	}

	ret = IMP_IVS_DestroyChn(chn_num);
	if (ret < 0) {
		ERROR_LOG("IMP_IVS_DestroyChn(%d) failed\n", chn_num);
		return -1;
	}

	IMP_IVS_DestroyBaseMoveInterface(interface);

	return 0;
}

/*******************************************************************************
*@ Description    :设置移动侦测的灵敏度
*@ Input          :<sensor>灵敏度【对正常摄像机范围是0-4，对全景摄像机范围是0-8】
*@ Output         :
*@ Return         :成功：返回0 ； 失败：返回-1
*@ attention      :
*******************************************************************************/
int ivs_set_sense(int sensor)
{
#if 0
	int ret = 0;
	IMP_IVS_MoveParam param;
	int i = 0;

	ret = IMP_IVS_GetParam(IVS_SRC_CHN_ID, &param);
	if (ret < 0) {
		ERROR_LOG("IMP_IVS_GetParam(%d) failed\n", IVS_SRC_CHN_ID);
		return -1;
	}

	for( i = 0 ; i < param.roiRectCnt ; i++){
	  param.sense[i] = sensor;
	}

	ret = IMP_IVS_SetParam(IVS_SRC_CHN_ID, &param);
	if (ret < 0) {
		ERROR_LOG("IMP_IVS_SetParam(%d) failed\n", IVS_SRC_CHN_ID);
		return -1;
	}
#endif

	return 0;
}




/*******************************************************************************
*@ Description    :移动侦测主功能函数
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
void* move_main_process(void*args)
{
	pthread_detach(pthread_self());
		
	int ret;
	IMPIVSInterface *inteface = NULL;
	IMP_IVS_BaseMoveOutput *result = NULL;


	/* Step.7 ivs move start */
	ret = ivs_move_start(IVS_GROUP_NUM, IVS_MOVE_CHN_NUM, &inteface);
	if (ret < 0) {
		ERROR_LOG("ivs_move_start(%d, %d) failed\n",IVS_GROUP_NUM,IVS_MOVE_CHN_NUM);
		pthread_exit(NULL);
	}


	while (1/*后续再添加退出标志*/) 
	{
		ret = IMP_IVS_PollingResult(IVS_MOVE_CHN_NUM, IMP_IVS_DEFAULT_TIMEOUTMS);
		if (ret < 0) 
		{
			ERROR_LOG("IMP_IVS_PollingResult(%d, %d) failed\n", IVS_MOVE_CHN_NUM, IMP_IVS_DEFAULT_TIMEOUTMS);
			continue;
		}
		ret = IMP_IVS_GetResult(IVS_MOVE_CHN_NUM, (void **)&result);
		if (ret < 0) {
			ERROR_LOG("IMP_IVS_GetResult(%d) failed\n", IVS_MOVE_CHN_NUM);
			continue;
		}

#if 1
		ret = filter_move_results(result);
		if(ret > 0)
		{
			printf("-----------------------------------------move alarm!!! ret = %d\n",ret);
		}
		//DEBUG_LOG("not have move alarm!\n");
#endif 	
		//printf("move result->datalen(%d)\n",result->datalen);// 640*360下result->datalen = 3600
#if  0  //DEBUG 
		int j = 0;
		for(j = 0; j < result->datalen; j ++) 
		{
			/*640*360的范围，算法是按照8*8的块为单位进行检测，检测结果为一个字节的数值（>0就说明有东西移动），
				总体结果数据存储在result->data
				总长度result->datalen = (640/8)*(360/8) = 3600个字节
			*/
			printf("%3d ",*(result->data + j));
			if(j%(SENSOR_WIDTH_SECOND/SAD_MODE_SIZE) == 0);//打印完成一行回车
		}
		printf("\n////////////////////////////////////////////////////////////////\n");
#endif

		ret = IMP_IVS_ReleaseResult(IVS_MOVE_CHN_NUM, (void *)result);
		if (ret < 0) 
		{
			ERROR_LOG("IMP_IVS_ReleaseResult(%d) failed\n", IVS_MOVE_CHN_NUM);
			pthread_exit(NULL);
		}

	}

	ret = ivs_move_stop(IVS_MOVE_CHN_NUM, inteface);
	if (ret < 0) 
	{
		ERROR_LOG("ivs_move_stop(%d) failed\n",IVS_MOVE_CHN_NUM);
		pthread_exit(NULL);
	}
	
	pthread_exit(NULL);
	return 0;
}



/*******************************************************************************
*@ Description    :启动移动侦测主线程
*@ Input          :
*@ Output         :
*@ Return         :成功：返回0 ； 失败：返回-1
*@ attention      :
*******************************************************************************/
int start_move_main_thread(void)
{
	pthread_t ptid;
	if (pthread_create(&ptid, NULL, move_main_process,NULL) < 0) {
		ERROR_LOG("create move_main_process failed\n");
		return -1;
	}

	return 0;
}


