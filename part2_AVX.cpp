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

int cnt=0;
void YUV2RGB(BYTE * yy,BYTE* uu,BYTE* vv,BYTE* rr,BYTE* gg,BYTE* bb)
{
/*	R= Y + 1.402 * (V-128) ;
	G= Y - 0.34413 * (U-128) - 0.71414 * (V-128);
	B= Y + 1.772 * (U-128);*/

    char half[32];
    for (int i = 0; i < 32; ++i)
    {
    	half[i] = 0x80;
    }
	__asm__(
        "vmovups (%0),%%ymm1\n"
        "vmovups (%1),%%ymm2\n"
        "vmovups (%2),%%ymm3\n"
        //"vpsubb %%ymm3,%%ymm2\n"
        //"vpsubb %%ymm3,%%ymm1\n"
        "vpsubb %%ymm3,%%ymm2,%%ymm2\n"
        "vpsubb %%ymm3,%%ymm1,%%ymm1\n"
        "vmovups %%ymm1,(%0)\n"
        "vmovups %%ymm2,(%1)\n"
        :
        :"r"(uu),"r"(vv),"r"(half)
    );
    
    char v_tmp[32],u_tmp[32];
    float ratio_v[8],ratio_u[8];
    float v_f[8],u_f[8];
    //R

	ratio_v[0] = ratio_v[1] = ratio_v[2] = ratio_v[3] = 1.402;
    ratio_v[4] = ratio_v[5] = ratio_v[6] = ratio_v[7] = 1.402;
    for(int i=0;i<32;i+=8)
    {
    	for (int j = 0; j < 8; ++j)
    	{
    		v_f[j] = (char)vv[i+j];
    	}
    	__asm__(
    		"vmovups (%0),%%ymm0\n"
    		"vmovups (%1),%%ymm1\n"
    		//"vmulps %%ymm1,%%ymm0\n"
    		"vmulps %%ymm1,%%ymm0,%%ymm0\n"
            "vmovups %%ymm0,(%0)"
    		:
            :"r"(v_f),"r"(ratio_v)
    	);
    	for (int j = 0; j < 8; ++j)
    	{
        	v_tmp[i+j]=v_f[j];
    	}
    }
    __asm__(
        "vmovups (%1),%%ymm0\n"
        "vmovups (%2),%%ymm1\n"
        //"vpaddb %%ymm1,%%ymm0\n"
        "vpaddb %%ymm1,%%ymm0,%%ymm0\n"
        //"movq %0,%%rbx\n"        
        //"movups %%ymm0,(%%rbx)\n"
        "vmovups %%ymm0,(%0)\n"
        :
        :"r"(rr),"r"(yy),"r"(v_tmp)
    );
    
    //G     G= Y - 0.34413 * (U-128) - 0.71414 * (V-128);
   	ratio_u[0] = ratio_u[1] = ratio_u[2] = ratio_u[3] = -0.34413;
    ratio_u[4] = ratio_u[5] = ratio_u[6] = ratio_u[7] = -0.34413;
   	ratio_v[0] = ratio_v[1] = ratio_v[2] = ratio_v[3] = -0.71414;
    ratio_v[4] = ratio_v[5] = ratio_v[6] = ratio_v[7] = -0.71414;
    for(int i=0;i<32;i+=8)
    {
    	for (int j = 0; j < 8; ++j)
    	{
    		v_f[j] = (char)vv[i+j];
    		u_f[j] = (char)uu[i+j];
    	}
    	__asm__(
    		"vmovups (%0),%%ymm0\n"
    		"vmovups (%1),%%ymm1\n"
    		//"vmulps %%ymm1,%%ymm0\n"
    		"vmulps %%ymm1,%%ymm0,%%ymm0\n"
            "vmovups %%ymm0,(%0)\n"			
            "vmovups (%2),%%ymm0\n"
    		"vmovups (%3),%%ymm1\n"
    		//"vmulps %%ymm1,%%ymm0\n"
    		"vmulps %%ymm1,%%ymm0,%%ymm0\n"
            "vmovups %%ymm0,(%2)\n"
    		:
            :"r"(v_f),"r"(ratio_v),"r"(u_f),"r"(ratio_u)
    	);
    	for (int j = 0; j < 8; ++j)
    	{
        	v_tmp[i+j] = v_f[j];
        	u_tmp[i+j] = u_f[j];
    	}
    }
    __asm__(
        "vmovups (%1),%%ymm0\n"
        "vmovups (%2),%%ymm1\n"
        "vmovups (%3),%%ymm2\n"
        //"vpaddb %%ymm1,%%ymm0\n"
        //"vpaddb %%ymm2,%%ymm0\n"
        "vpaddb %%ymm1,%%ymm0,%%ymm0\n"
        "vpaddb %%ymm2,%%ymm0,%%ymm0\n"
        //"movq %0,%%rbx\n"
        //"movups %%ymm0,(%%rbx)\n"
        "vmovups %%ymm0,(%0)\n"
        :
        :"r"(gg),"r"(yy),"r"(v_tmp),"r"(u_tmp)
    );
    
    //B     B= Y + 1.772 * (U-128);
    ratio_u[0] = ratio_u[1] = ratio_u[2] = ratio_u[3] = 1.772;
    ratio_u[4] = ratio_u[5] = ratio_u[6] = ratio_u[7] = 1.772;
    for(int i=0;i<32;i+=8)
    {
    	for (int j = 0; j < 8; ++j)
    	{
    		u_f[j] = (char)uu[i+j];
    	}
    	__asm__(
    		"vmovups (%0),%%ymm0\n"
    		"vmovups (%1),%%ymm1\n"
    		//"vmulps %%ymm1,%%ymm0\n"
    		"vmulps %%ymm1,%%ymm0,%%ymm0\n"
            //"movups %%ymm0,%0"
            "vmovups %%ymm0,(%0)\n"
    		:
            :"r"(u_f),"r"(ratio_u)
    	);
    	for (int j = 0; j < 8; ++j)
    	{
        	u_tmp[i+j]=u_f[j];
    	}
    }
    __asm__(
        "vmovups (%1),%%ymm0\n"
        "vmovups (%2),%%ymm1\n"
        //"vpaddb %%ymm1,%%ymm0\n"
        "vpaddb %%ymm1,%%ymm0,%%ymm0\n"
        //"movq %0,%%rbx\n"
        //"movups %%ymm0,(%%rbx)\n"
        "vmovups %%ymm0,(%0)\n"
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
   	char half[32];
    for (int i = 0; i < 32; ++i)
    {
    	half[i] = 0x80;
    }
    BYTE r_tmp[32],g_tmp[32],b_tmp[32];
    float r_f[32],g_f[32],b_f[32];
    float rr_f[32],gg_f[32],bb_f[32];
    float ratio_r[8],ratio_g[8],ratio_b[8];
    //alpha
    float a[8],base[8];
    a[0]=a[1]=a[2]=a[3]=a[4]=a[5]=a[6]=a[7]=alpha;
    base[0]=base[1]=base[2]=base[3]=base[4]=base[5]=base[6]=base[7]=256;
    for (int j = 0; j < 32; ++j)
    {
        rr_f[j] = rr[j];
        gg_f[j] = gg[j];
        bb_f[j] = bb[j];
    }
 
    __asm__(
        "vmovups (%3),%%ymm1\n"
        "vmovups (%4),%%ymm2\n"
        "vmovups (%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vdivps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,(%0)\n"
        "vmovups 32(%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vdivps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,32(%0)\n"
        "vmovups 64(%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vdivps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,64(%0)\n"
        "vmovups 96(%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vdivps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,96(%0)\n"
        
        "vmovups (%1),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vdivps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,(%1)\n"
        "vmovups 32(%1),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vdivps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,32(%1)\n"
        "vmovups 64(%1),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vdivps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,64(%1)\n"
        "vmovups 96(%1),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vdivps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,96(%1)\n"
        
        "vmovups (%2),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vdivps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,(%2)\n"
        "vmovups 32(%2),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vdivps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,32(%2)\n"
        "vmovups 64(%2),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vdivps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,64(%2)\n"
        "vmovups 96(%2),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vdivps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,96(%2)\n"
        :
        :"r"(rr_f),"r"(gg_f),"r"(bb_f),"r"(a),"r"(base)
    );


    //Y   Y = 0.299 * R + 0.587 * G + 0.114 * B;
    for(int i=0;i<8;++i)
    {
        ratio_r[i]=0.299;
        ratio_g[i]=0.587;
        ratio_b[i]=0.114;
    }

    for (int j = 0; j < 32; ++j)
    {
        r_f[j] = rr_f[j];
        g_f[j] = gg_f[j];
        b_f[j] = bb_f[j];
    }
    
    __asm__(
        "vmovups 0(%0),%%ymm0\n"
        "vmovups (%1),%%ymm1\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,0(%0)\n"
        "vmovups 0(%2),%%ymm0\n"
        "vmovups (%3),%%ymm2\n"
        "vmulps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,0(%2)\n"
        "vmovups 0(%4),%%ymm0\n"
        "vmovups (%5),%%ymm3\n"
        "vmulps %%ymm3,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,0(%4)\n"

        "vmovups 32(%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,32(%0)\n"
        "vmovups 32(%2),%%ymm0\n"
        "vmulps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,32(%2)\n"
        "vmovups 32(%4),%%ymm0\n"
        "vmulps %%ymm3,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,32(%4)\n"

        "vmovups 64(%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,64(%0)\n"
        "vmovups 64(%2),%%ymm0\n"
        "vmulps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,64(%2)\n"
        "vmovups 64(%4),%%ymm0\n"
        "vmulps %%ymm3,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,64(%4)\n"

        "vmovups 96(%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,96(%0)\n"
        "vmovups 96(%2),%%ymm0\n"
        "vmulps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,96(%2)\n"
        "vmovups 96(%4),%%ymm0\n"
        "vmulps %%ymm3,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,96(%4)\n"
        :
        :"r"(r_f),"r"(ratio_r),"r"(g_f),"r"(ratio_g),"r"(b_f),"r"(ratio_b)
    );
    for (int j = 0; j < 32; ++j)
    {
        r_tmp[j] = r_f[j];
        g_tmp[j] = g_f[j];
        b_tmp[j] = b_f[j];
    }
	
    __asm__(
        "vmovups (%1),%%ymm0\n"
        "vmovups (%2),%%ymm1\n"
        "vmovups (%3),%%ymm2\n"
        //"vpaddb %%ymm1,%%ymm0\n"
        //"vpaddb %%ymm2,%%ymm0\n"
        "vpaddb %%ymm1,%%ymm0,%%ymm0\n"
        "vpaddb %%ymm2,%%ymm0,%%ymm0\n"
        //"movq %0,%%rbx\n"
        //"movups %%ymm0,(%%rbx)\n"
        "vmovups %%ymm0,(%0)\n"
        //:"=m"(yy)
        :
        :"r"(yy),"r"(r_tmp),"r"(g_tmp),"r"(b_tmp) 
    );

    //U   U = - 0.1687 * R - 0.3313 * G + 0.5 * B + 128;
    for(int i=0;i<8;++i)
    {
        ratio_r[i]=-0.1687;
        ratio_g[i]=-0.3313;
        ratio_b[i]=0.5;
    }
    
    for (int j = 0; j < 32; ++j)
    {
        r_f[j] = rr_f[j];
        g_f[j] = gg_f[j];
        b_f[j] = bb_f[j];
    }
    
    __asm__(
        "vmovups 0(%0),%%ymm0\n"
        "vmovups (%1),%%ymm1\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,0(%0)\n"
        "vmovups 0(%2),%%ymm0\n"
        "vmovups (%3),%%ymm2\n"
        "vmulps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,0(%2)\n"
        "vmovups 0(%4),%%ymm0\n"
        "vmovups (%5),%%ymm3\n"
        "vmulps %%ymm3,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,0(%4)\n"

        "vmovups 32(%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,32(%0)\n"
        "vmovups 32(%2),%%ymm0\n"
        "vmulps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,32(%2)\n"
        "vmovups 32(%4),%%ymm0\n"
        "vmulps %%ymm3,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,32(%4)\n"

        "vmovups 64(%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,64(%0)\n"
        "vmovups 64(%2),%%ymm0\n"
        "vmulps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,64(%2)\n"
        "vmovups 64(%4),%%ymm0\n"
        "vmulps %%ymm3,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,64(%4)\n"

        "vmovups 96(%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,96(%0)\n"
        "vmovups 96(%2),%%ymm0\n"
        "vmulps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,96(%2)\n"
        "vmovups 96(%4),%%ymm0\n"
        "vmulps %%ymm3,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,96(%4)\n"
        :
        :"r"(r_f),"r"(ratio_r),"r"(g_f),"r"(ratio_g),"r"(b_f),"r"(ratio_b)
    );
    for (int j = 0; j < 32; ++j)
    {
        r_tmp[j] = r_f[j];
        g_tmp[j] = g_f[j];
        b_tmp[j] = b_f[j];
    }
    
    __asm__(
        "vmovups (%1),%%ymm0\n"
        "vmovups (%2),%%ymm1\n"
        "vmovups (%3),%%ymm2\n"
        "vmovups (%4),%%ymm3\n"
        //"vpaddb %%ymm1,%%ymm0\n"
        //"vpaddb %%ymm2,%%ymm0\n"
        //"vpaddb %%ymm3,%%ymm0\n"
        "vpaddb %%ymm1,%%ymm0,%%ymm0\n"
        "vpaddb %%ymm2,%%ymm0,%%ymm0\n"
        "vpaddb %%ymm3,%%ymm0,%%ymm0\n"
        //"movq %0,%%rbx\n"
        //"movups %%ymm0,(%%rbx)\n"
        "vmovups %%ymm0,(%0)\n"
        //:"=m"(yy)
        :
        :"r"(uu),"r"(r_tmp),"r"(g_tmp),"r"(b_tmp) ,"r"(half) 
    );


    //V  V = 0.5 * R - 0.4187 * G - 0.0813 * B + 128;
    for(int i=0;i<8;++i)
    {
        ratio_r[i]=0.5;
        ratio_g[i]=-0.4187;
        ratio_b[i]=-0.0813;
    }    

    
    for (int j = 0; j < 32; ++j)
    {
        r_f[j] = rr_f[j];
        g_f[j] = gg_f[j];
        b_f[j] = bb_f[j];
    }
    
    __asm__(
        "vmovups 0(%0),%%ymm0\n"
        "vmovups (%1),%%ymm1\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,0(%0)\n"
        "vmovups 0(%2),%%ymm0\n"
        "vmovups (%3),%%ymm2\n"
        "vmulps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,0(%2)\n"
        "vmovups 0(%4),%%ymm0\n"
        "vmovups (%5),%%ymm3\n"
        "vmulps %%ymm3,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,0(%4)\n"

        "vmovups 32(%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,32(%0)\n"
        "vmovups 32(%2),%%ymm0\n"
        "vmulps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,32(%2)\n"
        "vmovups 32(%4),%%ymm0\n"
        "vmulps %%ymm3,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,32(%4)\n"

        "vmovups 64(%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,64(%0)\n"
        "vmovups 64(%2),%%ymm0\n"
        "vmulps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,64(%2)\n"
        "vmovups 64(%4),%%ymm0\n"
        "vmulps %%ymm3,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,64(%4)\n"

        "vmovups 96(%0),%%ymm0\n"
        "vmulps %%ymm1,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,96(%0)\n"
        "vmovups 96(%2),%%ymm0\n"
        "vmulps %%ymm2,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,96(%2)\n"
        "vmovups 96(%4),%%ymm0\n"
        "vmulps %%ymm3,%%ymm0,%%ymm0\n"
        "vmovups %%ymm0,96(%4)\n"
        :
        :"r"(r_f),"r"(ratio_r),"r"(g_f),"r"(ratio_g),"r"(b_f),"r"(ratio_b)
    );
    for (int j = 0; j < 32; ++j)
    {
        r_tmp[j] = r_f[j];
        g_tmp[j] = g_f[j];
        b_tmp[j] = b_f[j];
    }

    
    __asm__(
        "vmovups (%1),%%ymm0\n"
        "vmovups (%2),%%ymm1\n"
        "vmovups (%3),%%ymm2\n"
        "vmovups (%4),%%ymm3\n"
        //"vpaddb %%ymm1,%%ymm0\n"
        //"vpaddb %%ymm2,%%ymm0\n"
        //"vpaddb %%ymm3,%%ymm0\n"
        "vpaddb %%ymm1,%%ymm0,%%ymm0\n"
        "vpaddb %%ymm2,%%ymm0,%%ymm0\n"
        "vpaddb %%ymm3,%%ymm0,%%ymm0\n"
        //"movq %0,%%rbx\n"
        //"movups %%ymm0,(%%rbx)\n"
        "vmovups %%ymm0,(%0)\n"
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
	//unsigned char Y,U,V,R,G,B;
	string str;

    start = clock();
    fp_out = fopen("part2-AVX.yuv","wb");
	for( ; num < 86; alpha -=3 )
	{
		num++;
		if(fp_out==NULL)
		{
			printf("Error: open file failed\n");
			return 0;
		}
		
		BYTE yy[32],uu[32],vv[32];
		BYTE R[32],G[32],B[32];

		for(int pixel=0;pixel<Height*Width; pixel += 32)
		{

			int idx;
			// get 8 Y, 8 U, 8 V 
			for(int i=0;i<32;++i)
			{
				yy[i]=buffer_y[pixel+i];
				idx=((pixel+i)/(2*Width))*Width/2 +(pixel+i-((pixel+i)/Width)*Width)/2;
				uu[i]=buffer_u[idx];
				vv[i]=buffer_v[idx];
			}
			
			//YUV2RGB

			YUV2RGB(yy,uu,vv,R,G,B);

            /*
			for (int i = 0; i < 32; ++i)
			{
				R[pixel+i] = R[pixel+i]*alpha/256;
				G[pixel+i] = G[pixel+i]*alpha/256;
				B[pixel+i] = B[pixel+i]*alpha/256;
			}	*/
			
			BYTE buffer_tu[32],buffer_tv[32];
			RGB2YUV(buffer_wy+pixel,buffer_tu,buffer_tv,R,G,B);
			
			for (int i = 0; i < 32; i+=2)
			{
				buffer_wu[idx-i/2] = buffer_tu[32-i-1];	
				buffer_wv[idx-i/2] = buffer_tv[32-i-1];	
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
