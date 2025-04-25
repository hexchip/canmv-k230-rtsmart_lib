include ../../../mkenv.mk
include ../../../toolchain.mk

DIR = $(shell basename `pwd`)
LIB = $(RTSMART_HAL_LIB_INSTALL_PATH)/lib$(DIR).a
BUILD := $(SDK_RTSMART_BUILD_DIR)/libs/rtsmart_hal/$(DIR)

CFILES := $(wildcard *.c)
COBJS	:= $(patsubst %, $(BUILD)/%, $(CFILES:.c=.o))
CDEPS	:= $(patsubst %, $(BUILD)/%, $(CFILES:.c=.o.d))

CINCS  := $(wildcard *.h)

CFLAGS := -std=gnu99 -fopenmp -march=rv64imafdcv -mabi=lp64d -mcmodel=medany
CFLAGS += -I.

.PHONY: all clean distclean

.PHONY: check_path
check_path:
	@if [ -z "$(RTSMART_HAL_LIB_INSTALL_PATH)" ]; then \
		echo "Error: RTSMART_HAL_LIB_INSTALL_PATH is not set"; \
		exit 1; \
	fi
	@if [ ! -d "$(RTSMART_HAL_LIB_INSTALL_PATH)" ]; then \
		mkdir -p "$(RTSMART_HAL_LIB_INSTALL_PATH)"; \
	fi
	@if [ -z "$(RTSMART_HAL_INC_INSTALL_PATH)" ]; then \
		echo "Error: RTSMART_HAL_INC_INSTALL_PATH is not set"; \
		exit 1; \
	fi
	@if [ ! -d "$(RTSMART_HAL_INC_INSTALL_PATH)" ]; then \
		mkdir -p "$(RTSMART_HAL_INC_INSTALL_PATH)"; \
	fi

all: check_path $(LIB)
	@$(ECHO) "Make lib$(DIR) done."
	@$(CP) $(CINCS) $(RTSMART_HAL_INC_INSTALL_PATH)

clean:
	@$(RM) $(COBJS) $(CDEPS)

distclean: clean

$(LIB): $(COBJS)
	@$(ECHO) "AR $@"
	@$(AR) rcs $@ $^

$(COBJS): $(BUILD)/%.o: %.c
	@$(ECHO) [CC] $<
	@$(CC) $(CFLAGS) -MD -MP -MF $@.d -c $< -o $@

# $(sort $(var)) removes duplicates
#
# The net effect of this, is it causes the objects to depend on the
# object directories (but only for existence), and the object directories
# will be created if they don't exist.
OBJ_DIRS = $(sort $(dir $(COBJS)))
$(COBJS): | $(OBJ_DIRS)
$(OBJ_DIRS):
	@mkdir -p $@

-include $(CDEPS)
