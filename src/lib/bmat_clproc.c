#include "clutil.h"
#include "bmat.h"
#include "time.h"

#define NTIMES 50

bmat cl_erode(bmat src, char *kfile, char *kfunc, int ksize)
{
     cl_device_id dev;
     cl_context ctx;
     cl_command_queue queue;
     cl_kernel erode; cl_mem d_src, d_dst;
     size_t global[2];
     size_t local[2];

     bmat dst = bmat_copy(src);
     int bytes = bmat_bytes(src);
     int count = bmat_count(src);

     cl_set_dim(global, local, src.M, src.N);

     TIME_START;
     queue = cl_create_queue(&dev, &ctx);
     erode = cl_compile_kernel(ctx, dev, kfile, kfunc);
     d_src = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            bytes, src.data, NULL);
     d_dst = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
                            bytes, dst.data, NULL);
     int narg = 0;
     clSetKernelArg(erode, narg++, sizeof(cl_mem), &d_src);
     clSetKernelArg(erode, narg++, sizeof(cl_mem), &d_dst);
     clSetKernelArg(erode, narg++, sizeof(int), &(src.M));
     clSetKernelArg(erode, narg++, sizeof(int), &(src.N));
     clSetKernelArg(erode, narg++, sizeof(int), &ksize);
     
     for (int x=0; x < NTIMES; x++) {
          clEnqueueNDRangeKernel(queue, erode, 2, NULL, global, local, 0, NULL,
                                 NULL);
          clFinish(queue);
          clEnqueueReadBuffer(queue, d_dst, CL_TRUE, 0, bytes, dst.data, 0,
                              NULL, NULL);
     }
     
     TIME_STOP;
     TIME_NORM_PRINT(NTIMES, count, src.size);

     clReleaseCommandQueue(queue);
     clReleaseContext(ctx);
     clReleaseDevice(dev);
     clReleaseKernel(erode);
     clReleaseMemObject(d_src);
     clReleaseMemObject(d_dst);
     return dst;
}

//------------------------------------------------------------------------------
bmat cl_gh(bmat src, char *kfile, char *kfun)
{
     cl_device_id dev;
     cl_context ctx;
     cl_command_queue queue;
     cl_kernel gh_kernel, del_kernel;
     cl_mem d_del, d_dst, d_c;
     size_t global[2], local[2];

     bmat dst  = bmat_copy(src);
     bmat del  = bmat_zeros(src.M, src.N);
     int bytes = bmat_bytes(src);
     int count = bmat_count(src);

     cl_set_dim(global, local, src.M, src.N);
     queue = cl_create_queue(&dev, &ctx);
     gh_kernel = cl_compile_kernel(ctx, dev, kfile, kfun);
     del_kernel = cl_compile_kernel(ctx, dev, kfile, "delete");

     TIME_START;
     for (int x = 0; x < NTIMES; x++) {
          d_dst = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                                 bytes, src.data, NULL);

          d_del = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                 bytes, del.data, NULL);

          d_c = clCreateBuffer(ctx, CL_MEM_READ_WRITE, sizeof(int), NULL, NULL);
          
          int narg1=0;
          clSetKernelArg(gh_kernel, narg1++, sizeof(cl_mem), &d_dst);
          clSetKernelArg(gh_kernel, narg1++, sizeof(cl_mem), &d_del);
          clSetKernelArg(gh_kernel, narg1++, sizeof(cl_mem), &d_c);
          clSetKernelArg(gh_kernel, narg1++, sizeof(int)   , &src.M);
          clSetKernelArg(gh_kernel, narg1++, sizeof(int)   , &src.N);
          
          int narg2=0;
          clSetKernelArg(del_kernel, narg2++, sizeof(cl_mem), &d_dst);
          clSetKernelArg(del_kernel, narg2++, sizeof(cl_mem), &d_del);
          clSetKernelArg(del_kernel, narg2++, sizeof(int)   , &src.M);
          clSetKernelArg(del_kernel, narg2++, sizeof(int)   , &src.N);
          
          int count=0, iter=1;
          do {
               count = 0;
               clSetKernelArg(gh_kernel,  narg1, sizeof(int), &iter);
               clSetKernelArg(del_kernel, narg2, sizeof(int), &iter);
               clEnqueueWriteBuffer(queue, d_c, CL_TRUE, 0, sizeof(int), &count,
                                    0, NULL, NULL);
          
               clEnqueueNDRangeKernel(queue, gh_kernel , 2, NULL, global, local,
                                      0, NULL, NULL);
               
               clEnqueueNDRangeKernel(queue, del_kernel, 2, NULL, global, local,
                                      0, NULL, NULL);
               clFinish(queue);
               clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0, sizeof(int), &count,
                                   0, NULL, NULL);
               iter++;
               
          } while(count);

          clEnqueueReadBuffer(queue, d_dst, CL_TRUE, 0, bytes, dst.data, 0,
                              NULL, NULL);
     }
     TIME_STOP;
     TIME_NORM_PRINT(NTIMES, count, src.size);

     clReleaseMemObject(d_dst);
     clReleaseMemObject(d_del);
     clReleaseMemObject(d_c);
     clReleaseKernel(gh_kernel);
     clReleaseKernel(del_kernel);
     clReleaseCommandQueue(queue);
     clReleaseContext(ctx);
     return dst;
}

#undef NTIMES
