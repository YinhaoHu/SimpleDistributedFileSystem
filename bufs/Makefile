# -----------------------------------------------------------------------------
#			Makefile For Basic Unix File System in the SDFS 
#
#	Type `make help' to see the target list.
#
#	This Makefile does the following things:
#		Generate the final shared library.
#		Test each section
#		Test all
#   	Make the mkfs program
#		
# -----------------------------------------------------------------------------


# =============================================================================
help:
	@echo "Basic Unix File System supports the following targets"
	@echo "  1 final        : generate the final shared library (default target)"
	@echo "  2 test_all     : test all parts of the BUFS"
	@echo "  3 test_high    : test the user interface"
	@echo "  4 test_low     : test inode, dir and datablock"
	@echo "  5 mkfs         : make the mkfs program"
	@echo "  6 copy         : copy the result lib to server direcotry."
	@echo "  7 final_clean  : clean the build direcotry created in make final" 
# =============================================================================
# final section : make the final shared library of basic unix file system.

FINAL_CC := gcc
FINAL_CPPFLAGS = -I $(FINAL_USERINCLUDE_DIR)
FINAL_CFLAGS := -c -Wall -O2 -fPIC
FINAL_LDFLAGS = -shared -Wl,-soname,$(SONAME)

FINAL_SRC_DIR := src
FINAL_USERINCLUDE_DIR := user
FINAL_RESULT_DIR := result
FINAL_BUILD_DIR := build

MAJOR_VERSION := 1
MINOR_VERSION := 0.2

SONAME := libbufs.so.$(MAJOR_VERSION)
REALNAME := libbufs.so.$(MAJOR_VERSION).$(MINOR_VERSION)

FINAL_SRCS := $(wildcard $(FINAL_SRC_DIR)/*.c)
FINAL_OBJS := $(FINAL_SRCS:$(FINAL_SRC_DIR)/%.c=$(FINAL_BUILD_DIR)/%.o)

# Make the final shared library.

final: $(FINAL_RESULT_DIR)/$(SONAME) 
	@echo BUFS Shared Lib was build done in direcotry: $(FINAL_RESULT_DIR) 

$(FINAL_RESULT_DIR)/$(SONAME):$(FINAL_OBJS) 
	@mkdir -p $(dir $@)
	$(FINAL_CC) $(FINAL_LDFLAGS) $(FINAL_OBJS) -o $(FINAL_RESULT_DIR)/$(REALNAME)

$(FINAL_BUILD_DIR)/%.o : $(FINAL_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(FINAL_CC) $(FINAL_CFLAGS) $(FINAL_CPPFLAGS) $< -o $@ 

.PHONY: final_clean

final_clean:
	rm -r $(FINAL_BUILD_DIR) $(FINAL_RESULT_DIR)

# =============================================================================
# test section: make and run the test units 
#
#  1 test_low: test inode, datablock, dir
#  2 test_high: test user interface

TEST_CC := gcc
TEST_CPPFLAGS = $(addprefix -I,$(TEST_INC_DIR))
TEST_CFLAGS = 

TEST_DIR := test
TEST_INC_DIR := ./src ./user
TEST_FRAMEWORK =  
TEST_IMG_NAME := bufs_img
TEST_ALL := test_all

# Specifications for test low level.
LOW_LEVEL_SRCS := src/datablock.c src/dir.c src/etc.c src/inode.c \
					src/mkfs.c src/super.c src/mount.c

TEST_LOW_LEVEL_SRCS := $(TEST_FRAMEWORK) $(LOW_LEVEL_SRCS)
TEST_LOW_LEVEL_DRIVER := $(TEST_DIR)/test_low_level.c
TEST_LOW_TARGET := test_low

# Specifications for test high level
HIGH_LEVEL_SRCS := src/read.c src/write.c src/unlink.c src/create.c \
					src/list.c src/lookup.c src/stat.c

TEST_HIGH_LEVEL_SRCS := $(TEST_FRAMEWORK) $(HIGH_LEVEL_SRCS) $(LOW_LEVEL_SRCS)
TEST_HIGH_LEVEL_DRIVER := $(TEST_DIR)/test_high_level.c
TEST_HIGH_TARGET := test_high

# Make and run tests.

$(TEST_ALL): $(TEST_LOW_TARGET) $(TEST_HIGH_TARGET)

$(TEST_LOW_TARGET):
	@echo "[$@ : Test the low level in the BUFS architecture. Result follows]"
	@$(TEST_CC) $(TEST_CPPFLAGS) $(TEST_CFLAGS) $(TEST_LOW_LEVEL_DRIVER) $(TEST_LOW_LEVEL_SRCS) \
		-o $(TEST_DIR)/$@
	@$(TEST_DIR)/$@ $(TEST_DIR)/$(TEST_IMG_NAME)

$(TEST_HIGH_TARGET):
	@echo "[$@ : Test the high level in the BUFS architecture. Result follows]"
	@$(TEST_CC) $(TEST_CPPFLAGS) $(TEST_CFLAGS) $(TEST_HIGH_LEVEL_DRIVER) $(TEST_HIGH_LEVEL_SRCS) \
		-o $(TEST_DIR)/$@
	@$(TEST_DIR)/$@ $(TEST_DIR)/$(TEST_IMG_NAME)


.PHONY += clean_$(TEST_LOW_TARGET)
clean_$(TEST_LOW_TARGET):
	rm -r -v $(TEST_DIR)/$(TEST_IMG_NAME) $(TEST_DIR)/$(TEST_LOW_TARGET)

.PHONY += clean_$(TEST_HIGH_TARGET)
clean_$(TEST_HIGH_TARGET):
	rm -r -v $(TEST_DIR)/$(TEST_IMG_NAME) $(TEST_DIR)/$(TEST_HIGH_TARGET) 


# =============================================================================
# mkfs section: make the mkfs program

MKFS_CC = gcc
MKFS_CFLAGS = -O2
MKFS_CPPFLAGS = $(addprefix -I ,$(MKFS_INC_DIR))

MKFS_INC_DIR := user 
MKFS_SRC_DIR := src 
MKFS_PROG_DIR := prog
MKFS_RESULT := result

MKFS_SRCS := $(shell find $(MKFS_SRC_DIR) -name "*.c")

mkfs:
	$(MKFS_CC) $(MKFS_CFLAGS) $(MKFS_CPPFLAGS) $(MKFS_PROG_DIR)/$@.c $(MKFS_SRCS) -o $(MKFS_RESULT)/$@

# =============================================================================
# copy section: copy the final to the server/lib. This is just for
# making developing easier.

copy:
	cp $(FINAL_RESULT_DIR)/$(REALNAME) ../server/lib
	cp user/user.h ../server/include/bufs.h