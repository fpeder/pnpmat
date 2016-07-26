#include <stdio.h>
#include <unistd.h>

#include "auz.h"

static char *usage = "usage: %s -i img -k size [-n iter] [-v]\n";
static char *kfile = "./build/erode_kernel.cl";
static char *func[] = {"erode_smem3", "erode_smem5", "erode_smem7"};

int niter=1, ksize=0, verbose=0;
char *fname=NULL, *outfile=NULL;
int count;

void parse_args(int argc, char **argv)
{
     extern char *optarg;
     int c, err=0;
     while ((c = getopt(argc, argv, "i:k:n:s:vt")) != -1) {
          switch (c) {
          case 'i': fname = optarg; break;
          case 'k': ksize = atoi(optarg); break;
          case 'n': niter = atoi(optarg); break;
          case 'v': verbose = 1; break;
          case '?': err = 1; break;
          }
     }
     if (!fname || !ksize || err) {
          fprintf(stderr, usage, argv[0]);
          exit(-1);
     }
}

int main(int argc, char **argv)
{
     bmat src, dst;

     parse_args(argc, argv);
     src = bmat_read(fname);
     count = bmat_count(src);
     dst = cl_erode(src, kfile, "erode", ksize);

     if (verbose) bmat_print(dst);
     bmat_write(dst, "e2.dat");

     switch (ksize) {
     case 3:
          dst = cl_erode(src, kfile, func[0], ksize);
          break;
     case 5:
          dst = cl_erode(src, kfile, func[1], ksize);
          break;
     case 7:
          dst = cl_erode(src, kfile, func[2], ksize);
          break;
     }

     if (verbose) bmat_print(dst);
     bmat_write(dst, "e3.dat");
     bmat_free(src);
     bmat_free(dst);
     return 0;
}



