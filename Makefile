include mkenv.mk

subdirs-y := 

ifndef RTT_LIBS_DISABLED
subdirs-y += rtsmart_hal
subdirs-y += hexchip
subdirs-y += 3rd-party
subdirs-$(CONFIG_RTSMART_LIBS_ENABLE_TESTCASES) += testcases
endif

.PHONY: all clean distclean

all:
	@$(foreach dir,$(subdirs-y),$(MAKE) -C $(dir) all || exit $?;)
	@echo "Make RT-Smart Libraries done."

clean:
	@$(foreach dir,$(subdirs-y),$(MAKE) -C $(dir) clean || exit $?;)
	@echo "Make RT-Smart Libraries clean done."

distclean:
	@$(foreach dir,$(subdirs-y),$(MAKE) -C $(dir) distclean || exit $?;)
	@echo "Make RT-Smart Libraries distclean done."
