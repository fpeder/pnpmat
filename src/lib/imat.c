#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "imat.h"


imat imat_init(int M, int N)
{
     imat m = {M, N, M*N, NULL};

     m.data = (int *)malloc(sizeof(int)*m.size);
     return m;
}


imat imat_copy(imat m)
{
     imat copy = imat_init(m.M, m.N);

     memcpy(copy.data, m.data, m.size*sizeof(int));
     return copy;
}


imat imat_zeros(int M, int N)
{
     size_t i;
     imat m = imat_init(M, N);

     for (i=0; i<m.size; m.data[i++]=0);
     return m;
}


void imat_free(imat m)
{
     free(m.data);
}

imat imat_read(char *file)
{
     int magic; int M; int N;
     FILE *pf = fopen(file, "rb");
     fread(&magic, sizeof(int), 1, pf);
     assert(magic == 666);
     fread(&M, sizeof(int), 1, pf);
     fread(&N, sizeof(int), 1, pf);
     imat mat = {M, N, M*N, NULL};
     mat.data = (int *)malloc(M*N*sizeof(int));
     uint8_t val; int i;
     for (i=0; i<M*N; i++) {
          fread(&val, sizeof(uint8_t), 1, pf);
          mat.data[i] = (int)val;
     }
     fclose(pf);
     return mat;
}

void imat_set(imat m, int i, int j, int v)
{
     m.data[i*m.N + j] = v;
}

void imat_clear(imat m, int i, int j)
{
     m.data[i*m.N + j] = 0;
}

int imat_get(imat m, int i, int j)
{
     return m.data[i*m.N + j];
}

void imat_print(imat m)
{
     int i, j;
     for (i=0; i<m.M; i++) {
          for (j=0; j<m.N; j++) {
               //printf("%d", imat_get(m, i, j));
               printf("%d", m.data[i*m.N+j]);
          }
          printf("\n");
     }
     printf("\n");
}

int imat_bytes(imat m)
{
     return sizeof(int)*m.size;
}
