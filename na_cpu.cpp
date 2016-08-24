#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "CImg.h"
#include <windows.h>




using namespace cimg_library;

int main(int argc, char *argv[]) {
	
	SYSTEMTIME be4,after;
	WORD diff1,diff2;
    CImg<unsigned char> in(argv[1]);
	CImg<unsigned char> in2(in);
    CImg<unsigned char> out(in);


	cimg_forXY(in2,x,y) {
        out(x,y,0,0)=rand()%256;
     	out(x,y,0,1)=rand()%256;
     	out(x,y,0,2)=rand()%256;
    		
	}	

	int promien;
	if(argv[2]){
    	promien=atoi(argv[2]);
  	}else{
    	promien=0;
    	printf("Brak parametru\n");
  	}
	GetSystemTime(&be4);


	/***************MEDIANA */

	int mr, mg, mb;
    int dod = (2*promien+1)*(2*promien+1)/2;
    int r[256];
    int g[256];
    int b[256];
	cimg_forXY(in,x,y) {
            mr = mg = mb = 0;
            for (int k = 0 ; k < 256 ; k++)
                r[k] = g[k] = b[k] = 0;

            for (int k = x-promien ; k <= promien+x&&k<in.width() ; k++)
                for (int l = y-promien ; l <=promien+y&&l<in.height() ; l++)
                {
                    r[*in.data(abs(k),abs(l),0,0)] ++;
                    g[*in.data(abs(k),abs(l),0,1)] ++;
                    b[*in.data(abs(k),abs(l),0,2)] ++;
                }

            int pom;
            int k;

            pom = 0 ;
            for (k = 0; k <256 && pom<dod ; k++)
                pom+=r[k];
            mr = k-1;

            pom = 0 ;
            for (k = 0; k <256 && pom<dod ; k++)
                pom+=g[k];
            mg = k-1;

            pom = 0 ;
            for (k = 0; k <256 && pom<dod ; k++)
                pom+=b[k];
            mb = k-1;

            out(x,y,0,0)=mr;
            out(x,y,0,1)=mg;
            out(x,y,0,2)=mb;

            //printf("1.5 ");
    }
	GetSystemTime(&after);
	diff1 =  ((after.wSecond * 1000) + after.wMilliseconds)-((be4.wSecond * 1000) + be4.wMilliseconds);
	std::cout<< diff1<<" ms"<<std::endl;
	/********************************************************/
	/************JASNOSC**************************************
	int step=54;	
	cimg_forXY(in,x,y) {
		int r,g,b;
    	r=*in.data(x,y,0,0)+step;
    	g=*in.data(x,y,0,1)+step;
    	b=*in.data(x,y,0,2)+step;
    	if(r>255){
        	out(x,y,0,0)=255;
    	}else if(r<0){
        	out(x,y,0,0)=0;
    	}else{
        	out(x,y,0,0)=r;
    	}
    	if(g>255){
    	    out(x,y,0,1)=255;
    	}else if(g<0){
    	    out(x,y,0,1)=0;
    	}else{
     	   out(x,y,0,1)=g;
    	}
    	if(b>255){
     	   out(x,y,0,2)=255;
    	}else if(b<0){
     	   out(x,y,0,2)=0;
    	}else{
     	   out(x,y,0,2)=b;
    	}	
	}
	*******************************************************/

	/*********  KONTRAST                *****************
	int step=2;
	float a;
    if(step>=1){
        a=1+step/127.0f;
    }else{
        a=(1-(abs(step)/127.0f));
    }	
	cimg_forXY(in,x,y) {
		int r,g,b;
    	r=*in.data(x,y,0,0);
    	g=*in.data(x,y,0,1);
    	b=*in.data(x,y,0,2);
    	if(a*(r-128)+128>255){
        	out(x,y,0,0)=255;
    	}else if(a*(r-128)+128<0){
        	out(x,y,0,0)=0;
    	}else{
        	out(x,y,0,0)=r;
    	}
    	if(a*(g-128)+128>255){
    	    out(x,y,0,1)=255;
    	}else if(a*(g-128)+128<0){
    	    out(x,y,0,1)=0;
    	}else{
     	   out(x,y,0,1)=g;
    	}
    	if(a*(b-128)+128>255){
     	   out(x,y,0,2)=255;
    	}else if(a*(b-128)+128<0){
     	   out(x,y,0,2)=0;
    	}else{
     	   out(x,y,0,2)=b;
    	}	
	}
	*******************************************************/
	/* GAMMA BEZ LUT
	int step=2;
	float gamma;
    if(step>=1){
        gamma=step;
    }else{
        gamma=1-(abs(step)/10.0f);
    }
	cimg_forXY(in,x,y) {
		int r,g,b;
    	r=*in.data(x,y,0,0);
    	g=*in.data(x,y,0,1);
    	b=*in.data(x,y,0,2);
    	if(255 * powl(r/255.0, 1/gamma)>255){
        	out(x,y,0,0)=255;
    	}else if(255 * powl(r/255.0, 1/gamma)<0){
        	out(x,y,0,0)=0;
    	}else{
        	out(x,y,0,0)=r;
    	}
    	if(255 * powl(g/255.0, 1/gamma)>255){
    	    out(x,y,0,1)=255;
    	}else if(255 * powl(g/255.0, 1/gamma)<0){
    	    out(x,y,0,1)=0;
    	}else{
     	   out(x,y,0,1)=g;
    	}
    	if(255 * powl(b/255.0, 1/gamma)>255){
     	   out(x,y,0,2)=255;
    	}else if(255 * powl(b/255.0, 1/gamma)<0){
     	   out(x,y,0,2)=0;
    	}else{
     	   out(x,y,0,2)=b;
    	}	
	}
	**/
/* GAMMA Z LUT
	int LUT[256]; 
	int step=2;
	float gamma;
    if(step>=1){
        gamma=step;
    }else{
        gamma=1-(abs(step)/10.0f);
    }
	for (int i=0; i<256; i++) {
        if ((255.0f * powl(i/255.0f, 1.0f/gamma)) > 255) {
            LUT[i] = 255;
        }else{
            LUT[i] =(int)( 255 * powl(i/255.0f, 1.0f/gamma));
        }
    }

	cimg_forXY(in,x,y) {
		int r,g,b;
    	r=*in.data(x,y,0,0);
    	g=*in.data(x,y,0,1);
    	b=*in.data(x,y,0,2);
        out(x,y,0,0)=LUT[r];
     	out(x,y,0,1)=LUT[g];
     	out(x,y,0,2)=LUT[b];
    		
	}
*/

/* FILTR MINIMUM

	cimg_forXY(in,x,y) {
            
			unsigned int min_kolor=3*(*in.data(x,y,0,0))+6*(*in.data(x,y,0,1))+*in.data(x,y,0,2);
			unsigned char r,g,b;
			r=*in.data(x,y,0,0);
			g=*in.data(x,y,0,1);
			b=*in.data(x,y,0,2);
            for (int k = x-promien ; k <= promien+x&&k<in.width() ; k++)
                for (int l = y-promien ; l <=promien+y&&l<in.height() ; l++){					
					unsigned int kolor=3*(*in.data(abs(k),abs(l),0,0))+6*(*in.data(abs(k),abs(l),0,1))+*in.data(abs(k),abs(l),0,2);
					if(min_kolor>kolor){
                    	r=*in.data(abs(k),abs(l),0,0);
                    	g=*in.data(abs(k),abs(l),0,1);
                    	b=*in.data(abs(k),abs(l),0,2);
					}
                }
            out(x,y,0,0)=r;
            out(x,y,0,1)=g;
            out(x,y,0,2)=b;
}
********************************************/
/* SUMA
	int r,g,b;
	cimg_forXY(in,x,y) {
		
		r=*in.data(x,y,0,0)+*in2.data(x,y,0,0);
    	g=*in.data(x,y,0,1)+*in2.data(x,y,0,1);
    	b=*in.data(x,y,0,2)+*in2.data(x,y,0,2);
    	if(r>255){
        	out(x,y,0,0)=255;
    	}else{
        	out(x,y,0,0)=r;
    	}
    	if(g>255){
    	    out(x,y,0,1)=255;
    	}else{
     	   out(x,y,0,1)=g;
    	}
    	if(b>255){
     	   out(x,y,0,2)=255;
    	}else{
     	   out(x,y,0,2)=b;
    	}	
	}	
*/	
/* PRZEMNOŻENIE OBRAZÓW
	int r,g,b;
	cimg_forXY(in,x,y) {
		
		r=*in.data(x,y,0,0)**in2.data(x,y,0,0);
    	g=*in.data(x,y,0,1)**in2.data(x,y,0,1);
    	b=*in.data(x,y,0,2)**in2.data(x,y,0,2);
    	if(r>255){
        	out(x,y,0,0)=255;
    	}else{
        	out(x,y,0,0)=r;
    	}
    	if(g>255){
    	    out(x,y,0,1)=255;
    	}else{
     	   out(x,y,0,1)=g;
    	}
    	if(b>255){
     	   out(x,y,0,2)=255;
    	}else{
     	   out(x,y,0,2)=b;
    	}	
	}
*/

/**HISTOGRAM
	int* hist=new int[10*256];
	cimg_forXY(in,k,l){
		hist[3*(*in.data(k,l,0,0))+6*(*in.data(k,l,0,1))+*in.data(k,l,0,2)]++;
	}
	
	delete[] hist;
**/


    return 0;
}
