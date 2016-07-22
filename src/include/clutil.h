#ifndef CLUTIL_H
#define CLUTIL_H

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)
#define GS_M 16
#define GS_N 16

char              *read_kernel        (char *);
void               cl_set_dim         (size_t *, size_t *, int,  int);
cl_command_queue   cl_create_queue    (cl_device_id *, cl_context *);
void               cl_print_err_log   (cl_program, cl_device_id);
cl_kernel          cl_compile_kernel  (cl_context, cl_device_id, char *, char *);

#endif /* CLUTIL_H */
