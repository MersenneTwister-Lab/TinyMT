#for GNU make

FILES = LICENSE.txt \
	CHANGE-LOG.txt \
	readme.html \
	readme-jp.html

DC_FILES = LICENSE.txt \
	Makefile \
	mainpage.txt \
	tinymt32dc.0.20.txt \
	tinymt64dc.0.20.txt \
	doxygen.cfg \
	output.hpp \
	tinymt32dc.cpp \
	tinymt64dc.cpp \
	parse_opt.cpp \
	tinymt32search.hpp \
	tinymt64search.hpp \
	getid.cpp \
	parse_opt.h

TINYMT_FILES = LICENSE.txt \
	Makefile \
	check32.c \
	check32.out.txt \
	check64.c \
	check64.out.txt \
	doxygen.cfg \
	mainpage.txt \
	tinymt32.c \
	tinymt32.h \
	tinymt64.c \
	tinymt64.h

VERSION = 1.1.1
DIR = TinyMT-src-${VERSION}

.PHONY : tar.gz
tar.gz: $(FILES)
	mkdir ${DIR}
	cp ${FILES} ${DIR}
	mkdir ${DIR}/dc
	mkdir ${DIR}/dc/include
	mkdir ${DIR}/dc/src
	(cd dc; cp -r include ../${DIR}/dc)
	(cd dc/src; cp ${DC_FILES} ../../${DIR}/dc/src)
	mkdir ${DIR}/tinymt
	(cd tinymt; cp ${TINYMT_FILES} ../${DIR}/tinymt)
	tar czvf ${DIR}.tar.gz ${DIR}/*
	rm -rf ${DIR}

.PHONY : zip
zip: $(FILES)
	mkdir ${DIR}
	cp ${FILES} ${DIR}
	mkdir ${DIR}/dc
	mkdir ${DIR}/dc/include
	mkdir ${DIR}/dc/src
	(cd dc; cp -r include ../${DIR}/dc)
	(cd dc/src; cp ${DC_FILES} ../../${DIR}/dc/src)
	mkdir ${DIR}/tinymt
	(cd tinymt; cp ${TINYMT_FILES} ../${DIR}/tinymt)
	zip -r ${DIR}.zip ${DIR}/*
	rm -rf ${DIR}

clean:
	rm -f *.o *~
