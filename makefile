################################################################################
# Makefile that builds 'rlibrary' with Ubuntu/Linux targets                    #
################################################################################
.PHONY: install clean test

# include build variables
include rlibrary-build-vars.mk

# object file lists
# (rlibrary/utility)
UTILITY_OBJ_files = $(addprefix $(OBJDIR)/,rutil_def_memory.o rutil_strcmp.o)
# (rlibrary/integration)
INTEGRATION_OBJ_files = $(addprefix $(OBJDIR)/,rintrg_ostream_str.o rintrg_istream_str.o)
# (rlibrary)
OBJ_files = $(addprefix $(OBJDIR)/,rstream.o rstringstream.o rlasterr.o rfilename.o riodevice.o rstdio.o) $(UTILITY_OBJ_files) $(INTEGRATION_OBJ_files)

# library file
LIB_rlibrary = $(addprefix $(LIBDIR)/,librlibrary.a)

$(LIB_rlibrary): $(OBJDIR) $(LIBDIR) $(OBJ_files)
	$(BUILD_LIB) $(LIB_rlibrary) $(OBJ_files)

$(OBJDIR)/rstream.o: rstream.cpp $(RSTREAM_H) $(RSTACK_H)
	$(BUILD_OBJ) $(OBJ_OUT)rstream.o rstream.cpp

$(OBJDIR)/rstringstream.o: rstringstream.cpp $(RSTRINGSTREAM_H)
	$(BUILD_OBJ) $(OBJ_OUT)rstringstream.o rstringstream.cpp

# [sys]
$(OBJDIR)/rlasterr.o: rlasterr.cpp rlasterr_posix.cpp $(RLASTERR_H)
	$(BUILD_OBJ) $(OBJ_OUT)rlasterr.o rlasterr.cpp -D RLIBRARY_BUILD_POSIX

$(UTILITY_OBJ_files): utility/*.cpp
	make -C utility

$(INTEGRATION_OBJ_files): integration/*.cpp
	make -C integration

# [sys]
$(OBJDIR)/rfilename.o: rfilename.cpp rfilename_posix.cpp $(RFILENAME_H) $(RLASTERR_H)
	$(BUILD_OBJ) $(OBJ_OUT)rfilename.o rfilename.cpp -D RLIBRARY_BUILD_POSIX

# [sys]
$(OBJDIR)/riodevice.o: riodevice.cpp riodevice_posix.cpp $(RIODEVICE_H) $(RLASTERR_H)
	$(BUILD_OBJ) $(OBJ_OUT)riodevice.o riodevice.cpp -D RLIBRARY_BUILD_POSIX

# [sys]
$(OBJDIR)/rstdio.o: rstdio.cpp rstdio_posix.cpp $(RSTDIO_H)
	$(BUILD_OBJ) $(OBJ_OUT)rstdio.o rstdio.cpp -D RLIBRARY_BUILD_POSIX

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

# create the directory in which the rlibrary headers
# are to be placed
$(LOCINCDIR):
	mkdir -p $(LOCINCDIR)

clean:
	rm --verbose $(OBJDIR)/*.o
	rm --verbose $(LIB_rlibrary)

test: $(LIB_rlibrary)
	make -C rtest
