INSTALL_DIR := /opt/lljvm

SUBDIRS = llxvm-cc runtime

all: subdirs

.PHONY: subdirs

subdirs:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
	rm -f llxvm.jar

install: llxvm.jar $(SUBDIRS)
	mkdir -p $(INSTALL_DIR)/bin
	mkdir -p $(INSTALL_DIR)/lib
	mkdir -p $(INSTALL_DIR)/share/tools
	mkdir -p $(INSTALL_DIR)/include
	mkdir -p $(INSTALL_DIR)/include/sys
	cp llxvm.jar $(INSTALL_DIR)/lib
	cp llxvm-cc/llxvm-cc $(INSTALL_DIR)/bin
	cp tools/jasmin.jar $(INSTALL_DIR)/share/tools
	cp -fr runtime/libc/include/* $(INSTALL_DIR)/include

test: llxvm.jar $(SUBDIRS)
	$(MAKE) -C test test
