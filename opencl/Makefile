#DEBUG = -DDEBUG -ggdb

# Linux
# OPENCL = -lOpenCL
# OSX 10.6
OPENCL = -framework opencl
CPP = g++ -O3 -D__STDC_CONSTANT_MACROS -I../tinymt -I../jump ${DEBUG}
CC = gcc -O3 -std=c99 -I../tinymt -I../jump ${DEBUG}

.SUFFIXES:      .cl .clh .cli

test32: test32.cpp \
	parse_opt.o \
	file_reader.o \
	tinymt32.o \
	test32.cli
	${CPP} ${OPENCL} -o $@ test32.cpp \
	parse_opt.o \
	file_reader.o \
	tinymt32.o

sample32: sample32.cpp \
	parse_opt.o \
	file_reader.o \
	sample32.cli
	${CPP} ${OPENCL} -o $@ sample32.cpp \
	parse_opt.o \
	file_reader.o

test32_jump: test32_jump.cpp \
	parse_opt2.o \
	tinymt32.o \
	jump32.o \
	f2_polynomial.o \
	test32_jump.cli
	${CPP} ${OPENCL} -o $@ test32_jump.cpp \
	parse_opt2.o \
	tinymt32.o \
	f2_polynomial.o \
	jump32.o

compile: compile.cpp
	${CPP} ${OPENCL} -o $@ compile.cpp

#tinymt32.o:../release/tinymt/tinymt32.c
#	${CC} -c -o $@ ../release/tinymt/tinymt32.c

jump32.o:../jump/jump32.c
	${CC} -c -o $@ ../jump/jump32.c

f2_polynomial.o:../jump/f2-polynomial.c
	${CC} -c -o $@ ../jump/f2-polynomial.c

.cpp.o:
	${CPP} -c -o $@ $<

test32.cli: test32.cl \
	tinymt32.clh \
	tinymt32def.h \
	tinymt.clh

test32_jump.cli: test32_jump.cl \
	tinymt32_jump.clh \
	f2_polynomial.clh \
	tinymt32def.h \
	tinymt.clh

.cl.cli:
	cpp ${DEBUG} $< $@

clean:
	rm -rf *.o *~ *.dSYM *.cli