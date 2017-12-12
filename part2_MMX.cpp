#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<sstream>
using namespace std;
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


void YUV2RGB(unsigned char &Y,unsigned char &U,unsigned char &V,unsigned char &R,unsigned char &G,unsigned char &B)
{
	R= Y + 1.402 * (V-128) ;
	G= Y - 0.34413 * (U-128) - 0.71414 * (V-128);
	B= Y + 1.772 * (U-128);
}

void RGB2YUV(unsigned char &Y,unsigned char &U,unsigned char &V,unsigned char &R,unsigned char &G,unsigned char &B)
{
	Y = 0.299 * R + 0.587 * G + 0.114 * B;
	U = - 0.1687 * R - 0.3313 * G + 0.5 * B + 128;
	V = 0.5 * R - 0.4187 * G - 0.0813 * B + 128;
}

int main()
{
	fp_in=fopen("dem2.yuv","r");
	if(fp_in==NULL)
	{
		printf("Error: open file falied\n");
		return 0;
	}

	unsigned char *buffer_y=(unsigned char*)malloc(sizeof(char)*Height*Width);
	unsigned char *buffer_wy=(unsigned char*)malloc(sizeof(char)*Height*Width);
	unsigned char *buffer_u=(unsigned char*)malloc(sizeof(char)*Height*Width/4);
	unsigned char *buffer_wu=(unsigned char*)malloc(sizeof(char)*Height*Width/4);
	unsigned char *buffer_v=(unsigned char*)malloc(sizeof(char)*Height*Width/4);
	unsigned char *buffer_wv=(unsigned char*)malloc(sizeof(char)*Height*Width/4);

	fread(buffer_y,Height*Width,1,fp_in);
	fread(buffer_u,Height*Width/4,1,fp_in);
	fread(buffer_v,Height*Width/4,1,fp_in);
	
	fclose(fp_in);

	unsigned char alpha=255;
	int num=0;
	unsigned char Y,U,V,R,G,B;
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
    
    fp_out = fopen("part2-2.yuv","wb");
	for( ; num < 86; alpha -=3 )
	{
		//printf("num=%d\n",num);
		memset(buffer,0,sizeof(buffer));
		
		//stringstream ss;
		//ss<<num;
		num++;

		//str=ss.str();
		//str+=".yuv";
		//fp_out=fopen(str.c_str(),"wb");
		if(fp_out==NULL)
		{
			printf("Error: open file failed\n");
			return 0;
		}
		//fwrite(&bmp_head,1,sizeof(FileHead),fp_out);
		//fwrite(&bmp_info,1,sizeof(Infohead),fp_out);
		
		for(int pixel=0;pixel<Height*Width;++pixel)
		{
			Y = buffer_y[pixel];
			//idx=((pixel/Width-1)>0?(pixel/Width-1)-1:0)*Width/2+(pixel-(pixel/Width)*Width)/2;
			int idx=(pixel/(2*Width))*Width/2  + (pixel-(pixel/Width)*Width)/2;
			U = buffer_u[idx];
			V = buffer_v[idx];
			
			//YUV2RGB
			YUV2RGB(Y,U,V,R,G,B);
		
			R = R*alpha/256;
			G = G*alpha/256;
			B = B*alpha/256;
			
			//printf("R:%d, G:%d, B:%d\n",R,G,B);
			/*
			buffer[pixel].r=R;
			buffer[pixel].g=G;
			buffer[pixel].b=B;
			buffer[pixel].a=alpha;
			*/
			RGB2YUV(Y,U,V,R,G,B);
			buffer_wy[pixel] = Y;
			buffer_wu[idx] = U;
			buffer_wv[idx] = V;

			//fprintf(fp_out, "%c%c%c%c", alpha,R,G,B);

		}
		fwrite((BYTE*)buffer_wy,1,Height*Width,fp_out);
		fwrite((BYTE*)buffer_wu,1,Height*Width/4,fp_out);
		fwrite((BYTE*)buffer_wv,1,Height*Width/4,fp_out);
		//fwrite((BYTE*)buffer,1,Height*Width*4,fp_out);
		//fclose(fp_out);
	}
	fclose(fp_out);
	return 0;
}
