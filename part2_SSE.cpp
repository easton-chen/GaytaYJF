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
unsigned char alpha;

typedef int LONG;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;


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

    char half[16];
    for (int i = 0; i < 16; ++i)
    {
    	half[i] = 0x80;
    }
	__asm__(
        "movups (%0),%%xmm1\n"
        "movups (%1),%%xmm2\n"
        "movups (%2),%%xmm3\n"
        "psubb %%xmm3,%%xmm2\n"
        "psubb %%xmm3,%%xmm1\n"
        "movups %%xmm1,(%0)\n"
        "movups %%xmm2,(%1)\n"
        :
        :"r"(uu),"r"(vv),"r"(half)
    );
    
    char v_tmp[16],u_tmp[16];
    float ratio_v[4],ratio_u[4];
    float v_f[4],u_f[4];
    //R

	ratio_v[0] = ratio_v[1] = ratio_v[2] = ratio_v[3] = 1.402;
    for(int i=0;i<16;i+=4)
    {
    	for (int j = 0; j < 4; ++j)
    	{
    		v_f[j] = (char)vv[i+j];
    	}
    	__asm__(
    		"movups (%0),%%xmm0\n"
    		"movups (%1),%%xmm1\n"
    		"mulps %%xmm1,%%xmm0\n"
    		"movups %%xmm0,(%0)"
    		:
            :"r"(v_f),"r"(ratio_v)
    	);
    	for (int j = 0; j < 4; ++j)
    	{
        	v_tmp[i+j]=v_f[j];
    	}
    }
    __asm__(
        "movups (%1),%%xmm0\n"
        "movups (%2),%%xmm1\n"
        "paddb %%xmm1,%%xmm0\n"
        //"movq %0,%%rbx\n"        
        //"movups %%xmm0,(%%rbx)\n"
        "movups %%xmm0,(%0)\n"
        :
        :"r"(rr),"r"(yy),"r"(v_tmp)
    );
    
    //G     G= Y - 0.34413 * (U-128) - 0.71414 * (V-128);
   	ratio_u[0] = ratio_u[1] = ratio_u[2] = ratio_u[3] = -0.34413;
   	ratio_v[0] = ratio_v[1] = ratio_v[2] = ratio_v[3] = -0.71414;
    for(int i=0;i<16;i+=4)
    {
    	for (int j = 0; j < 4; ++j)
    	{
    		v_f[j] = (char)vv[i+j];
    		u_f[j] = (char)uu[i+j];
    	}
    	__asm__(
    		"movups (%0),%%xmm0\n"
    		"movups (%1),%%xmm1\n"
    		"mulps %%xmm1,%%xmm0\n"
    		"movups %%xmm0,(%0)\n"
			"movups (%2),%%xmm0\n"
    		"movups (%3),%%xmm1\n"
    		"mulps %%xmm1,%%xmm0\n"
    		"movups %%xmm0,(%2)\n"
    		:
            :"r"(v_f),"r"(ratio_v),"r"(u_f),"r"(ratio_u)
    	);
    	for (int j = 0; j < 4; ++j)
    	{
        	v_tmp[i+j] = v_f[j];
        	u_tmp[i+j] = u_f[j];
    	}
    }
    __asm__(
        "movups (%1),%%xmm0\n"
        "movups (%2),%%xmm1\n"
        "movups (%3),%%xmm2\n"
        "paddb %%xmm1,%%xmm0\n"
        "paddb %%xmm2,%%xmm0\n"
        //"movq %0,%%rbx\n"
        //"movups %%xmm0,(%%rbx)\n"
        "movups %%xmm0,(%0)\n"
        :
        :"r"(gg),"r"(yy),"r"(v_tmp),"r"(u_tmp)
    );
    
    //B     B= Y + 1.772 * (U-128);
    ratio_u[0] = ratio_u[1] = ratio_u[2] = ratio_u[3] = 1.772;
    for(int i=0;i<16;i+=4)
    {
    	for (int j = 0; j < 4; ++j)
    	{
    		u_f[j] = (char)uu[i+j];
    	}
    	__asm__(
    		"movups (%0),%%xmm0\n"
    		"movups (%1),%%xmm1\n"
    		"mulps %%xmm1,%%xmm0\n"
    		//"movups %%xmm0,%0"
            "movups %%xmm0,(%0)\n"
    		:
            :"r"(u_f),"r"(ratio_u)
    	);
    	for (int j = 0; j < 4; ++j)
    	{
        	u_tmp[i+j]=u_f[j];
    	}
    }
    __asm__(
        "movups (%1),%%xmm0\n"
        "movups (%2),%%xmm1\n"
        "paddb %%xmm1,%%xmm0\n"
        //"movq %0,%%rbx\n"
        //"movups %%xmm0,(%%rbx)\n"
        "movups %%xmm0,(%0)\n"
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
   	char half[16];
    for (int i = 0; i < 16; ++i)
    {
    	half[i] = 0x80;
    }
    BYTE r_tmp[16],g_tmp[16],b_tmp[16];
    float r_f[16],g_f[16],b_f[16];
    float rr_f[16],gg_f[16],bb_f[16];
    float ratio_r[4],ratio_g[4],ratio_b[4];
    //alpha
    float a[4],base[4];
    a[0]=a[1]=a[2]=a[3]=alpha;
    base[0]=base[1]=base[2]=base[3]=256;
    for (int j = 0; j < 16; ++j)
    {
        rr_f[j] = rr[j];
        gg_f[j] = gg[j];
        bb_f[j] = bb[j];
    }
 
    __asm__(
        "movups (%3),%%xmm1\n"
        "movups (%4),%%xmm2\n"
        "movups (%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "divps %%xmm2,%%xmm0\n"
        "movups %%xmm0,(%0)\n"
        "movups 16(%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "divps %%xmm2,%%xmm0\n"
        "movups %%xmm0,16(%0)\n"
        "movups 32(%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "divps %%xmm2,%%xmm0\n"
        "movups %%xmm0,32(%0)\n"
        "movups 48(%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "divps %%xmm2,%%xmm0\n"
        "movups %%xmm0,48(%0)\n"
        
        "movups (%1),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "divps %%xmm2,%%xmm0\n"
        "movups %%xmm0,(%1)\n"
        "movups 16(%1),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "divps %%xmm2,%%xmm0\n"
        "movups %%xmm0,16(%1)\n"
        "movups 32(%1),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "divps %%xmm2,%%xmm0\n"
        "movups %%xmm0,32(%1)\n"
        "movups 48(%1),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "divps %%xmm2,%%xmm0\n"
        "movups %%xmm0,48(%1)\n"
        
        "movups (%2),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "divps %%xmm2,%%xmm0\n"
        "movups %%xmm0,(%2)\n"
        "movups 16(%2),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "divps %%xmm2,%%xmm0\n"
        "movups %%xmm0,16(%2)\n"
        "movups 32(%2),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "divps %%xmm2,%%xmm0\n"
        "movups %%xmm0,32(%2)\n"
        "movups 48(%2),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "divps %%xmm2,%%xmm0\n"
        "movups %%xmm0,48(%2)\n"
        :
        :"r"(rr_f),"r"(gg_f),"r"(bb_f),"r"(a),"r"(base)
    );
    


    //Y   Y = 0.299 * R + 0.587 * G + 0.114 * B;
    ratio_r[0] = ratio_r[1] = ratio_r[2] = ratio_r[3] = 0.299;
    ratio_g[0] = ratio_g[1] = ratio_g[2] = ratio_g[3] = 0.587;
    ratio_b[0] = ratio_b[1] = ratio_b[2] = ratio_b[3] = 0.114;
    for (int j = 0; j < 16; ++j)
    {
        r_f[j] = rr_f[j];
        g_f[j] = gg_f[j];
        b_f[j] = bb_f[j];
    }
    
	__asm__(
		"movups 0(%0),%%xmm0\n"
		"movups (%1),%%xmm1\n"
		"mulps %%xmm1,%%xmm0\n"
		"movups %%xmm0,0(%0)\n"
		"movups 0(%2),%%xmm0\n"
		"movups (%3),%%xmm2\n"
		"mulps %%xmm2,%%xmm0\n"
		"movups %%xmm0,0(%2)\n"
		"movups 0(%4),%%xmm0\n"
		"movups (%5),%%xmm3\n"
		"mulps %%xmm3,%%xmm0\n"
		"movups %%xmm0,0(%4)\n"

        "movups 16(%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "movups %%xmm0,16(%0)\n"
        "movups 16(%2),%%xmm0\n"
        "mulps %%xmm2,%%xmm0\n"
        "movups %%xmm0,16(%2)\n"
        "movups 16(%4),%%xmm0\n"
        "mulps %%xmm3,%%xmm0\n"
        "movups %%xmm0,16(%4)\n"

        "movups 32(%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "movups %%xmm0,32(%0)\n"
        "movups 32(%2),%%xmm0\n"
        "mulps %%xmm2,%%xmm0\n"
        "movups %%xmm0,32(%2)\n"
        "movups 32(%4),%%xmm0\n"
        "mulps %%xmm3,%%xmm0\n"
        "movups %%xmm0,32(%4)\n"

        "movups 48(%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "movups %%xmm0,48(%0)\n"
        "movups 48(%2),%%xmm0\n"
        "mulps %%xmm2,%%xmm0\n"
        "movups %%xmm0,48(%2)\n"
        "movups 48(%4),%%xmm0\n"
        "mulps %%xmm3,%%xmm0\n"
        "movups %%xmm0,48(%4)\n"
        :
		:"r"(r_f),"r"(ratio_r),"r"(g_f),"r"(ratio_g),"r"(b_f),"r"(ratio_b)
	);
    for (int j = 0; j < 16; ++j)
    {
        r_tmp[j] = r_f[j];
        g_tmp[j] = g_f[j];
        b_tmp[j] = b_f[j];
    }

	
    __asm__(
        "movups (%1),%%xmm0\n"
        "movups (%2),%%xmm1\n"
        "movups (%3),%%xmm2\n"
        "paddb %%xmm1,%%xmm0\n"
        "paddb %%xmm2,%%xmm0\n"
        //"movq %0,%%rbx\n"
        //"movups %%xmm0,(%%rbx)\n"
        "movups %%xmm0,(%0)\n"
        //:"=m"(yy)
        :
        :"r"(yy),"r"(r_tmp),"r"(g_tmp),"r"(b_tmp) 
    );

    //U   U = - 0.1687 * R - 0.3313 * G + 0.5 * B + 128;
    ratio_r[0] = ratio_r[1] = ratio_r[2] = ratio_r[3] = - 0.1687;
    ratio_g[0] = ratio_g[1] = ratio_g[2] = ratio_g[3] = - 0.3313;
    ratio_b[0] = ratio_b[1] = ratio_b[2] = ratio_b[3] = 0.5;
    for (int j = 0; j < 16; ++j)
    {
        r_f[j] = rr_f[j];
        g_f[j] = gg_f[j];
        b_f[j] = bb_f[j];
    }
    
    __asm__(
        "movups 0(%0),%%xmm0\n"
        "movups (%1),%%xmm1\n"
        "mulps %%xmm1,%%xmm0\n"
        "movups %%xmm0,0(%0)\n"
        "movups 0(%2),%%xmm0\n"
        "movups (%3),%%xmm2\n"
        "mulps %%xmm2,%%xmm0\n"
        "movups %%xmm0,0(%2)\n"
        "movups 0(%4),%%xmm0\n"
        "movups (%5),%%xmm3\n"
        "mulps %%xmm3,%%xmm0\n"
        "movups %%xmm0,0(%4)\n"

        "movups 16(%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "movups %%xmm0,16(%0)\n"
        "movups 16(%2),%%xmm0\n"
        "mulps %%xmm2,%%xmm0\n"
        "movups %%xmm0,16(%2)\n"
        "movups 16(%4),%%xmm0\n"
        "mulps %%xmm3,%%xmm0\n"
        "movups %%xmm0,16(%4)\n"

        "movups 32(%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "movups %%xmm0,32(%0)\n"
        "movups 32(%2),%%xmm0\n"
        "mulps %%xmm2,%%xmm0\n"
        "movups %%xmm0,32(%2)\n"
        "movups 32(%4),%%xmm0\n"
        "mulps %%xmm3,%%xmm0\n"
        "movups %%xmm0,32(%4)\n"

        "movups 48(%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "movups %%xmm0,48(%0)\n"
        "movups 48(%2),%%xmm0\n"
        "mulps %%xmm2,%%xmm0\n"
        "movups %%xmm0,48(%2)\n"
        "movups 48(%4),%%xmm0\n"
        "mulps %%xmm3,%%xmm0\n"
        "movups %%xmm0,48(%4)\n"
        :
        :"r"(r_f),"r"(ratio_r),"r"(g_f),"r"(ratio_g),"r"(b_f),"r"(ratio_b)
    );
    for (int j = 0; j < 16; ++j)
    {
        r_tmp[j] = r_f[j];
        g_tmp[j] = g_f[j];
        b_tmp[j] = b_f[j];
    }
    

    
    __asm__(
        "movups (%1),%%xmm0\n"
        "movups (%2),%%xmm1\n"
        "movups (%3),%%xmm2\n"
        "movups (%4),%%xmm3\n"
        "paddb %%xmm1,%%xmm0\n"
        "paddb %%xmm2,%%xmm0\n"
        "paddb %%xmm3,%%xmm0\n"
        //"movq %0,%%rbx\n"
        //"movups %%xmm0,(%%rbx)\n"
        "movups %%xmm0,(%0)\n"
        //:"=m"(yy)
        :
        :"r"(uu),"r"(r_tmp),"r"(g_tmp),"r"(b_tmp) ,"r"(half) 
    );


    //V  V = 0.5 * R - 0.4187 * G - 0.0813 * B + 128;
    ratio_r[0] = ratio_r[1] = ratio_r[2] = ratio_r[3] = 0.5;
    ratio_g[0] = ratio_g[1] = ratio_g[2] = ratio_g[3] = -0.4187;
    ratio_b[0] = ratio_b[1] = ratio_b[2] = ratio_b[3] = -0.0813 ;
    for (int j = 0; j < 16; ++j)
    {
        r_f[j] = rr_f[j];
        g_f[j] = gg_f[j];
        b_f[j] = bb_f[j];
    }
    
    __asm__(
        "movups 0(%0),%%xmm0\n"
        "movups (%1),%%xmm1\n"
        "mulps %%xmm1,%%xmm0\n"
        "movups %%xmm0,0(%0)\n"
        "movups 0(%2),%%xmm0\n"
        "movups (%3),%%xmm2\n"
        "mulps %%xmm2,%%xmm0\n"
        "movups %%xmm0,0(%2)\n"
        "movups 0(%4),%%xmm0\n"
        "movups (%5),%%xmm3\n"
        "mulps %%xmm3,%%xmm0\n"
        "movups %%xmm0,0(%4)\n"

        "movups 16(%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "movups %%xmm0,16(%0)\n"
        "movups 16(%2),%%xmm0\n"
        "mulps %%xmm2,%%xmm0\n"
        "movups %%xmm0,16(%2)\n"
        "movups 16(%4),%%xmm0\n"
        "mulps %%xmm3,%%xmm0\n"
        "movups %%xmm0,16(%4)\n"

        "movups 32(%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "movups %%xmm0,32(%0)\n"
        "movups 32(%2),%%xmm0\n"
        "mulps %%xmm2,%%xmm0\n"
        "movups %%xmm0,32(%2)\n"
        "movups 32(%4),%%xmm0\n"
        "mulps %%xmm3,%%xmm0\n"
        "movups %%xmm0,32(%4)\n"

        "movups 48(%0),%%xmm0\n"
        "mulps %%xmm1,%%xmm0\n"
        "movups %%xmm0,48(%0)\n"
        "movups 48(%2),%%xmm0\n"
        "mulps %%xmm2,%%xmm0\n"
        "movups %%xmm0,48(%2)\n"
        "movups 48(%4),%%xmm0\n"
        "mulps %%xmm3,%%xmm0\n"
        "movups %%xmm0,48(%4)\n"
        :
        :"r"(r_f),"r"(ratio_r),"r"(g_f),"r"(ratio_g),"r"(b_f),"r"(ratio_b)
    );
    for (int j = 0; j < 16; ++j)
    {
        r_tmp[j] = r_f[j];
        g_tmp[j] = g_f[j];
        b_tmp[j] = b_f[j];
    }
    

    
    __asm__(
        "movups (%1),%%xmm0\n"
        "movups (%2),%%xmm1\n"
        "movups (%3),%%xmm2\n"
        "movups (%4),%%xmm3\n"
        "paddb %%xmm1,%%xmm0\n"
        "paddb %%xmm2,%%xmm0\n"
        "paddb %%xmm3,%%xmm0\n"
        //"movq %0,%%rbx\n"
        //"movups %%xmm0,(%%rbx)\n"
        "movups %%xmm0,(%0)\n"
        //:"=m"(yy)
        :
        :"r"(vv),"r"(r_tmp),"r"(g_tmp),"r"(b_tmp) ,"r"(half) 
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

	alpha=255;
	int num=0;
	unsigned char Y,U,V,R,G,B;
	string str;

    start = clock();
    fp_out = fopen("part2-SSE.yuv","wb");
	for( ; num < 86; alpha -=3 )
	{
		num++;
		if(fp_out==NULL)
		{
			printf("Error: open file failed\n");
			return 0;
		}
		
		BYTE yy[16],uu[16],vv[16];
		BYTE R[16],G[16],B[16];

		for(int pixel=0;pixel<Height*Width; pixel += 16)
		{

			int idx;
			// get 8 Y, 8 U, 8 V 
			for(int i=0;i<16;++i)
			{
				yy[i]=buffer_y[pixel+i];
				idx=((pixel+i)/(2*Width))*Width/2 +(pixel+i-((pixel+i)/Width)*Width)/2;
				uu[i]=buffer_u[idx];
				vv[i]=buffer_v[idx];
			}
			
			//YUV2RGB

			YUV2RGB(yy,uu,vv,R,G,B);
            /*
			for (int i = 0; i < 16; ++i)
			{
				R[pixel+i] = R[pixel+i]*alpha/256;
				G[pixel+i] = G[pixel+i]*alpha/256;
				B[pixel+i] = B[pixel+i]*alpha/256;
			}	
			*/
			BYTE buffer_tu[16],buffer_tv[16];
			RGB2YUV(buffer_wy+pixel,buffer_tu,buffer_tv,R,G,B);
			
			for (int i = 0; i < 16; i+=2)
			{
				buffer_wu[idx-i/2] = buffer_tu[16-i-1];	
				buffer_wv[idx-i/2] = buffer_tv[16-i-1];	
			}
		}
		fwrite((BYTE*)buffer_wy,1,Height*Width,fp_out);
		fwrite((BYTE*)buffer_wu,1,Height*Width/4,fp_out);
		fwrite((BYTE*)buffer_wv,1,Height*Width/4,fp_out);
	}
	fclose(fp_out);
	stop = clock();
	//printf("start=%lu stop=%lu\n stop-start=%lf\n", start,stop,1.0*(stop-start));
	duration = ((double)(stop - start))*1000/ CLOCKS_PER_SEC;
	printf("total time= %f ms\n", duration);
	return 0;
}
