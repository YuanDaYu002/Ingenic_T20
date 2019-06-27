 
/***************************************************************************
* @file: ts_print.h
* @author:   
* @date:  4,10,2019
* @brief:  控制打印输出
* @attention:
***************************************************************************/
#ifndef _TS_PRINT_H
#define _TS_PRINT_H
#include "typeport.h"

#define TS_DEBUG // 打印控制开关
#define TS_ERROR
#define TS_FATAR

#ifdef TS_DEBUG
#define TS_DEBUG_LOG(args...)  \
		DEBUG_LOG(args)
#else
#define TS_DEBUG_LOG(args...)
#endif  /*TS_DEBUG*/


#ifdef TS_ERROR
#define TS_ERROR_LOG(args...)  \
		ERROR_LOG(args)
#else
#define TS_ERROR_LOG(args...)
#endif  /*TS_ERROR*/


#ifdef TS_FATAR
#define TS_FATAR_LOG(args...)  \
		FATAR_LOG(args)
#else
#define TS_FATAR_LOG(args...)
#endif  /*TS_FATAR*/




#endif



