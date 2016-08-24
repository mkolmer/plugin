const sampler_t samp2 = CLK_FILTER_NEAREST |
                        CLK_NORMALIZED_COORDS_FALSE |
                        CLK_ADDRESS_CLAMP_TO_EDGE;

__kernel void filtr(__read_only  image2d_t img1,
                      __write_only image2d_t img2,
                      const int r,const int w,const int h)  {


    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    if(coord.x<w&&coord.y<h){
	uint4 val = read_imageui(img1, samp2, coord);
    	write_imageui(img2, coord, val);
	}
}


__kernel void LUT(__read_only  image2d_t img1, __write_only image2d_t img2,__global const int* LUT,const int w,const int h){
	
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	if(coord.x<w&&coord.y<h){
		uint4 val = read_imageui(img1, samp2, coord);
		val.x=LUT[val.x];
		val.y=LUT[val.y];
		val.z=LUT[val.z];
		write_imageui(img2, coord,val);

 	}

}

__kernel void histogram(__read_only  image2d_t img1, __global int* HR,__global int* HG,__global int* HB,const int w,const int h){
	
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	if(coord.x<w&&coord.y<h){
		uint4 val = read_imageui(img1, samp2, coord);
		HR[val.x]++;
		HG[val.y]++;
		HB[val.z]++;
 	}
}

__kernel void wyrownanie(__read_only  image2d_t img1, __write_only image2d_t img2,const int w,const int h,__global int* tabR,__global int* tabG,__global int* tabB){

    int2 coord = (int2)(get_global_id(0), get_global_id(1));
	if(coord.x<w&&coord.y<h){
		uint4 val = read_imageui(img1, samp2, coord);
		uint4 val2;
        val2.x=(tabR[val.x]*255.0f)/(w*h);
        val2.y=(tabG[val.y]*255.0f)/(w*h);
        val2.z=(tabB[val.z]*255.0f)/(w*h);
        write_imageui(img2, coord,val2);
    }
}

__kernel void roz(__read_only  image2d_t img1,__write_only  image2d_t img2,const int w,const int h,const int a,const int b,const int c,const int d){
	
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	if(coord.x<w&&coord.y<h){
		uint4 val = read_imageui(img1, samp2, coord);
		int r=((val.x-a)*(d-c))/((b-a)+c); 
		int g=((val.y-a)*(d-c))/((b-a)+c);
		int b=((val.z-a)*(d-c))/((b-a)+c);
		if(r>255){
    	    r=255;
    	}else if(r<0){
     	   r=0;
    	}
    	if(g>255){
     	   g=255;
    	}else if(g<0){
     	   g=0;
    	}
    	if(b>255){
    	    b=255;
    	}else if(b<0){
     	   b=0;
    	}
    	write_imageui(img2, coord, (uint4)(r,g,b,1));
 	}

}

__kernel void sum(__read_only  image2d_t img1, __read_only image2d_t img2,const int w,const int h,__write_only image2d_t img3){
	
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	if(coord.x<w&&coord.y<h){
		uint4 val = read_imageui(img1, samp2, coord);
		uint4 val2 = read_imageui(img2, samp2, coord);
		uint4 val3;
		if(val.x+val2.x>255){
    	    		val3.x=255;
    		}else{
     	   		val3.x+=val2.x;
    		}

    		if(val.y+val2.y>255){
     	   		val3.y=255;
    		}else{
     	  		val3.y+=val2.y;
    		}

    		if(val.z+val2.z>255){
    	    		val3.z=255;
    		}else{
        		val3.z+=val2.z;
    		}
		write_imageui(img3, coord,val3);

 	}
} 

__kernel void dot_img(__read_only  image2d_t img1, __read_only image2d_t img2,const int w,const int h,__write_only image2d_t img3){
	
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	if(coord.x<w&&coord.y<h){
		uint4 val = read_imageui(img1, samp2, coord);
		uint4 val2 = read_imageui(img2, samp2, coord);
		uint4 val3;
		if(val.x*val2.x>255){
    	    		val3.x=255;
    		}else{
     	   		val3.x*=val2.x;
    		}

    		if(val.y*val2.y>255){
     	   		val3.y=255;
    		}else{
     	  		val3.y*=val2.y;
    		}

    		if(val.z*val2.z>255){
    	    		val3.z=255;
    		}else{
        		val3.z*=val2.z;
    		}
		write_imageui(img3, coord,val3);

 	}
} 

__kernel void gamma(__read_only  image2d_t img1, __write_only image2d_t img2,const float step,const int w,const int h) {

	
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
	if(coord.x<w&&coord.y<h){    
		uint4 val = read_imageui(img1, samp2, coord);
		int r=255 * pow(val.x/255.0f, 1.0f/step); 
		int g=255 * pow(val.y/255.0f, 1.0f/step);
		int b=255 * pow(val.z/255.0f, 1.0f/step);
		if(r>255){
    	    val.x=255;
    	}else if(r<0){
     	   val.x=0;
    	}else{
     	   val.x=r;
    	}
    	if(g>255){
     	   val.y=255;
    	}else if(g<0){
     	   val.y=0;
    	}else{
     	   val.y=g;
    	}
    	if(b>255){
    	    val.z=255;
    	}else if(b<0){
     	   val.z=0;
    	}else{
        	val.z=b;
    	}
    	write_imageui(img2, coord, val);
	}
}

__kernel void brightness(__read_only  image2d_t img1, __write_only image2d_t img2,const int step,const int w,const int h) {

	
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
	if(coord.x<w&&coord.y<h){    
		uint4 val = read_imageui(img1, samp2, coord);
		if(val.x+step>255){
    		val.x=255;
    	}else if(val.x+step<0){
     	   val.x=0;
    	}else{
     	   val.x+=step;
    	}
    	if(val.y+step>255){
     	   val.y=255;
    	}else if(val.y+step<0){
     	   val.y=0;
    	}else{
     	   val.y+=step;
    	}
    	if(val.z+step>255){
    	    val.z=255;
    	}else if(val.z+step<0){
     	   val.z=0;
    	}else{
        	val.z+=step;
    	}
    	write_imageui(img2, coord, val);
	}

}


__kernel void cont(__read_only  image2d_t img1, __write_only image2d_t img2,
						const float a,const int w,const int h) {

	
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
	if(coord.x<w&&coord.y<h){
		uint4 val = read_imageui(img1, samp2, coord);
		int r=a*(val.x-127.0f)+127.0f; 
		int g=a*(val.y-127.0f)+127.0f;
		int b=a*(val.z-127.0f)+127.0f;
		if(r>255){
    	    val.x=255;
    	}else if(r<0){
     	   val.x=0;
    	}else{
     	   val.x=r;
    	}
    	if(g>255){
     	   val.y=255;
    	}else if(g<0){
     	   val.y=0;
    	}else{
     	   val.y=g;
    	}
    	if(b>255){
    	    val.z=255;
    	}else if(b<0){
     	   val.z=0;
    }	else{
        	val.z=b;
    	}
    	write_imageui(img2, coord, val);
	}

}

__kernel void minimum(__read_only image2d_t srcImage,
                      __write_only image2d_t dstImage,
                      const int r,const int h,const int w){


    int x=get_global_id(0);
    int y=get_global_id(1);
    if(x<w&&y<h){
        uint4 kolor,min_kolor;

        min_kolor =read_imageui(srcImage,samp2,(int2)(x,y));
        for (int i = abs(x-r) ; i <= x+r&&i<h;i++){
            for (int j = abs(y-r); j<=y+r&&j<w;j++){
                kolor =read_imageui(srcImage,samp2,(int2)(i,j));
                int pix=3*kolor.x + 6*kolor.y + kolor.z;
                int min_kolor_pix=3*min_kolor.x + 6*min_kolor.y + min_kolor.z;
                if (min_kolor_pix > pix){
                    min_kolor=kolor;
                }
            }
        }
        write_imageui(dstImage,(int2)(x,y),min_kolor);
    }
}
__kernel void med(__read_only  image2d_t img1,
                      __write_only image2d_t img2,
                      const int r,const int w,const int h,int dod){
	int x=get_global_id(0);
    int y=get_global_id(1);
	uint4 color;
	if(x<w&&y<h){
		//kolor =read_imageui(img1,samp2,(int2)(abs(i),abs(j)));
		write_imageui(img2, (int2)(x,y), (uint4)(255,255,0,0));
		
	}
					  
}
__kernel void mediana(__read_only  image2d_t img1,
                      __write_only image2d_t img2,
                      const int r,const int w,const int h,int dod) {

	int x=get_global_id(0);
    int y=get_global_id(1);
	if(x<w&&y<h){
    	unsigned char R[256];
    	unsigned char G[256];
    	unsigned char B[256];

    	for (int j = 0; j<256; j++) {
        	R[j]=G[j]=B[j]=0;
    	}

    	uint4 kolor;
    	int pom,k,mr,mg,mb;

		
    	for (int i = x-r ; i <= x+r&&i<w; i++) {
        	for (int j = y-r; j<=y+r&&j<h; j++) {

            kolor =read_imageui(img1,samp2,(int2)(abs(i),abs(j)));
            R[kolor.x]++;
            G[kolor.y]++;
            B[kolor.z]++;

        	}
    	}
		

    	
    	pom = 0 ;
    	for (k = 0; k <256 && pom<dod ; k++)
        	pom+=R[k];

    	mr = k-1;

		pom = 0 ;
    	for (k = 0; k <256 && pom<dod ; k++)
        	pom+=G[k];

    	mg = k-1;

    	pom = 0 ;
    	for (k = 0; k <256 && pom<dod ; k++)
        	pom+=B[k];

    	mb = k-1;
		
    	// samo argb                             r  g  b a
    	write_imageui(img2, (int2)(x,y), (uint4)(mr,mg,mb,255));
	}

}

