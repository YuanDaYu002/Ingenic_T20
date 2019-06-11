 
/***************************************************************************
* @file: CircularBuffer_print.h
* @author:   
* @date:  4,10,2019
* @brief:  控制打印输出
* @attention:
***************************************************************************/
#ifndef _CIRCULAR_BUFFER_PRINT_H
#define _CIRCULAR_BUFFER_PRINT_H
#include "typeport.h"

//#define CBUF_DEBUG // 打印控制开关
#define CBUF_ERROR
#define CBUF_FATAR

#ifdef CBUF_DEBUG
#define CBUF_DEBUG_LOG(args...)  \
		DEBUG_LOG(args)
#else
#define CBUF_DEBUG_LOG(args...)
#endif  /*CBUF_DEBUG*/


#ifdef CBUF_ERROR
#define CBUF_ERROR_LOG(args...)  \
		ERROR_LOG(args)
#else
#define CBUF_ERROR_LOG(args...)
#endif  /*CBUF_ERROR*/


#ifdef CBUF_FATAR
#define CBUF_FATAR_LOG(args...)  \
		FATAR_LOG(args)
#else
#define CBUF_FATAR_LOG(args...)
#endif  /*CBUF_FATAR*/




#endif



