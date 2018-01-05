MAKEROOT = $(shell pwd)
# 主程序Makefile
CC := arm-linux-gcc

MINIGUI_PATH=/opt/TC_Project/MiniGUI/minigui3.0.12_20141021/Sources/output
COMPILER_PATH=/usr/local/arm_linux_4.2/arm-none-linux-gnueabi
KERNERL_PATH=/opt/TC_Project/Kernel/kernel/linux-2.6.35.4

# 在指定目录下生成的应用程序
EXE = v_$(TYPE)
BIN_TARGET = ${BIN_DIR}/${EXE}
CFLAGS = -muclibc -g 

SRC_DIR = $(MAKEROOT)/src
OBJ_DIR = $(MAKEROOT)/obj_$(TYPE)
BIN_DIR = $(MAKEROOT)/out

# INC_DIR 目录下为相应库的头文件
INC_DIR = \
		  $(MINIGUI_PATH)/include \
		  $(KERNERL_PATH)/include \
		  $(COMPILER_PATH)/include \
		  $(COMPILER_PATH)/include/libpng12

include evn.mk

CFLAGS += ${addprefix -I,${INC_DIR}}

# wildcard:扩展通配符，notdir;去除路径，patsubst;替换通配符

OBJ = $(patsubst %.c,${OBJ_DIR}/%.o,$(notdir ${SRC}))
DEPS = $(patsubst %.c, ${OBJ_DIR}/%.d, $(notdir ${SRC}))

# 链接路径
# -Xlinker编译时可重复查找依赖库，和库的次序无关
LIB_DIR = \
		  $(MINIGUI_PATH)/lib \
		  $(MAKEROOT)/src/libs \
		  $(COMPILER_PATH)/lib

XLINKER = -Xlinker "-(" -lts -lfreetype -lpng -lminigui_ths -ljpeg \
		  -ldl -lm -lpthread -lsqlite3 -lmp3 -lmad -Xlinker "-)" 

export CC LIB_DIR CFLAGS OBJ_DIR INC_DIR DEPS
# $@：表示目标文件，$^：表示所有的依赖文件，$<：表示第一个依赖文件，$?：表示比目标还要新的依赖文件列表
all: make_C ${BIN_TARGET} 

make_C:
	@mkdir -p ${BIN_DIR}
	@mkdir -p ${OBJ_DIR}
	@make -C src 

# 在指定目录下，将所有的.c文件编译为相应的同名.o文件
${BIN_TARGET}:${OBJ}
	@$(CC) -muclibc -g -o $@ $(OBJ) ${addprefix -L,${LIB_DIR}} ${XLINKER}

debug:
	make -C src debug

.PHONY:clean
clean:
	@-rm -rf ${BIN_TARGET} obj_* ${BIN_DIR}
