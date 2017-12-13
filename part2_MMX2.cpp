#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<sstream>
#include<time.h>
//#include<emmintrin.h>
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

int cnt=0;
void YUV2RGB(BYTE * yy,BYTE* uu,BYTE* vv,BYTE* rr,BYTE* gg,BYTE* bb)
{
/*	R= Y + 1.402 * (V-128) ;
	G= Y - 0.34413 * (U-128) - 0.71414 * (V-128);
	B= Y + 1.772 * (U-128);*/
    
    long long half=0x8080808080808080;
	__asm__(
        "movq (%0),%%mm1\n"
        "movq (%1),%%mm2\n"
        "movq %2,%%mm3\n"
        "psubb %%mm3,%%mm2\n"
        "psubb %%mm3,%%mm1\n"
        "movq %%mm1,(%0)\n"
        "movq %%mm2,(%1)\n"
        "emms"
        :
        :"r"(uu),"r"(vv),"r"(half)
    );
    
    char v_tmp[8],u_tmp[8];
    //R
    
    for(int i=0;i<8;i++)
    {
        v_tmp[i]=(char)vv[i]*1.402;
    }
    __asm__(
        "movq (%1),%%mm0\n"
        "movq (%2),%%mm1\n"
        "paddb %%mm1,%%mm0\n"
        "movq %0,%%rbx\n"
        "movq %%mm0,(%%rbx)\n"
        "emms"
        :
        :"r"(rr),"r"(yy),"r"(v_tmp)
    );
    
    //G
    for(int i=0;i<8;i++)
    {
        u_tmp[i]=(char)uu[i]*-0.34413;
        v_tmp[i]=(char)vv[i]*-0.71414;
    }
    __asm__(
        "movq (%1),%%mm0\n"
        "movq (%2),%%mm1\n"
        "movq (%3),%%mm2\n"
        "paddb %%mm1,%%mm0\n"
        "paddb %%mm2,%%mm0\n"
        "movq %0,%%rbx\n"
        "movq %%mm0,(%%rbx)\n"
        "emms"
        :
        :"r"(gg),"r"(yy),"r"(v_tmp),"r"(u_tmp)
    );
    
    //B
    for(int i=0;i<8;i++)
    {
        u_tmp[i]=(char)uu[i]*1.772;
    }
    __asm__(
        "movq (%1),%%mm0\n"
        "movq (%2),%%mm1\n"
        "paddb %%mm1,%%mm0\n"
        "movq %0,%%rbx\n"
        "movq %%mm0,(%%rbx)\n"
        "emms"
        :
        :"r"(bb),"r"(yy),"r"(u_tmp)
    );
    
}

void RGB2YUV(BYTE * yy,BYTE* uu,BYTE* vv,BYTE* rr,BYTE* gg,BYTE* bb)
{
/*	Y = 0.299 * R + 0.587 * G + 0.114 * B;
	U = - 0.1687 * R - 0.3313 * G + 0.5 * B + 128;
	V = 0.5 * R - 0.4187 * G - 0.0813 * B + 128;
*/
    long long half=0x8080808080808080;
    BYTE r_tmp[8],g_tmp[8],b_tmp[8];
    //Y
    for(int i=0;i<=7;++i)
	{
		r_tmp[i]=rr[i]*0.299;
		g_tmp[i]=gg[i]*0.587;
		b_tmp[i]=bb[i]*0.114;
	}

    __asm__(
        "movq (%1),%%mm0\n"
        "movq (%2),%%mm1\n"
        "movq (%3),%%mm2\n"
        "paddb %%mm1,%%mm0\n"
        "paddb %%mm2,%%mm0\n"
        "movq %0,%%rbx\n"
        "movq %%mm0,(%%rbx)\n"
        "emms"
        :
        :"r"(yy),"r"(r_tmp),"r"(g_tmp),"r"(b_tmp) 
    );
    //U
    for(int i=0;i<=7;++i)
	{
		r_tmp[i]=rr[i]*-0.1687;
		g_tmp[i]=gg[i]*-0.3313;
		b_tmp[i]=bb[i]*0.5;
	}
    __asm__(
        "movq (%1),%%mm0\n"
        "movq (%2),%%mm1\n"
        "movq (%3),%%mm2\n"
        "movq %4,%%mm3\n"
        "paddb %%mm1,%%mm0\n"
        "paddb %%mm2,%%mm0\n"
        "paddb %%mm3,%%mm0\n"
        "movq %0,%%rbx\n"
        "movq %%mm0,(%%rbx)\n"
        "emms"
        :
        :"r"(uu),"r"(r_tmp),"r"(g_tmp),"r"(b_tmp),"r"(half) 
    );
    //V
    for(int i=0;i<=7;++i)
	{
		r_tmp[i]=rr[i]*0.5;
		g_tmp[i]=gg[i]*-0.4187;
		b_tmp[i]=bb[i]*-0.0813;
	}
    __asm__(
        "movq (%1),%%mm0\n"
        "movq (%2),%%mm1\n"
        "movq (%3),%%mm2\n"
        "movq %4,%%mm3\n"
        "paddb %%mm1,%%mm0\n"
        "paddb %%mm2,%%mm0\n"
        "paddb %%mm3,%%mm0\n"
        "movq %0,%%rbx\n"
        "movq %%mm0,(%%rbx)\n"
        "emms"
        :
        :"r"(vv),"r"(r_tmp),"r"(g_tmp),"r"(b_tmp),"r"(half) 
    );

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

    start = clock();
    fp_out = fopen("part2-MMX2.yuv","wb");
	for( ; num < 86; alpha -=3 )
	{
		num++;
		//printf("num=%d\n", num);
		if(fp_out==NULL)
		{
			printf("Error: open file failed\n");
			return 0;
		}
		
		BYTE yy[8],uu[8],vv[8];
		BYTE R[Height*Width],G[Height*Width],B[Height*Width];

		for(int pixel=0;pixel<Height*Width; pixel += 8)
		{

			int idx;
			// get 8 Y, 8 U, 8 V 
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
			
			BYTE buffer_tu[8],buffer_tv[8];
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
	//printf("start=%lu stop=%lu\n stop-start=%lf\n", start,stop,1.0*(stop-start));
	duration = ((stop - start))*1000/ CLOCKS_PER_SEC;
	printf("total time= %d ms\n", duration);
	return 0;
}
