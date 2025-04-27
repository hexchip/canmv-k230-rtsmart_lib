include ../../mkenv.mk

subdirs-y := fpioa gpio timer sbus

.PHONY: all clean distclean

all:
	@$(foreach dir,$(subdirs-y),make -C $(dir) all || exit $?;)
	@echo "Make RT-Smart Hal Drivers done."

clean:
	@$(foreach dir,$(subdirs-y),make -C $(dir) clean || exit $?;)
	@$(RM) $(RTSMART_HAL_LIB_INSTALL_PATH) $(RTSMART_HAL_INC_INSTALL_PATH)
	@echo "Make RT-Smart Hal Drivers clean done."

distclean: clean
	@$(foreach dir,$(subdirs-y),make -C $(dir) distclean || exit $?;)
	@echo "Make RT-Smart Hal Drivers distclean done."
