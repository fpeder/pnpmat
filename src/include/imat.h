#ifndef IMAT_H
#define IMAT_H

typedef struct _imat {
     int M;
     int N;
     int size;
     int *data;
} imat;

imat imat_init(int M, int N);
imat imat_zeros(int M, int N);
imat imat_copy(imat m);
void imat_free(imat m);
void imat_set(imat m, int i, int j, int v);
void imat_clear(imat m, int i, int j);
int  imat_get(imat m, int i, int j);
void imat_print(imat m);
int  imat_bytes(imat m);
imat imat_read(char *filename);

#endif /* IMAT_H */
