
/***************************************************************************
* @file:buf.h 
* @author:   
* @date:  4,16,2019
* @brief:  缓存操作相关函数
* @attention:
***************************************************************************/
#ifndef _BUF_H
#define _BUF_H

typedef struct _buf_t
{
	char*			buf;			//缓冲区buf位置
	int				buf_size;		//缓冲区buf大小
	char*			w_pos;			//写指针位置
}buf_t;

int init_buf(buf_t*buf,int size);
int write_buf(buf_t*dst_buf,void*src_buf,int write_size);
int read_buf(buf_t*buf,void*out_buf,int read_size,int offset);
int reset_buf(buf_t*buf);
int realloc_buf(buf_t*buf,int size);
int free_buf(buf_t*buf);
int w_pos_add(buf_t*dst_buf,int len);
int w_pos_sub(buf_t*dst_buf,int len);








#endif

