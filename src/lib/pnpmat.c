#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "time.h"
#include "util.h"
#include "pnpmat.h"

#define CLOCKWISE // comment this when guo-hall


pnpmat pnpmat_light_init(int M, int N, int ksize)
{
     pnpmat mat;
     mat.M      = M;
     mat.N      = N;
     mat.N_     = N % 64 ? DIV64(N) + 1 : DIV64(N);
     mat.size   = M * N;
     mat.size_  = M * mat.N_;
     mat.R      = ksize / 2;
     mat.KSIZE  = ksize;
     mat.neigh  = NULL;
     mat.neigh3 = NULL;
     mat.neigh5 = NULL;
     mat.neigh7 = NULL;
     return mat;
}

pnpmat pnpmat_init(int M, int N, int ksize)
{
     pnpmat mat = pnpmat_light_init(M, N, ksize);
     mat.data  = (uint64_t *)malloc(UINT64_SIZE * mat.size_);
     switch (mat.R) {
     case 1: mat.neigh  = (uint8_t  *)malloc(UINT8_SIZE  * mat.size); break;
     case 2: mat.neigh5 = (uint32_t *)malloc(UINT32_SIZE * mat.size); break;
     case 3: mat.neigh7 = (uint64_t *)malloc(UINT64_SIZE * mat.size); break;
     default: ;
     }
     return mat;
}

pnpmat pnpmat_zeros(int M, int N, int ksize)
{
     pnpmat mat = pnpmat_light_init(M, N, ksize);
     mat.data  = (uint64_t *)calloc(mat.size_, UINT64_SIZE);
     switch (mat.R) {
     case 1: mat.neigh  = (uint8_t  *)calloc(mat.size, UINT8_SIZE); break;
     case 2: mat.neigh5 = (uint32_t *)calloc(mat.size, UINT32_SIZE); break;
     case 3: mat.neigh7 = (uint64_t *)calloc(mat.size, UINT64_SIZE); break;
     default: ;
     }
     return mat;
}

pnpmat pnpmat_copy(pnpmat mat)
{
     pnpmat copy = pnpmat_init(mat.M, mat.N, mat.KSIZE);
     memcpy(copy.data, mat.data, mat.size_ * UINT64_SIZE);
     switch (mat.R) {
     case 1: memcpy(copy.neigh,  mat.neigh,  mat.size * UINT8_SIZE);  break;
     case 2: memcpy(copy.neigh5, mat.neigh5, mat.size * UINT32_SIZE); break;    
     case 3: memcpy(copy.neigh7, mat.neigh7, mat.size * UINT64_SIZE); break;
     default: ;
     }
     return copy;
}

static inline
void pnpmat_j_to_ko(int j, int *k, int *o)
{
     *k = j >> 6;
     *o = j & 63;
}

inline int pnpmat_get(pnpmat mat, int i, int j)
{
     int k, o;
     pnpmat_j_to_ko(j, &k, &o);
     return mat.data[i*mat.N_ + k] & (1ULL << (63-o)) ? 1 : 0;
}

uint64_t pnpmat_get_neigh(pnpmat mat, int i, int j)
{
     uint64_t n;
     int row = i * mat.N;
     
     switch (mat.R) {
     case 1: n = (uint64_t)mat.neigh [row + j]; break;
     case 2: n = (uint64_t)mat.neigh5[row + j]; break;
     case 3: n = (uint64_t)mat.neigh7[row + j]; break;
     default: n = 0;
     }
     return n;
}

inline int pnpmat_nneigh(pnpmat mat)
{
     return mat.KSIZE * mat.KSIZE - 1;
}

void pnpmat_set(pnpmat mat, int i, int j)
{
     int k, o;
     const int r=i*mat.N, N=mat.N;

     pnpmat_j_to_ko(j, &k, &o);
     mat.data[i*mat.N_ + k] |= 1ULL << (63 - o);

     switch (mat.R) {
     case 1: {
          uint8_t *m = mat.neigh;
#ifdef CLOCKWISE
          m[r-N + j-1] |= 1<<3;
          m[r-N + j  ] |= 1<<2;
          m[r-N + j+1] |= 1<<1;
          m[r   + j-1] |= 1<<4;
          m[r   + j+1] |= 1<<0;
          m[r+N + j-1] |= 1<<5;
          m[r+N + j  ] |= 1<<6;
          m[r+N + j+1] |= 1<<7;
#else
          m[r-N + j-1] |= 1;
          m[r-N + j  ] |= 1<<1;
          m[r-N + j+1] |= 1<<2;
          m[r   + j-1] |= 1<<3;
          m[r   + j+1] |= 1<<4;
          m[r+N + j-1] |= 1<<5;
          m[r+N + j  ] |= 1<<6;
          m[r+N + j+1] |= 1<<7;
#endif     
          break;
     }
     case 2: {
          // unrolling is good
          uint32_t *m = mat.neigh5;
          
          m[r-2*N + j-2] |= 1;
          m[r-2*N + j-1] |= 1<<1;
          m[r-2*N + j  ] |= 1<<2;
          m[r-2*N + j+1] |= 1<<3;
          m[r-2*N + j+2] |= 1<<4;
          
          m[r-  N + j-2] |= 1<<5;
          m[r-  N + j-1] |= 1<<6;
          m[r-  N + j  ] |= 1<<7;
          m[r-  N + j+1] |= 1<<8;
          m[r-  N + j+2] |= 1<<9;
          
          m[r     + j-2] |= 1<<10;
          m[r     + j-1] |= 1<<11;
          m[r     + j+1] |= 1<<12;
          m[r     + j+2] |= 1<<13;

          m[r+  N + j-2] |= 1<<14;
          m[r+  N + j-1] |= 1<<15;
          m[r+  N + j  ] |= 1<<16;
          m[r+  N + j+1] |= 1<<17;
          m[r+  N + j+2] |= 1<<18;

          m[r+2*N + j-2] |= 1<<19;
          m[r+2*N + j-1] |= 1<<20;
          m[r+2*N + j  ] |= 1<<21;
          m[r+2*N + j+1] |= 1<<22;
          m[r+2*N + j+2] |= 1<<23;

          break;
     }
     case 3: {
          //unrolling is good
          uint64_t *m = mat.neigh7;

          m[r-3*N + j-3] |= 1;
          m[r-3*N + j-2] |= 1ULL << 1;
          m[r-3*N + j-1] |= 1ULL << 2;
          m[r-3*N + j  ] |= 1ULL << 3;
          m[r-3*N + j+1] |= 1ULL << 4;
          m[r-3*N + j+2] |= 1ULL << 5;
          m[r-3*N + j+3] |= 1ULL << 6;

          m[r-2*N + j-3] |= 1ULL << 7;
          m[r-2*N + j-2] |= 1ULL << 8;
          m[r-2*N + j-1] |= 1ULL << 9;
          m[r-2*N + j  ] |= 1ULL << 10;
          m[r-2*N + j+1] |= 1ULL << 11;
          m[r-2*N + j+2] |= 1ULL << 12;
          m[r-2*N + j+3] |= 1ULL << 13;

          m[r-  N + j-3] |= 1ULL << 14;
          m[r-  N + j-2] |= 1ULL << 15;
          m[r-  N + j-1] |= 1ULL << 16;
          m[r-  N + j  ] |= 1ULL << 17;
          m[r-  N + j+1] |= 1ULL << 18;
          m[r-  N + j+2] |= 1ULL << 19;
          m[r-  N + j+3] |= 1ULL << 20;

          m[r     + j-3] |= 1ULL << 21;
          m[r     + j-2] |= 1ULL << 22;
          m[r     + j-1] |= 1ULL << 23;
          m[r     + j+1] |= 1ULL << 24;
          m[r     + j+2] |= 1ULL << 25;
          m[r     + j+3] |= 1ULL << 26;

          m[r+  N + j-3] |= 1ULL << 27;
          m[r+  N + j-2] |= 1ULL << 28;
          m[r+  N + j-1] |= 1ULL << 29;
          m[r+  N + j  ] |= 1ULL << 30;
          m[r+  N + j+1] |= 1ULL << 31;
          m[r+  N + j+2] |= 1ULL << 32;
          m[r+  N + j+3] |= 1ULL << 33;

          m[r+2*N + j-3] |= 1ULL << 34;
          m[r+2*N + j-2] |= 1ULL << 35;
          m[r+2*N + j-1] |= 1ULL << 36;
          m[r+2*N + j  ] |= 1ULL << 37;
          m[r+2*N + j+1] |= 1ULL << 38;
          m[r+2*N + j+2] |= 1ULL << 39;
          m[r+2*N + j+3] |= 1ULL << 40;

          m[r+3*N + j-3] |= 1ULL << 41;
          m[r+3*N + j-2] |= 1ULL << 42;
          m[r+3*N + j-1] |= 1ULL << 43;
          m[r+3*N + j  ] |= 1ULL << 44;
          m[r+3*N + j+1] |= 1ULL << 45;
          m[r+3*N + j+2] |= 1ULL << 46;
          m[r+3*N + j+3] |= 1ULL << 47;
          
          break;
     }
     }
}

void pnpmat_clear(pnpmat mat, int i, int j)
{
     int k, o;
     const int N = mat.N, r = i*mat.N;
     
     pnpmat_j_to_ko(j, &k, &o);
     mat.data [i*mat.N_ + k]  &= ~(1ULL << (63 - o));
     switch (mat.R) {
     case 1: {
          uint8_t *m = mat.neigh;
#ifdef CLOCKWISE
          mat.neigh[r-N + j-1] &= ~(1<<3);
          mat.neigh[r-N + j  ] &= ~(1<<2);
          mat.neigh[r-N + j+1] &= ~(1<<1);
          mat.neigh[r   + j-1] &= ~(1<<4);
          mat.neigh[r   + j+1] &= ~(1<<0);
          mat.neigh[r+N + j-1] &= ~(1<<5);
          mat.neigh[r+N + j  ] &= ~(1<<6);
          mat.neigh[r+N + j+1] &= ~(1<<7);
#else
          mat.neigh[r-N + j-1] &= ~1;
          mat.neigh[r-N + j  ] &= ~(1<<1);
          mat.neigh[r-N + j+1] &= ~(1<<2);
          mat.neigh[r   + j-1] &= ~(1<<3);
          mat.neigh[r   + j+1] &= ~(1<<4);
          mat.neigh[r+N + j-1] &= ~(1<<5);
          mat.neigh[r+N + j  ] &= ~(1<<6);
          mat.neigh[r+N + j+1] &= ~(1<<7);
#endif
          break;
     }
     case 2: {
          // unrolling is good
          uint32_t *m = mat.neigh5;
          
          m[r-2*N + j-2] &= ~1;
          m[r-2*N + j-1] &= ~(1<<1);
          m[r-2*N + j  ] &= ~(1<<2);
          m[r-2*N + j+1] &= ~(1<<3);
          m[r-2*N + j+2] &= ~(1<<4);
          
          m[r-  N + j-2] &= ~(1<<5);
          m[r-  N + j-1] &= ~(1<<6);
          m[r-  N + j  ] &= ~(1<<7);
          m[r-  N + j+1] &= ~(1<<8);
          m[r-  N + j+2] &= ~(1<<9);
          
          m[r         + j-2] &= ~(1<<10);
          m[r         + j-1] &= ~(1<<11);
          m[r         + j+1] &= ~(1<<12);
          m[r         + j+2] &= ~(1<<13);

          m[r+  N + j-2] &= ~(1<<14);
          m[r+  N + j-1] &= ~(1<<15);
          m[r+  N + j  ] &= ~(1<<16);
          m[r+  N + j+1] &= ~(1<<17);
          m[r+  N + j+2] &= ~(1<<18);

          m[r+2*N + j-2] &= ~(1<<19);
          m[r+2*N + j-1] &= ~(1<<20);
          m[r+2*N + j  ] &= ~(1<<21);
          m[r+2*N + j+1] &= ~(1<<22);
          m[r+2*N + j+2] &= ~(1<<23);

          break;
     }
     case 3: {
          uint64_t *m = mat.neigh7;

          m[r-3*N + j-3] &= ~1;
          m[r-3*N + j-2] &= ~(1ULL << 1);
          m[r-3*N + j-1] &= ~(1ULL << 2);
          m[r-3*N + j  ] &= ~(1ULL << 3);
          m[r-3*N + j+1] &= ~(1ULL << 4);
          m[r-3*N + j+2] &= ~(1ULL << 5);
          m[r-3*N + j+3] &= ~(1ULL << 6);

          m[r-2*N + j-3] &= ~(1ULL << 7);
          m[r-2*N + j-2] &= ~(1ULL << 8);
          m[r-2*N + j-1] &= ~(1ULL << 9);
          m[r-2*N + j  ] &= ~(1ULL << 10);
          m[r-2*N + j+1] &= ~(1ULL << 11);
          m[r-2*N + j+2] &= ~(1ULL << 12);
          m[r-2*N + j+3] &= ~(1ULL << 13);

          m[r-  N + j-3] &= ~(1ULL << 14);
          m[r-  N + j-2] &= ~(1ULL << 15);
          m[r-  N + j-1] &= ~(1ULL << 16);
          m[r-  N + j  ] &= ~(1ULL << 17);
          m[r-  N + j+1] &= ~(1ULL << 18);
          m[r-  N + j+2] &= ~(1ULL << 19);
          m[r-  N + j+3] &= ~(1ULL << 20);

          m[r     + j-3] &= ~(1ULL << 21);
          m[r     + j-2] &= ~(1ULL << 22);
          m[r     + j-1] &= ~(1ULL << 23);
          m[r     + j+1] &= ~(1ULL << 24);
          m[r     + j+2] &= ~(1ULL << 25);
          m[r     + j+3] &= ~(1ULL << 26);

          m[r+  N + j-3] &= ~(1ULL << 27);
          m[r+  N + j-2] &= ~(1ULL << 28);
          m[r+  N + j-1] &= ~(1ULL << 29);
          m[r+  N + j  ] &= ~(1ULL << 30);
          m[r+  N + j+1] &= ~(1ULL << 31);
          m[r+  N + j+2] &= ~(1ULL << 32);
          m[r+  N + j+3] &= ~(1ULL << 33);

          m[r+2*N + j-3] &= ~(1ULL << 34);
          m[r+2*N + j-2] &= ~(1ULL << 35);
          m[r+2*N + j-1] &= ~(1ULL << 36);
          m[r+2*N + j  ] &= ~(1ULL << 37);
          m[r+2*N + j+1] &= ~(1ULL << 38);
          m[r+2*N + j+2] &= ~(1ULL << 39);
          m[r+2*N + j+3] &= ~(1ULL << 40);

          m[r+3*N + j-3] &= ~(1ULL << 41);
          m[r+3*N + j-2] &= ~(1ULL << 42);
          m[r+3*N + j-1] &= ~(1ULL << 43);
          m[r+3*N + j  ] &= ~(1ULL << 44);
          m[r+3*N + j+1] &= ~(1ULL << 45);
          m[r+3*N + j+2] &= ~(1ULL << 46);
          m[r+3*N + j+3] &= ~(1ULL << 47);

          break;
     }
     default: ;
     }
}

void pnpmat_free(pnpmat mat)
{
     free(mat.data);
     switch (mat.R) {
     case 1: free(mat.neigh); break;
     case 2: free(mat.neigh5); break;
     case 3: free(mat.neigh7);  break;
     default: ;
     }
}

pnpmat pnpmat_read(char *file, int ksize)
{
     pnpmat mat;
     uint8_t val;
     int M, N, magic, i, j;
     FILE *pf = fopen(file, "rb"); assert(pf);
     fread(&magic, sizeof(int), 1, pf);
     assert(magic == 666);
     fread(&M, sizeof(int), 1, pf);
     fread(&N, sizeof(int), 1, pf);
     mat = pnpmat_zeros(M, N, ksize);
     for (i = 0; i < M; i++) {
          for (j = 0; j < N; j++) {
               fread(&val, sizeof(uint8_t), 1, pf);
               if (val) pnpmat_set(mat, i, j);
          }
     }
     fclose(pf);
     return mat;
}

void pnpmat_write(pnpmat m, char *file)
{
     int magic=666;
     FILE *pf = fopen(file, "wb"); assert(pf);
     fwrite(&magic, sizeof(int), 1, pf);
     fwrite(&(m.M), sizeof(int), 1, pf);
     fwrite(&(m.N), sizeof(int), 1, pf);
     for (int i=0; i<m.size_; i++) {
          uint64_t v1 = m.data[i];
          for (int c=0; c<64 && c<m.N; c++) {
               uint8_t v2 = (v1 >> (63-c)) & 1ULL;
               fwrite(&v2, sizeof(uint8_t), 1, pf);
          }
     }
}

int pnpmat_count(pnpmat mat)
{
     int i, count = 0;
     for(i = 0; i < mat.M * mat.N_; i++)
          count += __builtin_popcountll(mat.data[i]);
     return count;
}

void pnpmat_print(pnpmat mat)
{
     int i, j, c;
     uint64_t v;
     for (i=0; i<mat.M; i++) {
          for (j=0; j<mat.N_; j++) {
               v = mat.data[i*mat.N_ + j];
               for (c=0; c<64 && c<mat.N; c++)
                    printf("%d", (int)(v>>(63-c) & 1ULL));
          } NL;
     } NL;
}

int pnpmat_foreach(pnpmat mat, int *ii, int *jj)
{
     static int i=0; int o;
     static uint64_t mask=~0;
     uint64_t asd;
     while(i < mat.M * mat.N_) {
          asd = mat.data[i] & mask;
          if (asd) {
               o = __builtin_clzll(asd);
               mask = (1ULL << (63-o)) - 1;
               *ii = i / mat.N_;
               *jj = ((i % mat.N_) << 6) + o;
               return 1;
          } else {i++; mask=~0;}
     }
     i=0; mask=~0;
     return 0;
}

int pnpmat_foreach_in(pnpmat mat, int *ii, int *jj, const int ksize)
{
     static int i=0; int o;
     static uint64_t mask=~0;
     const int R = ksize/2;
     uint64_t asd;
     while(i < mat.M * mat.N_) {
          asd = mat.data[i] & mask;
          if (asd) {
               o = __builtin_clzll(asd);
               mask = (1ULL << (63-o)) - 1;
               *ii = i / mat.N_;
               *jj = ((i % mat.N_) << 6) + o;

               if (*ii < R || *ii >= mat.M-R || *jj < R || *jj >= mat.N-R)
                    continue;

               return 1;
          } else {i++; mask=~0;}
     }
     i=0; mask=~0;
     return 0;
}

int pnpmat_foreach_zero_in(pnpmat mat, int *ii, int *jj, const int ksize)
{
     static int i=0; int o;
     static uint64_t mask = ~0;
     const int R = ksize/2;
     uint64_t asd;

     while(i < mat.M * mat.N_) {
          asd = (~mat.data[i]) & mask;
          if (asd) {
               o = __builtin_clzll(asd);
               mask = (1ULL << (63-o)) - 1;
               *ii = i / mat.N_;
               *jj = ((i % mat.N_) << 6) + o;

               if (*ii < R || *ii >= mat.M-R || *jj < R || *jj >= mat.N-R)
                    continue;

               return 1;
          } else {i++; mask=~0;}
     }
     i=0; mask=~0;
     return 0;
}

void pnpmat_print_neigh3(pnpmat m, int i, int j) 
{
     uint8_t v = pnpmat_get_neigh(m, i, j);
     for (int i = 7; i >= 0; i--)
          printf("%d", (v >> i) &1);
     printf(" ");
}

void pnpmat_print_all(pnpmat mat)
{
     int i, j;
     int k, o, v;

     for (i=0; i<mat.M; i++) {
          for (j=0; j < mat.N; j++) {
               k = DIV64(j);
               o = j & (64-1);
               v = (mat.data[i*mat.N_ + k] >> (63-o)) &1;
               if (v) {
                    pnpmat_print(mat);
                    printf("%d %d -> ", i , j);
                    pnpmat_print_neigh3(mat, i, j);
                    printf("\n----------\n");}
          }
     }
}
