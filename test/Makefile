include ../Makefile.inc

clean:
	rm -fr *.class *.bc *.j *.ll c-testsuite-master c-testsuite.zip
	
test: c-testsuite


c-testsuite : c-testsuite.zip
	unzip -oq $<
	cd c-testsuite-master/tests/single-exec && for PRG in *.c ; do mv $$PRG c_test_$$PRG ; done
	cd c-testsuite-master/tests/single-exec && for PRG in *.c ; do ../../../mkandrun.sh $$(echo $$PRG  | sed 's/\.c//'); done

c-testsuite.zip:
		wget -O c-testsuite.zip https://github.com/c-testsuite/c-testsuite/archive/refs/heads/master.zip
