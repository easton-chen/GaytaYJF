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
FILE* fp_dem1=NULL,*fp_dem2=NULL,*fp_out=NULL;
clock_t start, stop;
int duration;
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
    for (int i = 0; i < 8; ++i)
    {
        ratio_v[i] = 1.402;
    }

    for(int i=0;i<32;i+=8)
    {
    	for (int j = 0; j < 8; ++j)
    	{
    		v_f[j] = (char)vv[i+j];
    	}
    	__asm__(
    		"vmovups (%0),%%ymm0\n"
    		"vmovups (%1),%%ymm1\n"
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
        "vpaddb %%ymm1,%%ymm0,%%ymm0\n"
        //"movq %0,%%rbx\n"        
        //"vmovups %%ymm0,(%%rbx)\n"
        "vmovups %%ymm0,(%0)\n"
        :
        :"r"(rr),"r"(yy),"r"(v_tmp)
    );
    

    //G     G= Y - 0.34413 * (U-128) - 0.71414 * (V-128);
    for (int i = 0; i < 8; ++i)
    {
        ratio_u[i] = -0.34413;
        ratio_v[i] = -0.71414;
    }

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
    		"vmulps %%ymm1,%%ymm0,%%ymm0\n"
    		"vmovups %%ymm0,(%0)\n"
			"vmovups (%2),%%ymm0\n"
    		"vmovups (%3),%%ymm1\n"
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
        "vpaddb %%ymm1,%%ymm0,%%ymm0\n"
        "vpaddb %%ymm2,%%ymm0,%%ymm0\n"
        //"movq %0,%%rbx\n"
        //"vmovups %%ymm0,(%%rbx)\n"
        "vmovups %%ymm0,(%0)\n"
        :
        :"r"(gg),"r"(yy),"r"(v_tmp),"r"(u_tmp)
    );
    
    //B     B= Y + 1.772 * (U-128);
    for (int i = 0; i < 8; ++i)
    {
        ratio_u[i] = 1.772;
    }

    for(int i=0;i<32;i+=8)
    {
    	for (int j = 0; j < 8; ++j)
    	{
    		u_f[j] = (char)uu[i+j];
    	}
    	__asm__(
    		"vmovups (%0),%%ymm0\n"
    		"vmovups (%1),%%ymm1\n"
    		"vmulps %%ymm1,%%ymm0,%%ymm0\n"
    		//"vmovups %%ymm0,%0"
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
        "vpaddb %%ymm1,%%ymm0,%%ymm0\n"
        //"movq %0,%%rbx\n"
        //"vmovups %%ymm0,(%%rbx)\n"
        "vmovups %%ymm0,(%0)\n"
        :
        :"r"(bb),"r"(yy),"r"(u_tmp)
    );
}

void RGB2YUV(BYTE * yy,BYTE* uu,BYTE* vv,BYTE* rr,BYTE* gg,BYTE* bb)
{
/*	Y = 0.299 * R + 0.587 * G + 0.114 * B;
	U = - 0.3287 * R - 0.3313 * G + 0.5 * B + 128;
	V = 0.5 * R - 0.4187 * G - 0.0813 * B + 128;
*/
   	char half[32];
    for (int i = 0; i < 32; ++i)
    {
    	half[i] = 0x80;
    }
    BYTE r_tmp[32],g_tmp[32],b_tmp[32];
    float r_f[32],g_f[32],b_f[32];
    
    float ratio_r[8],ratio_g[8],ratio_b[8];


    //Y   Y = 0.299 * R + 0.587 * G + 0.114 * B;
    for (int i = 0; i < 8; ++i)
    {
        ratio_r[i] = 0.299;
        ratio_g[i] = 0.587;
        ratio_b[i] = 0.114;
    }
    
    for (int j = 0; j < 32; ++j)
    {
        r_f[j] = rr[j];
        g_f[j] = gg[j];
        b_f[j] = bb[j];
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
        "vpaddb %%ymm1,%%ymm0,%%ymm0\n"
        "vpaddb %%ymm2,%%ymm0,%%ymm0\n"
        //"movq %0,%%rbx\n"
        //"vmovups %%ymm0,(%%rbx)\n"
        "vmovups %%ymm0,(%0)\n"
        //:"=m"(yy)
        :
        :"r"(yy),"r"(r_tmp),"r"(g_tmp),"r"(b_tmp) 
    );

    //U   U = - 0.1687 * R - 0.3313 * G + 0.5 * B + 128;
    for (int i = 0; i < 8; ++i)
    {
        ratio_r[i] = -0.1687;
        ratio_g[i] = -0.3313;
        ratio_b[i] = 0.5;
    }
    
    for (int j = 0; j < 32; ++j)
    {
        r_f[j] = rr[j];
        g_f[j] = gg[j];
        b_f[j] = bb[j];
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
        "vpaddb %%ymm1,%%ymm0,%%ymm0\n"
        "vpaddb %%ymm2,%%ymm0,%%ymm0\n"
        "vpaddb %%ymm3,%%ymm0,%%ymm0\n"
        //"movq %0,%%rbx\n"
        //"vmovups %%ymm0,(%%rbx)\n"
        "vmovups %%ymm0,(%0)\n"
        //:"=m"(yy)
        :
        :"r"(uu),"r"(r_tmp),"r"(g_tmp),"r"(b_tmp) ,"r"(half) 
    );


    //V  V = 0.5 * R - 0.4187 * G - 0.0813 * B + 128;
    for (int i = 0; i < 8; ++i)
    {
        ratio_r[i] = 0.5;
        ratio_g[i] = -0.4187;
        ratio_b[i] = -0.0813;
    }
    
    for (int j = 0; j < 32; ++j)
    {
        r_f[j] = rr[j];
        g_f[j] = gg[j];
        b_f[j] = bb[j];
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
        "vpaddb %%ymm1,%%ymm0,%%ymm0\n"
        "vpaddb %%ymm2,%%ymm0,%%ymm0\n"
        "vpaddb %%ymm3,%%ymm0,%%ymm0\n"
        //"movq %0,%%rbx\n"
        //"vmovups %%ymm0,(%%rbx)\n"
        "vmovups %%ymm0,(%0)\n"
        //:"=m"(yy)
        :
        :"r"(vv),"r"(r_tmp),"r"(g_tmp),"r"(b_tmp) ,"r"(half) 
    );

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
    int num=0,idx;
    //unsigned char Y,U,V,R,G,B,Y1,Y2,U1,U2,V1,V2,R1,R2,G1,G2,B1,B2;

    BYTE yy1[32],uu1[32],vv1[32];
    BYTE yy2[32],uu2[32],vv2[32];
    
    BYTE R1[32],G1[32],B1[32];
    BYTE R2[32],G2[32],B2[32];

    BYTE R[32],G[32],B[32];
    
    BYTE buffer_tu[32],buffer_tv[32];

    float a1[8],base[8],a2[8];
    float R1_f[32],G1_f[32],B1_f[32];
    float R2_f[32],G2_f[32],B2_f[32];
    start = clock();
    fp_out = fopen("part3-AVX.yuv","wb");
	for( ; num < 86; alpha -=3 )
	{
		num++;
		if(fp_out==NULL)
		{
			printf("Error: open file failed\n");
			return 0;
		}

		for(int pixel=0;pixel<Height*Width; pixel += 32)
		{

			int idx;
			// get 8 Y, 8 U, 8 V 
			for(int i=0;i<32;++i)
			{
				yy1[i]=buffer_y_dem1[pixel+i];
                yy2[i]=buffer_y_dem2[pixel+i];
				idx=((pixel+i)/(2*Width))*Width/2 +(pixel+i-((pixel+i)/Width)*Width)/2;
				uu1[i]=buffer_u_dem1[idx];
                uu2[i]=buffer_u_dem2[idx];
				vv1[i]=buffer_v_dem1[idx];
                vv2[i]=buffer_v_dem2[idx];
			}
			
			//YUV2RGB

			YUV2RGB(yy1,uu1,vv1,R1,G1,B1);
            YUV2RGB(yy2,uu2,vv2,R2,G2,B2);
            
            /*
			for (int i = 0; i < 32; ++i)
            {
                R[i] = ( R1[i] * alpha + R2[i] * (256-alpha) )/256;
                G[i] = ( G1[i] * alpha + G2[i] * (256-alpha) )/256;
                B[i] = ( B1[i] * alpha + B2[i] * (256-alpha) )/256;
            }*/
            
            for (int i = 0; i < 8; ++i)
            {
                a1[i]=alpha;
                a2[i]=256-alpha;
                base[i]=256;
            }
            for (int i = 0; i < 32; ++i)
            {
                R1_f[i] = R1[i];
                R2_f[i] = R2[i];
                G1_f[i] = G1[i];
                G2_f[i] = G2[i];
                B1_f[i] = B1[i];
                B2_f[i] = B2[i];
            }
            __asm__(
                "vmovups (%6),%%ymm2\n"  //a1
                "vmovups (%7),%%ymm3\n"  //a2
                "vmovups (%8),%%ymm4\n"  //256
                "vmovups (%0),%%ymm0\n"
                "vmovups (%3),%%ymm1\n"
                "vmulps %%ymm2,%%ymm0,%%ymm0\n"
                "vmulps %%ymm3,%%ymm1,%%ymm1\n"
                "vaddps %%ymm1,%%ymm0,%%ymm0\n"
                "vdivps %%ymm4,%%ymm0,%%ymm0\n"
                "vmovups %%ymm0,(%0)\n"
                "vmovups (%1),%%ymm0\n"
                "vmovups (%4),%%ymm1\n"
                "vmulps %%ymm2,%%ymm0,%%ymm0\n"
                "vmulps %%ymm3,%%ymm1,%%ymm1\n"
                "vaddps %%ymm1,%%ymm0,%%ymm0\n"
                "vdivps %%ymm4,%%ymm0,%%ymm0\n"
                "vmovups %%ymm0,(%1)\n"
                "vmovups (%2),%%ymm0\n"
                "vmovups (%5),%%ymm1\n"
                "vmulps %%ymm2,%%ymm0,%%ymm0\n"
                "vmulps %%ymm3,%%ymm1,%%ymm1\n"
                "vaddps %%ymm1,%%ymm0,%%ymm0\n"
                "vdivps %%ymm4,%%ymm0,%%ymm0\n"
                "vmovups %%ymm0,(%2)\n"

                "vmovups 32(%0),%%ymm0\n"
                "vmovups 32(%3),%%ymm1\n"
                "vmulps %%ymm2,%%ymm0,%%ymm0\n"
                "vmulps %%ymm3,%%ymm1,%%ymm1\n"
                "vaddps %%ymm1,%%ymm0,%%ymm0\n"
                "vdivps %%ymm4,%%ymm0,%%ymm0\n"
                "vmovups %%ymm0,32(%0)\n"
                "vmovups 32(%1),%%ymm0\n"
                "vmovups 32(%4),%%ymm1\n"
                "vmulps %%ymm2,%%ymm0,%%ymm0\n"
                "vmulps %%ymm3,%%ymm1,%%ymm1\n"
                "vaddps %%ymm1,%%ymm0,%%ymm0\n"
                "vdivps %%ymm4,%%ymm0,%%ymm0\n"
                "vmovups %%ymm0,32(%1)\n"
                "vmovups 32(%2),%%ymm0\n"
                "vmovups 32(%5),%%ymm1\n"
                "vmulps %%ymm2,%%ymm0,%%ymm0\n"
                "vmulps %%ymm3,%%ymm1,%%ymm1\n"
                "vaddps %%ymm1,%%ymm0,%%ymm0\n"
                "vdivps %%ymm4,%%ymm0,%%ymm0\n"
                "vmovups %%ymm0,32(%2)\n"

                "vmovups 64(%0),%%ymm0\n"
                "vmovups 64(%3),%%ymm1\n"
                "vmulps %%ymm2,%%ymm0,%%ymm0\n"
                "vmulps %%ymm3,%%ymm1,%%ymm1\n"
                "vaddps %%ymm1,%%ymm0,%%ymm0\n"
                "vdivps %%ymm4,%%ymm0,%%ymm0\n"
                "vmovups %%ymm0,64(%0)\n"
                "vmovups 64(%1),%%ymm0\n"
                "vmovups 64(%4),%%ymm1\n"
                "vmulps %%ymm2,%%ymm0,%%ymm0\n"
                "vmulps %%ymm3,%%ymm1,%%ymm1\n"
                "vaddps %%ymm1,%%ymm0,%%ymm0\n"
                "vdivps %%ymm4,%%ymm0,%%ymm0\n"
                "vmovups %%ymm0,64(%1)\n"
                "vmovups 64(%2),%%ymm0\n"
                "vmovups 64(%5),%%ymm1\n"
                "vmulps %%ymm2,%%ymm0,%%ymm0\n"
                "vmulps %%ymm3,%%ymm1,%%ymm1\n"
                "vaddps %%ymm1,%%ymm0,%%ymm0\n"
                "vdivps %%ymm4,%%ymm0,%%ymm0\n"
                "vmovups %%ymm0,64(%2)\n"

                "vmovups 96(%0),%%ymm0\n"
                "vmovups 96(%3),%%ymm1\n"
                "vmulps %%ymm2,%%ymm0,%%ymm0\n"
                "vmulps %%ymm3,%%ymm1,%%ymm1\n"
                "vaddps %%ymm1,%%ymm0,%%ymm0\n"
                "vdivps %%ymm4,%%ymm0,%%ymm0\n"
                "vmovups %%ymm0,96(%0)\n"
                "vmovups 96(%1),%%ymm0\n"
                "vmovups 96(%4),%%ymm1\n"
                "vmulps %%ymm2,%%ymm0,%%ymm0\n"
                "vmulps %%ymm3,%%ymm1,%%ymm1\n"
                "vaddps %%ymm1,%%ymm0,%%ymm0\n"
                "vdivps %%ymm4,%%ymm0,%%ymm0\n"
                "vmovups %%ymm0,96(%1)\n"
                "vmovups 96(%2),%%ymm0\n"
                "vmovups 96(%5),%%ymm1\n"
                "vmulps %%ymm2,%%ymm0,%%ymm0\n"
                "vmulps %%ymm3,%%ymm1,%%ymm1\n"
                "vaddps %%ymm1,%%ymm0,%%ymm0\n"
                "vdivps %%ymm4,%%ymm0,%%ymm0\n"
                "vmovups %%ymm0,96(%2)\n"
                :
                :"r"(R1_f),"r"(G1_f),"r"(B1_f),"r"(R2_f),"r"(G2_f),"r"(B2_f),"r"(a1),"r"(a2),"r"(base)
            );
			for (int i = 0; i < 32; ++i)
            {
                R1[i] = R1_f[i];
                G1[i] = G1_f[i];
                B1[i] = B1_f[i];
                R2[i] = R2_f[i];
                G2[i] = G2_f[i];
                B2[i] = B2_f[i];
            }
            
			BYTE buffer_tu[32],buffer_tv[32];
			RGB2YUV(buffer_wy+pixel,buffer_tu,buffer_tv,R1,G1,B1);
			
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
	duration = (stop - start)*1000/ CLOCKS_PER_SEC;
	printf("total time= %d ms\n", duration);
	return 0;
}
