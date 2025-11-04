inc_dir := 
inc_dir += $(SDK_RTSMART_SRC_DIR)/rtsmart/userapps
inc_dir += $(SDK_RTSMART_SRC_DIR)/rtsmart/userapps/sdk/rt-thread/include
inc_dir += $(SDK_RTSMART_SRC_DIR)/rtsmart/userapps/sdk/rt-thread/components/dfs
inc_dir += $(SDK_RTSMART_SRC_DIR)/rtsmart/userapps/sdk/rt-thread/components/drivers
inc_dir += $(SDK_RTSMART_SRC_DIR)/rtsmart/userapps/sdk/rt-thread/components/finsh
inc_dir += $(SDK_RTSMART_SRC_DIR)/rtsmart/userapps/sdk/rt-thread/components/net

LIB_CFLAGS += $(addprefix -I, $(inc_dir)) -DHAVE_CCONFIG_H
LIB_LDFLAGS += -L$(SDK_RTSMART_SRC_DIR)/rtsmart/userapps/sdk/rt-thread/lib/risc-v/rv64
LIB_LDFLAGS += -Wl,--start-group -Wl,-whole-archive -lrtthread -Wl,-no-whole-archive -Wl,--end-group
