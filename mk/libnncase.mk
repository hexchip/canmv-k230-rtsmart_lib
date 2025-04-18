
nncase_inc_dir :=
nncase_inc_dir += $(SDK_RTSMART_SRC_DIR)/libs/nncase/riscv64
nncase_inc_dir += $(SDK_RTSMART_SRC_DIR)/libs/nncase/riscv64/nncase/include
nncase_inc_dir += $(SDK_RTSMART_SRC_DIR)/libs/nncase/riscv64/rvvlib/include

NNCASE_INC = $(addprefix -I, $(nncase_inc_dir))
NNCASE_LIBS = -lNncase.Runtime.Native -lnncase.rt_modules.k230 -lfunctional_k230 -lrvv -lstdc++
NNCASE_LIB_DIR = -L$(SDK_RTSMART_SRC_DIR)/libs/nncase/riscv64/nncase/lib -L$(SDK_RTSMART_SRC_DIR)/libs/nncase/riscv64/rvvlib
