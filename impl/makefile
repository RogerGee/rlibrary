###################################################################
# Makefile that builds 'rlibrary/impl' with Ubuntu/Linux targets  #
###################################################################

include ../rlibrary-build-vars.mk

PREVOBJDIR = ../$(OBJDIR)
PREV_OBJ_OUT = -o $(PREVOBJDIR)/

OBJECT_FILES = terminfo.o
OBJECTS = $(addprefix $(PREVOBJDIR)/,$(OBJECT_FILES))
BUILD_OBJ := $(BUILD_OBJ) -I..

all: $(OBJECTS)

$(PREVOBJDIR)/terminfo.o: terminfo.cpp terminfo.h
	$(BUILD_OBJ) $(PREV_OBJ_OUT)terminfo.o terminfo.cpp
