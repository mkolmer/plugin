#include <iostream>
#include <CL/cl.h>
#include <windows.h>
#include <cuda_profiler_api.h>

#include "CImg.h"
#define MAX_SOURCE_SIZE (0x100000)

#define LOCAL_SIZE 8

#ifdef DEBUG
const char * get_error_string(cl_int err) {
    switch(err) {
    case 0:
        return "CL_SUCCES";
    case -1:
        return "CL_DEVICE_NOT_FOUND";
    case -2:
        return "CL_DEVICE_NOT_AVAILABLE";
    case -3:
        return "CL_COMPILER_NOT_AVAILABLE";
    case -4:
        return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5:
        return "CL_OUT_OF_RESOURCES";
    case -6:
        return "CL_OUT_OF_HOST_MEMORY";
    case -7:
        return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8:
        return "CL_MEM_COPY_OVERLAP";
    case -9:
        return "CL_IMAGE_FORMAT_MISMATCH";
    case -10:
        return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11:
        return "CL_BUILD_PROGRAM_FAILURE";
    case -12:
        return "CL_MAP_FAILURE";
    case -30:
        return "CL_INVALID_VALUE";
    case -31:
        return "CL_INVALID_DEVICE_TYPE";
    case -32:
        return "CL_INVALID_PLATFORM";
    case -33:
        return "CL_INVALID_DEVICE";
    case -34:
        return "CL_INVALID_CONTEXT";
    case -35:
        return "CL_INVALID_QUEUE_PROPERTIES";
    case -36:
        return "CL_INVALID_COMMAND_QUEUE";
    case -37:
        return "CL_INVALID_HOST_PTR";
    case -38:
        return "CL_INVALID_MEM_OBJECT";
    case -39:
        return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40:
        return "CL_INVALID_IMAGE_SIZE";
    case -41:
        return "CL_INVALID_SAMPLER";
    case -42:
        return "CL_INVALID_BINARY";
    case -43:
        return "CL_INVALID_BUILD_OPTIONS";
    case -44:
        return "CL_INVALID_PROGRAM";
    case -45:
        return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46:
        return "CL_INVALID_KERNEL_NAME";
    case -47:
        return "CL_INVALID_KERNEL_DEFINITION";
    case -48:
        return "CL_INVALID_KERNEL";
    case -49:
        return "CL_INVALID_ARG_INDEX";
    case -50:
        return "CL_INVALID_ARG_VALUE";
    case -51:
        return "CL_INVALID_ARG_SIZE";
    case -52:
        return "CL_INVALID_KERNEL_ARGS";
    case -53:
        return "CL_INVALID_WORK_DIMENSION";
    case -54:
        return "CL_INVALID_WORK_GROUP_SIZE";
    case -55:
        return "CL_INVALID_WORK_ITEM_SIZE";
    case -56:
        return "CL_INVALID_GLOBAL_OFFSET";
    case -57:
        return "CL_INVALID_EVENT_WAIT_LIST";
    case -58:
        return "CL_INVALID_EVENT";
    case -59:
        return "CL_INVALID_OPERATION";
    case -60:
        return "CL_INVALID_GL_OBJECT";
    case -61:
        return "CL_INVALID_BUFFER_SIZE";
    case -62:
        return "CL_INVALID_MIP_LEVEL";
    case -63:
        return "CL_INVALID_GLOBAL_WORK_SIZE";
    default:
        return "Unknown OpenCL error";
    }
}
#endif

using namespace cimg_library;


int main(int argc, char *argv[]) {
	SYSTEMTIME be4,after;
	WORD diff1,diff2;
	FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("./kernel.cl", "r");
    if (!fp) {
        fprintf(stderr, "Nie wczytano kernela.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;

    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
#ifdef DEBUG 
    std::cout<<get_error_string(ret)<<" clGetPlatformIDs"<<std::endl;
#endif
    ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1,
                          &device_id, &ret_num_devices);
#ifdef DEBUG 
    std::cout<<get_error_string(ret)<<" clGetDeviceIDs"<<std::endl;
    char* value;
    size_t valueSize;
    clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &valueSize);
    value = (char*) malloc(valueSize);
    clGetDeviceInfo(device_id, CL_DEVICE_NAME, valueSize, value, NULL);
    std::cout<<"Urz¹dzenie: "<<value<<std::endl;
    free(value);
#endif

    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
#ifdef DEBUG 
    std::cout<<get_error_string(ret)<<" CreateImage2d"<<std::endl;
#endif
    _cl_image_format format;
    format.image_channel_order = CL_RGBA;
    format.image_channel_data_type = CL_UNSIGNED_INT8;

    cimg_library::CImg<cl_uchar> in(argv[1]);
	int width=in.width();
	int height=in.height();
    cl_uchar4* intab=new cl_uchar4[width*height];
    int i=0;
	
    cimg_forXY(in,x,y) {
        intab[i].x =*in.data(x,y,0,0);
        intab[i].y =*in.data(x,y,0,1);
        intab[i].z =*in.data(x,y,0,2);
        i++;
    }
	 cudaProfilerStart(); 
    cl_mem clImage1 = clCreateImage2D(context, CL_MEM_READ_ONLY  ,&format ,width, height,  0, intab, &ret);
    cl_mem clImage2 = clCreateImage2D(context, CL_MEM_WRITE_ONLY ,&format,width, height, 0, 0, &ret);
    //cl::Image2D clImage1(context, CL_MEM_READ_ONLY, &format ,width, height,  0, intab (void*)image.data);
    //cl::Image2D clImage2(context, CL_MEM_WRITE_ONLY, &format ,width, height,  0, 0);
	///cl_mem clImage3 =clCreateImage2D(context, CL_MEM_WRITE_ONLY ,&format,width, height, 0, NULL, &ret);
#ifdef DEBUG 
    std::cout<<get_error_string(ret)<<" CreateImage2d"<<std::endl;
#endif
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
#ifdef DEBUG
    std::cout<<get_error_string(ret)<<" CreateCommandQueue"<<std::endl;
#endif
    size_t origin[] = {0,0,0};
    size_t region[] = {width, height,1};
	GetSystemTime(&be4);
    ret = clEnqueueWriteImage(command_queue, clImage1, CL_TRUE, origin, region,0,0, intab, 0, NULL,NULL );
	ret = clEnqueueWriteImage(command_queue, clImage2, CL_TRUE, origin, region,0,0, intab, 0, NULL,NULL );
	GetSystemTime(&after);
	diff1 = ((after.wSecond * 1000) + after.wMilliseconds)-((be4.wSecond * 1000) + be4.wMilliseconds);
   #ifdef DEBUG
	#endif
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
#ifdef DEBUG
    std::cout<<get_error_string(ret)<<" Create program"<<std::endl;
#endif
    ret = clBuildProgram(program, 1, &device_id,NULL,NULL, NULL);
#ifdef DEBUG
    char *build_log;
    size_t ret_val_size;
    ret = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);
    build_log =(char*) malloc(ret_val_size+1);
    ret = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
    build_log[ret_val_size] = '\0';
    fprintf(stderr, "%s\n", build_log );
#endif
    cl_kernel kernel = clCreateKernel(program, "mediana", &ret);
    ret=clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&clImage1);
#ifdef DEBUG
    std::cout<<get_error_string(ret)<<" SetArgument"<<std::endl;
#endif
    ret=clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&clImage2);
#ifdef DEBUG
    std::cout<<get_error_string(ret)<<" SetArgument"<<std::endl;
#endif
	int step;
    if(argv[2]){
    	step=atoi(argv[2]);
  	}else{
    	step=0;
    	printf("Brak parametru\n");
  	}
	/*Kontrast*
	float a;
	if(step>=1){
        a=1+step/127.0f;
    }else{
        a=(1-(abs(step)/127.0f));
    }
	**/

	/*Gamma*
	float a;
    if(step>=1.0f){
        a=step;
    }else{
        a=1.0f-(abs(step)/10.0f);
    }
	*/
	
	/*Lut do Gammy*
    int LUT[256];
    for (int i=0; i<256; i++) {
        if ((255.0f * powl(i/255.0f, 1.0f/a)) > 255) {
            LUT[i] = 255;
        }else{
            LUT[i] =(int)( 255 * powl(i/255.0f, 1.0f/a));
        }
    }
	cl_mem LUT_D = clCreateBuffer(context, CL_MEM_READ_ONLY, 256*sizeof(int), NULL, NULL);
	*/
    ret=clSetKernelArg(kernel, 2,sizeof(int),(void *)&step);
	ret=clSetKernelArg(kernel, 3,sizeof(int),(void *)&width);
	ret=clSetKernelArg(kernel, 4,sizeof(int),(void *)&height);
	int dod;
	dod = ((step+1)*(step+1))/2;
	ret=clSetKernelArg(kernel, 5,sizeof(int),(void *)&dod);

	/**ROZSZERZENIE HISTOGRAMU**
	int a=0;
    int b=10;
    int c=0;
    int d=14;
	ret=clSetKernelArg(kernel, 4, sizeof(int), (void *)&a);
	ret=clSetKernelArg(kernel, 5, sizeof(int), (void *)&b);
    ret=clSetKernelArg(kernel, 6, sizeof(int), (void *)&c);
	ret=clSetKernelArg(kernel, 7, sizeof(int), (void *)&d);
	***************************/
#ifdef DEBUG
    std::cout<<get_error_string(ret)<<" SetArgument"<<std::endl;
#endif
    size_t GWSize[2]= {width+(LOCAL_SIZE-width%LOCAL_SIZE),height+(LOCAL_SIZE-height%LOCAL_SIZE)};
    size_t local[2]= {LOCAL_SIZE,LOCAL_SIZE};
    int dim=2;
    cl_uchar3* outTAB= new cl_uchar3[width*height];
    cl_event event;
    
    /**OPERACJA LUT*******
	clEnqueueWriteBuffer(command_queue, LUT_D, CL_TRUE, 0, 256*sizeof(int), &LUT, 0, NULL, NULL );
	ret=clSetKernelArg(kernel, 2,sizeof(cl_mem),(void *)&LUT_D);
	
    *********************/
	GetSystemTime(&be4);
    ret = clEnqueueNDRangeKernel(command_queue, kernel, dim, NULL, (const size_t *)GWSize, (const size_t *)local, 0, NULL, &event);
#ifdef DEBUG
    std::cout<<get_error_string(ret)<< " after EnqueueNDRange"<<std::endl;
#endif
    clWaitForEvents(1,&event);
	GetSystemTime(&after);
    ret = clEnqueueReadImage(command_queue, clImage2, CL_TRUE, origin, region, 0, 0, outTAB,0, NULL, NULL);
	diff2 = ((after.wSecond * 1000) + after.wMilliseconds)-((be4.wSecond * 1000) + be4.wMilliseconds);
	cudaProfilerStop(); 
#ifdef DEBUG
    std::cout<<get_error_string(ret)<<"  ReadImage"<<std::endl;
#endif
    #ifdef DEBUG
	#else
    #endif
    cimg_library::CImg<cl_uchar> outIMG(in);
    outIMG.fill(0);
    i=0;
    cimg_forXY(outIMG,x,y) {
        outIMG(x,y,0,0)=outTAB[i].x;
        outIMG(x,y,0,1)=outTAB[i].y;
        outIMG(x,y,0,2)=outTAB[i].z;
        i++;
    }
    
#ifdef DEBUG
	std::cout<<diff1+diff2<<" ms"<<std::endl;
    outIMG.display();
	//outIMG.save("/home/marek/cl/image/lena2.bmp");
#endif
    clReleaseMemObject(clImage1);
    clReleaseMemObject(clImage2);
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
#ifdef DEBUG
    std::cout<<get_error_string(ret)<< " Cleaning"<<std::endl;
#endif


	return 0;

	


	return 0;
}
