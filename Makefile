# Makefile to build 'rarraytestsuite'
# cfg3-generated by rzon on neptune at 10:11 on 02.12.2013.
.PHONY: all rarraytestsuite again clean profile debug profile_clean debug_clean profile_again debug_again 

all rarraytestsuite clean again:
	cd obj/release && $(MAKE) $@

debug:
	cd obj/debug && $(MAKE)

debug_clean:
	cd obj/debug && $(MAKE) clean

debug_again:
	cd obj/debug && $(MAKE) again

profile:
	cd obj/profile && $(MAKE)

profile_clean:
	cd obj/profile && $(MAKE) clean

profile_again:
	cd obj/profile && $(MAKE) again

install: fakegas
	mkdir -p *.h/bin
	cp rarraytestsuite *.h/bin
