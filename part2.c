#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<sstream>
#define Height 1080
#define Width  1920
FILE* fp_in=NULL,*fp_out=NULL;

typedef int LONG;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef struct {
        WORD    bfType;//2
        DWORD   bfSize;//4
        WORD    bfReserved1;//2
        WORD    bfReserved2;//2
        DWORD   bfOffBits;//4
}__attribute__((packed))FileHead;
typedef struct{
        DWORD      biSize;//4
        LONG       biWidth;//4
        LONG       biHeight;//4
        WORD       biPlanes;//2
        WORD       biBitCount;//2
        DWORD      biCompress;//4
        DWORD      biSizeImage;//4
        LONG       biXPelsPerMeter;//4
        LONG       biYPelsPerMeter;//4
        DWORD      biClrUsed;//4
        DWORD      biClrImportant;//4
}__attribute__((packed))Infohead;
typedef struct 
{
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
} ARGB_data;
using namespace std;
int main()
{
	fp_in=fopen("dem2.yuv","r");
	if(fp_in==NULL)
	{
		printf("Error: open file falied\n");
		return 0;
	}

	char *buffer_y=(char*)malloc(sizeof(char)*Height*Width);

	char *buffer_u=(char*)malloc(sizeof(char)*Height*Width/4);

	char *buffer_v=(char*)malloc(sizeof(char)*Height*Width/4);

	fread(buffer_y,Height*Width,1,fp_in);
	fread(buffer_u,Height*Width/4,1,fp_in);
	fread(buffer_v,Height*Width/4,1,fp_in);
	
	fclose(fp_in);

	int alpha=256,num=1;
	char Y,U,V,R,G,B;
	string str;
	FileHead bmp_head;
	Infohead bmp_info;

	bmp_head.bfType=0x4d42;
	bmp_head.bfSize=Height*Width*4+sizeof(FileHead)+sizeof(Infohead);
	bmp_head.bfReserved1=bmp_head.bfReserved2=0;
	bmp_head.bfOffBits=bmp_head.bfSize-Height*Width*4;
	
	bmp_info.biSize=40;
    bmp_info.biWidth=Width;
    bmp_info.biHeight=Height;
    bmp_info.biPlanes=1;
    bmp_info.biBitCount = 32;
    bmp_info.biCompress=0;
    bmp_info.biSizeImage=0;
    bmp_info.biXPelsPerMeter=0;
    bmp_info.biYPelsPerMeter=0;
    bmp_info.biClrUsed=0;
    bmp_info.biClrImportant=0;

    ARGB_data buffer[Height*Width];

	//for( ; alpha<256 ; alpha +=3 )
	{
		memset(buffer,0,sizeof(buffer));
		stringstream ss;
		ss<<num;
		num++;

		str=ss.str();
		str+=".bmp";
		fp_out=fopen(str.c_str(),"wb");
		if(fp_out==NULL)
		{
			printf("Error: open file failed\n");
			return 0;
		}
		fwrite(&bmp_head,1,sizeof(FileHead),fp_out);
		fwrite(&bmp_info,1,sizeof(Infohead),fp_out);

		for(int pixel=0;pixel<Height*Width;++pixel)
		{
			Y=buffer_y[pixel];
			//idx=((pixel/Width-1)>0?(pixel/Width-1)-1:0)*Width/2+(pixel-(pixel/Width)*Width)/2;
			int idx=(pixel/(2*Width))*Width/2  + (pixel-(pixel/Width)*Width)/2;
			U=buffer_u[idx];
			V=buffer_v[idx];

			R= Y + 1.140 * V ;
			G= Y - 0.394 * U - 0.581 * V;
			B= Y + 2.032 * U;

			R=R>255?255:R;
			R=R<0?0:R;
			G=G>255?255:G;
			G=G<0?0:G;
			B=B>255?255:B;
			B=B<0?0:B;

			R *= alpha/256;
			G *= alpha/256;
			B *= alpha/256;

			//printf("R:%d, G:%d, B:%d\n",R,G,B);
			buffer[pixel].r=R;
			buffer[pixel].g=G;
			buffer[pixel].b=B;
			buffer[pixel].a=alpha;

			//fprintf(fp_out, "%c%c%c%c", alpha,R,G,B);

		}
		fwrite((BYTE*)buffer,1,Height*Width*4,fp_out);
		fclose(fp_out);
	}
	return 0;
}