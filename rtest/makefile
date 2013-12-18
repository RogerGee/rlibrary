#########################################################################
# Makefile for rlibrary/rtest, the RLibrary testing module              #
#########################################################################

# assume this target has the rlibrary as its dependecy; in other words
# this makefile should never be executed directly
rtest: rtest.cpp ../lib/librlibrary.a
	compile rtest -I.. -L../lib -lrlibrary -g
