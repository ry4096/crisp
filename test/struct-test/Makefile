COMP = gcc


LIB_SOURCES = struct_test.c
BIN_SOURCES = struct_test.c struct_test_main.c
HEADERS = struct_test.h
OUTLIB = libstruct_test.so
OUTBIN = struct_test

LIB_OBJECTS = ${LIB_SOURCES:%.c=%.o}
BIN_OBJECTS = ${BIN_SOURCES:%.c=%.o}

OBJECTS = ${LIB_OBJECTS} ${BIN_OBJECTS}

all : ${OUTLIB} ${OUTBIN}

${OUTBIN} : ${BIN_OBJECTS}
	${COMP} -o ${OUTBIN} ${BIN_OBJECTS}

${OUTLIB} : ${LIB_OBJECTS}
	${COMP} -dynamic -shared -o ${OUTLIB} ${LIB_OBJECTS}

%.o : %.c
	${COMP} -o $@ -c $<

clean :
	rm -f ${OBJECTS}

spotless : clean
	rm -f ${OUTLIB}
	rm -f ${OUTBIN}
