#for GNU make

FILES = FILES.txt \
	LICENSE.txt \
	CHANGE-LOG.txt \
	README.txt \

VERSION = 1.0
DIR = tinymt-src-${VERSION}

.PHONY : tar.gz
tar.gz: $(FILES)
	mkdir ${DIR}
	tar czvf ${DIR}.tar.gz ${DIR}/*
	rm -rf ${DIR}

.PHONY : zip
zip: $(FILES)
	mkdir ${DIR}
	rm ${DIR}.zip
	zip -r ${DIR}.zip ${DIR}/*
	rm -rf ${DIR}

clean:
	rm -f *.o *~
