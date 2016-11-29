# Ambisonic

AMBISONIC_GITURL = https://github.com/magwyz/ambisonic-lib.git

PKGS += ambisonic

ifeq ($(call need_pkg,"ambisonic"),)
PKGS_FOUND += ambisonic
endif

$(TARBALLS)/ambisonic-git.tar.xz:
	$(call download_git,$(AMBISONIC_GITURL))

.sum-ambisonic: ambisonic-git.tar.xz
	$(warning $@ not implemented)
	touch $@

ambisonic: ambisonic-git.tar.xz .sum-ambisonic
	rm -Rf $@-git
	mkdir -p $@-git
	$(XZCAT) "$<" | (cd $@-git && tar xv --strip-components=1)
	$(MOVE)

.ambisonic: ambisonic toolchain.cmake
	-cd $< && rm CMakeCache.txt
	cd $< && $(HOSTVARS) $(CMAKE)
	cd $< && $(MAKE) install
	touch $@
