#DEBUG = -DDEBUG -ggdb

# Linux
# OPENCL = -lOpenCL
# OSX 10.6
OPENCL = -framework opencl
CPP = g++ -O3 -D__STDC_CONSTANT_MACROS -I../tinymt -I../jump ${DEBUG}
CC = gcc -O3 -std=c99 -I../tinymt -I../jump ${DEBUG}

.SUFFIXES:  .cli .cl .clh

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

sample32_jump: sample32_jump.cpp \
	parse_opt2.o \
	sample32_jump.cli
	${CPP} ${OPENCL} -o $@ sample32_jump.cpp \
	parse_opt2.o

test32_jump2: test32_jump2.cpp \
	tinymt32.o \
	jump32.o \
	f2_polynomial.o \
	test32_jump2.cli
	${CPP} ${OPENCL} -o $@ test32_jump2.cpp \
	tinymt32.o \
	f2_polynomial.o \
	jump32.o

test64: test64.cpp \
	parse_opt.o \
	file_reader.o \
	tinymt64.o \
	test64.cli
	${CPP} ${OPENCL} -o $@ test64.cpp \
	parse_opt.o \
	file_reader.o \
	tinymt64.o

test64_jump: test64_jump.cpp \
	parse_opt2.o \
	tinymt64.o \
	jump64.o \
	f2_polynomial.o \
	test64_jump.cli
	${CPP} ${OPENCL} -o $@ test64_jump.cpp \
	parse_opt2.o \
	tinymt64.o \
	f2_polynomial.o \
	jump64.o

test64_jump2: test64_jump2.cpp \
	tinymt64.o \
	jump64.o \
	f2_polynomial.o \
	test64_jump2.cli
	${CPP} ${OPENCL} -o $@ test64_jump2.cpp \
	tinymt64.o \
	f2_polynomial.o \
	jump64.o

compile: compile.cpp
	${CPP} ${OPENCL} -o $@ compile.cpp

tinymt32.o:../tinymt/tinymt32.c
	${CC} -c -o $@ ../tinymt/tinymt32.c

tinymt64.o:../tinymt/tinymt64.c
	${CC} -c -o $@ ../tinymt/tinymt64.c

jump32.o:../jump/jump32.c
	${CC} -c -o $@ ../jump/jump32.c

jump64.o:../jump/jump64.c
	${CC} -c -o $@ ../jump/jump64.c

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
	tinymt32def.h \
	tinymt.clh

test64.cli: test64.cl \
	tinymt64.clh \
	tinymt64def.h \
	tinymt.clh

test64_jump.cli: test64_jump.cl \
	tinymt64_jump.clh \
	tinymt64def.h \
	tinymt.clh

.cl.cli:
	cpp ${DEBUG} $< $@

clean:
	rm -rf *.o *~ *.dSYM *.cli