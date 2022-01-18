SUBDIRS = java libc

include ../Makefile.inc

.PHONY: subdirs

subdirs:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done
	$(MAKE) -C java dist
	cp java/dist/llxvm.jar ..
	
clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
