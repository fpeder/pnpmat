#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics  : enable

#define R     1
#define R2    2
#define GS_M  16
#define GS_N  16
//#define CONSTANT_ACCESS


#define GET(i, j) ((i)*W + j)
#define MIN(x, y) ((x) < (y) ? (x) : (y))


int compute_c(uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar);
int compute_n(uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar);
int compute_d(uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar, int);
int delete_condition(int, int, int); 


__kernel
void gh(__global uchar *I,
        __global uchar *O,
        __global int *count,
        const int H, const int W, const int iter)
{
     int i = get_global_id(0);
     int j = get_global_id(1);
 
     uchar p1, p2, p3;
     uchar p8, p0, p4;
     uchar p7, p6, p5;
     uchar c, d,  n;

     if (i > H || j > W)
          return;

#ifdef CONSTANT_ACCESS
     p0 = I[GET(i  , j  )];
     p1 = I[GET(i-1, j-1)];
     p2 = I[GET(i-1, j  )];
     p3 = I[GET(i-1, j+1)];
     p4 = I[GET(i  , j+1)];
     p5 = I[GET(i+1, j+1)];
     p6 = I[GET(i+1, j  )];
     p7 = I[GET(i+1, j-1)];
     p8 = I[GET(i  , j-1)];

     c = compute_c(p1, p2, p3, p8, p4, p7, p6, p5);
     n = compute_n(p1, p2, p3, p8, p4, p7, p6, p5);
     d = compute_d(p1, p2, p3, p8, p4, p7, p6, p5, iter);

     if (p0 && delete_condition(c, n, d)) {
          O[GET(i, j)] = iter;
          atomic_inc(count);
     }
#else
     if ((p0 = I[GET(i  , j  )])) {
          p1 = I[GET(i-1, j-1)];
          p2 = I[GET(i-1, j  )];
          p3 = I[GET(i-1, j+1)];
          p4 = I[GET(i  , j+1)];
          p5 = I[GET(i+1, j+1)];
          p6 = I[GET(i+1, j  )];
          p7 = I[GET(i+1, j-1)];
          p8 = I[GET(i  , j-1)];

          c = compute_c(p1, p2, p3, p8, p4, p7, p6, p5);
          n = compute_n(p1, p2, p3, p8, p4, p7, p6, p5);
          d = compute_d(p1, p2, p3, p8, p4, p7, p6, p5, iter);

          if (delete_condition(c, n, d)) {
               O[GET(i, j)] = iter;
               atomic_inc(count);
          }
     }
#endif
}


__kernel
void gh_smem(__global uchar *I,
             __global uchar *O,
             __global int *count,
             const int H, const int W, const int iter)
{
     __local uchar smem[GS_M + R2][GS_N + R2];
     
     int y = get_local_id(0);
     int x = get_local_id(1);
     int i = get_global_id(0);
     int j = get_global_id(1);

     int in;

     uchar p1, p2, p3;
     uchar p8, p0, p4;
     uchar p7, p6, p5;
     uchar c, d, n;

     //---------------------------------------------------------------
     i -= R; j -= R;

     in = i >= 0 && j >= 0;
     smem[y][x] = in ? I[GET(i, j)] : 0;

     if (y < R2) {
          in = (j >= 0) && (i + GS_M < H);
          smem[y + GS_M][x] = in ? I[GET(i + GS_M, j)] : 0;
     }
     if (x < R2) {
          in = (i >= 0) && (j + GS_N) < W;
          smem[y][x + GS_N] = in ? I[GET(i, j + GS_N)] : 0;
     }
     if (y < R2 && x < R2) {
          in = (i + GS_M < H) && (j + GS_N < W);
          smem[y + GS_M][x + GS_N] = in ? I[GET(i + GS_M, j + GS_N)] : 0;
     }
     barrier(CLK_LOCAL_MEM_FENCE);

     y += R; x += R; i += R; j += R;
     //---------------------------------------------------------------

     if (i > H || j > W)
          return;

#ifdef CONSTANT_ACCESS
     p0 = smem[y  ][x  ];
     p1 = smem[y-1][x-1];
     p2 = smem[y-1][x  ];
     p3 = smem[y-1][x+1];
     p4 = smem[y  ][x+1];
     p5 = smem[y+1][x+1];
     p6 = smem[y+1][x  ];
     p7 = smem[y+1][x-1];
     p8 = smem[y  ][x-1];

     c = compute_c(p1, p2, p3, p8, p4, p7, p6, p5);
     n = compute_n(p1, p2, p3, p8, p4, p7, p6, p5);
     d = compute_d(p1, p2, p3, p8, p4, p7, p6, p5, iter);

     if (p0 && delete_condition(c, n, d)) {
          O[GET(i, j)] = iter;
          atomic_inc(count);
     }
#else
     if ((p0 = smem[y  ][x  ])) {
          p1 = smem[y-1][x-1];
          p2 = smem[y-1][x  ];
          p3 = smem[y-1][x+1];
          p4 = smem[y  ][x+1];
          p5 = smem[y+1][x+1];
          p6 = smem[y+1][x  ];
          p7 = smem[y+1][x-1];
          p8 = smem[y  ][x-1];

          c = compute_c(p1, p2, p3, p8, p4, p7, p6, p5);
          n = compute_n(p1, p2, p3, p8, p4, p7, p6, p5);
          d = compute_d(p1, p2, p3, p8, p4, p7, p6, p5, iter);

          if (delete_condition(c, n, d)) {
               O[GET(i, j)] = iter;
               atomic_inc(count);
          }
     }
#endif
}

__kernel
void delete(__global uchar *mat,
            __global uchar *pos,
            const int H, const int W, const int iter)
{
     int i = get_global_id(0);
     int j = get_global_id(1);
     int ij = i*W + j;

     if (i > H || j > W)
          return;

     if (pos[ij] == iter)
          mat[ij] = 0;
}

int compute_c(uchar p1, uchar p2, uchar p3,
              uchar p8,           uchar p4,
              uchar p7, uchar p6, uchar p5)
{
     return ((!p2 && (p3 || p4)) + (!p4 && (p5 || p6)) +
             (!p6 && (p7 || p8)) + (!p8 && (p1 || p2)));
}

int compute_n(uchar p1, uchar p2, uchar p3,
              uchar p8,           uchar p4,
              uchar p7, uchar p6, uchar p5)
{
     int n1 = (p1 || p2) + (p3 || p4) + (p5 || p6) + (p7 || p8);
     int n2 = (p2 || p3) + (p4 || p5) + (p6 || p7) + (p8 || p1);

     return min(n1, n2);
}

int compute_d(uchar p1, uchar p2, uchar p3,
              uchar p8,           uchar p4,
              uchar p7, uchar p6, uchar p5, int iter)
{
     return iter % 2 == 0 ? ((p2 || p3 || (!p5)) && p4) :
                            ((p6 || p7 || (!p1)) && p8);
}

int delete_condition(int c, int n, int d)
{
     return c == 1 && n >= 2 && n <= 3 && d == 0;
}
