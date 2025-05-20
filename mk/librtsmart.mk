inc_dir := 
inc_dir += $(SDK_RTSMART_SRC_DIR)/rtsmart/userapps
inc_dir += $(SDK_RTSMART_SRC_DIR)/rtsmart/userapps/sdk/rt-thread/include
inc_dir += $(SDK_RTSMART_SRC_DIR)/rtsmart/userapps/sdk/rt-thread/components/drivers
inc_dir += $(SDK_RTSMART_SRC_DIR)/rtsmart/userapps/sdk/rt-thread/components/drivers

LIB_CFLAGS += $(addprefix -I, $(inc_dir))
LIB_LDFLAGS +=
