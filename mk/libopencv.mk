inc_dir := $(SDK_RTSMART_SRC_DIR)/libs/opencv/include/opencv4

lib_dir := $(SDK_RTSMART_SRC_DIR)/libs/opencv/lib
lib_dir += $(SDK_RTSMART_SRC_DIR)/libs/opencv/lib/opencv4/3rdparty

LIB_CFLAGS += $(addprefix -I, $(inc_dir))
LIB_LDFLAGS += $(addprefix -L, $(lib_dir)) 
LIB_LDFLAGS += -Wl,--start-group -lstdc++ -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lzlib -llibjpeg-turbo -llibopenjp2 -llibpng -llibtiff -llibwebp -lcsi_cv -latomic -Wl,--end-group
