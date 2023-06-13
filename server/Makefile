# -------------------------------------------------------------------------------
# |																			    |
# |		 Project Management Makefile of Simple Distributed File System Server   |
# |																			    |
# -------------------------------------------------------------------------------
#
#	Type `make help` to see what can this Makefile do.
#
#
TARGETS = help

.PHONY : help
help:
	@echo "Supported Make Targets:"
	@echo ${foreach target,$(TARGETS),"\t->$(target)\n"}

# ================================================================================
#								Section: Release
#	This section has one target: release. Type `make release` to call this target.
#   The release version will be generated in the `result` direcotry.
#   Do NOT change anything in the `result` directory manually because of the 
#	Dynamic linking.
#
#	As you can see, this is not actually a typical release version of a server.
#	Because we don't have any configuration for system service and so on.
#	But in this lab project, I think it is fine.
# ================================================================================

RELEASE_CC := gcc
RELEASE_CFLAGS := -c -Wall
RELEASE_CPPFLAGS = ${addprefix -I ,$(RELEASE_INC_DIR)}

RELEASE_LDFLAGS = -Wl,-rpath,'$$ORIGIN/../lib' -L$(RELEASE_LIB_DIR) -lbufs 
RELEASE_INC_DIR := ./include ./src
RELEASE_BUILD_DIR := ./build
RELEASE_RESULT_DIR := ./result
RELEASE_LIB_DIR := ./lib
RELEASE_SRC_DIR := src
RELEASE_EXTRAS_DIR := ./extras

RELEASE_MKFS_PROG := mkfs
RELEASE_BUFS_IMG_NAME := bufs_img

RELEASE_SRCS = $(shell find $(RELEASE_SRC_DIR) -name "*.c")
RELEASE_OBJS = ${patsubst %.c,$(RELEASE_BUILD_DIR)/%.o,$(RELEASE_SRCS)}

RELEASE_TARGET := release
RELEASE_SERVER_FILE_NAME := server

.PHONY += $(RELEASE_TARGET)
TARGETS += $(RELEASE_TARGET)


$(RELEASE_TARGET):$(RELEASE_OBJS)   
	mkdir -p $(RELEASE_RESULT_DIR)
	$(RELEASE_CC) $^ -o $(RELEASE_RESULT_DIR)/$(RELEASE_SERVER_FILE_NAME) $(RELEASE_LDFLAGS)
	cp $(RELEASE_EXTRAS_DIR)/$(RELEASE_MKFS_PROG) $(RELEASE_RESULT_DIR)
	$(RELEASE_EXTRAS_DIR)/$(RELEASE_MKFS_PROG) $(RELEASE_RESULT_DIR)/$(RELEASE_BUFS_IMG_NAME)

$(RELEASE_BUILD_DIR)/%.o:%.c 
	mkdir -p $(dir $@)
	$(RELEASE_CC) $(RELEASE_CPPFLAGS) $(RELEASE_CFLAGS) $< -o $@

RELEASE_CLEAN_TARGET := clean_release

.PHONY += $(RELEASE_CLEAN_TARGET)
TARGETS += $(RELEASE_CLEAN_TARGET)

$(RELEASE_CLEAN_TARGET):
	rm -rf $(RELEASE_RESULT_DIR)
	rm -rf $(RELEASE_BUILD_DIR)

# ================================================================================
#								Section: Test
#	This section has two targets: test and rebuild_test.
#   Each time you try rebuild_test, all the files in `build`
#	will be removed and the test_img will be new one by mkfs.
# ================================================================================
TEST_CC := gcc
TEST_CFLAGS := -c -Wall
TEST_CPPFLAGS = -D_TEST_CONFIG=1 ${addprefix -I ,$(TEST_INC_DIR)}

TEST_LDFLAGS = -Wl,-rpath,'$$ORIGIN/../lib' -L$(TEST_LIB_DIR) -lbufs 
TEST_INC_DIR := ./include ./src
TEST_BUILD_DIR := test_build
TEST_RESULT_DIR := test
TEST_LIB_DIR := ./lib

TEST_SRCS := $(shell find ./src -name "*.c")


TEST_OBJS := ${patsubst %.c,$(TEST_BUILD_DIR)/%.o,$(TEST_SRCS)}
TEST_PORT := 6828
TEST_IMG_NAME := test_img

.PHONY += test
TARGETS += test
test:$(TEST_OBJS)
	@$(TEST_RESULT_DIR)/mkfs $(TEST_RESULT_DIR)/$(TEST_IMG_NAME)
	@$(TEST_CC) $^ -o $(TEST_RESULT_DIR)/$@ $(TEST_LDFLAGS) 
	@$(TEST_RESULT_DIR)/$@ $(TEST_PORT) $(TEST_RESULT_DIR)/$(TEST_IMG_NAME)

$(TEST_BUILD_DIR)/%.o:%.c
	@mkdir -p $(dir $@)
	@$(TEST_CC) $(TEST_CPPFLAGS) $(TEST_CFLAGS) $< -o $@

.PHONY += rebuild_test
TARGETS += rebuild_test

rebuild_test:clean_test test

.PHONY += clean_test
TARGETS += clean_test
clean_test:
	@rm -f $(TEST_RESULT_DIR)/$(TEST_IMG_NAME)
	@rm -r build