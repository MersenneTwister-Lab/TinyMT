#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1

# uncomment next line if you installed NTL with gf2x or gmp
#OPTLIB = -lgf2x
#OPTLIB = -lgmp

INC = -I../include

CC = gcc -Wall -O3 -std=c99 -Wmissing-prototypes -Wextra $(DDEBUG)

CPPOPT = -Wall -Wextra -O3 $(M64) $(INC) \
	$(DDEBUG) -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS
CPP = g++ $(CPPOPT)
#CPP = icc $(CPPOPT)

LINKOPT += -lntl $(OPTLIB)

all: tinymt32dc tinymt64dc getid

tinymt32dc:  tinymt32dc.cpp tinymt32search.hpp output.hpp parse_opt.o
	${CPP} -o $@ tinymt32dc.cpp parse_opt.o ${LINKOPT}

tinymt64dc:  tinymt64dc.cpp tinymt64search.hpp output.hpp parse_opt.o
	${CPP} -o $@ tinymt64dc.cpp parse_opt.o ${LINKOPT}

getid: getid.cpp
	${CPP} -o $@ getid.cpp ${LINKOPT}

doc:
	doxygen doxygen.cfg

.cpp.o:
	${CPP} -c $<

.c.o:
	${CC} -c $<

clean:
	rm -rf *.o *~ *.dSYM
