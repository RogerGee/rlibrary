################################################################################
# Makefile that builds 'rlibrary/utility' with Ubuntu/Linux targets            #
################################################################################

# include build variables
include ../rlibrary-build-vars.mk

DEPENDS = $(addprefix ../,$(RUTILITY_H))
POD = ../$(OBJDIR)
PREV_OBJ_OUT = -o $(POD)/

BUILD_OBJ := $(BUILD_OBJ) -I..

all: $(POD)/rutil_def_memory.o $(POD)/rutil_strcmp.o $(POD)/rutil_strncmp.o $(POD)/rutil_strlen.o $(POD)/rutil_strcpy.o $(POD)/rutil_strncpy.o $(POD)/rutil_to_lower.o $(POD)/rutil_to_upper.o $(POD)/rutil_strip_whitespace.o

$(POD)/rutil_def_memory.o: rutil_def_memory.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rutil_def_memory.o rutil_def_memory.cpp

$(POD)/rutil_strcmp.o: rutil_strcmp.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rutil_strcmp.o rutil_strcmp.cpp

$(POD)/rutil_strncmp.o: rutil_strncmp.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rutil_strncmp.o rutil_strncmp.cpp

$(POD)/rutil_strlen.o: rutil_strlen.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rutil_strlen.o rutil_strlen.cpp

$(POD)/rutil_strcpy.o: rutil_strcpy.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rutil_strcpy.o rutil_strcpy.cpp

$(POD)/rutil_strncpy.o: rutil_strncpy.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rutil_strncpy.o rutil_strncpy.cpp

$(POD)/rutil_to_lower.o: rutil_to_lower.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rutil_to_lower.o rutil_to_lower.cpp

$(POD)/rutil_to_upper.o: rutil_to_upper.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rutil_to_upper.o rutil_to_upper.cpp

$(POD)/rutil_strip_whitespace.o: rutil_strip_whitespace.cpp $(DEPENDS)
	$(BUILD_OBJ) $(PREV_OBJ_OUT)rutil_strip_whitespace.o rutil_strip_whitespace.cpp
