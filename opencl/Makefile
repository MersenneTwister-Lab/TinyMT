#DEBUG = -DDEBUG -ggdb
#CL_OPT = -DINCLUDE_IMPOSSIBLE

# Linux
# OPENCL = -lOpenCL
# OSX 10.6
# OPENCL = -framework opencl

CPP = g++
CPP_OPTIONS = -Wall -Wextra -O3 -D__STDC_CONSTANT_MACROS \
	-I../tinymt -I../jump ${DEBUG} ${CL_OPT}
CC = gcc
CC_OPTIONS = -Wall -Wextra -O3 -std=c99 -I../tinymt -I../jump ${DEBUG}

KERNEL_OPT = -DKERNEL_PROGRAM
#KERNEL_OPT = -DKERNEL_PROGRAM -DHAVE_DOUBLE

TEST_EXE = test32 test32_jump test32_jump2 test64 test64_jump test64_jump2
SAMPLE_EXE = sample32 sample32_jump sample64 sample64_jump
ALL_EXE = ${TEST_EXE} ${SAMPLE_EXE}
VERSION = 1.0
DIR = TinyMT-opencl-src-${VERSION}

FILES = Makefile \
	FILES.txt \
	LICENSE.txt \
	CHANGE-LOG.txt \
	Makefile \
	check.sh \
	compile.cpp \
	doxygen.cfg \
	file_reader.cpp \
	file_reader.h \
	mainpage.txt \
	opencl_tools.hpp \
	readme-jp.html \
	readme.html \
	sample32.cl \
	sample32.cpp \
	sample32_jump.cl \
	sample32_jump.cpp \
	sample64.cl \
	sample64.cpp \
	sample64_jump.cl \
	sample64_jump.cpp \
	test32.cl \
	test32.cpp \
	test32_jump.cl \
	test32_jump.cpp \
	test32_jump2.cl \
	test32_jump2.cpp \
	test64.cl \
	test64.cpp \
	test64_jump.cl \
	test64_jump.cpp \
	test64_jump2.cl \
	test64_jump2.cpp \
	test_common.h \
	tinymt.clh \
	tinymt32.clh \
	tinymt32_jump.clh \
	tinymt32_jump_table.clh \
	tinymt32dc.0.2000.txt \
	tinymt32def.h \
	tinymt64.clh \
	tinymt64_jump.clh \
	tinymt64_jump_table.clh \
	tinymt64dc.0.2000.txt \
	tinymt64def.h

.SUFFIXES:  .cli .cl .clh

all: ${ALL_EXE}

test32: test32.cpp \
	file_reader.o \
	tinymt32.o \
	test32.cli
	${CPP} ${CPP_OPTIONS} ${OPENCL} -o $@ test32.cpp \
	file_reader.o \
	tinymt32.o

sample32: sample32.cpp \
	file_reader.o \
	sample32.cli
	${CPP} ${CPP_OPTIONS} ${OPENCL} -o $@ sample32.cpp \
	file_reader.o

test32_jump: test32_jump.cpp \
	tinymt32.o \
	jump32.o \
	f2_polynomial.o \
	test32_jump.cli
	${CPP} ${CPP_OPTIONS} ${OPENCL} -o $@ test32_jump.cpp \
	tinymt32.o \
	f2_polynomial.o \
	jump32.o

sample32_jump: sample32_jump.cpp \
	sample32_jump.cli
	${CPP} ${CPP_OPTIONS} ${OPENCL} -o $@ sample32_jump.cpp

test32_jump2: test32_jump2.cpp \
	tinymt32.o \
	jump32.o \
	f2_polynomial.o \
	test32_jump2.cli
	${CPP} ${CPP_OPTIONS} ${OPENCL} -o $@ test32_jump2.cpp \
	tinymt32.o \
	f2_polynomial.o \
	jump32.o

test64: test64.cpp \
	file_reader.o \
	tinymt64.o \
	test64.cli
	${CPP} ${CPP_OPTIONS} ${OPENCL} -o $@ test64.cpp \
	file_reader.o \
	tinymt64.o

sample64: sample64.cpp \
	file_reader.o \
	sample64.cli
	${CPP} ${CPP_OPTIONS} ${OPENCL} -o $@ sample64.cpp \
	file_reader.o

test64_jump: test64_jump.cpp \
	tinymt64.o \
	jump64.o \
	f2_polynomial.o \
	test64_jump.cli
	${CPP} ${CPP_OPTIONS} ${OPENCL} -o $@ test64_jump.cpp \
	tinymt64.o \
	f2_polynomial.o \
	jump64.o

sample64_jump: sample64_jump.cpp \
	sample64_jump.cli
	${CPP} ${CPP_OPTIONS} ${OPENCL} -o $@ sample64_jump.cpp

test64_jump2: test64_jump2.cpp \
	tinymt64.o \
	jump64.o \
	f2_polynomial.o \
	test64_jump2.cli
	${CPP} ${CPP_OPTIONS} ${OPENCL} -o $@ test64_jump2.cpp \
	tinymt64.o \
	f2_polynomial.o \
	jump64.o

compile: compile.cpp
	${CPP} ${CPP_OPTIONS} ${OPENCL} -o $@ compile.cpp

tinymt32.o:../tinymt/tinymt32.c
	${CC} ${CC_OPTIONS} -c -o $@ ../tinymt/tinymt32.c

tinymt64.o:../tinymt/tinymt64.c
	${CC} ${CC_OPTIONS} -c -o $@ ../tinymt/tinymt64.c

jump32.o:../jump/jump32.c
	${CC} ${CC_OPTIONS} -c -o $@ ../jump/jump32.c

jump64.o:../jump/jump64.c
	${CC} ${CC_OPTIONS} -c -o $@ ../jump/jump64.c

f2_polynomial.o:../jump/f2-polynomial.c
	${CC} ${CC_OPTIONS} -c -o $@ ../jump/f2-polynomial.c

.cpp.o:
	${CPP} ${CPP_OPTIONS} -c -o $@ $<

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

test64_jump2.cli: test64_jump.cl \
	tinymt64_jump.clh \
	tinymt64def.h \
	tinymt.clh

check:${TEST_EXE}
	./check.sh

.cl.cli:
	cpp ${DEBUG} ${KERNEL_OPT} $< $@

clean:
	rm -rf *.o *~ *.dSYM *.cli

doc:
	doxygen doxygen.cfg

tar.gz:
	mkdir ${DIR}
	mkdir ${DIR}/opencl
	cp ${FILES} ${DIR}/opencl
	tar czvf ${DIR}.tar.gz ${DIR}/*
	rm -rf ${DIR}

zip:
	mkdir ${DIR}
	mkdir ${DIR}/opencl
	cp ${FILES} ${DIR}/opencl
	zip -r ${DIR}.zip ${DIR}/*
	rm -rf ${DIR}
