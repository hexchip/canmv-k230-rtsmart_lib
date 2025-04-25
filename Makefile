include mkenv.mk

subdirs-y := rtsmart_hal 3rd-party

.PHONY: all clean distclean

all:
	@$(foreach dir,$(subdirs-y),make -C $(dir) all || exit $?;)
	@echo "Make RT-Smart Libraries done."

clean:
	@$(foreach dir,$(subdirs-y),make -C $(dir) clean || exit $?;)
	@echo "Make RT-Smart Libraries clean done."

distclean:
	@$(foreach dir,$(subdirs-y),make -C $(dir) distclean || exit $?;)
	@echo "Make RT-Smart Libraries distclean done."
