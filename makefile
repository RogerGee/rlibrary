################################################################################
# Makefile that builds 'rlibrary' with Ubuntu/Linux targets                    #
################################################################################
.PHONY: install uninstall clean test

# include build variables
include rlibrary-build-vars.mk

# object file lists
# (rlibrary/utility)
UTILITY_OBJ_files = $(addprefix $(OBJDIR)/,rutil_def_memory.o rutil_strcmp.o rutil_strncmp.o rutil_strlen.o rutil_strcpy.o rutil_strncpy.o rutil_to_lower.o rutil_to_upper.o rutil_strip_whitespace.o)
# (rlibrary/integration)
INTEGRATION_OBJ_files = $(addprefix $(OBJDIR)/,rintrg_ostream_str.o rintrg_istream_str.o)
# (rlibrary/impl)
IMPL_OBJ_files = $(addprefix $(OBJDIR)/,terminfo.o)
# (rlibrary)
OBJ_files = $(addprefix $(OBJDIR)/,rstream.o rstreammanip.o rstringstream.o rlasterr.o rfilename.o riodevice.o rstdio.o rfile.o) $(UTILITY_OBJ_files) $(INTEGRATION_OBJ_files) $(IMPL_OBJ_files)

# library file
LIB_rlibrary_name = librlibrary.a
LIB_rlibrary = $(addprefix $(LIBDIR)/,$(LIB_rlibrary_name))

$(LIB_rlibrary): $(OBJDIR) $(LIBDIR) $(OBJ_files)
	$(BUILD_LIB) $(LIB_rlibrary) $(OBJ_files)

$(OBJDIR)/rstream.o: rstream.cpp $(RSTREAM_H) $(RSTACK_H)
	$(BUILD_OBJ) $(OBJ_OUT)rstream.o rstream.cpp

$(OBJDIR)/rstreammanip.o: rstreammanip.cpp $(RSTREAMMANIP_H)
	$(BUILD_OBJ) $(OBJ_OUT)rstreammanip.o rstreammanip.cpp

$(OBJDIR)/rstringstream.o: rstringstream.cpp $(RSTRINGSTREAM_H)
	$(BUILD_OBJ) $(OBJ_OUT)rstringstream.o rstringstream.cpp

# [sys]
$(OBJDIR)/rlasterr.o: rlasterr.cpp rlasterr_posix.cpp $(RLASTERR_H)
	$(BUILD_OBJ) $(OBJ_OUT)rlasterr.o rlasterr.cpp -D RLIBRARY_BUILD_POSIX

# [sys]
$(OBJDIR)/rfilename.o: rfilename.cpp rfilename_posix.cpp $(RFILENAME_H) $(RLASTERR_H)
	$(BUILD_OBJ) $(OBJ_OUT)rfilename.o rfilename.cpp -D RLIBRARY_BUILD_POSIX

# [sys]
$(OBJDIR)/riodevice.o: riodevice.cpp riodevice_posix.cpp $(RIODEVICE_H) $(RLASTERR_H)
	$(BUILD_OBJ) $(OBJ_OUT)riodevice.o riodevice.cpp -D RLIBRARY_BUILD_POSIX

# [sys]
$(OBJDIR)/rstdio.o: rstdio.cpp rstdio_posix.cpp $(RSTDIO_H) $(RSTREAMMANIP_H)
	$(BUILD_OBJ) $(OBJ_OUT)rstdio.o rstdio.cpp -D RLIBRARY_BUILD_POSIX

# [sys]
$(OBJDIR)/rfile.o: rfile.cpp rfile_posix.cpp $(RFILE_H) $(RLASTERR_H)
	$(BUILD_OBJ) $(OBJ_OUT)rfile.o rfile.cpp -D RLIBRARY_BUILD_POSIX

# build other components of the library in subdirectories
$(UTILITY_OBJ_files): utility/*.cpp $(RUTILITY_H)
	make -C utility

$(INTEGRATION_OBJ_files): integration/*.cpp
	make -C integration

$(IMPL_OBJ_files): impl/*.cpp impl/*.h
	make -C impl

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

# install the library by copying the headers
# and the library file to the system local directory
install: $(LOCINCDIR)
	cp --verbose $(LIB_rlibrary) /usr/local/lib
	cp --verbose *.h $(LOCINCDIR)
	cp --verbose *.tcc $(LOCINCDIR)

uninstall:
	rm --verbose /usr/local/lib/$(LIB_rlibrary_name)
	rm --verbose -rf $(LOCINCDIR)

# create the directory in which the rlibrary headers
# are to be placed
$(LOCINCDIR):
	mkdir -p $(LOCINCDIR)

clean:
	rm --verbose $(OBJDIR)/*.o
	rm --verbose $(LIB_rlibrary)

test: $(LIB_rlibrary)
	make -C rtest
