 
/***************************************************************************
* @file:video.c 
* @author:   
* @date:  5,29,2019
* @brief:  视频编码相关函数
* @attention:
***************************************************************************/
#include "imp_encoder.h"

#include "sample-common.h"
#include "typeport.h"
#include "video.h"

/*---#编码Channel码率控制器模式---------------------------------------------------*/
static const int S_RC_METHOD = ENC_RC_MODE_SMART;

/*---#编码通道参数配置------------------------------------------------------------*/
struct chn_conf chn[FS_CHN_NUM] = {
	{
		.index = CH0_INDEX, 		//main channel
		.enable = CHN0_EN,
		.fs_chn_attr = {
			.pixFmt = PIX_FMT_NV12, 				//图片格式
			.outFrmRateNum = VENC_FRAME_RATE_NUM, 	//通道的输出帧率分子
			.outFrmRateDen = VENC_FRAME_RATE_DEN,	//通道的输出帧率分母
			.nrVBs = 3,								//video buffer 数量
			.type = FS_PHY_CHANNEL, 				//通道类型

			/*图片裁剪属性*/
			.crop.enable = CROP_EN, 				//使能
			.crop.top = 0,							//裁剪左起始点
			.crop.left = 0,							//裁剪上起始点
			.crop.width = SENSOR_WIDTH,				//图片裁剪宽度
			.crop.height = SENSOR_HEIGHT,			//图片裁剪高度

			/*图片缩放属性*/
			.scaler.enable = 0,						//不使能

			.picWidth = SENSOR_WIDTH,				//图片宽度
			.picHeight = SENSOR_HEIGHT,				//图片高度
		   },
		.framesource_chn =	{ DEV_ID_FS, 0, 0},		//视频源{设备ID，组ID，输出ID}
		.imp_encoder = { DEV_ID_ENC, 0, 0},			//编码  	{设备ID，组ID，输出ID}
		.osdcell = {DEV_ID_OSD, 0, 0},				//OSD	{设备ID，组ID，输出ID}
	},
	{
		.index = CH1_INDEX,			//second channel
		.enable = CHN1_EN,
		.fs_chn_attr = {
			.pixFmt = PIX_FMT_NV12,					//图片格式
			.outFrmRateNum = VENC_FRAME_RATE_NUM,	//通道的输出帧率分子
			.outFrmRateDen = VENC_FRAME_RATE_DEN,	//通道的输出帧率分母
			.nrVBs = 3,								//video buffer 数量
			.type = FS_PHY_CHANNEL,					//通道类型

			/*图片裁剪属性*/
			.crop.enable = CROP_EN,					//使能
			.crop.top = 0,							//裁剪左起始点
			.crop.left = 0,							//裁剪上起始点
			.crop.width = SENSOR_WIDTH,				//图片裁剪宽度
			.crop.height = SENSOR_HEIGHT,			//图片裁剪高度

			/*图片缩放属性*/
			.scaler.enable = 1,						//使能
			.scaler.outwidth = SENSOR_WIDTH_SECOND,	//缩放后的图片宽度
			.scaler.outheight = SENSOR_HEIGHT_SECOND,//缩放后的图片高度

			.picWidth = SENSOR_WIDTH_SECOND,		//图片宽度
			.picHeight = SENSOR_HEIGHT_SECOND,		//图片高度
		   },
		.framesource_chn =	{ DEV_ID_FS, 1, 0},		//视频源{设备ID，组ID，输出ID}
		.imp_encoder = { DEV_ID_ENC, 1, 0},			//编码	{设备ID，组ID，输出ID}
		.osdcell = {DEV_ID_OSD, 1, 0},				//OSD	{设备ID，组ID，输出ID}
	},
};

			


/*******************************************************************************
*@ Description    :视频图像编码初始化函数
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int video_init(void)
{
	int i, ret;

	IMPEncoderAttr *enc_attr;//编码器参数指针
	IMPEncoderRcAttr *rc_attr;//码率控制器参数指针
	IMPFSChnAttr *imp_chn_attr_tmp;
	IMPEncoderCHNAttr channel_attr;

	for (i = 0; i <  FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			
			/* Step.2 FrameSource init */
			/*---#创建 FrameSource(VI) 通道+设置参数----------------------------------------*/
			ret = IMP_FrameSource_CreateChn(chn[i].index, &chn[i].fs_chn_attr);
			if(ret < 0){
				ERROR_LOG("IMP_FrameSource_CreateChn(chn%d) error !\n", chn[i].index);
				return HLE_RET_ERROR;
			}

			ret = IMP_FrameSource_SetChnAttr(chn[i].index, &chn[i].fs_chn_attr);
			if (ret < 0) {
				ERROR_LOG("IMP_FrameSource_SetChnAttr(chn%d) error !\n",  chn[i].index);
				return HLE_RET_ERROR;
			}
			
			/*---#创建编码 group ------------------------------------------------------------*/
			ret = IMP_Encoder_CreateGroup(chn[i].index);
			if (ret < 0) {
				ERROR_LOG( "IMP_Encoder_CreateGroup(%d) error !\n", i);
				return HLE_RET_ERROR;
			}

			/* Step.3.1 Encoder init */
			/*---#编码器参数设置-------------------------------------------------------------*/
			imp_chn_attr_tmp = &chn[i].fs_chn_attr;
			memset(&channel_attr, 0, sizeof(IMPEncoderCHNAttr));
			enc_attr = &channel_attr.encAttr;
			enc_attr->enType = PT_H264;
			enc_attr->bufSize = 0;
			enc_attr->profile = 1;
			enc_attr->picWidth = imp_chn_attr_tmp->picWidth;
			enc_attr->picHeight = imp_chn_attr_tmp->picHeight;
			/*---#码率控制器参数设置------------------------------------------------------------*/
			rc_attr = &channel_attr.rcAttr;
            rc_attr->outFrmRate.frmRateNum = imp_chn_attr_tmp->outFrmRateNum;
            rc_attr->outFrmRate.frmRateDen = imp_chn_attr_tmp->outFrmRateDen;
            rc_attr->maxGop = 2 * rc_attr->outFrmRate.frmRateNum / rc_attr->outFrmRate.frmRateDen;
            if (S_RC_METHOD == ENC_RC_MODE_CBR) {
                rc_attr->attrRcMode.rcMode = ENC_RC_MODE_CBR;
                rc_attr->attrRcMode.attrH264Cbr.outBitRate = (double)2000.0 * (imp_chn_attr_tmp->picWidth * imp_chn_attr_tmp->picHeight) / (1280 * 720);
                rc_attr->attrRcMode.attrH264Cbr.maxQp = 45;
                rc_attr->attrRcMode.attrH264Cbr.minQp = 15;
                rc_attr->attrRcMode.attrH264Cbr.iBiasLvl = 0;
                rc_attr->attrRcMode.attrH264Cbr.frmQPStep = 3;
                rc_attr->attrRcMode.attrH264Cbr.gopQPStep = 15;
                rc_attr->attrRcMode.attrH264Cbr.adaptiveMode = false;
                rc_attr->attrRcMode.attrH264Cbr.gopRelation = false;

                rc_attr->attrHSkip.hSkipAttr.skipType = IMP_Encoder_STYPE_N1X;
                rc_attr->attrHSkip.hSkipAttr.m = 0;
                rc_attr->attrHSkip.hSkipAttr.n = 0;
                rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = 0;
                rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = 0;
                rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
                rc_attr->attrHSkip.maxHSkipType = IMP_Encoder_STYPE_N1X;
            } else if (S_RC_METHOD == ENC_RC_MODE_VBR) {
                rc_attr->attrRcMode.rcMode = ENC_RC_MODE_VBR;
                rc_attr->attrRcMode.attrH264Vbr.maxQp = 45;
                rc_attr->attrRcMode.attrH264Vbr.minQp = 15;
                rc_attr->attrRcMode.attrH264Vbr.staticTime = 2;
                rc_attr->attrRcMode.attrH264Vbr.maxBitRate = (double)2000.0 * (imp_chn_attr_tmp->picWidth * imp_chn_attr_tmp->picHeight) / (1280 * 720);
                rc_attr->attrRcMode.attrH264Vbr.iBiasLvl = 0;
                rc_attr->attrRcMode.attrH264Vbr.changePos = 80;
                rc_attr->attrRcMode.attrH264Vbr.qualityLvl = 2;
                rc_attr->attrRcMode.attrH264Vbr.frmQPStep = 3;
                rc_attr->attrRcMode.attrH264Vbr.gopQPStep = 15;
                rc_attr->attrRcMode.attrH264Vbr.gopRelation = false;

                rc_attr->attrHSkip.hSkipAttr.skipType = IMP_Encoder_STYPE_N1X;
                rc_attr->attrHSkip.hSkipAttr.m = 0;
                rc_attr->attrHSkip.hSkipAttr.n = 0;
                rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = 0;
                rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = 0;
                rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
                rc_attr->attrHSkip.maxHSkipType = IMP_Encoder_STYPE_N1X;
            } else if (S_RC_METHOD == ENC_RC_MODE_SMART) {
                rc_attr->attrRcMode.rcMode = ENC_RC_MODE_SMART;
                rc_attr->attrRcMode.attrH264Smart.maxQp = 45;
                rc_attr->attrRcMode.attrH264Smart.minQp = 15;
                rc_attr->attrRcMode.attrH264Smart.staticTime = 2;
                rc_attr->attrRcMode.attrH264Smart.maxBitRate = (double)2000.0 * (imp_chn_attr_tmp->picWidth * imp_chn_attr_tmp->picHeight) / (1280 * 720);
                rc_attr->attrRcMode.attrH264Smart.iBiasLvl = 0;
                rc_attr->attrRcMode.attrH264Smart.changePos = 80;
                rc_attr->attrRcMode.attrH264Smart.qualityLvl = 2;
                rc_attr->attrRcMode.attrH264Smart.frmQPStep = 3;
                rc_attr->attrRcMode.attrH264Smart.gopQPStep = 15;
                rc_attr->attrRcMode.attrH264Smart.gopRelation = false;

                rc_attr->attrHSkip.hSkipAttr.skipType = IMP_Encoder_STYPE_N1X;
                rc_attr->attrHSkip.hSkipAttr.m = rc_attr->maxGop - 1;
                rc_attr->attrHSkip.hSkipAttr.n = 1;
                rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = 6;
                rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = 0;
                rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
                rc_attr->attrHSkip.maxHSkipType = IMP_Encoder_STYPE_N1X;
            } else { /* fixQp */
                rc_attr->attrRcMode.rcMode = ENC_RC_MODE_FIXQP;
                rc_attr->attrRcMode.attrH264FixQp.qp = 42;

                rc_attr->attrHSkip.hSkipAttr.skipType = IMP_Encoder_STYPE_N1X;
                rc_attr->attrHSkip.hSkipAttr.m = 0;
                rc_attr->attrHSkip.hSkipAttr.n = 0;
                rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = 0;
                rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = 0;
                rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
                rc_attr->attrHSkip.maxHSkipType = IMP_Encoder_STYPE_N1X;
            }
			
			/*---#创建编码通道+设置参数------------------------------------------------------------*/
			ret = IMP_Encoder_CreateChn(chn[i].index, &channel_attr);
			if (ret < 0) {
				ERROR_LOG("IMP_Encoder_CreateChn(%d) error !\n", i);
				return HLE_RET_ERROR;
			}

			ret = IMP_Encoder_RegisterChn(chn[i].index, chn[i].index);
			if (ret < 0) {
				ERROR_LOG("IMP_Encoder_RegisterChn(%d, %d) error: %d\n",
						chn[i].index, chn[i].index, ret);
				return HLE_RET_ERROR;
			}
			
		}

		
	}

	/*---#jpeg图像抓拍部分初始化------------------------------------------------------------*/
	ret = jpeg_init();
	if (ret < 0) {
		ERROR_LOG("jpeg init failed\n");
		return HLE_RTE_ERROR;
	}

	return HLE_RET_OK;
	
}

/*******************************************************************************
*@ Description    :开始视频编码（使能通道）
*@ Input          :<index>要使能的编码通道
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int video_start(int index)
{
	if(chn < 0 || chn >= FS_CHN_NUM)
	{
		ERROR_LOG("index(%d) is out of range!\n",index);
		return HLE_RET_ERROR;
	}
	
	int ret = 0, i = 0;
	
	/* Enable channels */
	if (chn[i].enable) 
	{
		ret = IMP_FrameSource_EnableChn(chn[i].index);
		if (ret < 0) 
		{
			ERROR_LOG("IMP_FrameSource_EnableChn(%d) error: %d\n", ret, chn[i].index);
			return HLE_RET_ERROR;
		}
	}

	return HLE_RET_OK;

}

/*******************************************************************************
*@ Description    :
*@ Input          :<args>编码通道的编号（通道号）
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
void *get_h264_stream_func(void *args)
{
	pthread_detach(pthread_self());
	
	int i, j, ret;
	char stream_path[64];

	i = (int ) (*((int*)args));

	video_start(i);//开始视频编码

	ret = IMP_Encoder_StartRecvPic(i);
	if (ret < 0) {
		ERROR_LOG("IMP_Encoder_StartRecvPic(%d) failed\n", i);
		return ((void *)-1);
	}

	/*---#DEBUG------------------------------------------------------------*/
	sprintf(stream_path, "%s/stream-%d.h264",
			STREAM_FILE_PATH_PREFIX, i);

	DEBUG_LOG("Open Stream file %s ", stream_path);
	int stream_fd = open(stream_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (stream_fd < 0) {
		ERROR_LOG( "failed: %s\n", strerror(errno));
		return ((void *)-1);
	}
	DEBUG_LOG( "OK\n");

	for (j = 0; j < NR_FRAMES_TO_SAVE; j++) 
	{
		ret = IMP_Encoder_PollingStream(i, 1000);
		if (ret < 0) 
		{
			ERROR_LOG( "Polling stream timeout\n");
			continue;
		}

		IMPEncoderStream stream;
		/* Get H264 Stream */
		ret = IMP_Encoder_GetStream(i, &stream, 1);
		if (ret < 0) 
		{
			ERROR_LOG( "IMP_Encoder_GetStream() failed\n");
			return ((void *)-1);
		}
		//DEBUG_LOG( "i=%d, stream.packCount=%d, stream.h264RefType=%d\n", i, stream.packCount, stream.h264RefType);

		ret = save_stream(stream_fd, &stream);
		if (ret < 0) 
		{
			close(stream_fd);
			return ((void *)ret);
		}

		IMP_Encoder_ReleaseStream(i, &stream);
	}

	close(stream_fd);
		
	ret = IMP_Encoder_StopRecvPic(i);
	if (ret < 0) {
		ERROR_LOG( "IMP_Encoder_StopRecvPic() failed\n");
		return ((void *)-1);
	}

	video_stop();//停止视频编码
	
	pthread_exit(0) ;
}



/*******************************************************************************
*@ Description    :创建线程获取编码视频帧
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int video_get_h264_stream_task(void)
{
	unsigned int i;
	int ret;
	pthread_t tid[FS_CHN_NUM];

	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = pthread_create(&tid[i], NULL, get_h264_stream_func, &chn[i].index);
			if (ret < 0) {
				ERROR_LOG( "Create Chn%d get_h264_stream_func \n",chn[i].index);
			}
		}
	}


	return HLE_RET_OK;
}


/*******************************************************************************
*@ Description    :暂停视频编码
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :
*******************************************************************************/
int video_stop(void)
{
	int ret = 0, i = 0;
	/* Enable channels */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable){
			ret = IMP_FrameSource_DisableChn(chn[i].index);
			if (ret < 0) {
				ERROR_LOG("IMP_FrameSource_DisableChn(%d) error: %d\n", ret, chn[i].index);
				return HLE_RET_ERROR;
			}
		}
	}
	return HLE_RET_OK;

}

static int encoder_chn_exit(int encChn)
{
	int ret;
	IMPEncoderCHNStat chn_stat;
	ret = IMP_Encoder_Query(encChn, &chn_stat);
	if (ret < 0) {
		ERROR_LOG( "IMP_Encoder_Query(%d) error: %d\n",
					encChn, ret);
		return -1;
	}

	if (chn_stat.registered) {
		ret = IMP_Encoder_UnRegisterChn(encChn);
		if (ret < 0) {
			ERROR_LOG( "IMP_Encoder_UnRegisterChn(%d) error: %d\n",
						encChn, ret);
			return -1;
		}

		ret = IMP_Encoder_DestroyChn(encChn);
		if (ret < 0) {
			ERROR_LOG( "IMP_Encoder_DestroyChn(%d) error: %d\n",
						encChn, ret);
			return -1;
		}
	}

	return 0;
}

/*******************************************************************************
*@ Description    :退出视频编码
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :1.调用该函数前需要先调用video_stop函数
					2.抓拍jpg图片也会连带退出（因为抓拍和视频编码是绑定在同一通道上的）
*******************************************************************************/
int video_exit(void)
{
	int ret ;

	//后边需要修改

	/* Step.c Encoder exit */
	ret = encoder_chn_exit(ENC_H264_CHANNEL);
	if (ret < 0) {
		ERROR_LOG("Encoder Channel %d exit	error: %d\n",
					ENC_H264_CHANNEL, ret);
		return HLE_RET_ERROR;
	}

	jpeg_exit(); //jpeg的通道退出操作必须放在这，否则会早成后边的释放操作失败
	
	
	ret = IMP_Encoder_DestroyGroup(0);
	if (ret < 0) {
		ERROR_LOG("IMP_Encoder_DestroyGroup(0) error: %d\n", ret);
		return HLE_RET_ERROR;
	}

	/* Step.d FrameSource exit */
	for (i = 0; i <  FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			/*Destroy channel i*/
			ret = IMP_FrameSource_DestroyChn(i);
			if (ret < 0) {
				ERROR_LOG("IMP_FrameSource_DestroyChn() error: %d\n", ret);
				return HLE_RET_ERROR;
			}
		}
	}


	/* Step.e System exit */

	return HLE_RET_OK;
}

static int save_stream(int fd, IMPEncoderStream *stream)
{
	int ret, i, nr_pack = stream->packCount;

	for (i = 0; i < nr_pack; i++) {
		ret = write(fd, (void *)stream->pack[i].virAddr,
					stream->pack[i].length);
		if (ret != stream->pack[i].length) {
			ERROR_LOG( "stream write error:%s\n", strerror(errno));
			return -1;
		}
		//DEBUG_LOG( "stream->pack[%d].dataType=%d\n", i, ((int)stream->pack[i].dataType.h264Type));
	}

	return 0;
}



/*******************************************************************************
*@ Description    :jpeg抓拍系统初始化
*@ Input          :
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :A.一路Group同时支持一路H264和一路JPEG抓拍
				   B.jpeg通道的通道号其实是2/3，不能与视频编码通道0/1产生冲突
*******************************************************************************/
int jpeg_init(void)
{
	int i, ret;
	IMPEncoderAttr *enc_attr;
	IMPEncoderCHNAttr channel_attr;
	IMPFSChnAttr *imp_chn_attr_tmp;

	for (i = 0; i <  FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			imp_chn_attr_tmp = &chn[i].fs_chn_attr;
			memset(&channel_attr, 0, sizeof(IMPEncoderCHNAttr));
			enc_attr = &channel_attr.encAttr;
			enc_attr->enType = PT_JPEG;
			enc_attr->bufSize = 0;
			enc_attr->profile = 0;
			enc_attr->picWidth = imp_chn_attr_tmp->picWidth;
			enc_attr->picHeight = imp_chn_attr_tmp->picHeight;

			/* Create Channel 注意，+2是为了不与前边的两个视频编码通道产生冲突*/
			ret = IMP_Encoder_CreateChn(2 + chn[i].index, &channel_attr);
			if (ret < 0) {
				ERROR_LOG("IMP_Encoder_CreateChn(%d) error: %d\n",
							chn[i].index, ret);
				return HLE_RET_ERROR;
			}

			/* Resigter Channel */
			ret = IMP_Encoder_RegisterChn(i, 2 + chn[i].index);
			if (ret < 0) {
				ERROR_LOG("IMP_Encoder_RegisterChn(0, %d) error: %d\n",
							chn[i].index, ret);
				return HLE_RET_ERROR;
			}
		}
	}

	return HLE_RET_OK;
}



/*******************************************************************************
*@ Description    :抓拍一帧jpeg图像
*@ Input          :<index>对应视频的编码通道[0,1]
*@ Output         :
*@ Return         :成功：HLE_RET_OK(0) ; 失败：HLE_RET_ERROR(-1)
*@ attention      :1.内部会自动换算成视频通道对应的抓拍通道
					2.抓拍一帧jpeg图像后会自动停止接收抓拍图像
*******************************************************************************/
int jpeg_get_one_snap(unsigned int index)
{
	int i, ret;
	char snap_path[64];
	
	if(index < 0 || index >= FS_CHN_NUM){
		ERROR_LOG("encChn out of range !\n");
		return HLE_RET_ERROR;
	}
	
	if (chn[index].enable) {
		
		ret = IMP_Encoder_StartRecvPic(2 + index);
		if (ret < 0) {
			ERROR_LOG( "IMP_Encoder_StartRecvPic(%d) failed\n", 2 + index);
			return HLE_RET_ERROR;
		}

		sprintf(snap_path, "%s/snap-%d.jpg",SNAP_FILE_PATH_PREFIX, index);

		ERROR_LOG( "Open Snap file %s ", snap_path);
		int snap_fd = open(snap_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
		if (snap_fd < 0) {
			ERROR_LOG( "failed: %s\n", strerror(errno));
			return HLE_RET_ERROR;
		}
		DEBUG_LOG( "OK\n");

		/* Polling JPEG Snap, set timeout as 1000msec */
		ret = IMP_Encoder_PollingStream(2 + index, 1000);
		if (ret < 0) {
			ERROR_LOG( "Polling stream timeout\n");
			continue;
		}

		IMPEncoderStream stream;
		/* Get JPEG Snap */
		ret = IMP_Encoder_GetStream(index + 2, &stream, 1);
		if (ret < 0) {
			ERROR_LOG( "IMP_Encoder_GetStream() failed\n");
			return HLE_RET_ERROR;
		}

		ret = save_stream(snap_fd, &stream);
		if (ret < 0) {
			close(snap_fd);
			return ret;
		}

		IMP_Encoder_ReleaseStream(2 + index, &stream);

		close(snap_fd);

		ret = IMP_Encoder_StopRecvPic(2 + index);
		if (ret < 0) {
			ERROR_LOG( "IMP_Encoder_StopRecvPic() failed\n");
			return HLE_RET_ERROR;
		}
	}
	else{
		ERROR_LOG("encode channel[%d] is not enable!\n",index);
		return HLE_RET_ERROR;
	}
		
	
	return HLE_RET_OK;
}


/*******************************************************************************
*@ Description    :退出jpeg编码通道
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :1.该函数不能单独调用，是函数 video_exit 的一部分
*******************************************************************************/
static int jpeg_exit(void)
{
	int ret;
	ret = encoder_chn_exit(ENC_JPEG_CHANNEL);
	if (ret < 0) {
		ERROR_LOG("Encoder Channel %d exit	error: %d\n",
					ENC_JPEG_CHANNEL, ret);
		return HLE_RET_ERROR;
	}
	
	return HLE_RET_OK;
}




