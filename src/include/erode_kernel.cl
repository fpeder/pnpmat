/*
 *
 */

#include "erosion.h"

#define GET(i, j) (i)*W + (j)


__kernel
void erode(__global uchar *src, __global uchar *dst,
           const int H, const int W)
{
     int i = get_global_id(0);
     int j = get_global_id(1);

     const int TOT = SZ * SZ;
     int sum = 0;
     int dy, dx, y, x;
     for (dy = -R; dy <= R; dy++) {
          for (dx = -R; dx <= R; dx++) {
               y = i + dy;
               x = j + dx;
               if (y >= 0 && y < H && x >= 0 && x < W)
                    sum += src[GET(y, x)];
          }
     }
     dst[GET(y, x)] = sum == TOT;

}

__kernel
void erode_textmem(__read_only image2d_t src,
                   __write_only image2d_t dst,
                   const int H, const int W)
{
     int i = get_global_id(0);
     int j = get_global_id(1);

}


__kernel
void erode_smem(__global uchar *src,  __global uchar *dst,
                const int H, const int W)
{
     __local int smem[GS_M + 2*R][GS_N + 2*R];

     int i  = get_local_id(0); 
     int j  = get_local_id(1);
     int ii = get_global_id(0);
     int jj = get_global_id(1);

     // ----------------------------------------------
     ii -= R; jj -= R;
     int in;
     in = ii >= 0 && jj >= 0;
     smem[i][j] = in ? src[GET(ii, jj)] : 0;
     if (i < R*2) {
          in = jj >= 0 && ii+GS_M < H;
          smem[i+GS_M][j] = in ? src[GET(ii+GS_M, jj)] : 0;
     }
     if (j < R*2) {
          in = ii >= 0 && jj+GS_N < W;
          smem[i][j+GS_N] = in ? src[GET(ii, jj+GS_N)] : 0;
     }
     if (i < R*2 && j < R*2) {
          in = ii+GS_M < H && jj+GS_N < W;
          smem[i+GS_M][j+GS_N] = in ? src[GET(ii+GS_M, jj+GS_N)] : 0;
     }
     i+=R; j+=R; ii+=R; jj+=R;

     barrier(CLK_LOCAL_MEM_FENCE);
     //-----------------------------------------------
     const int TOT = SZ*SZ;
     int sum = 0;
     int dy, dx;
     for (dy = -R; dy <= R; dy++) {
          for (dx = -R; dx <= R; dx++) {
               sum += smem[i+dy][j+dx];
          }
     }
     dst[GET(ii, jj)] = sum == TOT;
}
