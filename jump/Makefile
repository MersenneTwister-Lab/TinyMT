#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1
#M64 = -m64 -L/usr/local/64bit/lib -I/usr/local/64bit/include


CC = gcc -Wall -O3 -std=c99 -Wmissing-prototypes -Wextra \
	-msse3  \
	-L. -I../tinymt $(M64) $(DDEBUG) $(DPG)

CPPOPT = -Wall -Wextra -O3 $(M64) $(INC) -I../tinymt \
	$(DDEBUG) -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS
CPP = g++ $(CPPOPT)

# comment out next line if you are using NTL with gf2x
# NTL_OPT = -lgf2x
# comment out next line if you are using NTL with gmp
# NTL_OPT = -lgmp

FILES = Makefile \
	doxygen.cfg \
	jump32.c \
	jump32.h \
	jump64.c \
	jump64.h \
	jump_test32.c \
	jump_test64.c \
	mainpage.txt \
	ntl_jump_test32.cpp \
	f2-polynomial.c \
	f2-polynomial.h \
	readme.html \
	readme-jp.html \
	sample.c \
	CHANGE-LOG.txt

VERSION = 1.2
DIR = TinyMTJump-src-${VERSION}

all: jump_test32 jump_test64

jump_test32: jump_test32.c ../tinymt/tinymt32.o f2-polynomial.o jump32.o
	${CC} -o $@ jump_test32.c ../tinymt/tinymt32.o f2-polynomial.o jump32.o

jump_test64: jump_test64.c ../tinymt/tinymt64.o f2-polynomial.o jump64.o
	${CC} -o $@ jump_test64.c ../tinymt/tinymt64.o f2-polynomial.o jump64.o

ntl_jump_test32: ntl_jump_test32.cpp ../tinymt/tinymt32.o
	${CPP} -o $@ ntl_jump_test32.cpp ../tinymt/tinymt32.o -lntl ${NTL_OPT}

calc_jump_poly: calc_jump_poly.cpp
	${CPP} -o $@ calc_jump_poly.cpp -lntl ${NTL_OPT}

sample: sample.c ../tinymt/tinymt32.o f2-polynomial.o jump32.o
	${CC} -o $@ sample.c ../tinymt/tinymt32.o f2-polynomial.o jump32.o

doc: mainpage.txt doxygen.cfg
	doxygen doxygen.cfg

.PHONY : tar.gz
tar.gz: $(FILES)
	mkdir -p ${DIR}/jump
	cp ${FILES} ${DIR}/jump
	tar czvf ${DIR}.tar.gz ${DIR}/*
	rm -rf ${DIR}

.PHONY : zip
zip: $(FILES)
	mkdir -p ${DIR}/jump
	cp ${FILES} ${DIR}/jump
	zip -r ${DIR}.zip ${DIR}/*
	rm -rf ${DIR}

.c.o:
	${CC} -c $<

clean:
	rm -rf *.o *~ *.dSYM
