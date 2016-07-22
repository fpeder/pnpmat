#ifndef PNPMAT_H
#define PNPMAT_H

#include <stdint.h>

#define DIV64(x) ((x) >> 6)
#define MUL64(x) ((x) << 6)

#define UINT8_SIZE  sizeof(uint8_t)
#define UINT32_SIZE sizeof(uint32_t)
#define UINT64_SIZE sizeof(uint64_t)

typedef struct _pnpmat {
     int M;
     int N;
     int N_;
     int size;
     int size_;
     int KSIZE;
     int R;
     uint64_t *data; 
     uint8_t  *neigh; // = NULL;
     uint8_t  *neigh3; // = NULL;
     uint32_t *neigh5; // = NULL;
     uint64_t *neigh7; // = NULL;

} pnpmat;


pnpmat    pnpmat_light_init       (int M, int N, int R);
pnpmat    pnpmat_init             (int M, int N, int R);
void      pnpmat_set              (pnpmat mat, int i, int j);
void      pnpmat_clear            (pnpmat mat, int i, int j);
void      pnpmat_free             (pnpmat mat);
int       pnpmat_get              (pnpmat mat, int i, int j);
pnpmat    pnpmat_zeros            (int M, int N, int R);
pnpmat    pnpmat_read             (char *file, int R);
void      pnpmat_write            (pnpmat m, char *file);
int       pnpmat_count            (pnpmat mat);
void      pnpmat_print            (pnpmat mat);
int       pnpmat_foreach          (pnpmat mat, int *ii, int *jj);
int       pnpmat_foreach_in       (pnpmat mat, int *ii, int *jj,
                                    const int ksize);
int       pnpmat_foreach_zero_in  (pnpmat mat, int *ii, int *jj,
                                    const int ksize);
pnpmat    pnpmat_copy             (pnpmat mat);
uint64_t  pnpmat_get_neigh        (pnpmat mat, int i, int j);
void      pnpmat_print_all        (pnpmat mat);
void      pnpmat_print_neigh3     (pnpmat m, int i, int j); 
int       pnpmat_nneigh           (pnpmat mat);

pnpmat   pnpmat_erode      (pnpmat mat, const int ksize);
pnpmat   pnpmat_dilate     (pnpmat mat, const int ksize);

#endif /* PNPMAT_H */
