include ../../../mkenv.mk
include ../../../toolchain.mk

DIR = $(shell basename `pwd`)
LIB = $(RTSMART_HAL_LIB_INSTALL_PATH)/lib$(DIR).a

CFILES := $(wildcard *.c)
COBJS  := $(CFILES:.c=.o)
CDEPS  := $(CFILES:.c=.o.d)

CINCS  := $(wildcard *.h)

CFLAGS := -std=gnu99 -fopenmp -march=rv64imafdcv -mabi=lp64d -mcmodel=medany
CFLAGS += -I.

.PHONY: all clean distclean

.PHONY: ensure_install_path
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

$(COBJS): %.o: %.c
	@$(ECHO) [CC] $<
	@$(CC) $(CFLAGS) -MD -MP -MF $@.d -c $< -o $@

-include $(CDEPS)
