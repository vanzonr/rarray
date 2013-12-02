# Redirecting: let makefiles in ../obj do the real work
.PHONY: all tests clean again
all tests clean again:
	cd ../obj/release && $(MAKE) $@
