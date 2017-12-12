#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<sstream>
#include<time.h>
using namespace std;
#define Height 1080
#define Width  1920
FILE* fp_dem1=NULL,*fp_dem2=NULL,*fp_out=NULL;
clock_t start, stop;
double duration;
typedef unsigned char BYTE;
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
	fp_dem1=fopen("dem1.yuv","r");
	fp_dem2=fopen("dem2.yuv","r");
	if(fp_dem1==NULL||fp_dem2==NULL)
	{ 
		printf("Error: open file falied\n");
		return 0;
	}


	unsigned char *buffer_y_dem1=(unsigned char*)malloc(sizeof(char)*Height*Width);
	unsigned char *buffer_u_dem1=(unsigned char*)malloc(sizeof(char)*Height*Width/4);
	unsigned char *buffer_v_dem1=(unsigned char*)malloc(sizeof(char)*Height*Width/4);

	fread(buffer_y_dem1,Height*Width,1,fp_dem1);
	fread(buffer_u_dem1,Height*Width/4,1,fp_dem1);
	fread(buffer_v_dem1,Height*Width/4,1,fp_dem1);

	fclose(fp_dem1);

	unsigned char *buffer_y_dem2=(unsigned char*)malloc(sizeof(char)*Height*Width);
	unsigned char *buffer_u_dem2=(unsigned char*)malloc(sizeof(char)*Height*Width/4);
	unsigned char *buffer_v_dem2=(unsigned char*)malloc(sizeof(char)*Height*Width/4);
	
	fread(buffer_y_dem2,Height*Width,1,fp_dem2);
	fread(buffer_u_dem2,Height*Width/4,1,fp_dem2);
	fread(buffer_v_dem2,Height*Width/4,1,fp_dem2);

	fclose(fp_dem2);

	unsigned char *buffer_wy=(unsigned char*)malloc(sizeof(char)*Height*Width);
	unsigned char *buffer_wu=(unsigned char*)malloc(sizeof(char)*Height*Width/4);
	unsigned char *buffer_wv=(unsigned char*)malloc(sizeof(char)*Height*Width/4);


	unsigned char alpha=255;
	int num=0;
	unsigned char Y,U,V,R,G,B,Y1,Y2,U1,U2,V1,V2,R1,R2,G1,G2,B1,B2;

    start = clock();
    fp_out = fopen("part3.yuv","wb");
	if(fp_out==NULL)
	{
		printf("Error: open file failed\n");
		return 0;
	}

	for( ; num < 86; alpha -=3 )
	{	
		num++;	
		for(int pixel=0;pixel<Height*Width;++pixel)
		{
			int idx=(pixel/(2*Width))*Width/2  + (pixel-(pixel/Width)*Width)/2;
			
			Y1 = buffer_y_dem1[pixel];
			U1 = buffer_u_dem1[idx];
			V1 = buffer_v_dem1[idx];

			Y2 = buffer_y_dem2[pixel];
			U2 = buffer_u_dem2[idx];
			V2 = buffer_v_dem2[idx];			

			//YUV2RGB
			YUV2RGB(Y1,U1,V1,R1,G1,B1);
			YUV2RGB(Y2,U2,V2,R2,G2,B2);			
		
			R = ( R1 * alpha + R2 * (256-alpha) )/256;
			G = ( G1 * alpha + G2 * (256-alpha) )/256;
			B = ( B1 * alpha + B2 * (256-alpha) )/256;
			
			RGB2YUV(Y,U,V,R,G,B);

			buffer_wy[pixel] = Y;
			buffer_wu[idx] = U;
			buffer_wv[idx] = V;

		}
		fwrite((BYTE*)buffer_wy,1,Height*Width,fp_out);
		fwrite((BYTE*)buffer_wu,1,Height*Width/4,fp_out);
		fwrite((BYTE*)buffer_wv,1,Height*Width/4,fp_out);

	}
	fclose(fp_out);
	stop = clock();
	duration = ((double)(stop - start))*1000/ CLOCKS_PER_SEC;
	printf("total time= %f ms\n", duration);
	return 0;
}
