include ../mkenv.mk

subdirs-y := freetype

.PHONY: all clean distclean

all:
	@$(foreach dir,$(subdirs-y),make -C $(dir) all || exit $?;)
	@echo "Make RT-Smart 3rd-party done."

clean:
	@$(foreach dir,$(subdirs-y),make -C $(dir) clean || exit $?;)
	@echo "Make RT-Smart 3rd-party clean done."

distclean:
	@$(foreach dir,$(subdirs-y),make -C $(dir) distclean || exit $?;)
	@echo "Make RT-Smart 3rd-party distclean done."
