CC = gcc800
CFLAGS = -std=gnu99
TIMEFLAGS = -O3 -D NDEBUG

REFERENCE_DIR = reference
SRC_DIR = src
TEST_DIR = test

# TEST = $(TEST_DIR)/testheapmgr.c
TEST = $(TEST_DIR)/testheapmgr_add.c
# TEST_ADD = $(TEST_DIR)/testheapmgr_add.c
CHUNK_BASE = $(REFERENCE_DIR)/chunkbase.c
CHUNK_BASE_H = $(REFERENCE_DIR)/chunkbase.h
HEAPMGR_H = $(REFERENCE_DIR)/heapmgr.h
HEAPMGR_GNU = $(REFERENCE_DIR)/heapmgrgnu.c
HEAPMGR_KR = $(REFERENCE_DIR)/heapmgrkr.c
HEAPMGR_BASE = $(REFERENCE_DIR)/heapmgrbase.c
HEAPMGR1 = $(SRC_DIR)/heapmgr1.c
HEAPMGR2 = $(SRC_DIR)/heapmgr2.c
CHUNK = $(SRC_DIR)/chunk.c
CHUNK_H = $(SRC_DIR)/chunk.h

all: time2all

test1:
	$(CC) $(CFLAGS) $(TEST) $(HEAPMGR1) $(CHUNK) -o $(TEST_DIR)/testheapmgr1

test1_add:
	$(CC) $(CFLAGS) $(TEST_ADD) $(HEAPMGR1) $(CHUNK) -o $(TEST_DIR)/testheapmgr1

test2:
	$(CC) $(CFLAGS) $(TEST) $(HEAPMGR2) $(CHUNK) -o $(TEST_DIR)/testheapmgr2

testall: test1 test2

time1:
	$(CC) $(TIMEFLAGS) $(CFLAGS) $(TEST) $(HEAPMGR1) $(CHUNK) -o $(TEST_DIR)/testheapmgr1

time2:
	$(CC) $(TIMEFLAGS) $(CFLAGS) $(TEST) $(HEAPMGR2) $(CHUNK) -o $(TEST_DIR)/testheapmgr2

time1all: timegnu timekr timebase time1

time2all: timegnu timekr timebase time1 time2

timegnu:
	$(CC) $(TIMEFLAGS) $(CFLAGS) $(TEST) $(HEAPMGR_GNU) -o $(TEST_DIR)/testheapmgrgnu

timekr:
	$(CC) $(TIMEFLAGS) $(CFLAGS) $(TEST) $(HEAPMGR_KR) -o $(TEST_DIR)/testheapmgrkr

timebase:
	$(CC) $(TIMEFLAGS) $(CFLAGS) $(TEST) $(HEAPMGR_BASE) $(CHUNK_BASE) -o $(TEST_DIR)/testheapmgrbase

clean:
	rm -f $(TEST_DIR)/testheapmgrgnu $(TEST_DIR)/testheapmgrkr $(TEST_DIR)/testheapmgrbase $(TEST_DIR)/testheapmgr1 $(TEST_DIR)/testheapmgr2 $(TEST_DIR)/testheapmgr1_debug

DEBUGFLAGS = -g -O0

debug1:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(TEST) $(HEAPMGR1) $(CHUNK) -o $(TEST_DIR)/testheapmgr1_debug

debug2:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(TEST) $(HEAPMGR2) $(CHUNK) -o $(TEST_DIR)/testheapmgr2_debug

debugall: debug1 debug2