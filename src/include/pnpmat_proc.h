#ifndef PNPMAT_PROC_H
#define PNPMAT_PROC_H

pnpmat  pnpmat_erode    (pnpmat mat, const int ksize);
pnpmat  pnpmat_dilate   (pnpmat mat, const int ksize);
void    pnpmat_rl       (pnpmat src, uint32_t *pt);
pnpmat  pnpmat_guohall  (pnpmat m);
void    pnpmat_moore    (pnpmat src, int *count, uint32_t *contour);

#endif /* PNPMAT_PROC_H */
