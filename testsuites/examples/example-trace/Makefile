# SPDX-License-Identifier: BSD-2-Clause

# Copyright (C) 2026 embedded brains GmbH & Co. KG

# The default of RTEMS_ROOT fits a copy of this directory in src/ of an
# installation prefix.  A build in the RTEMS source tree has to set RTEMS_ROOT.
EXAMPLE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
RTEMS_ROOT ?= $(abspath $(EXAMPLE_DIR)../..)

EMPTY :=
SPACE := $(EMPTY) $(EMPTY)
RTEMS_PKG_CONFIG_DIR = $(RTEMS_ROOT)/lib/pkgconfig

# Without RTEMS_CPU and RTEMS_BSP, the example builds for the only BSP of the
# installation prefix.
ifeq ($(RTEMS_CPU)$(RTEMS_BSP),)
RTEMS_BSPS := $(basename $(notdir $(wildcard $(RTEMS_PKG_CONFIG_DIR)/*-rtems*-*.pc)))
ifneq ($(words $(RTEMS_BSPS)),1)
$(error Set RTEMS_CPU and RTEMS_BSP to the BSP of one of these pkg-config files in $(RTEMS_PKG_CONFIG_DIR): $(RTEMS_BSPS))
endif
RTEMS_CPU := $(word 1,$(subst -, ,$(RTEMS_BSPS)))
RTEMS_BSP := $(subst $(SPACE),-,$(wordlist 3,$(words $(subst -, ,$(RTEMS_BSPS))),$(subst -, ,$(RTEMS_BSPS))))
endif

RTEMS_API ?= $(patsubst $(RTEMS_CPU)-rtems%-$(RTEMS_BSP).pc,%,$(notdir $(wildcard $(RTEMS_PKG_CONFIG_DIR)/$(RTEMS_CPU)-rtems*-$(RTEMS_BSP).pc)))
RTEMS_RUN ?= $(firstword $(filter-out $(RTEMS_ROOT)/bin/$(RTEMS_CPU)-rtems%,$(wildcard $(RTEMS_ROOT)/bin/*-run)))

PKG_CONFIG = $(RTEMS_PKG_CONFIG_DIR)/$(RTEMS_CPU)-rtems$(RTEMS_API)-$(RTEMS_BSP).pc
BUILDDIR = b-$(RTEMS_CPU)-$(RTEMS_BSP)

DEPFLAGS = -MT $@ -MD -MP -MF $(basename $@).d
WARNFLAGS = -Wall -Wextra
OPTFLAGS = -O2 -g -ffunction-sections -fdata-sections
MAPFLAGS = -ffile-prefix-map=$(RTEMS_ROOT)/=
EXEEXT = .exe

ABI_FLAGS = $(shell pkg-config --define-variable=prefix=$(RTEMS_ROOT) --cflags $(PKG_CONFIG))
LDFLAGS = $(shell pkg-config --define-variable=prefix=$(RTEMS_ROOT) --variable=LDFLAGS $(PKG_CONFIG))
CFLAGS = $(DEPFLAGS) $(WARNFLAGS) $(ABI_FLAGS) $(OPTFLAGS) $(MAPFLAGS)
CXXFLAGS = $(DEPFLAGS) $(WARNFLAGS) $(ABI_FLAGS) $(OPTFLAGS) $(MAPFLAGS)
ASFLAGS = $(ABI_FLAGS)

CCLINK = $(CC) $(CFLAGS) -Wl,-Map,$(basename $@).map
CXXLINK = $(CXX) $(CXXFLAGS) -Wl,-Map,$(basename $@).map

$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $(abspath $<) -o $@

$(BUILDDIR)/%.o: %.S | $(BUILDDIR)
	$(CC) $(CPPFLAGS) -DASM $(CFLAGS) -c $(abspath $<) -o $@

$(BUILDDIR)/%.o: %.cc | $(BUILDDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(abspath $<) -o $@

$(BUILDDIR)/%.o: %.cpp | $(BUILDDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(abspath $<) -o $@

$(BUILDDIR)/%.o: %.s | $(BUILDDIR)
	$(AS) $(ASFLAGS) $< -o $@

export PATH := $(RTEMS_ROOT)/bin:$(PATH)

export AR = $(RTEMS_CPU)-rtems$(RTEMS_API)-ar
export AS = $(RTEMS_CPU)-rtems$(RTEMS_API)-as
export CC = $(RTEMS_CPU)-rtems$(RTEMS_API)-gcc
export CXX = $(RTEMS_CPU)-rtems$(RTEMS_API)-g++
export LD = $(RTEMS_CPU)-rtems$(RTEMS_API)-ld
export NM = $(RTEMS_CPU)-rtems$(RTEMS_API)-nm
export OBJCOPY = $(RTEMS_CPU)-rtems$(RTEMS_API)-objcopy
export RANLIB = $(RTEMS_CPU)-rtems$(RTEMS_API)-ranlib
export SIZE = $(RTEMS_CPU)-rtems$(RTEMS_API)-size
export STRIP = $(RTEMS_CPU)-rtems$(RTEMS_API)-strip

APP = $(BUILDDIR)/example-trace$(EXEEXT)

APP_OBJS =
APP_OBJS += $(BUILDDIR)/appl-config.o
APP_OBJS += $(BUILDDIR)/main.o

all: $(APP)

$(BUILDDIR):
	mkdir $(BUILDDIR)

$(APP): $(APP_OBJS)
	$(CCLINK) $^ -lrtemsbsp -lrtemscpu $(LDFLAGS) -lrtemstest -o $@

LOG = $(BUILDDIR)/log.txt

CTFDIR = $(BUILDDIR)/ctf

$(LOG): $(APP)
	$(RTEMS_RUN) $(RTEMS_CPU)-$(RTEMS_BSP) $< | tee $(LOG)

$(CTFDIR): $(LOG)
	rm -rf $@
	mkdir $@
	rtems-record-lttng -t $< -o $@

run: $(LOG)

ctf: $(CTFDIR)

babeltrace: $(CTFDIR)
	babeltrace2 $< > $(BUILDDIR)/babeltrace.txt
	head -n 100 $(BUILDDIR)/babeltrace.txt

clean:
	rm -rf $(BUILDDIR)

-include $(APP_OBJS:%.o=%.d)
