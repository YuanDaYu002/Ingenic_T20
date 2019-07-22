#ifndef HAL_ZIKU_H
#define HAL_ZIKU_H

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "typeport.h"

#define USE_VECTOR_FONT  1    //使用（矢量字库生成的）点阵字库

/*所有字符点阵组合成最终OSD点阵的信息描述结构*/
typedef struct _OSD_matrix_info_t
{
	int width; 	/*width of glyph*/
    int height; /*height of glyph*/
    int bpl; 	/*bytes per line*/
	unsigned char* matrix_data;
}OSD_matrix_info_t;



/*（单个）字形点阵描述结构*/
typedef struct _tag_GLYPH_LATTICE
{
    int width; 	/*width of glyph*/
    int height; /*height of glyph*/
    int bpl; 	/*bytes per line*/
    const char *bits; /*start address of glyph lattice，该字形点阵所在的位置*/
} GLYPH_LATTICE;

typedef enum 
{
	RESOLUTION_1920x1080 = 1,
	RESOLUTION_960x544,
	RESOLUTION_640x360,
	RESOLUTION_480x272
} video_resolution;


/*
 * function: int zk_init()
 * description:
 *              init zk
 * return:
 *              0, success
 *              -1, fail
 * */
int zk_init(void);


/*
 * function: char *zk_get_lattice(unsigned char *str, GLYPH_LATTICE *lattice)
 * description:
 *              找到str字符串中第一个字(单字节ASCII或双字节GBK字符)的点阵
 * arguments:
 *              str, 需要解析的字符串
 *              lattice, str中已处理字节的字形点阵信息
 * return:
 *              返回该次调用已处理的str中的字节数
 *              0, 未处理任何字节
 *              1, 处理了一个字节(ASCII)
 *              2, 处理了两个字节(GBK)
 *              -1, fail
 * */
int zk_get_lattice(const unsigned char *str, GLYPH_LATTICE *lattice);

/*
 * function: int str2matrix(const HLE_U8 *str, HLE_U8 *matrix_data)
 * description:
 *              将str字符串转换成整片的点阵信息
 * arguments:
 *              str, 需要转换的字符串
 *              matrix_data 转换后的点阵信息数据
 * return:		void
 *
 * */
void str2matrix(const HLE_U8 *str, HLE_U8 *matrix_data);

int str2matrix_vector(HLE_U8 *str, OSD_matrix_info_t* ret_info,int enc_w ,int enc_h );

/*
 * function: int get_matrix_width(const HLE_U8 *str)
 * description:
 *              将str字符串转换成整片的点阵后，点阵的宽度，单位为像素
 * arguments:
 *              str, 需要转换的字符串
 				resolution：按照哪种分辨率计算
 				width：OSD点阵的宽度（像素点个数）
 				height：OSD点阵的高度（像素点个数）
 * return:	成功：0  失败 -1
 *
 * */
int  get_matrix_width(const HLE_U8 *str,video_resolution resolution,HLE_U32* width,HLE_U32* height);


#endif /*HAL_ZIKU_H*/






