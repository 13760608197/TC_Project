TYPE = WM
CFLAGS += -DTC3000_DE -DTC3000_WM

INC_DIR += $(MAKEROOT)/src/application
INC_DIR += $(MAKEROOT)/src/drivers
INC_DIR += $(MAKEROOT)/src/gui
INC_DIR += $(MAKEROOT)/src/net

SRC = $(wildcard ${SRC_DIR}/application/*.c)
SRC += $(wildcard ${SRC_DIR}/gui/*.c)
SRC += $(wildcard ${SRC_DIR}/drivers/*.c)
SRC += $(wildcard ${SRC_DIR}/net/*.c)
