#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "clutil.h"


char *read_kernel(char *filename)
{
     FILE *pf;
     char *source;
     if (!(pf = fopen(filename, "r"))) {
          fprintf(stderr, "Failed to load kernel\n");
          exit(1);
     }
     source = (char *)malloc(MAX_SOURCE_SIZE);
     memset(source, 0, MAX_SOURCE_SIZE);
     fread(source, 1, MAX_SOURCE_SIZE, pf);
     fclose(pf);
     return source;
}


void cl_set_dim(size_t *global, size_t *local, int M, int N)
{
     int H = M % GS_M == 0 ? M : M + M % GS_M;
     int W = N % GS_N == 0 ? N : N + N % GS_N;

     global[0] = H; local[0] = GS_M;
     global[1] = W; local[1] = GS_N;
}


cl_command_queue cl_create_queue(cl_device_id *dev, cl_context *ctx)
{
     clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, dev, NULL);
     *ctx = clCreateContext(0, 1, dev, NULL, NULL, NULL);
     return clCreateCommandQueue(*ctx, *dev, 0, NULL);
}


void cl_print_err_log(cl_program prog, cl_device_id dev)
{
     size_t len; char buffer[2048];
     clGetProgramBuildInfo(prog, dev, CL_PROGRAM_BUILD_LOG, sizeof(buffer),
			   buffer, &len);
     printf("%s", buffer);
     exit(-2);
}


cl_kernel cl_compile_kernel(cl_context ctx, cl_device_id dev, char *kfile,
			    char *fun)
{
     int err;
     cl_kernel kernel;
     cl_program prog;
     char *ksrc = read_kernel(kfile);
     prog = clCreateProgramWithSource(ctx, 1,(const char **)&ksrc, NULL, NULL);
     err  = clBuildProgram(prog, 0, NULL, NULL, NULL, NULL);
     if (err) {
	  cl_print_err_log(prog, dev);
     }
     kernel = clCreateKernel(prog, fun, NULL);
     clReleaseProgram(prog);
     return kernel;
}
