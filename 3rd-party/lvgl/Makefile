include ../../mkenv.mk
include ../../toolchain.mk

SRC_DIRS := .
CFILES :=
CFILES_EXCLUDE :=
CWARN := 
CFLAGS := -I. -Iinclude -I$(SDK_SRC_ROOT_DIR)/include/generated 
CFLAGS += -fopenmp -march=rv64imafdcv -mabi=lp64d -mcmodel=medany $(CWARN) -U__linux__ -D__RTTHREAD__

CXXFILES :=
CXXFILES_EXCLUDE :=
CXXFLAGS := $(CFLAGS)

CFLAGS += -std=gnu99

DIR = $(shell basename `pwd`)
LOCAL_SRC_DIR  = $(shell pwd)
LIB = $(RTSMART_3RD_PARTY_LIB_INSTALL_PATH)/lib$(DIR).a
BUILD =  $(SDK_RTSMART_BUILD_DIR)/libs/3rd-party/lvgl

LVGL_PORT_HEADER_FILES := lv_conf.h lv_rt_thread_conf.h lv_rt_thread_port.h

LVGL_DIR = lvgl
LVGL_SRC_DIRS += $(shell find $(LVGL_DIR)/src -type d)

ifdef CONFIG_PKG_LVGL_USING_EXAMPLES
	LVGL_SRC_DIRS += $(shell find $(LVGL_DIR)/examples -type d)
endif

ifdef CONFIG_PKG_LVGL_USING_DEMOS
	LVGL_SRC_DIRS += $(shell find $(LVGL_DIR)/demos -type d)
endif

LVGL_FILTERED_DIRS = $(filter-out $(LVGL_DIR)/src/libs/thorvg/rapidjson/msinttypes,$(LVGL_SRC_DIRS))
LVGL_HEADER_FILES := $(wildcard $(LVGL_DIR)/*.h $(LVGL_DIR)/*.hpp)
LVGL_HEADER_FILES += $(foreach dir, $(LVGL_FILTERED_DIRS), $(wildcard $(dir)/*.h $(dir)/*.hpp))

SRC_DIRS += $(LVGL_FILTERED_DIRS)

CFLAGS += -I$(LVGL_DIR)/src
ifdef PKG_LVGL_USING_EXAMPLES
	CFLAGS += -I$(LVGL_DIR)/examples
endif

ifdef PKG_LVGL_USING_DEMOS
	CFLAGS += -I$(LVGL_DIR)/demos
endif

LIB_CFLAGS :=
include $(SDK_RTSMART_SRC_DIR)/libs/mk/librtsmart.mk
include $(SDK_RTSMART_SRC_DIR)/libs/mk/libhexchip.mk
CFLAGS += $(LIB_CFLAGS)
CXXFLAGS += $(LIB_CFLAGS)

CFILES_ALL        = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
CXXFILES_ALL      = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))

CFILES      += $(filter-out $(CFILES_EXCLUDE), $(CFILES_ALL))
CXXFILES    += $(filter-out $(CXXFILES_EXCLUDE), $(CXXFILES_ALL))

COBJS	      := $(patsubst %, $(BUILD)/%, $(CFILES:.c=.o))
CXXOBJS	:= $(patsubst %, $(BUILD)/%, $(CXXFILES:.cpp=.o))
OBJS	      := $(COBJS) $(CXXOBJS)

CDEPS	      := $(patsubst %, $(BUILD)/%, $(CFILES:.c=.o.d))
CXXDEPS	:= $(patsubst %, $(BUILD)/%, $(CXXFILES:.cpp=.o.d))
DEPS	      := $(CDEPS) $(CXXDEPS)

.PHONY: check_path
check_path:
	@if [ -z "$(RTSMART_3RD_PARTY_LIB_INSTALL_PATH)" ]; then \
		echo "Error: RTSMART_3RD_PARTY_LIB_INSTALL_PATH is not set"; \
		exit 1; \
	fi
	@if [ ! -d "$(RTSMART_3RD_PARTY_LIB_INSTALL_PATH)" ]; then \
		mkdir -p "$(RTSMART_3RD_PARTY_LIB_INSTALL_PATH)"; \
	fi
	@if [ -z "$(RTSMART_3RD_PARTY_INC_INSTALL_PATH)" ]; then \
		echo "Error: RTSMART_3RD_PARTY_INC_INSTALL_PATH is not set"; \
		exit 1; \
	fi
	@if [ ! -d "$(RTSMART_3RD_PARTY_INC_INSTALL_PATH)" ]; then \
		mkdir -p "$(RTSMART_3RD_PARTY_INC_INSTALL_PATH)"; \
	fi

.PHONY: all
all: check_path $(LIB) copy_lvgl_port_headers copy_lvgl_headers
	@echo "Make lib$(DIR) done."

$(LIB): $(OBJS) 
	@echo [AR] $@
	@$(AR) rcs $@ $^

$(COBJS) : $(BUILD)/%.o : %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) -MD -MP -MF $@.d -c $< -o $@

$(CXXOBJS) : $(BUILD)/%.o : %.cpp
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) -MD -MP -MF $@.d -c $< -o $@

.PHONY: copy_lvgl_port_headers
copy_lvgl_port_headers: $(patsubst %, $(RTSMART_3RD_PARTY_INC_INSTALL_PATH)/%, $(LVGL_PORT_HEADER_FILES))

.PHONY: copy_lvgl_headers
copy_lvgl_headers: $(patsubst %, $(RTSMART_3RD_PARTY_INC_INSTALL_PATH)/%, $(LVGL_HEADER_FILES))

$(RTSMART_3RD_PARTY_INC_INSTALL_PATH)/%.h: %.h
	@mkdir -p $(dir $@)
	@cp -fp $< $@

$(RTSMART_3RD_PARTY_INC_INSTALL_PATH)/%.hpp: %.hpp
	@mkdir -p $(dir $@)
	@cp -fp $< $@

.PHONY: clean
clean:
	@rm -rf $(LIB) $(OBJS) $(DEPS)

# $(sort $(var)) removes duplicates
#
# The net effect of this, is it causes the objects to depend on the
# object directories (but only for existence), and the object directories
# will be created if they don't exist.
OBJ_DIRS = $(sort $(dir $(OBJS)))
$(OBJS): | $(OBJ_DIRS)
$(OBJ_DIRS):
	@$(MKDIR) -p $@

-include $(DEPS)

.PHONY: test
test:
	@echo $(DEMO_ALL)