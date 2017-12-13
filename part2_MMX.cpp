#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<sstream>
#include<time.h>
#include<emmintrin.h>
using namespace std;
#define Height 1080
#define Width  1920
FILE* fp_in=NULL,*fp_out=NULL;
clock_t start, stop;
double duration;

typedef unsigned char BYTE;

__m64 Get_m64(long long n)
{
	union __m64__m64
	{
		__m64 m;
		long long i;
	}mi;
	mi.i=n;
	return mi.m;
}

int cnt=0;
void YUV2RGB(BYTE * yy,BYTE* uu,BYTE* vv,BYTE* rr,BYTE* gg,BYTE* bb)
{
/*	R= Y + 1.402 * (V-128) ;
	G= Y - 0.34413 * (U-128) - 0.71414 * (V-128);
	B= Y + 1.772 * (U-128);*/

	__m64* Y=(__m64 *) yy;
	__m64* U=(__m64 *) uu;
	__m64* V=(__m64 *) vv;

	__m64* R=(__m64 *) rr;
	__m64* G=(__m64 *) gg;
	__m64* B=(__m64 *) bb;

	__m64 tmp1,tmp2;
	_mm_empty();
	char u_tmp[8] ,v_tmp[8];
	long long half=0x8080808080808080;
	__m64 n1 = Get_m64(half);

	tmp1 = _mm_sub_pi8 (*U , n1);   // mmx U-128 signed
	tmp2 = _mm_sub_pi8 (*V , n1);   //	mmx V-128 signed

	memcpy(&v_tmp,&tmp2,8);
	memcpy(&u_tmp,&tmp1,8);
	/*
	printf("U-128:");
	for(int i=0;i<=7;++i)
	{	
		printf("%d\t",u_tmp[i] );
	}
	printf("\n");
	printf("V-128:");
	for(int i=0;i<=7;++i)
	{	
		printf("%d\t",v_tmp[i] );
	}
	printf("\n");

	*/

	//R
	memcpy(&v_tmp,&tmp2,8);
	//printf("RRRRRRRRRRRRRRRRR\n");
	for(int i=0;i<=7;++i)
		v_tmp[i]*=1.402;
	/*
	for(int i=0;i<=7;++i)
	{	
		printf("%d\t",v_tmp[i] );
	}	
	printf("\n");
	*/
	*R = _mm_add_pi8(*Y , *((__m64 *)v_tmp));

	//G
	memcpy(&v_tmp,&tmp2,8);
	memcpy(&u_tmp,&tmp1,8);

	_mm_empty();
	for(int i=0;i<=7;++i)
	{	
		u_tmp[i]*=-0.34413;
		v_tmp[i]*=-0.71414;
	}
	//printf("GGGGGGGGGGGGGGG\n");
	//printf("%llx\n",*((long long*)u_tmp) );
	/*	for(int i=0;i<=7;++i)
	{	
		printf("%d\t",u_tmp[i] );
	}
	printf("\n");*/
	*G = _mm_add_pi8(*Y ,*((__m64*)u_tmp));

	//printf("%llx\n",*((long long*)G) );
	*G = _mm_add_pi8(*G ,*((__m64*)v_tmp));
	//printf("%llx\n",*((long long*)G) );
	//B
	memcpy(&u_tmp,&tmp1,8);
	_mm_empty();
	for(int i=0;i<=7;++i)
		u_tmp[i] *=1.772;

	*B = _mm_add_pi8(*Y, *((__m64*)u_tmp));
	_mm_empty();
}

void RGB2YUV(BYTE * yy,BYTE* uu,BYTE* vv,BYTE* rr,BYTE* gg,BYTE* bb)
{
/*	Y = 0.299 * R + 0.587 * G + 0.114 * B;
	U = - 0.1687 * R - 0.3313 * G + 0.5 * B + 128;
	V = 0.5 * R - 0.4187 * G - 0.0813 * B + 128;
*/

	__m64* Y=(__m64 *) yy;
	__m64* U=(__m64 *) uu;
	__m64* V=(__m64 *) vv;

	__m64* R=(__m64 *) rr;
	__m64* G=(__m64 *) gg;
	__m64* B=(__m64 *) bb;

	__m64 tmp1,tmp2;
	_mm_empty();

	long long half=0x8080808080808080;
	__m64 n1 = Get_m64(half);

	BYTE r_tmp[8],g_tmp[8],b_tmp[8];

	//Y
	memcpy(r_tmp,rr,8);
	memcpy(g_tmp,gg,8);
	memcpy(b_tmp,bb,8);
	_mm_empty();
	for(int i=0;i<=7;++i)
	{
		r_tmp[i]*=0.299;
		g_tmp[i]*=0.587;
		b_tmp[i]*=0.114;
	}

	*Y = _mm_add_pi8(*((__m64*)r_tmp),*((__m64*)g_tmp));
	*Y = _mm_add_pi8(*Y , *((__m64*)b_tmp));

	//U
	memcpy(r_tmp,rr,8);
	memcpy(g_tmp,gg,8);
	memcpy(b_tmp,bb,8);
		_mm_empty();
	for(int i=0;i<=7;++i)
	{
		r_tmp[i]*=-0.1687;
		g_tmp[i]*=-0.3313;
		b_tmp[i]*=0.5;
	}	

	*U = _mm_add_pi8(*((__m64*)r_tmp),*((__m64*)g_tmp));
	*U = _mm_add_pi8(*U , *((__m64*)b_tmp));
	*U = _mm_add_pi8(*U , n1);

	//V
	memcpy(r_tmp,rr,8);
	memcpy(g_tmp,gg,8);
	memcpy(b_tmp,bb,8);
		_mm_empty();
	for(int i=0;i<=7;++i)
	{
		r_tmp[i]*=0.5;
		g_tmp[i]*=-0.4187;
		b_tmp[i]*=-0.0813;
	}	

	*V = _mm_add_pi8(*((__m64*)r_tmp),*((__m64*)g_tmp));
	*V = _mm_add_pi8(*V , *((__m64*)b_tmp));
	*V = _mm_add_pi8(*V , n1);	
	_mm_empty();
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
    fp_out = fopen("part2-MMX.yuv","wb");

	for( ; num < 86; alpha -=3 )
	{
		num++;

		if(fp_out==NULL)
		{
			printf("Error: open file failed\n");
			return 0;
		}		

		for(int pixel=0;pixel<Height*Width; pixel += 8)
		{			
			// get 8 Y, 8 U, 8 V 
			for(int i=0;i<=7;++i)
			{
				yy[i]=buffer_y[pixel+i];
				
				idx=((pixel+i)/(2*Width))*Width/2 +(pixel+i-((pixel+i)/Width)*Width)/2;
				uu[i]=buffer_u[idx];
				vv[i]=buffer_v[idx];
			}
			
			//YUV2RGB
			/*
			printf("Y:");
			for(int i=0;i<=7;++i)
				printf("%d\t",yy[i]);
			printf("\n");
			printf("U:");
			for(int i=0;i<=7;++i)
				printf("%d\t",uu[i]);
			printf("\n");
			printf("V:");
			for(int i=0;i<=7;++i)
				printf("%d\t",vv[i]);
			printf("\n\n");
			*/
			YUV2RGB(yy,uu,vv,R+pixel,G+pixel,B+pixel);
			/*
			printf("R:");
			for(int i=0;i<=7;++i)
				printf("%d\t",R[pixel+i]);
			printf("\n");
			printf("G:");
			for(int i=0;i<=7;++i)
				printf("%d\t",G[pixel+i]);
			printf("\n");
			printf("B:");
			for(int i=0;i<=7;++i)
				printf("%d\t",B[pixel+i]);
			printf("\n\n");
			*/
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
			/*
			printf("Y:");
			for(int i=0;i<=7;++i)
				printf("%d\t",buffer_wy[pixel+i]);
			printf("\n");
			printf("U:");
			for(int i=0;i<=7;++i)
				printf("%d\t",buffer_tu[i]);
			printf("\n");
			printf("V:");
			for(int i=0;i<=7;++i)
				printf("%d\t",buffer_tv[i]);
			printf("\n\n\n\n");

			*/
		}
		fwrite((BYTE*)buffer_wy,1,Height*Width,fp_out);
		fwrite((BYTE*)buffer_wu,1,Height*Width/4,fp_out);
		fwrite((BYTE*)buffer_wv,1,Height*Width/4,fp_out);
		//fwrite((BYTE*)buffer,1,Height*Width*4,fp_out);
		//fclose(fp_out);
	}
	fclose(fp_out);
	stop = clock();
	printf("start=%lu stop=%lu\n stop-start=%lf\n", start,stop,1.0*(stop-start));
	duration = ((double)(stop - start))*1000/ CLOCKS_PER_SEC;
	printf("total time= %f ms\n", duration);
	return 0;
}
