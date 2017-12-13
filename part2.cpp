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
float duration;


typedef int LONG;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;

void YUV2RGB(unsigned char *Y,unsigned char *U,unsigned char *V,unsigned char *R,unsigned char *G,unsigned char *B)
{
	/*
	*R= *Y + 1.402 * (*V-128) ;
	*G= *Y - 0.34413 * (*U-128) - 0.71414 * (*V-128);
	*B= *Y + 1.772 * (*U-128);
	*/
	/*
	R= Y + 1.140 * V;
  	G= Y - 0.394 * U - 0.581 * V;
  	B= Y + 2.032 * U;
  	*/
  	*V = *V - 128;
  	*U = *U - 128;
  	char tmp_u,tmp_v;
  	//R
  	tmp_v = (char)(*V) * 1.402;
  	*R = *Y + tmp_v;
  	//G
  	tmp_v = (char)(*V) * -0.71414;
  	tmp_u = (char)(*U) * -0.34413;
  	*G = *Y + tmp_v + tmp_u;
  	//B
  	tmp_u = (char)(*U) * 1.772;
  	*B = *Y + tmp_u;
}

void RGB2YUV(unsigned char *Y,unsigned char *U,unsigned char *V,unsigned char *R,unsigned char *G,unsigned char *B)
{
	/*
	*Y = 0.299 * (*R) + 0.587 * (*G) + 0.114 * (*B);
	*U = - 0.1687 * (*R) - 0.3313 * (*G) + 0.5 * (*B) + 128;
	*V = 0.5 * (*R) - 0.4187 * (*G) - 0.0813 * (*B) + 128;
	*/
	/*
	Y= 0.299 * R + 0.587 * G + 0.114 * B;
  	U= -0.147 * R - 0.289 * G + 0.436 * B;
  	V= 0.615 * R - 0.515 * G - 0.100 * B;
  	*/
  	unsigned char tmp_r,tmp_g,tmp_b;
  	//Y
  	tmp_r = (*R) * 0.299;
  	tmp_g = (*G) * 0.587;
  	tmp_b = (*B) * 0.114;
  	*Y = tmp_r + tmp_g + tmp_b;
  	//U
  	tmp_r = (*R) * -0.1687;
  	tmp_g = (*G) * -0.3313;
  	tmp_b = (*B) * 0.5;
  	*U = tmp_r + tmp_g + tmp_b + 128;
  	//V
  	tmp_r = (*R) * 0.5;
  	tmp_g = (*G) * -0.4187;
  	tmp_b = (*B) * -0.0813;
  	*V = tmp_r + tmp_g + tmp_b + 128;
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
	unsigned char Y,U,V,R,G,B;
	string str;
	
    start = clock();
    fp_out = fopen("part2-2.yuv","wb");
	for( ; num < 86; alpha -=3 )
	{
		//printf("num=%d\n",num);
		
		
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
			
			idx=(pixel/(2*Width))*Width/2  + (pixel-(pixel/Width)*Width)/2;
			U = buffer_u[idx];
			V = buffer_v[idx];
			
			//YUV2RGB
			YUV2RGB(&Y,&U,&V,&R,&G,&B);
		
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
			RGB2YUV(&Y,&U,&V,&R,&G,&B);
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
	stop = clock();
	printf("start=%lu stop=%lu\n stop-start=%lu\n", start,stop,(stop-start));
	//printf("%d\n",CLOCKS_PER_SEC);
	duration = (float)(stop - start)/ CLOCKS_PER_SEC;
	printf("total time= %f s\n", duration);
	return 0;
}
