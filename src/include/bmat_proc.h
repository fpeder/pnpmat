#ifndef BMAT_PROC_H
#define BMAT_PROC_H

bmat bmat_erode(bmat src, const int ksize);
bmat bmat_dilate(bmat src, const int ksize);
void bmat_moore(bmat mat, int *count, uint32_t *contour);
void bmat_rl(bmat src, uint32_t *rl);
bmat bmat_guohall(bmat m);

#endif /* BMAT_PROC_H */
