STLINUXTV_TOPDIR=$(STLINUXTV_SOURCE_PATH)

STM_ARCH ?= arm
STM_TARGET_ARCH ?= armv7

STM_BASE_PREFIX ?= /opt/STM/STLinux-2.4
TARGET_DIR ?= $(STM_BASE_PREFIX)/devkit/$(STM_TARGET_ARCH)/target

PKG_CONFIG ?= $(STM_BASE_PREFIX)/host/bin/pkg-config
PKG_CONFIG_LIBDIR ?= $(STM_BASE_PREFIX)/devkit/armv7/target/usr/lib/pkgconfig
PKG_CONFIG_SYSROOT_DIR ?= $(STM_BASE_PREFIX)/devkit/armv7/target

INC		= -I $(STLINUXTV_TOPDIR) \
                  -I $(PKG_CONFIG_SYSROOT_DIR)/usr/include/ \
		  `PKG_CONFIG_LIBDIR=$(PKG_CONFIG_LIBDIR) \
			PKG_CONFIG_SYSROOT_DIR=$(PKG_CONFIG_SYSROOT_DIR) \
			$(PKG_CONFIG) --cflags \
			directfb`
CFLAGS += -Wall $(INC)
LDFLAGS += `PKG_CONFIG_LIBDIR=$(PKG_CONFIG_LIBDIR) \
			PKG_CONFIG_SYSROOT_DIR=$(PKG_CONFIG_SYSROOT_DIR) \
			$(PKG_CONFIG) --libs \
			directfb`
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CROSS_COMPILE)gcc
TARGET		= v4l2lut8
TARGET2		= v4l2lut8_2
SRCS		= $(addprefix ./,$(TARGET).c)
SRCS2		= $(addprefix ./,$(TARGET2).c)
OBJS		= $(patsubst %.c,%.o,$(SRCS))
OBJS2		= $(patsubst %.c,%.o,$(SRCS2))

INCLUDES = $(STLINUXTV_TOPDIR)/linux/include/linux/stmvout.h \
           $(STLINUXTV_TOPDIR)/utils/v4l2_helper.h

all: $(TARGET) $(TARGET2)

$(TARGET): $(OBJS)
$(TARGET2): $(OBJS2)

dep:

install: $(TARGET) $(TARGET2)
	install -m 755 -p $(TARGET) $(TARGET_DIR)/usr/bin
	install -m 755 -p $(TARGET2) $(TARGET_DIR)/usr/bin

clean :
	find . -name "*.o" -exec rm \{\} \;
	find . -name $(TARGET) -exec rm \{\} \;
	find . -name $(TARGET2) -exec rm \{\} \;

$(OBJS): $(INCLUDES)
$(OBJS2): $(INCLUDES)

