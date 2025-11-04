include ../mkenv.mk

include $(SDK_SRC_ROOT_DIR)/.config

subdirs-y := display_manager

.PHONY: all clean distclean

all:
	@$(foreach dir,$(subdirs-y),$(MAKE) -C $(dir) all || exit $?;)
	@echo "Make RT-Smart hexchip libs done."

clean:
	@$(foreach dir,$(subdirs-y),$(MAKE) -C $(dir) clean || exit $?;)
	@echo "Make RT-Smart hexchip libs clean done."

distclean:
	@$(foreach dir,$(subdirs-y),$(MAKE) -C $(dir) distclean || exit $?;)
	@echo "Make RT-Smart hexchip libs distclean done."
