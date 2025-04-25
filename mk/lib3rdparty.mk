rtsmart_3rd_party_lib_dir := $(SDK_RTSMART_BUILD_DIR)/libs/3rd-party/lib
rtsmart_3rd_party_inc_dir := $(SDK_RTSMART_BUILD_DIR)/libs/3rd-party/include

RTSMART_3RD_PARTY_INC := -I$(rtsmart_3rd_party_inc_dir)
RTSMART_3RD_PARTY_LIBS := $(addprefix -l,$(subst lib, ,$(basename $(notdir $(foreach dir, $(rtsmart_3rd_party_lib_dir), $(wildcard $(dir)/*))))))
RTSMART_3RD_PARTY_LIB_DIR := $(addprefix -L, $(rtsmart_3rd_party_lib_dir))
