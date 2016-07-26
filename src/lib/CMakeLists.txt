if(OpenCL_FOUND)
	add_library(auz bmat.c bmat_proc.c bmat_clproc.c
        	        time.c clutil.c util.c
                	pnpmat.c pnpmat_proc.c)
        configure_file(gh_kernel.cl    ${CMAKE_BINARY_DIR} COPYONLY)
        configure_file(erode_kernel.cl ${CMAKE_BINARY_DIR} COPYONLY)
else()
	add_library(auz bmat.c bmat_proc.c time.c util.c
                        pnpmat.c pnpmat_proc.c)
endif()

configure_file(ltab.dat        ${CMAKE_BINARY_DIR} COPYONLY)
configure_file(rtab.dat        ${CMAKE_BINARY_DIR} COPYONLY)
