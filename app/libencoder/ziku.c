#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "ziku.h"
#include "ziku_array_vector.h"


/*
GBK的编码范围
                范围      第1字节        第2字节            编码数     字数
水准 GBK/1        A1–A9       A1–FE           846         717
水准 GBK/2        B0–F7       A1–FE           6,768       6,763
水准 GBK/3        81–A0       40–FE (7F除外) 6,080      6,080
水准 GBK/4        AA–FE       40–A0 (7F除外) 8,160      8,160
水准 GBK/5        A8–A9       40–A0 (7F除外) 192        166
用户定义        AA–AF       A1–FE           564
用户定义        F8–FE       A1–FE           658
用户定义        A1–A7       40–A0 (7F除外) 672
合计:                                         23,940      21,886
 */

#define ZK_USE_GBK3
#define ZK_USE_GBK4
#define ZK_USE_GBK5

struct ZK_HEAD
{
#define ZK_MAGIC  0x544E4F46
    int magic;
    HLE_U8 q_start;
    HLE_U8 q_end;
    HLE_U8 w_start;
    HLE_U8 w_end;
    HLE_U8 width;
    HLE_U8 height;
    HLE_U8 pad[2];
};

struct ZK
{
    struct ZK_HEAD head;
    int bpl; /*bytes per line*/
    int size; /*size of the buf*/
    char *buf; /*buf to hold the zk lattice*/
    char *file; /*zk file name*/
};

//===矢量字体部分======================================================
/*矢量转点阵字库，单字符描述结构*/
struct CHARACTER_INFO
{
      HLE_U16   width;          //当前字符的宽度
      HLE_U16   height;         //当前字符的高度
      HLE_U32   matrix_size;    //当前字符点阵数据的字节数（不包括该 CHARACTER_INFO） 
};

/*矢量点阵字库描述头结构*/
struct ZK_HEAD_VECTOR
{
#define ZK_MAGIC  0x544E4F46
    int         magic;
    HLE_U8      q_start;    //字库内部字符的编码区间，开始值（英文字符指ASCII码）
    HLE_U8      q_end;      //字库内部字符的编码区间，结束值（英文字符指ASCII码）
    HLE_U8      w_start;
    HLE_U8      w_end;
    HLE_U32     data_size;  //后边数据部分的大小（所有的 CHARACTER_INFO + 点阵数据部分）
    HLE_U32     ASCII_offset[256];  //专门描述ASCII字符的偏移量（距离文件头的偏移字节数），下标与ASCII码相对应（如果为0则代表不支持该ASCII码）
   
};

/*矢量字库描述结构*/
struct ZK_VECTOR
{
    struct ZK_HEAD_VECTOR head;
    int size;   /*size of the buf*/
    char *buf;  /*buf to hold the zk lattice，字库点阵数据的缓存起始位置*/
    char *file; /*zk file name*/
};



static struct ZK_VECTOR zks_vector[] = {
    {
        {0}, 0, NULL,
        "/system/font/ASCII_vector_w1920"
    },
    {
        {0}, 0, NULL,
        "/system/font/ASCII_vector_w960"
    },
    {
        {0}, 0, NULL,
        "/system/font/ASCII_vector_w640"
    },
    {
        {0}, 0, NULL,
        "/system/font/ASCII_vector_w480"
    }
};


#define ZK_QU_COUNT(pzk) (pzk->head.q_end - pzk->head.q_start + 1)
#define ZK_WEI_COUNT(pzk)   (pzk->head.w_end - pzk->head.w_start + 1)

#define ARRAY_COUNT(array)     (sizeof(array)/sizeof(array[0]))


/*
功能：加载字库到缓存（矢量版本）
参数:
    @zk : 字库文件描述结构 （需要提前填充zk->file文件名）
返回：
    成功：0
    失败：-1
    
*/

static int load_zk_vector(struct ZK_VECTOR *zk)
{
#if  1  //使用字库数组的方式
	DEBUG_LOG("loading %s ...\n", zk->file);
	char* ziku_array = NULL;
	int ziku_array_size = 0;
	if(NULL != strstr(zk->file,"ASCII_vector_w1920"))
	{
		ziku_array = (char*)&ASCII_vector_w1920;
		ziku_array_size = sizeof(ASCII_vector_w1920);
	}
	else if(NULL != strstr(zk->file,"ASCII_vector_w960"))
	{
		ziku_array = (char*)&ASCII_vector_w960;
		ziku_array_size = sizeof(ASCII_vector_w960);
	}
	else if(NULL != strstr(zk->file,"ASCII_vector_w640"))
	{
		ziku_array = (char*)&ASCII_vector_w640;
		ziku_array_size = sizeof(ASCII_vector_w640);
	}	
	else if(NULL != strstr(zk->file,"ASCII_vector_w480"))
	{
		ziku_array = (char*)&ASCII_vector_w480;
		ziku_array_size = sizeof(ASCII_vector_w480);
	}
	else
	{
		ERROR_LOG(" loading ziku array error!!\n");
		return -1;
	}

    /*calc size from head*/
	memcpy(&zk->head,ziku_array,sizeof (zk->head));

    /*---对字库的数据长度进行校验----------------------------------------*/
    zk->size = zk->head.data_size;

    /*calc array size*/
    DEBUG_LOG("ziku %s size %d\n",zk->file,ziku_array_size);

    /*compare two size, check if them match*/
    if (ziku_array_size != zk->size + sizeof (zk->head)) 
    {
        ERROR_LOG("ziku array size and font size mismatch, this array is broken!\n");
      	return -1;
    }

    /*初始化字库buf 指针*/
    zk->buf = ziku_array + sizeof (zk->head);

    return 0;
	
#else //使用字库文件的方式
    DEBUG_LOG("loading %s ...\n", zk->file);

    int fd = open(zk->file, O_RDONLY);
    if (fd < 0) 
    {
        ERROR_LOG("open %s failed !\n",zk->file);
        return -1;
    }

    /*calc size from head*/
    if (hal_readn(fd, &zk->head, sizeof (zk->head)) != sizeof (zk->head)) 
    {
        ERROR_LOG("read head failed!\n");
        goto closefile;
    }

    /*---对字库的数据长度进行校验----------------------------------------*/
    zk->size = zk->head.data_size;

    /*calc file size*/
    int file_size = lseek(fd, 0, SEEK_END);
    DEBUG_LOG("ziku %s size %d\n",zk->file,file_size);

    /*compare two size, check if them match*/
    if (file_size != zk->size + sizeof (zk->head)) 
    {
        ERROR_LOG("file size and font size mismatch, this file is broken!\n");
        goto closefile;
    }

    /*malloc zk buf and fill it*/
    zk->buf = malloc(zk->size);
    if (zk->buf == NULL) 
    {
        ERROR_LOG("malloc failed!\n");
        goto closefile;
    }

    lseek(fd, sizeof (zk->head), SEEK_SET);
    if (hal_readn(fd, zk->buf, zk->size) != zk->size) 
    {
        ERROR_LOG("read ziku failed !\n");
        goto freebuf;
    }
	
	close(fd);
    return 0;

freebuf:
    free(zk->buf);
closefile:
    close(fd);
    return -1;
#endif

}



/*
 * function: int zk_init()
 * description:
 *              init zk
 * return:
 *              0, success
 *              -1, fail
 * */
int zk_init()
{
    int i;
        for (i = 0; i < ARRAY_COUNT(zks_vector); i++) 
        {
            if (load_zk_vector(zks_vector + i) < 0) 
            {
                ERROR_LOG("load zk %s failed\n", zks_vector[i].file);
                return -1;
            }
        }

    return 0;
}

/*
 * function: char *zk_get_lattice(HLE_U8 *str, GLYPH_LATTICE *lattice)
 * description:
 *              找到str字符串中第一个字(单字节ASCII或双字节GBK字符)的点阵
 * arguments:
 *              str, 需要解析的字符串
 *				resolution:按照哪种分辨率处理
 *              lattice, str中已处理字节的字形点阵信息
 * return:
 *              返回该次调用已处理的str中的字节数
 *              0, 未处理任何字节
 *              1, 处理了一个字节(ASCII)
 *              2, 处理了两个字节(GBK)
 *              -1, fail
 * */
//矢量字库版本
 int zk_get_lattice_vector(const HLE_U8 *str,video_resolution resolution, GLYPH_LATTICE *lattice)
 {
 	if(NULL == str || NULL == lattice)
 	{
		ERROR_LOG("Illegal parameter!\n");
		return -1;
	}

	struct ZK_VECTOR* cur_zks = NULL;
	if(resolution == RESOLUTION_1920x1080)
		cur_zks = &zks_vector[0];
	else if(resolution == RESOLUTION_960x544)
		cur_zks = &zks_vector[1];
	else if(resolution == RESOLUTION_640x360)
		cur_zks = &zks_vector[2];
	else if(resolution == RESOLUTION_480x272)
		cur_zks = &zks_vector[3];
	else
	{
		ERROR_LOG("Unsupported resolution!\n");
		return -1;
	}
	
	
     /*---判断第一个字符是否在ASCII码范围内------*/
     if (*str >= cur_zks->head.q_start && *str <= cur_zks->head.q_end) 
     {
        unsigned int offset = cur_zks->head.ASCII_offset[(unsigned char)*str];
        if( offset != 0) //支持该ASCII码
        {
            struct CHARACTER_INFO tmp_info = {0};
            memcpy(&tmp_info,cur_zks->buf + offset - sizeof(struct ZK_HEAD_VECTOR),sizeof(struct CHARACTER_INFO));
            
            lattice->width = tmp_info.width;
            lattice->height = tmp_info.height;
            lattice->bpl = (tmp_info.width + 7)/8; //向上8对齐
            lattice->bits = cur_zks->buf + offset - sizeof(struct ZK_HEAD_VECTOR) + sizeof(struct CHARACTER_INFO);
            return 1;
        }
        else
        {
            /*如果我们的字库中没有该字符的点阵，使用#符号来代替*/
            struct CHARACTER_INFO tmp_info = {0};
            char*  character = "#";
            offset = cur_zks->head.ASCII_offset[(unsigned char)*character];
            memcpy(&tmp_info,cur_zks->buf + offset - sizeof(struct ZK_HEAD_VECTOR),sizeof(struct ZK_HEAD_VECTOR));

            lattice->width = tmp_info.width;
            lattice->height = tmp_info.height;
            lattice->bpl = (tmp_info.width + 7)/8;
            lattice->bits = cur_zks->buf + offset - sizeof(struct ZK_HEAD_VECTOR) + sizeof(struct CHARACTER_INFO);
            return 1;
        }

     }
     else //不属于 ASCII 字符
     {
         /*使用#符号来代替*/
        struct CHARACTER_INFO tmp_info = {0};
        char*  character = "#";
        unsigned int offset = cur_zks->head.ASCII_offset[(unsigned char)*character];
        memcpy(&tmp_info,cur_zks->buf + offset - sizeof(struct ZK_HEAD_VECTOR),sizeof(struct ZK_HEAD_VECTOR));

        lattice->width = tmp_info.width;
        lattice->height = tmp_info.height;
        lattice->bpl = (tmp_info.width + 7)/8;
        lattice->bits = cur_zks->buf + offset - sizeof(struct ZK_HEAD_VECTOR) + sizeof(struct CHARACTER_INFO);
        return 1;
     }
     
    /* 暂时不支持其他语言字库
     int i;
     for (i = 1; i < ARRAY_COUNT(zks); i++) {
         if ((*str >= zks[i].head.q_start && *str <= zks[i].head.q_end)
             && (*(str + 1) >= zks[i].head.w_start && *(str + 1) <= zks[i].head.w_end)) {
             struct ZK *pzk = zks + i;
             lattice->width = pzk->head.width;
             lattice->height = pzk->head.height;
             lattice->bpl = pzk->bpl;
             lattice->bits = pzk->buf + ((*str - pzk->head.q_start) * ZK_WEI_COUNT(pzk)
                     + (*(str + 1) - pzk->head.w_start)) * pzk->bpl * pzk->head.height;
 
             return 2;
         }
     }
    */

     return -1;
 }


/* 
功能：得到str点阵（osd点点阵）的width (宽度的像素点数，bit位数)
	@str:源的字符串
	@resolution：按照哪种分辨率计算
	@width，@height：计算的结果
返回：
    成功： 0
    失败：-1
*/
int  get_matrix_width(const HLE_U8 *str,video_resolution resolution,HLE_U32* width,HLE_U32* height)
{
    int str_len = 0;

    while (*str != '\0') 
    {
        GLYPH_LATTICE ltc;
        int ret = 0;
        
        ret = zk_get_lattice_vector(str, resolution , &ltc);
        if(ret < 0)
        {
            ERROR_LOG("zk_get_lattice_vector failed !\n");
            return -1;
        }
      

        str += ret;
        str_len += ltc.bpl;
        *height = ltc.height;//一般height都是一样的。
        
    }
    
    *width = str_len * 8;//*8为了返回bit数（对应点阵图的像素点数）

    return 0;

}


/* 
功能：
    将字符串信息转换成OSD图的点阵数据 
参数：
    @str：（入）要转换的字符串
    @matrix_data：（返）转换后的OSD点阵数据
    @enc_w: 编码图像的宽度
    @enc_h: 编码图像的高度
返回：
    成功：0
    失败：-1
注意：该函数返回的数据是点阵数据，并不是bmp数据
	使用结束后需要释放: free(ret_info->matrix_data)
*/
int str2matrix_vector(HLE_U8 *str, OSD_matrix_info_t* ret_info,int enc_w ,int enc_h )
{
	if(NULL == str || NULL == ret_info)
	{
		ERROR_LOG("Illegal parameter !\n");
		return -1;
	}

	char resolution = -1;
	if(enc_w == 1920 && enc_h == 1080)	
		resolution = RESOLUTION_1920x1080;
	else if(enc_w == 960 && enc_h == 544)
		resolution = RESOLUTION_960x544;
	else if(enc_w == 640 && enc_h == 360)
		resolution = RESOLUTION_640x360;
	else if(enc_w == 480 && enc_h == 272)
		resolution = RESOLUTION_480x272;
	else
	{
		ERROR_LOG("Unsupported resolution!\n");
		return -1;
	}
	
    unsigned int matrix_width,matrix_height;//字符串OSD点阵数据（宽度+高度）像素点个数（bit位数）
    
    int ret = get_matrix_width(str ,resolution, &matrix_width , &matrix_height);
    if(ret < 0)
    {
        ERROR_LOG("get_matrix_width failed !\n");
        return -1;
    }
   // DEBUG_LOG("matrix_width(%d) matrix_height(%d)\n",matrix_width,matrix_height);
    
    int matrix_bytes = matrix_width / 8;     //字符串OSD点阵数据（宽度）字节数
    ret_info->width = matrix_width;
    ret_info->height = matrix_height;
    ret_info->bpl = matrix_bytes;
    ret_info->matrix_data = (unsigned char*)malloc(matrix_bytes * matrix_height);
    if(NULL == ret_info->matrix_data)
    {
        ERROR_LOG("malloc failed !\n");
        return -1;
    }
    memset(ret_info->matrix_data,0,matrix_bytes * matrix_height);
        
    HLE_U8 *tmp_data = ret_info->matrix_data;
    int i = 0;
    while (*str != '\0') 
    {
        GLYPH_LATTICE ltc;
        ret = zk_get_lattice_vector(str,resolution, &ltc);//一次获取一个字符的BMP数据描述信息
        if(ret < 0)
        {
            ERROR_LOG("call zk_get_lattice_vector failed !\n");
            free (ret_info->matrix_data);
            return -1;
        }
        str += ret;

        /*
        完整打印一个字符的点阵数据(到线性数组缓存)：
        整个字符串的点阵数据当做一个整体存储到一维线性数组里边，假设 OSD字符串为 ABC，
        A0、B0、C0代表各个字符点阵第0行的数据，依次类推A1、B1、C1代表第1行的数据......
        则，线性数组存储的数据为：A0 B0 C0 A1 B1 C1 A2 B2 C2 ........ 
        如此去理解i的作用就显而易见了
        */
        int j;
        for (j = 0; j < ltc.height; j++) //单个字符逐行打印
        {
            tmp_data = ret_info->matrix_data + ((j * matrix_bytes) + i);
            int k;
            for (k = 0; k < ltc.bpl; k++) //拷贝一行的字节数
            {
                *tmp_data++ = *ltc.bits++;
            }
        }

        /* i = 已经打印字符的宽度之和 */
        i += ltc.bpl;
    }

    return 0;
}




#if 0

/* 测试用函数,用在让串口打印出点阵, str 参数要和str2matrix()函数的str 一致 */
int draw_pic(const HLE_U8 *str, const HLE_U8 *matrix_data)
{
    int matrix_width = get_matrix_width(str);
    int matrix_bytes = matrix_width / 8;

    int j;
    for (j = 0; j < 32; j++) {
        int k;
        for (k = 0; k < matrix_bytes; k++) {
            int i;
            for (i = 7; i >= 0; i--) {
                if (*matrix_data & (1 << i))
                    printf("*");
                else
                    printf("-");
            }
            matrix_data++;
        }
        printf("\n");
    }
    printf("\n");

    return 0;
}

/* 测试用函数，用来产生一个 32x32 的点阵 */
void make_text(HLE_U8 *str_data)
{
    int i, j;

    for (i = 0; i < 32; i++) {
        for (j = 0; j < 1; j++) {
            *str_data++ = 0x5F;
        }
        for (j = 0; j < 3; j++) {
            *str_data++ = 0x00;
        }
    }
}

/* 用来打印单个字符的点阵 */
int draw_text(const HLE_U8 *str)
{
    while (*str != 0) {
        GLYPH_LATTICE ltc;
        str += zk_get_lattice(str, &ltc);

        const char *start = ltc.bits;
        int j;
        for (j = 0; j < ltc.height; j++) {
            int k;
            for (k = 0; k < ltc.width; k++) {
                if (start[k / 8] & (1 << (7 - k % 8)))
                    printf("*");
                else
                    printf("-");
            }
            start += ltc.bpl;
            printf("\n");
        }
        printf("\n");
    }

    return 0;
}

int main(int argc, char *argv[])
{
    HLE_U8 test_str[] = "hhhh";
    HLE_U8 data[8096];
    HLE_U8 m_text[1024];
    zk_init();
    //make_text(m_text);
    //draw_pic(test_str, m_text);

    str2matrix(test_str, data);
    draw_pic(test_str, data);

    return 0;
}

#endif





