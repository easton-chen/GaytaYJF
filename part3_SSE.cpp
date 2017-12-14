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
    
    float ratio_r[4],ratio_g[4],ratio_b[4];


    //Y   Y = 0.299 * R + 0.587 * G + 0.114 * B;
    ratio_r[0] = ratio_r[1] = ratio_r[2] = ratio_r[3] = 0.299;
    ratio_g[0] = ratio_g[1] = ratio_g[2] = ratio_g[3] = 0.587;
    ratio_b[0] = ratio_b[1] = ratio_b[2] = ratio_b[3] = 0.114;
    for (int j = 0; j < 16; ++j)
    {
        r_f[j] = rr[j];
        g_f[j] = gg[j];
        b_f[j] = bb[j];
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
        r_f[j] = rr[j];
        g_f[j] = gg[j];
        b_f[j] = bb[j];
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
        r_f[j] = rr[j];
        g_f[j] = gg[j];
        b_f[j] = bb[j];
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

    BYTE yy1[16],uu1[16],vv1[16];
    BYTE yy2[16],uu2[16],vv2[16];
    
    BYTE R1[16],G1[16],B1[16];
    BYTE R2[16],G2[16],B2[16];

    BYTE R[16],G[16],B[16];
    
    BYTE buffer_tu[16],buffer_tv[16];

    float a1[4],base[4],a2[4];
    float R1_f[16],G1_f[16],B1_f[16];
    float R2_f[16],G2_f[16],B2_f[16];
    start = clock();
    fp_out = fopen("part3-SSE.yuv","wb");
	for( ; num < 86; alpha -=3 )
	{
		num++;
		if(fp_out==NULL)
		{
			printf("Error: open file failed\n");
			return 0;
		}

		for(int pixel=0;pixel<Height*Width; pixel += 16)
		{

			int idx;
			// get 8 Y, 8 U, 8 V 
			for(int i=0;i<16;++i)
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
			for (int i = 0; i < 16; ++i)
            {
                R[i] = ( R1[i] * alpha + R2[i] * (256-alpha) )/256;
                G[i] = ( G1[i] * alpha + G2[i] * (256-alpha) )/256;
                B[i] = ( B1[i] * alpha + B2[i] * (256-alpha) )/256;
            }*/
            
            a1[0]=a1[1]=a1[2]=a1[3]=alpha;
            a2[0]=a2[1]=a2[2]=a2[3]=256-alpha;
            base[0]=base[1]=base[2]=base[3]=256;
            for (int i = 0; i < 16; ++i)
            {
                R1_f[i] = R1[i];
                R2_f[i] = R2[i];
                G1_f[i] = G1[i];
                G2_f[i] = G2[i];
                B1_f[i] = B1[i];
                B2_f[i] = B2[i];
            }
            __asm__(
                "movups (%6),%%xmm2\n"  //a1
                "movups (%7),%%xmm3\n"  //a2
                "movups (%8),%%xmm4\n"  //256
                "movups (%0),%%xmm0\n"
                "movups (%3),%%xmm1\n"
                "mulps %%xmm2,%%xmm0\n"
                "mulps %%xmm3,%%xmm1\n"
                "addps %%xmm1,%%xmm0\n"
                "divps %%xmm4,%%xmm0\n"
                "movups %%xmm0,(%0)\n"
                "movups (%1),%%xmm0\n"
                "movups (%4),%%xmm1\n"
                "mulps %%xmm2,%%xmm0\n"
                "mulps %%xmm3,%%xmm1\n"
                "addps %%xmm1,%%xmm0\n"
                "divps %%xmm4,%%xmm0\n"
                "movups %%xmm0,(%1)\n"
                "movups (%2),%%xmm0\n"
                "movups (%5),%%xmm1\n"
                "mulps %%xmm2,%%xmm0\n"
                "mulps %%xmm3,%%xmm1\n"
                "addps %%xmm1,%%xmm0\n"
                "divps %%xmm4,%%xmm0\n"
                "movups %%xmm0,(%2)\n"

                "movups 16(%0),%%xmm0\n"
                "movups 16(%3),%%xmm1\n"
                "mulps %%xmm2,%%xmm0\n"
                "mulps %%xmm3,%%xmm1\n"
                "addps %%xmm1,%%xmm0\n"
                "divps %%xmm4,%%xmm0\n"
                "movups %%xmm0,16(%0)\n"
                "movups 16(%1),%%xmm0\n"
                "movups 16(%4),%%xmm1\n"
                "mulps %%xmm2,%%xmm0\n"
                "mulps %%xmm3,%%xmm1\n"
                "addps %%xmm1,%%xmm0\n"
                "divps %%xmm4,%%xmm0\n"
                "movups %%xmm0,16(%1)\n"
                "movups 16(%2),%%xmm0\n"
                "movups 16(%5),%%xmm1\n"
                "mulps %%xmm2,%%xmm0\n"
                "mulps %%xmm3,%%xmm1\n"
                "addps %%xmm1,%%xmm0\n"
                "divps %%xmm4,%%xmm0\n"
                "movups %%xmm0,16(%2)\n"

                "movups 32(%0),%%xmm0\n"
                "movups 32(%3),%%xmm1\n"
                "mulps %%xmm2,%%xmm0\n"
                "mulps %%xmm3,%%xmm1\n"
                "addps %%xmm1,%%xmm0\n"
                "divps %%xmm4,%%xmm0\n"
                "movups %%xmm0,32(%0)\n"
                "movups 32(%1),%%xmm0\n"
                "movups 32(%4),%%xmm1\n"
                "mulps %%xmm2,%%xmm0\n"
                "mulps %%xmm3,%%xmm1\n"
                "addps %%xmm1,%%xmm0\n"
                "divps %%xmm4,%%xmm0\n"
                "movups %%xmm0,32(%1)\n"
                "movups 32(%2),%%xmm0\n"
                "movups 32(%5),%%xmm1\n"
                "mulps %%xmm2,%%xmm0\n"
                "mulps %%xmm3,%%xmm1\n"
                "addps %%xmm1,%%xmm0\n"
                "divps %%xmm4,%%xmm0\n"
                "movups %%xmm0,32(%2)\n"

                "movups 48(%0),%%xmm0\n"
                "movups 48(%3),%%xmm1\n"
                "mulps %%xmm2,%%xmm0\n"
                "mulps %%xmm3,%%xmm1\n"
                "addps %%xmm1,%%xmm0\n"
                "divps %%xmm4,%%xmm0\n"
                "movups %%xmm0,48(%0)\n"
                "movups 48(%1),%%xmm0\n"
                "movups 48(%4),%%xmm1\n"
                "mulps %%xmm2,%%xmm0\n"
                "mulps %%xmm3,%%xmm1\n"
                "addps %%xmm1,%%xmm0\n"
                "divps %%xmm4,%%xmm0\n"
                "movups %%xmm0,48(%1)\n"
                "movups 48(%2),%%xmm0\n"
                "movups 48(%5),%%xmm1\n"
                "mulps %%xmm2,%%xmm0\n"
                "mulps %%xmm3,%%xmm1\n"
                "addps %%xmm1,%%xmm0\n"
                "divps %%xmm4,%%xmm0\n"
                "movups %%xmm0,48(%2)\n"
                :
                :"r"(R1_f),"r"(G1_f),"r"(B1_f),"r"(R2_f),"r"(G2_f),"r"(B2_f),"r"(a1),"r"(a2),"r"(base)
            );

			for (int i = 0; i < 16; ++i)
            {
                R1[i] = R1_f[i];
                R2[i] = R2_f[i];
                G1[i] = G1_f[i];
                G2[i] = G2_f[i];
                B1[i] = B1_f[i];
                B2[i] = B2_f[i];
            }
            
			BYTE buffer_tu[16],buffer_tv[16];
			RGB2YUV(buffer_wy+pixel,buffer_tu,buffer_tv,R1,G1,B1);
			
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
	duration = (stop - start)*1000/ CLOCKS_PER_SEC;
	printf("total time= %d ms\n", duration);
	return 0;
}
