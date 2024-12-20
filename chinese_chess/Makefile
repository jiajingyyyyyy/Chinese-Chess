# 交叉编译工具链前缀
CROSS_COMPILE := ~/embedded_lab/workspace/gcc-aarch64/bin/aarch64-none-linux-gnu-

# 编译器
CC := $(CROSS_COMPILE)gcc

# 编译选项
CFLAGS := -Wall -O2 -march=armv8-a

# 链接选项
LDFLAGS := -Wall

# 头文件包含路径
INCLUDE := -I../common/external/include -I/usr/include/SDL2

# 库文件路径和库
LIB := -L../common/external/lib -L/usr/lib/aarch64-linux-gnu/ -lSDL2 -lSDL2_ttf -lSDL2_image -ljpeg -lfreetype -lpng -lasound -lz -lc -lm

# 可执行文件名
EXENAME := lab2

# 源文件
EXESRCS := main.c ../common/graphic.c ../common/touch.c ../common/image.c ../common/task.c

# 目标文件
EXEOBJS := $(patsubst %.c, %.o, $(EXESRCS))

# 包含通用规则
include ../common/rules.mk