 
/***************************************************************************
* @file: fmp4_print.h
* @author:   
* @date:  4,10,2019
* @brief:  控制打印输出
* @attention:
***************************************************************************/
#ifndef _FMP4_PRINT_H
#define _FMP4_PRINT_H
#include "typeport.h"

#define FMP4_DEBUG // 打印控制开关
#define FMP4_ERROR
#define FMP4_FATAR

#ifdef FMP4_DEBUG
#define FMP4_DEBUG_LOG(args...)  \
		DEBUG_LOG(args)
#else
#define FMP4_DEBUG_LOG(args...)
#endif  /*FMP4_DEBUG*/


#ifdef FMP4_ERROR
#define FMP4_ERROR_LOG(args...)  \
		ERROR_LOG(args)
#else
#define FMP4_ERROR_LOG(args...)
#endif  /*FMP4_ERROR*/


#ifdef FMP4_FATAR
#define FMP4_FATAR_LOG(args...)  \
		FATAR_LOG(args)
#else
#define FMP4_FATAR_LOG(args...)
#endif  /*FMP4_FATAR*/




#endif


