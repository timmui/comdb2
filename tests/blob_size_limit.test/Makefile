include $(TESTSROOTDIR)/testcase.mk
include $(TESTSROOTDIR)/../main.mk
export TEST_TIMEOUT=3m
CFLAGS+=-I$(TESTSROOTDIR)/../bbinc -D_GNU_SOURCE

CDB2LIBS=$(SYSPATH) $(SRCHOME)/cdb2api/libcdb2api.a $(SRCHOME)/protobuf/libcdb2protobuf.a -lprotobuf-c -lsqlite3 -lpthread -lssl -lcrypto

tool: comdb2_blobtest

comdb2_blobtest: comdb2_blobtest.o
	echo $(CC) -o $@ $?
	$(CC) -o $@ $? $(CDB2LIBS)
	
%.o: %.c
	echo $(CC) -o $@ -c $< $(CFLAGS) -I$(TESTSROOTDIR)/../cdb2api
	$(CC) -o $@ -c $< $(CFLAGS) -I$(TESTSROOTDIR)/../cdb2api
