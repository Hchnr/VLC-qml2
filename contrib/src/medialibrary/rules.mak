MEDIALIBRARY_HASH := cf5ce2cda9bf66bc8644c7321c89f3f42e42ff25
MEDIALIBRARY_VERSION := git-$(MEDIALIBRARY_HASH)
MEDIALIBRARY_GITURL := https://code.videolan.org/videolan/medialibrary/-/archive/$(MEDIALIBRARY_HASH)/medialibrary-$(MEDIALIBRARY_HASH).tar.gz

PKGS += medialibrary
ifeq ($(call need_pkg,"medialibrary"),)
PKGS_FOUND += medialibrary
endif

$(TARBALLS)/medialibrary-$(MEDIALIBRARY_VERSION).tar.gz:
	$(call download,$(MEDIALIBRARY_GITURL))

.sum-medialibrary: medialibrary-$(MEDIALIBRARY_VERSION).tar.gz
	$(warning $@ not implemented)
	touch $@

medialibrary: medialibrary-$(MEDIALIBRARY_VERSION).tar.gz .sum-medialibrary
	rm -rf $@-$(MEDIALIBRARY_VERSION)
	mkdir -p $@-$(MEDIALIBRARY_VERSION)
	tar xvzf "$<" --strip-components=1 -C $@-$(MEDIALIBRARY_VERSION)
	$(MOVE)

.medialibrary: medialibrary
	$(RECONF)
	cd $< && $(HOSTVARS_PIC) ./configure CXXFLAGS="-g -O0" --disable-tests --without-libvlc $(HOSTCONF)
	cd $< && $(MAKE)
	cd $< && $(MAKE) install
	touch $@

