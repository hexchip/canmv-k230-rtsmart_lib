include ../mkenv.mk

subdirs-y := drivers

.PHONY: all clean distclean

all:
	@$(foreach dir,$(subdirs-y),make -C $(dir) all || exit $?;)
	@echo "Make RT-Smart Hal done."

clean:
	@$(foreach dir,$(subdirs-y),make -C $(dir) clean || exit $?;)
	@echo "Make RT-Smart Hal clean done."

distclean:
	@$(foreach dir,$(subdirs-y),make -C $(dir) distclean || exit $?;)
	@echo "Make RT-Smart Hal distclean done."
