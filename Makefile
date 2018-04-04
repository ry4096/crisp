# edit this part as needed.

COMP = gcc
ASM = nasm

ARCH = x86
ASM_FMT = elf32

DEBUG_OPT = -g
RELEASE_OPT = -DNDEBUG -O3
COMP_OPT = -Isrc
LINK_OPT = -rdynamic

###########################################################

CRISP_BIN = crisp
CRISP_SOURCE = inter.c inter_type.c token.c parser.c dlist.c \
					 main.c list.c stack.c inter_func.c inter_dl.c \
					 refc.c hash_table.c hash_map_push.c path.c
CRISP_ASM_SOURCE = shift_${ARCH}.asm

CRISP_OBJS_DEBUG = ${CRISP_SOURCE:%.c=debug/%.o} ${CRISP_ASM_SOURCE:%.asm=debug/%.o}
CRISP_OBJS_RELEASE = ${CRISP_SOURCE:%.c=release/%.o} ${CRISP_ASM_SOURCE:%.asm=release/%.o}

CRISP_LIBS = -ldl



###########################################################

BITS_BIN = bits_test
BITS_SOURCE = bits_test.c

BITS_OBJS_DEBUG = ${BITS_SOURCE:%.c=debug/%.o}
BITS_OBJS_RELEASE = ${BITS_SOURCE:%.c=release/%.o}


###########################################################


HASH_BIN = hash_test
HASH_SOURCE = hash_table.c hash_test.c hash_map_push.c list.c

HASH_OBJS_DEBUG = ${HASH_SOURCE:%.c=debug/%.o}
HASH_OBJS_RELEASE = ${HASH_SOURCE:%.c=release/%.o}


###########################################################


PARSER_BIN = parser_test
PARSER_SOURCE = dlist.c parser.c token.c refc.c parser_test.c

PARSER_OBJS_DEBUG = ${PARSER_SOURCE:%.c=debug/%.o}
PARSER_OBJS_RELEASE = ${PARSER_SOURCE:%.c=release/%.o}


###########################################################


TOKEN_BIN = token_test
TOKEN_SOURCE = token.c token_test.c

TOKEN_OBJS_DEBUG = ${TOKEN_SOURCE:%.c=debug/%.o}
TOKEN_OBJS_RELEASE = ${TOKEN_SOURCE:%.c=release/%.o}


###########################################################


BIN_NAMES = ${CRISP_BIN} ${BITS_BIN} ${HASH_BIN} ${PARSER_BIN} ${TOKEN_BIN}
DEBUG_BIN = ${BIN_NAMES:%=debug/%}
RELEASE_BIN = ${BIN_NAMES:%=release/%}
ALL_BIN = ${DEBUG_BIN} ${RELEASE_BIN}


ALL_OBJ_DEBUG = ${CRISP_OBJS_DEBUG} ${BITS_OBJS_DEBUG} ${HASH_OBJS_DEBUG} ${PARSER_OBJS_DEBUG} ${TOKEN_OBJS_DEBUG}
ALL_OBJ_RELEASE = ${CRISP_OBJS_RELEASE} ${BITS_OBJS_RELEASE} ${HASH_OBJS_RELEASE} ${PARSER_OBJS_RELEASE} ${TOKEN_OBJS_RELEASE}
ALL_OBJS = ${ALL_OBJ_DEBUG} ${ALL_OBJ_RELEASE}


all : ${ALL_BIN}
debug : ${DEBUG_BIN}
release : ${RELEASE_BIN}

clean :
	rm -f ${ALL_OBJS}

spotless : clean
	rm -f ${ALL_BIN}


###########################################################


debug/${CRISP_BIN} : ${CRISP_OBJS_DEBUG}
	${COMP} ${DEBUG_OPT} ${LINK_OPT} -o debug/${CRISP_BIN} ${CRISP_OBJS_DEBUG} ${CRISP_LIBS} 

release/${CRISP_BIN} : ${CRISP_OBJS_RELEASE}
	${COMP} ${RELEASE_OPT} ${LINK_OPT} -o release/${CRISP_BIN} ${CRISP_OBJS_RELEASE} ${CRISP_LIBS} 


debug/${BITS_BIN} : ${BITS_OBJS_DEBUG}
	${COMP} ${DEBUG_OPT} ${LINK_OPT} -o debug/${BITS_BIN} ${BITS_OBJS_DEBUG}

release/${BITS_BIN} : ${BITS_OBJS_RELEASE}
	${COMP} ${RELEASE_OPT} ${LINK_OPT} -o release/${BITS_BIN} ${BITS_OBJS_RELEASE}


debug/${HASH_BIN} : ${HASH_OBJS_DEBUG}
	${COMP} ${DEBUG_OPT} ${LINK_OPT} -o debug/${HASH_BIN} ${HASH_OBJS_DEBUG}

release/${HASH_BIN} : ${HASH_OBJS_RELEASE}
	${COMP} ${RELEASE_OPT} ${LINK_OPT} -o release/${HASH_BIN} ${HASH_OBJS_RELEASE}


debug/${PARSER_BIN} : ${PARSER_OBJS_DEBUG}
	${COMP} ${DEBUG_OPT} ${LINK_OPT} -o debug/${PARSER_BIN} ${PARSER_OBJS_DEBUG}

release/${PARSER_BIN} : ${PARSER_OBJS_RELEASE}
	${COMP} ${RELEASE_OPT} ${LINK_OPT} -o release/${PARSER_BIN} ${PARSER_OBJS_RELEASE}


debug/${TOKEN_BIN} : ${TOKEN_OBJS_DEBUG}
	${COMP} ${DEBUG_OPT} ${LINK_OPT} -o debug/${TOKEN_BIN} ${TOKEN_OBJS_DEBUG}

release/${TOKEN_BIN} : ${TOKEN_OBJS_RELEASE}
	${COMP} ${RELEASE_OPT} ${LINK_OPT} -o release/${TOKEN_BIN} ${TOKEN_OBJS_RELEASE}


###########################################################


debug/%.o : src/%.c
	${COMP} ${DEBUG_OPT} ${COMP_OPT} -c $< -o $@

debug/%.o : src/%.asm
	${ASM} -f ${ASM_FMT} $< -o $@

release/%.o : src/%.c
	${COMP} ${RELEASE_OPT} ${COMP_OPT} -c $< -o $@

release/%.o : src/%.asm
	${ASM} -f ${ASM_FMT} $< -o $@


