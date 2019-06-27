 
/***************************************************************************
* @file:buf.c 
* @author:   
* @date:  4,16,2019
* @brief:  缓存操作相关函数
* @attention:
***************************************************************************/
#include <stdlib.h>
#include <string.h>


#include "buf.h"
#include "ts_print.h"

/*******************************************************************************
*@ Description    :buf 初始化函数
*@ Input          :<buf> buf首地址
					<size> 需要的buf大小（函数内部分配）
*@ Output         :
*@ Return         :成功：0 失败：-1
*@ attention      :
*******************************************************************************/
int  init_buf(buf_t*buf,int size)
{
	if(NULL == buf  || size <= 0)
	{
		TS_ERROR_LOG("Illegal parameter: buf(%p) size(%d)\n",buf,size);
		return -1;
	}
	
	buf->buf = (char*)malloc(size);
	if(NULL == buf->buf)
	{
		TS_ERROR_LOG("malloc failed!\n");
		return -1;
	}
	memset(buf->buf,0,size);

	buf->buf_size = size;
	buf->w_pos = buf->buf;

	return 0;
}

/*******************************************************************************
*@ Description    :缓存区“写”操作函数，自带边界检查，写指针偏移功能
*@ Input          :<dst_buf> 目标缓存buf
					<src_buf> 源数据buf
					<write_size>写多大
*@ Output         :
*@ Return         :
*@ attention      :
*******************************************************************************/
int write_buf(buf_t*dst_buf,void*src_buf,int write_size)
{
	if(dst_buf->w_pos + write_size > dst_buf->buf + dst_buf->buf_size)
	{
		TS_ERROR_LOG("TS buf overflow!!\n");
		return -1;
	}
	memcpy(dst_buf->w_pos,src_buf,write_size);
	dst_buf->w_pos += write_size;
	return 0;
}

/*******************************************************************************
*@ Description    :写指针向后移动 len 字节
*@ Input          :
*@ Output         :
*@ Return         :成功：0 失败（越界）：-1
*@ attention      :
*******************************************************************************/
int w_pos_add(buf_t*dst_buf,int len)
{
	if(NULL != dst_buf && len > 0)
	{
		if(dst_buf->w_pos + len <= dst_buf->w_pos + dst_buf->buf_size)
		{
			dst_buf->w_pos += len;
			return 0;
		}
	}
	return -1;
}

/*******************************************************************************
*@ Description    :写指针向前移动 len 字节
*@ Input          :
*@ Output         :
*@ Return         :成功：0 失败（越界）：-1
*@ attention      :
*******************************************************************************/
int w_pos_sub(buf_t*dst_buf,int len)
{
	if(NULL != dst_buf && len > 0)
	{
		if(dst_buf->w_pos - len >= dst_buf->buf)
		{
			dst_buf->w_pos -= len;
			return 0;
		}
	}
	return -1;
}



/*******************************************************************************
*@ Description    :读缓存buf数据
*@ Input          :<buf> 要读的缓存
					<read_size>读多大
					<offset>读偏移量（距离缓存头）
*@ Output         :<out_buf>将数据保存的位置
*@ Return         :成功：读到的字节数 失败：-1
*@ attention      :
*******************************************************************************/
int read_buf(buf_t*buf,void*out_buf,int read_size,int offset)
{
	if(NULL != buf && NULL != out_buf && read_size > 0 && offset >= 0)
	{
		int can_read_bytes = 0;
		if(buf->w_pos >= buf->buf + offset + read_size)//有足够的数据
			can_read_bytes = read_size;
		else //只能满足一部分数据
			can_read_bytes = buf->w_pos - (buf->buf + offset);

		memcpy(out_buf,buf->buf + offset,can_read_bytes);
		return can_read_bytes;
	}
	return -1;

}

/*******************************************************************************
*@ Description    :对buf缓存进行复位操作
*@ Input          :
*@ Output         :
*@ Return         :成功：0 失败：-1
*@ attention      :
*******************************************************************************/
int reset_buf(buf_t*buf)
{
	if(buf != NULL)
	{
		buf->w_pos = buf->buf;
		memset(buf->buf,0,buf->buf_size);
		return 0;
	}
	return -1;
}
/*******************************************************************************
*@ Description    :重新分配buf的大小
*@ Input          :<buf>buf指针
					<size>重新分配的空间大小
*@ Output         :
*@ Return         :
*@ attention      :内部自带reset功能
*******************************************************************************/
int realloc_buf(buf_t*buf,int size)
{
	if(NULL == buf  || size <= 0)
	{
		TS_ERROR_LOG("Illegal parameter: buf(%p) size(%d)\n",buf,size);
		return -1;
	}
	
	buf->buf = (char*)realloc(buf->buf,size);
	if(NULL == buf->buf)
	{
		TS_ERROR_LOG("realloc failed !\n");
		return -1;
	}
	memset(buf->buf,0,size);
	buf->buf_size = size;
	buf->w_pos = buf->buf;

	return 0;
}

/*******************************************************************************
*@ Description    :释放buf
*@ Input          :
*@ Output         :
*@ Return         :成功：0 失败：-1
*@ attention      :
*******************************************************************************/
int free_buf(buf_t*buf)
{
	if(NULL != buf)
	{
		if(buf->buf)
		{
			free(buf->buf);
			memset(buf,0,sizeof(buf_t));
			return 0;
		}
	}
	return -1;
}


