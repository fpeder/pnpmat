#define GS_M 16
#define GS_N 16

#define GET(i, j) (i)*W + j 


__kernel
void erode_unrolled(__global uchar *src,
                    __global uchar *dst,
                    const int H, const int W)
{
     int i = get_global_id(0);
     int j = get_global_id(1);
     uchar p1, p2, p3;
     uchar p4, p5, p6;
     uchar p7, p8, p9;
     if (i > H || j > W) return;
     if ((p5 = src[GET(i  , j  )])) {
          p1 = src[GET(i-1, j-1)];
          p2 = src[GET(i-1, j  )];
          p3 = src[GET(i-1, j+1)];
          p4 = src[GET(i  , j-1)];
          p6 = src[GET(i  , j+1)];
          p7 = src[GET(i+1, j-1)];
          p8 = src[GET(i+1, j  )];
          p9 = src[GET(i+1, j+1)];
          dst[GET(i, j)] = (p1 && p2 && p3 &&
                            p4 && p5 && p6 &&
                            p7 && p8 && p9);
     }
     
}

__kernel
void erode(__global uchar *src, __global uchar *dst,
           const int H, const int W, const int ksize)
{
     int i = get_global_id(0);
     int j = get_global_id(1);
     int sum = 0, dy, dx;
     const int R = ksize / 2;
     const int tot = ksize * ksize;
     if (i > H || j > W || !src[GET(i, j)]) return;
     for (dy = -R; dy <= R; dy++) {
          for (dx = -R; dx <= R; dx++) {
               sum += src[GET(i+dy, j+dx)];
          }
     }
     dst[GET(i, j)] = sum == tot;
}


__kernel
void erode_smem3(__global uchar *src,  __global uchar *dst,
                 const int H, const int W, const int ksize)
{
#define R 1
     __local uchar smem[GS_M + 2*R][GS_N + 2*R];

     int i  = get_local_id(0); 
     int j  = get_local_id(1);
     int ii = get_global_id(0);
     int jj = get_global_id(1);
     int sum = 0, dy, dx, in;
     const int tot = ksize * ksize;
     /* ------------------------------------------ */
     ii -= R; jj -= R;
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
     /* ----------------------------------------- */
     if (ii > H || jj > W || !src[GET(ii, jj)]) return;
     for (dy = -R; dy <= R; dy++) {
          for (dx = -R; dx <= R; dx++) {
               sum += smem[i+dy][j+dx];
          }
     }
     dst[GET(ii, jj)] = sum == tot;
#undef R
}

__kernel
void erode_smem5(__global uchar *src,  __global uchar *dst,
                 const int H, const int W, const int ksize)
{
#define R 2
     __local uchar smem[GS_M + 2*R][GS_N + 2*R];

     int i  = get_local_id(0); 
     int j  = get_local_id(1);
     int ii = get_global_id(0);
     int jj = get_global_id(1);
     int sum = 0, dy, dx, in;
     const int tot = ksize * ksize;
     /* ------------------------------------------ */
     ii -= R; jj -= R;
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
     /* ----------------------------------------- */
     if (ii > H || jj > W || !src[GET(ii, jj)]) return;
     for (dy = -R; dy <= R; dy++) {
          for (dx = -R; dx <= R; dx++) {
               sum += smem[i+dy][j+dx];
          }
     }
     dst[GET(ii, jj)] = sum == tot;
#undef R
}

__kernel
void erode_smem7(__global uchar *src,  __global uchar *dst,
                 const int H, const int W, const int ksize)
{
#define R 3
     __local uchar smem[GS_M + 2*R][GS_N + 2*R];

     int i  = get_local_id(0); 
     int j  = get_local_id(1);
     int ii = get_global_id(0);
     int jj = get_global_id(1);
     int sum = 0, dy, dx, in;
     const int tot = ksize * ksize;
     /* ------------------------------------------ */
     ii -= R; jj -= R;
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
     /* ----------------------------------------- */
     if (ii > H || jj > W || !src[GET(ii, jj)]) return;
     for (dy = -R; dy <= R; dy++) {
          for (dx = -R; dx <= R; dx++) {
               sum += smem[i+dy][j+dx];
          }
     }
     dst[GET(ii, jj)] = sum == tot;
#undef R
}

