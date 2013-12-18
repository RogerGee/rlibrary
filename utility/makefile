################################################################################
# Makefile that builds 'rlibrary/utility' with Ubuntu/Linux targets            #
################################################################################

# include build variables
include ../rlibrary-build-vars.mk

DEPENDS = $(addprefix ../,$(RUTILITY_H))
PREVOBJDIR = ../$(OBJDIR)
PREV_OBJ_OUT = -o $(PREVOBJDIR)/

all: $(PREVOBJDIR)/rutil_def_memory.o $(PREVOBJDIR)/rutil_strcmp.o

$(PREVOBJDIR)/rutil_def_memory.o: rutil_def_memory.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rutil_def_memory.o rutil_def_memory.cpp

$(PREVOBJDIR)/rutil_strcmp.o: rutil_strcmp.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rutil_strcmp.o rutil_strcmp.cpp
