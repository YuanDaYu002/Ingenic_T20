 
/***************************************************************************
* @file:encoder.c 
* @author:   
* @date:  5,29,2019
* @brief:  编码相关函数
* @attention:
***************************************************************************/




/*******************************************************************************
*@ Description    :IMP系统初始化
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
IMPSensorInfo sensor_info;
int system_init(void)
{
	int ret = 0;
	memset(&sensor_info, 0, sizeof(IMPSensorInfo));
	memcpy(sensor_info.name, SENSOR_NAME, sizeof(SENSOR_NAME));
	sensor_info.cbus_type = SENSOR_CUBS_TYPE;
	memcpy(sensor_info.i2c.type, SENSOR_NAME, sizeof(SENSOR_NAME));
	sensor_info.i2c.addr = SENSOR_I2C_ADDR;

	DEBUG_LOG("sample_system_init start...\n");

	ret = IMP_ISP_Open();
	if(ret < 0){
		ERROR_LOG("failed to open ISP\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_ISP_AddSensor(&sensor_info);
	if(ret < 0){
		ERROR_LOG(TAG, "failed to AddSensor\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_ISP_EnableSensor();
	if(ret < 0){
		ERROR_LOG(TAG, "failed to EnableSensor\n");
		return HLE_RET_ERROR;
	}

	ret = IMP_System_Init();
	if(ret < 0){
		ERROR_LOG(TAG, "IMP_System_Init failed\n");
		return HLE_RET_ERROR;
	}

	/* enable turning, to debug graphics */
	ret = IMP_ISP_EnableTuning();
	if(ret < 0){
		ERROR_LOG(TAG, "IMP_ISP_EnableTuning failed\n");
		return HLE_RET_ERROR;
	}

    ret = IMP_ISP_Tuning_SetSensorFPS(SENSOR_FRAME_RATE_NUM, SENSOR_FRAME_RATE_DEN);
    if (ret < 0){
        ERROR_LOG(TAG, "failed to set sensor fps\n");
        return HLE_RET_ERROR;
    }

	DEBUG_LOG("ImpSystemInit success\n");

	return HLE_RET_OK;
}

/*******************************************************************************
*@ Description    :退出IMP系统
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
int system_exit(void)
{
	int ret = 0;

	DEBUG_LOG("sample_system_exit start...\n");

	IMP_System_Exit();

	ret = IMP_ISP_DisableSensor();
	if(ret < 0){
		ERROR_LOG("failed to EnableSensor\n");
		return -1;
	}

	ret = IMP_ISP_DelSensor(&sensor_info);
	if(ret < 0){
		ERROR_LOG("failed to AddSensor\n");
		return -1;
	}

	ret = IMP_ISP_DisableTuning();
	if(ret < 0){
		ERROR_LOG("IMP_ISP_DisableTuning failed\n");
		return -1;
	}

	if(IMP_ISP_Close()){
		ERROR_LOG("failed to open ISP\n");
		return -1;
	}

	DEBUG_LOG(TAG, " sample_system_exit success\n");

	return 0;
}


int Vencoder_init(void)
{

}

int Aencoder_init(void)
{

}

/*---#对外接口部分------------------------------------------------------------*/

/*******************************************************************************
*@ Description    :编码系统初始化
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :内部初始化的顺序不可随意变动，修改时需要注意
*******************************************************************************/
int encoder_system_init(void)
{
	int ret,i;
	
	ret = system_init();
	if (ret < 0) {
		ERROR_LOG("jpeg init failed\n");
		return -1;
	}
		
	ret = video_init();
	if (ret < 0) {
		ERROR_LOG("jpeg init failed\n");
		return -1;
	}
	
	/*Bind 建立绑定关系， 数据源通道--->OSD叠加--->编码通道 放在最后初始化的 osd_init 中了
	  所以，osd的初始化必须位于最后。
	*/
	ret = osd_init();
	if (ret < 0) {
		ERROR_LOG("OSD init failed\n");
		return -1;
	}
	
	return 0;
	
}

/*******************************************************************************
*@ Description    :编码系统退出
*@ Input          :
*@ Output         :
*@ Return         :
*@ attention      :内部退出的先后顺序不能随意改变，修改时需要注意
*******************************************************************************/
int encoder_system_exit(void)
{
	video_exit();
	
	system_exit();
	
	
}


