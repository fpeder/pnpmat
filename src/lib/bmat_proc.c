#include <stdio.h>
#include "bmat.h"
#include "util.h"

//------------------------------------------------------------------------------
bmat bmat_erode(bmat src, const int ksize)
{
     const int tot = ksize * ksize;
     const int R = ksize / 2;
     int i, j, ki, kj, sum;
     bmat dst = bmat_copy(src);

     for (i = R; i < src.M-R; i++)
          for (j = R; j < src.N-R; j++) {
               if (bmat_get(src, i, j)) {
                    sum = 0;
                    for (ki = -R; ki <= R; ki++)
                         for (kj = -R; kj <= R; kj++)
                              sum += bmat_get(src, i+ki, j+kj);
                    if (sum != tot)
                         bmat_clear(dst, i, j);
               }
          }
     return dst;
}

bmat bmat_dilate(bmat src, const int ksize)
{
     const int tot = ksize * ksize;
     const int R = ksize / 2;
     int i, j, ki, kj, sum;
     bmat dst = bmat_copy(src);

     for (i = R; i < src.M-R; i++)
          for (j = R; j < src.N-R; j++) {
               if (!bmat_get(src, i, j)) {
                    sum = 0;
                    for (ki = -R; ki <= R; ki++)
                         for (kj = -R; kj <= R; kj++)
                              sum += bmat_get(src, i+ki, j+kj);
                    if (sum)
                         bmat_set(dst, i, j);
               }
          }
     return dst;
}

//------------------------------------------------------------------------------
void bmat_rl(bmat src, uint32_t *pt)
{
     uint8_t v;
     int k, l=0;
     for (int i=0; i<src.M; i++) {
          for (int j=0; j<src.N; j++) {
               if (bmat_get(src, i, j)) {
                    pt[l++] = CNTR_PT_PACK(i, j);
                    k = j+1;
                    while (bmat_get(src, i, k))
                         k++;
                    pt[l++] = CNTR_PT_PACK(i, k-1);
                    j = k;
               }
          }
     }
     pt[l] = 0;
}

//------------------------------------------------------------------------------
void bmat_moore(bmat mat, int *count, uint32_t *contour)
{
     const int8_t di[] = {0,  +1, +1, +1,  0, -1, -1, -1};
     const int8_t dj[] = {-1, -1,  0, +1, +1, +1,  0, -1};
     const int8_t map[] = {4, 5, 6, 7, 0, 1, 2, 3};
     const int8_t asd[8][8] = {7, 6, 5, 4, 3, 2, 1, 0,
                               0, 7, 6, 5, 4, 3, 2, 1,
                               1, 0, 7, 6, 5, 4, 3, 2,
                               2, 1, 0, 7, 6, 5, 4, 3,
                               3, 2, 1, 0, 7, 6, 5, 4,
                               4, 3, 2, 1, 0, 7, 6, 5,
                               5, 4, 3, 2, 1, 0, 7, 6,
                               6, 5, 4, 3, 2, 1, 0, 7};

     int i, j, l=0;
     uint32_t start, end=0;
     uint8_t prev=0, next;
     //bmat tmp = bmat_zeros(mat.M, mat.N);
     bmat_foreach(mat, &i, &j);
     start = CNTR_PT_PACK(i, j); 
     while (start != end) {
          int y, x, k;
          for (k=0; k<8; k++) {
               next = asd[prev][k];
               y = i + di[next];
               x = j + dj[next];
               //access++;
               if (bmat_get(mat, y, x))
                    break;
          }
          i = y; j = x;
          //bmat_set(tmp, y, x);
          end = CNTR_PT_PACK(y, x);
          contour[l++] = end;
          prev = map[next];
          (*count)++;
     }
     contour[l] = 0;
     //printf("%.2f\n", access/count);
     //return tmp;
}

//------------------------------------------------------------------------------
#define MIN(x, y) ((x) < (y) ? (x) : (y))
//#define CONSTANT_ACCESS

void gh_get_neighbors(bmat in, int i, int j, int *p)
{
     p[0] = bmat_get(in, i  , j  );
     p[1] = bmat_get(in, i-1, j-1);
     p[2] = bmat_get(in, i-1, j  );
     p[3] = bmat_get(in, i-1, j+1);
     p[4] = bmat_get(in, i  , j+1);
     p[5] = bmat_get(in, i+1, j+1);
     p[6] = bmat_get(in, i+1, j  );
     p[7] = bmat_get(in, i+1, j-1);
     p[8] = bmat_get(in, i  , j-1);
}

int gh_del_condition(int *p, int iter)
{
     int c, d, n, n1, n2;

     c = ((!p[2] && (p[3] || p[4])) + (!p[4] && (p[5] || p[6])) +
          (!p[6] && (p[7] || p[8])) + (!p[8] && (p[1] || p[2])));

     n1 = (p[1] || p[2]) + (p[3] || p[4]) + (p[5] || p[6]) + (p[7] || p[8]);
     n2 = (p[2] || p[3]) + (p[4] || p[5]) + (p[6] || p[7]) + (p[8] || p[1]);

     n = MIN(n1, n2);
     d = iter ? ((p[2] || p[3] || (!p[5])) && p[4]) :
                ((p[6] || p[7] || (!p[1])) && p[8]);
     
     return c == 1 && n >= 2 && n <= 3 && d == 0;
}

bmat bmat_guohall_iter(bmat in, int *count)
{
     static int iter=0, p[9];
     int i, j; bmat out = bmat_copy(in);

     for (i=1; i < in.M-1; i++)
          for (j=1; j < in.N-1; j++) {
#ifdef CONSTANT_ACCESS
               uint8_t v = bmat_get(in, i, j);
               gh_get_neighbors(in, i, j, p);
               if (v && del_condition(p, iter)) {
                    bmat_clear(out, i, j);
                    (*count)++;
               }
#else
               if (bmat_get(in, i, j)) {
                    gh_get_neighbors(in, i, j, p); 
                    if (gh_del_condition(p, iter)) {
                         bmat_clear(out, i, j);
                         (*count)++;
                    }
               }
#endif
          }
     iter ^= 1;
     return out;
}

bmat bmat_guohall(bmat m)
{
     int count;
     do {
          count = 0;
          m = bmat_guohall_iter(m, &count);
     } while (count);
     return m;
}
