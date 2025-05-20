# sdk lib and inc
inc_dir := $(SDK_RTSMART_SRC_DIR)/mpp/include/
inc_dir += $(SDK_RTSMART_SRC_DIR)/mpp/include/comm
inc_dir += $(SDK_RTSMART_SRC_DIR)/mpp/include/ioctl
inc_dir += $(SDK_RTSMART_SRC_DIR)/mpp/userapps/api/

lib_dir := $(SDK_RTSMART_SRC_DIR)/mpp/userapps/lib/
lib_dir += $(SDK_RTSMART_SRC_DIR)/mpp/middleware/lib/

LIB_CFLAGS += $(addprefix -I, $(inc_dir))
LIB_LDFLAGS += $(addprefix -L, $(lib_dir)) 
LIB_LDFLAGS += -Wl,--start-group $(addprefix -l,$(subst lib, ,$(basename $(notdir $(foreach dir, $(lib_dir), $(wildcard $(dir)/*)))))) -Wl,--end-group
