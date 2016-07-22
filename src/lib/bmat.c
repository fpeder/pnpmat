#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "bmat.h"
#include "util.h"

#define UINT8_SIZE sizeof(uint8_t)


bmat bmat_init_light(int M, int N)
{
     bmat m = {M, N, M*N, 0, NULL};
     return m;
}

bmat bmat_init(int M, int N)
{
     bmat m = {M, N, M*N, 0, NULL};
     m.data = (uint8_t *)malloc(M * N * UINT8_SIZE);
     assert(m.data);
     return m;
}

bmat bmat_zeros(int M, int N)
{
     bmat m = bmat_init_light(M, N);
     m.data = (uint8_t *)calloc(M*N, UINT8_SIZE);
     assert(m.data);
     return m;
}

bmat bmat_copy(bmat m)
{
     bmat copy = bmat_init(m.M, m.N);
     memcpy(copy.data, m.data, m.size * UINT8_SIZE);
     return copy;
}

bmat bmat_read(char *filename)
{
     bmat m;
     int i, M, N, magic; 
     FILE *pf = fopen(filename, "rb");
     assert(pf);
     fread(&magic, sizeof(int), 1, pf);
     assert(magic == 666);
     fread(&M, sizeof(int), 1, pf);
     fread(&N, sizeof(int), 1, pf);
     m = bmat_init(M, N);
     for (i = 0; i < m.size; i++)
          fread(m.data + i, UINT8_SIZE, 1, pf);
     fclose(pf);
     return m;
}

void bmat_write(bmat m, char *filename)
{
     int i, magic = 666;
     FILE *pf = fopen(filename, "wb");
     assert(pf);
     fwrite(&magic, sizeof(int), 1, pf);
     fwrite(&(m.M), sizeof(int), 1, pf);
     fwrite(&(m.N), sizeof(int), 1, pf);
     for (i = 0; i < m.size; i++) {
          fwrite(m.data + i, UINT8_SIZE, 1, pf);
     }
     fclose(pf);
}

int bmat_foreach(bmat m, int *i, int *j)
{
     static int k=0;
     for (; k < m.size; k++) {
          if (bmat_get(m, 0, k)) {
               *i = k / m.N;
               *j = k % m.N;
               k++;
               return 1;
          }
     }
     k=0;
     return 0;
}

void bmat_print(bmat m)
{
     int i, j;
     for (i = 0; i < m.M; i++) {
          for (j = 0; j < m.N; j++) {
               printf("%d", bmat_get(m, i, j));
          } NL;
     } NL;
}

int bmat_count(bmat m)
{
     int i, count = 0;
     for (i = 0; i < m.size; i++)
          count += m.data[i];
     return count;
}

inline void bmat_set(bmat m, int i, int j)
{
     m.data[(i)*(m.N) + j] = 1;
}

inline void bmat_set_val(bmat mat, int i, int j, int val)
{
     assert(val <= 255);
     mat.data[i*mat.N + j] = val;
}

inline void bmat_clear(bmat m, int i, int j)
{
     m.data[(i)*(m.N) + j] = 0;
}

inline uint8_t bmat_get(bmat m, int i, int j)
{
     return m.data[(i)*(m.N) + j];
}

inline int bmat_bytes(bmat m)
{
     return UINT8_SIZE * m.size;
}

inline void bmat_free(bmat m)
{
     free(m.data);
}


//////////////////////////////////////////////////////////////////////////////
/* bmat bmat_init_pad(int M, int N, int pad) */
/* { */
/*      bmat m = {M, N, M*N, pad, NULL}; */
/*      m.data = (uint8_t *)malloc((M+2*pad)*(N+2*pad)*UINT8_SIZE); */
/*      assert(m.data); */
/*      return m; */
/* } */


/* bmat bmat_zeros_pad(int M, int N, int pad) */
/* { */
/*      bmat m = {M, N, M*N, pad, NULL}; */
/*      m.data = (uint8_t *)calloc((M+2*pad)*(N+2*pad), UINT8_SIZE); */
/*      assert(m.data); */
/*      return m; */
/* } */


/* bmat bmat_copy_pad(bmat m) */
/* { */
/*      bmat copy = bmat_init_pad(m.M , m.N, m.pad); */
/*      memcpy(copy.data, m.data, (m.M+2*m.pad)*(m.N+2*m.pad) * UINT8_SIZE); */
/*      return copy; */
/* } */


/* void bmat_set_pad(bmat m, int i, int j) */
/* { */
/*      const int p = m.pad; */
/*      m.data[(i+p)*(m.N+p) + j+p] = 1; */
/* } */


/* void bmat_clear_pad(bmat m, int i, int j) */
/* { */
/*      const int p = m.pad; */
/*      m.data[(i+p)*(m.N+p) + j+p] = 0; */
/* } */


/* uint8_t bmat_get_pad(bmat m, int i, int j) */
/* { */
/*      const int p = m.pad; */
/*      return m.data[(i+p)*(m.N+p) + j+p]; */
/* } */


/* bmat bmat_read_pad(char *fname, int pad) */
/* { */
/*      int i, j, M, N, magic; */
/*      bmat m; */
/*      uint8_t val; */
/*      FILE *pf = fopen(fname, "rb"); */

/*      assert(pf); */
/*      fread(&magic, sizeof(int), 1, pf); */
/*      assert(magic == 666); */
/*      fread(&M, sizeof(int), 1, pf); */
/*      fread(&N, sizeof(int), 1, pf); */
/*      m = bmat_zeros_pad(M, N, pad); */
/*      for (i = 0; i < M; i++) */
/*           for (j = 0; j < N; j++) { */
/*                fread(&val, UINT8_SIZE, 1, pf); */
/*                if (val) bmat_set_pad(m, i, j); */
/*           } */
/*      fclose(pf); */
/*      return m; */
/* } */


/* void bmat_print_pad(bmat m) */
/* { */
/*      int i, j; */
/*      const int p = m.pad; */

/*      for (i = -p; i < m.M+p; i++) { */
/*           for (j = -p; j < m.N+p; j++) */
/*                printf("%d", bmat_get_pad(m, i, j)); */
/*           printf("\n"); */
/*      } */
/* } */
