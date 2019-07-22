该工程用来生成BMP--->点阵字库文件,目前只支持部分ASCII码字库。
矢量字体转换BMP图，再制作成点阵字库.

.字体大小设置在 creat_bmp_demo.c ---> TTF_OpenFont函数的调用处
	按实际字体点阵大小来看 12号字体最接近8*16点阵
	48适合 1920*1080分辨率
	24适合 960*544分辨率
	16适合 640*360分辨率
	12适合 480*272分辨率

.根目录下已经制作好的点阵字库： 
	ASCII_vector_w1920 ：48号字体，适合1920*1080分辨率
	ASCII_vector_w960 ：24号字体，适合960*544分辨率
	ASCII_vector_w640 ：16号字体，适合640*360分辨率
	ASCII_vector_w480 ：12号字体，适合480*272分辨率

/******运行*******************************************************************************/
前提:
	1.确保 freetype-2.4.10 、SDL2-2.0.8、 SDL2_ttf-2.0.15 已经正确安装到linux环境中
	2.1.SDL2_ttf-2.0.15_out 以及 SDL2-2.0.8_out 文件下为对应第三方库编译安装后生成的文件，链接时需要。
	3.运行程序前需要先设置环境变量：
		export LD_LIBRARY_PATH=$(CURRENT_PATH)/libs
		其中，$(CURRENT_PATH)依据具体工程路径而定
执行：
1. 	make clean
	make    /*生成:
			BMP位图生成程序 ：create_bmp 
			位图解析+点阵字库打包程序：parse_bmp
		*/
			
2. ldd creat_bmp //主要查看可执行程序 creat_bmp 的动态库链接是否成功,不成功将无法正常运行
3. ./creat_bmp
4. ./parse_bmp

输出：
   ./ASCII_vector 即为最终字库文件

工具：
	print_bin_file工具直接将字库数据打印成十六进制数，方便将字库移植成数组。用法举例： print_bin_file + ASCII_vector_w640
