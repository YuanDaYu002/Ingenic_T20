#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc,char*argv[])
{
	if(argc < 2)
	{
		printf("usage: ./print_bin_file  +  input file\n");
		return -1;
	}
	
	int fd = open(argv[1],O_RDONLY);
	if(fd < 0)
	{
		printf("open failed!\n");
		return -1;
	}
	int i = 0;
	int buf = 0;
	while(1 == read(fd,&buf,1))
	{
		printf("0x");
		printf("%-2x, ",buf);
		i++;
		if(i >= 16)
		{
			printf("\n");
			i = 0;
		}
		buf = 0;
	
	}

	return 0;
}
