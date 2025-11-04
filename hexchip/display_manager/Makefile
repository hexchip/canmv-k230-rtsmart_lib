include ../../mkenv.mk
include ../../toolchain.mk

include $(SDK_RTSMART_SRC_DIR)/libs/mk/librtsmart.mk

DIR = $(shell basename `pwd`)
LOCAL_SRC_DIR  = $(shell pwd)
LIB = $(RTSMART_HEXCHIP_LIB_INSTALL_PATH)/lib$(DIR).a
BUILD =  $(SDK_RTSMART_BUILD_DIR)/libs/hexchip/display_manager

SRC_DIRS := .

CFILES :=
CFILES_EXCLUDE :=
CFLAGS := -I. -Iinclude $(LIB_CFLAGS)

CXXFILES :=
CXXFILES_EXCLUDE :=
CXXFLAGS :=

CWARN := 

CFLAGS += -std=gnu99 -fopenmp -march=rv64imafdcv -mabi=lp64d -mcmodel=medany $(CWARN)

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
	@if [ -z "$(RTSMART_HEXCHIP_LIB_INSTALL_PATH)" ]; then \
		echo "Error: RTSMART_HEXCHIP_LIB_INSTALL_PATH is not set"; \
		exit 1; \
	fi
	@if [ ! -d "$(RTSMART_HEXCHIP_LIB_INSTALL_PATH)" ]; then \
		mkdir -p "$(RTSMART_HEXCHIP_LIB_INSTALL_PATH)"; \
	fi
	@if [ -z "$(RTSMART_HEXCHIP_INC_INSTALL_PATH)" ]; then \
		echo "Error: RTSMART_HEXCHIP_INC_INSTALL_PATH is not set"; \
		exit 1; \
	fi
	@if [ ! -d "$(RTSMART_HEXCHIP_INC_INSTALL_PATH)" ]; then \
		mkdir -p "$(RTSMART_HEXCHIP_INC_INSTALL_PATH)"; \
	fi

.PHONY: all
all: check_path $(LIB)
	@cp -rp $(LOCAL_SRC_DIR)/include/* $(RTSMART_HEXCHIP_INC_INSTALL_PATH)
	@echo "Make lib$(DIR) done."

$(LIB): $(OBJS) 
	@echo [LD] $@
	@$(AR) rcs $@ $^

$(COBJS) : $(BUILD)/%.o : %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) -MD -MP -MF $@.d -c $< -o $@

$(CXXOBJS) : $(BUILD)/%.o : %.cpp
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) -MD -MP -MF $@.d -c $< -o $@

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