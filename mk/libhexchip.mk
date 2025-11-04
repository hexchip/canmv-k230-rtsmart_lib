inc_dir := $(SDK_RTSMART_BUILD_DIR)/libs/hexchip/include
lib_dir := $(SDK_RTSMART_BUILD_DIR)/libs/hexchip/lib

LIB_CFLAGS += $(addprefix -I, $(inc_dir))
LIB_LDFLAGS += $(addprefix -L, $(lib_dir)) 
LIB_LDFLAGS += -Wl,--start-group $(addprefix -l,$(subst lib, ,$(basename $(notdir $(foreach dir, $(lib_dir), $(wildcard $(dir)/*)))))) -Wl,--end-group