inc_dir := $(SDK_RTSMART_BUILD_DIR)/libs/rtsmart_hal/include
lib_dir := $(SDK_RTSMART_BUILD_DIR)/libs/rtsmart_hal/lib

LIB_CFLAGS += $(addprefix -I, $(inc_dir))
LIB_LDFLAGS += $(addprefix -L, $(lib_dir)) 
LIB_LDFLAGS += -Wl,--start-group $(addprefix -l,$(subst lib, ,$(basename $(notdir $(foreach dir, $(lib_dir), $(wildcard $(dir)/*)))))) -Wl,--end-group
