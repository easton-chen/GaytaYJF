#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<sstream>
#include<time.h>
using namespace std;
#define Height 1080
#define Width  1920
FILE* fp_in=NULL,*fp_out=NULL;
clock_t start, stop;
int duration;


typedef int LONG;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;

void YUV2RGB(BYTE * yy,BYTE* uu,BYTE* vv,BYTE* rr,BYTE* gg,BYTE* bb)
{
	
/*	R= Y + 1.402 * (V-128) ;
	G= Y - 0.34413 * (U-128) - 0.71414 * (V-128);
	B= Y + 1.772 * (U-128);*/

	char tmp1[8],tmp2[8];
	char u_tmp[8],v_tmp[8];
	for(int i=0;i<=7;++i)
		tmp1[i]=uu[i]-128;
	for(int i=0;i<=7;++i)
		tmp2[i]=vv[i]-128;

	//R
	memcpy(&v_tmp,&tmp2,8);

	for(int i=0;i<=7;++i)
		v_tmp[i]*=1.402;

	for(int i=0;i<=7;++i)
		rr[i]=yy[i]+v_tmp[i];	

	//G
	memcpy(&v_tmp,&tmp2,8);
	memcpy(&u_tmp,&tmp1,8);

	for(int i=0;i<=7;++i)
	{	
		u_tmp[i]*=-0.34413;
		v_tmp[i]*=-0.71414;
	}

	for(int i=0;i<=7;++i)
	{
		gg[i]=yy[i]+u_tmp[i]+v_tmp[i];
	}		

	//B
	memcpy(&u_tmp,&tmp1,8);
	for(int i=0;i<=7;++i)
		u_tmp[i] *=1.772;

	for(int i=0;i<=7;++i)
		bb[i]=yy[i]+u_tmp[i];		
}

void RGB2YUV(BYTE * yy,BYTE* uu,BYTE* vv,BYTE* rr,BYTE* gg,BYTE* bb)
{
/*	Y = 0.299 * R + 0.587 * G + 0.114 * B;
	U = - 0.1687 * R - 0.3313 * G + 0.5 * B + 128;
	V = 0.5 * R - 0.4187 * G - 0.0813 * B + 128;
*/
	char tmp1[8],tmp2[8];
	char u_tmp[8],v_tmp[8];

	BYTE r_tmp[8],g_tmp[8],b_tmp[8];

	//Y
	memcpy(r_tmp,rr,8);
	memcpy(g_tmp,gg,8);
	memcpy(b_tmp,bb,8);

	for(int i=0;i<=7;++i)
	{
		r_tmp[i]*=0.299;
		g_tmp[i]*=0.587;
		b_tmp[i]*=0.114;
	}

	for(int i=0;i<=7;++i)
		yy[i]=r_tmp[i]+g_tmp[i]+b_tmp[i];

	//U
	memcpy(r_tmp,rr,8);
	memcpy(g_tmp,gg,8);
	memcpy(b_tmp,bb,8);

	for(int i=0;i<=7;++i)
	{
		r_tmp[i]*=-0.1687;
		g_tmp[i]*=-0.3313;
		b_tmp[i]*=0.5;
	}	

	for(int i=0;i<=7;++i)
		uu[i]=r_tmp[i]+g_tmp[i]+b_tmp[i]+128;

	//V
	memcpy(r_tmp,rr,8);
	memcpy(g_tmp,gg,8);
	memcpy(b_tmp,bb,8);

	for(int i=0;i<=7;++i)
	{
		r_tmp[i]*=0.5;
		g_tmp[i]*=-0.4187;
		b_tmp[i]*=-0.0813;
	}	

	for(int i=0;i<=7;++i)
		vv[i]=r_tmp[i]+g_tmp[i]+b_tmp[i]+128;
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
	int num=0,idx;

	BYTE yy[8],uu[8],vv[8];
	BYTE R[Height*Width],G[Height*Width],B[Height*Width];
	BYTE buffer_tu[8],buffer_tv[8];

    start = clock();
    fp_out = fopen("part2.yuv","wb");

	for( ; num < 86; alpha -=3 )
	{
		num++;

		if(fp_out==NULL)
		{
			printf("Error: open file failed\n");
			return 0;
		}
		
		for(int pixel=0;pixel<Height*Width;pixel += 8)
		{
			for(int i=0;i<=7;++i)
			{
				yy[i]=buffer_y[pixel+i];
				
				idx=((pixel+i)/(2*Width))*Width/2 +(pixel+i-((pixel+i)/Width)*Width)/2;
				uu[i]=buffer_u[idx];
				vv[i]=buffer_v[idx];
			}
			
			//YUV2RGB
			YUV2RGB(yy,uu,vv,R+pixel,G+pixel,B+pixel);
		
			for (int i = 0; i < 8; ++i)
			{
				R[pixel+i] = R[pixel+i]*alpha/256;
				G[pixel+i] = G[pixel+i]*alpha/256;
				B[pixel+i] = B[pixel+i]*alpha/256;
			}	
			
			RGB2YUV(buffer_wy+pixel,buffer_tu,buffer_tv,R+pixel,G+pixel,B+pixel);

			for (int i = 0; i < 8; i+=2)
			{
				buffer_wu[idx-i/2] = buffer_tu[8-i-1];	
				buffer_wv[idx-i/2] = buffer_tv[8-i-1];	
			}			

		}
		fwrite((BYTE*)buffer_wy,1,Height*Width,fp_out);
		fwrite((BYTE*)buffer_wu,1,Height*Width/4,fp_out);
		fwrite((BYTE*)buffer_wv,1,Height*Width/4,fp_out);

	}
	fclose(fp_out);
	stop = clock();
	
	duration = (stop - start)*1000/ CLOCKS_PER_SEC;
	printf("total time= %d ms\n", duration);
	return 0;
}
