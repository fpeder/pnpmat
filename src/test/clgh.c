#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "auz.h"

void usage(char *prog)
{
     printf("usage: %s <img>\n", prog);
     exit(-1);
}

int main(int argc, char **argv)
{
     bmat src, dst;
     char *ker_file, *img_file;

     if (argc != 2)
          usage(argv[0]);

     ker_file = "./build/gh_kernel.cl";
     img_file = argv[1];
     src = bmat_read(img_file);

     dst = cl_gh(src, ker_file, "gh");
     //bmat_print(dst);
     bmat_write(dst, "gh2.dat");

     dst = cl_gh(src, ker_file, "gh_smem");
     //bmat_print(dst);
     bmat_write(dst, "gh3.dat"); NL;

     bmat_free(dst);
     bmat_free(src);
     return 0;
}
