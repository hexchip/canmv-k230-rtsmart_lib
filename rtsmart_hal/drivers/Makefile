include ../../mkenv.mk

subdirs-y := fpioa gpio timer sbus i2c adc tsensor wdt

.PHONY: all clean distclean

all:
	@$(foreach dir,$(subdirs-y),$(MAKE) -C $(dir) all || exit $?;)
	@echo "Make RT-Smart Hal Drivers done."

clean:
	@$(foreach dir,$(subdirs-y),$(MAKE) -C $(dir) clean || exit $?;)
	@echo "Make RT-Smart Hal Drivers clean done."

distclean: clean
	@$(foreach dir,$(subdirs-y),$(MAKE) -C $(dir) distclean || exit $?;)
	@echo "Make RT-Smart Hal Drivers distclean done."
