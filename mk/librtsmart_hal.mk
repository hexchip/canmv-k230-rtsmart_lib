rtsmart_hal_lib_dir := $(SDK_RTSMART_BUILD_DIR)/libs/rtsmart_hal/lib

RTSMART_HAL_INC := -I$(SDK_RTSMART_BUILD_DIR)/libs/rtsmart_hal/include
RTSMART_HAL_LIBS := $(addprefix -l,$(subst lib, ,$(basename $(notdir $(foreach dir, $(rtsmart_hal_lib_dir), $(wildcard $(dir)/*))))))
RTSMART_HAL_LIB_DIR := $(addprefix -L, $(rtsmart_hal_lib_dir))
