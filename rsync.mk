$(eval $(call start_package))
RSYNC?=rsync-3.3.0

PACKAGE:=rsync

ARCHIVE_NAME:=$(RSYNC).tar.gz
DOWNLOAD_URL:=https://download.samba.org/pub/rsync/src/$(ARCHIVE_NAME)
PACKAGE_INSTALLED_FILES:=$(BUILD_DIR)/usr/bin/rsync

CFLAGS+=-I$(BUILD_DIR)/openssl/include
LDFLAGS+=-L$(BUILD_DIR)/openssl/

PACKAGE_WANT_PREPARE=true

define pkg-targets
$(BUILD_DIR)/$(PACKAGE)/stamp.configured: $(SRC_DIR)/$(PACKAGE)/stamp.prepared $(call depend-built,openssl)
	mkdir -p $(BUILD_DIR)/$(PACKAGE)
	cd "$(BUILD_DIR)/$(PACKAGE)";                                       \
	PATH=$(EXTRA_PATH):$(PATH) $(SRC_DIR)/$(PACKAGE)/$(RSYNC)/configure \
	  LD="$(LD)" CC="$(CC)" CFLAGS="$(CFLAGS)" CPPFLAGS="$(CFLAGS)"     \
	  --build x86_64-pc-linux-gnu --host $(CROSS)                       \
	  --disable-simd --disable-xxhash --disable-zstd --disable-lz4      \
	  CFLAGS="$(CFLAGS)" CPPFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"
	$(make-configured-stamp)

ifneq ($(IS_SRC_$(PACKAGE)_TARGET_PREPARED),true)
IS_SRC_$(PACKAGE)_TARGET_PREPARED:=true
$(SRC_DIR)/$(PACKAGE)/stamp.prepared: $(SRC_DIR)/$(PACKAGE)/stamp.unpacked
	cd "$(SRC_DIR)/$(PACKAGE)/$(RSYNC)"; \
		sed -i -e 's/^if.*\.git.*;/if false;/' mkgitver
	$(make-prepared-stamp)
endif

$(BUILD_DIR)/usr/bin/rsync: $(BUILD_DIR)/$(PACKAGE)/stamp.built
	mkdir -p $(BUILD_DIR)/usr/bin/
	cp -u "$(BUILD_DIR)/$(PACKAGE)/rsync" "$(BUILD_DIR)/usr/bin/"
	$(STRIP) "$(BUILD_DIR)/usr/bin/rsync"
endef

$(eval $(package))
