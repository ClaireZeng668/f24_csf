# CSF Assignment 2 Makefile
# You should not need to make any changes

.PHONY: solution.zip

CC = gcc
CFLAGS = -g -Wall -no-pie

ASMFLAGS = -g -no-pie -DASM_SOURCE

LDFLAGS = -no-pie

C_MAIN_SRCS = c_imgproc_main.c
C_MAIN_OBJS = $(C_MAIN_SRCS:.c=.o)

C_FN_SRCS = c_imgproc_fns.c
C_FN_OBJS = $(C_FN_SRCS:.c=.o)

C_COMMON_SRCS = image.c pnglite.c
C_COMMON_OBJS = $(C_COMMON_SRCS:.c=.o)

ASM_FN_SRCS = asm_imgproc_fns.S
ASM_FN_OBJS = $(ASM_FN_SRCS:.S=.o)

C_TEST_SRCS = tctest.c
C_TEST_OBJS = $(C_TEST_SRCS:.c=.o)

C_TEST_MAIN_SRCS = imgproc_tests.c
C_TEST_MAIN_OBJS = $(C_TEST_MAIN_SRCS:.c=.o)

EXES = c_imgproc c_imgproc_tests asm_imgproc asm_imgproc_tests

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

%.o : %.S
	$(CC) $(ASMFLAGS) -c $*.S -o $*.o

all : $(EXES)

c_imgproc : $(C_MAIN_OBJS) $(C_FN_OBJS) $(C_COMMON_OBJS)
	$(CC) $(LDFLAGS) -o $@ $+ -lz

c_imgproc_tests : $(C_TEST_MAIN_OBJS) $(C_FN_OBJS) $(C_TEST_OBJS) $(C_COMMON_OBJS)
	$(CC) $(LDFLAGS) -o $@ $+ -lz

asm_imgproc : $(C_MAIN_OBJS) $(ASM_FN_OBJS) $(C_COMMON_OBJS)
	$(CC) $(LDFLAGS) -o $@ $+ -lz

asm_imgproc_tests : $(C_TEST_MAIN_OBJS) $(ASM_FN_OBJS) $(C_TEST_OBJS) $(C_COMMON_OBJS)
	$(CC) $(LDFLAGS) -o $@ $+ -lz

# Use this target to prepare a zipfile to upload to Gradescope.
solution.zip :
	rm -f $@
	zip -9r $@ *.c *.h *.S Makefile README.txt

depend :
	$(CC) $(CFLAGS) -M $(C_MAIN_SRCS) $(C_FN_SRCS) $(C_COMMON_SRCS) $(C_TEST_SRCS) $(C_TEST_MAIN_SRCS) > depend.mak
	$(CC) $(ASMFLAGS) -M $(ASM_FN_SRCS) >> depend.mak

depend.mak :
	touch $@

clean :
	rm -f *.o $(EXES)

include depend.mak
