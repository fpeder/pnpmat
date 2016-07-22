#ifndef BMAT_H
#define BMAT_H

#include <stdint.h>


typedef struct _bmat {
     int M;     //rows
     int N;     //cols
     int size;
     int pad;
     uint8_t *data;
} bmat;

bmat    bmat_init_light  (int M, int N);
bmat    bmat_init        (int M, int N);
bmat    bmat_zeros       (int M, int N);
void    bmat_free        (bmat m);
void    bmat_set         (bmat m, int i, int j);
void    bmat_set_val     (bmat mat, int i, int j, int val);
void    bmat_clear       (bmat m, int i, int j);
uint8_t bmat_get         (bmat m, int i, int j);
int     bmat_foreach     (bmat m, int *i, int *j);
bmat    bmat_copy        (bmat m);
int     bmat_count       (bmat m);
bmat    bmat_read        (char *filename);
void    bmat_write       (bmat m, char *filename);
void    bmat_print       (bmat m);
int     bmat_bytes       (bmat m);

/* bmat     bmat_init_pad     (int M, int N, int pad); */
/* bmat     bmat_read_pad     (char *fname, int pad); */
/* void     bmat_print_pad    (bmat m); */
/* bmat     bmat_copy_pad     (bmat m); */
/* void     bmat_set_pad      (bmat m, int i, int j); */
/* void     bmat_clear_pad    (bmat m, int i, int j); */
/* uint8_t  bmat_get_pad      (bmat m, int i, int j); */

bmat    bmat_erode  (bmat m, const int sz);
bmat    bmat_dilate (bmat m, const int sz);


#endif /* BMAT_H */
