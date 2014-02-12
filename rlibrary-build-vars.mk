################################################################################
# Makefile that contains build variables for 'rlibrary'                        #
################################################################################

# directories
OBJDIR = lib/rlibrary-obj
LIBDIR = lib
LOCINCDIR = /usr/local/include/rlibrary
LOCLIBDIR = /usr/local/lib

# compiler options
BUILD = g++ -Wall -Werror -Wextra -Wshadow -Wfatal-errors -Wno-unused-variable -pedantic-errors --std=gnu++0x
BUILD_OBJ = g++ -c -Wall -Werror -Wextra -Wshadow -Wfatal-errors -Wno-unused-variable -pedantic-errors --std=gnu++0x
BUILD_LIB = ar cr
OBJ_OUT = -o $(OBJDIR)/

# header dependency lists
#  (stand-alone header files)
RTYPESTYPES_H = rtypestypes.h
RNODE_H = rnode.h
RFILEMODE_H = rfilemode.h
#  (header files with dependencies)
RALLOCATOR_H = rallocator.h $(RTYPESTYPES_H)
RSTRING_H = rstring.h rstring.tcc $(RTYPESTYPES_H)
RERROR_H = rerror.h $(RSTRING_H)
RLASTERR_H = rlasterr.h $(RERROR_H)
RQUEUE_H = rqueue.h $(RERROR_H) $(RALLOCATOR_H)
RSTACK_H = rstack.h $(RERROR_H) $(RALLOCATOR_H)
RDYNARRAY_H = rdynarray.h rdynarray.tcc $(RALLOCATOR_H) $(RERROR_H)
RLIST_H = rlist.h rlist.tcc $(RERROR_H) $(RTYPESTYPES_H) $(RNODE_H)
RSET_H = rset.h $(RLIST_H)
RSTREAM_H = rstream.h $(RSTRING_H) $(RQUEUE_H) $(RSET_H)
RSTREAMMANIP_H = rstreammanip.h $(RSTREAM_H)
RSTRINGSTREAM_H = rstringstream.h $(RSTREAM_H)
RUTILITY_H = rutility.h $(RTYPESTYPES_H)
RINTEGRATION_H = rintegration.h $(RSTRING_H)
RFILENAME_H = rfilename.h $(RSTRING_H) $(RDYNARRAY_H) $(RFILEMODE_H)
RRESOURCE_H = rresource.h rresource.tcc $(RTYPESTYPES_H) $(RERROR_H)
RIODEVICE_H = riodevice.h $(RRESOURCE_H) $(RSTRING_H) $(RSTACK_H)
RSTDIO_H = rstdio.h $(RIODEVICE_H)
RFILE_H = rfile.h $(RIODEVICE_H) $(RFILEMODE_H)
