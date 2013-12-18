################################################################################
# Makefile that builds 'rlibrary/integration' with Ubuntu/Linux targets        #
################################################################################

# include build variables
include ../rlibrary-build-vars.mk

DEPENDS = $(addprefix ../,$(RINTEGRATION_H))
PREVOBJDIR = ../$(OBJDIR)
PREV_OBJ_OUT = -o $(PREVOBJDIR)/

all: $(PREVOBJDIR)/rintrg_ostream_str.o $(PREVOBJDIR)/rintrg_istream_str.o

$(PREVOBJDIR)/rintrg_ostream_str.o: rintrg_ostream_str.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rintrg_ostream_str.o rintrg_ostream_str.cpp

$(PREVOBJDIR)/rintrg_istream_str.o: rintrg_istream_str.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rintrg_istream_str.o rintrg_istream_str.cpp
