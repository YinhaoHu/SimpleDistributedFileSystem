# =============================================================================
# 		SDFS Client Side Project Management Makefile
# 		This Makefile does the following things
# 			* test
#			* build shared library
#
# 		Type `make help` to see the supported make commonds
# =============================================================================
TARGETS =help

.PHONY =help
help:
	@echo "Supported Make Targets:"
	@echo ${foreach target,$(TARGETS),"\t->$(target)\n"}

# =============================================================================
# 							Section: build final
# =============================================================================
FINAL_REALNAME := libsdfs.so.1.0.1
FINAL_SONAME := libsdfs.so.1

FINAL_CC := gcc
FINAL_CFLAGS := -c -Wall -O2 -fPIC
FINAL_LDFLAGS = -shared -Wl,-soname,$(FINAL_SONAME)
FINAL_CPPFLAGS = $(addprefix -I ,$(FINAL_INC_DIR))

FINAL_INC_DIR := src
FINAL_BUILD_DIR := build
FINAL_SRC_DIR := src
FINAL_RESULT_DIR := result

FINAL_SRCS := $(shell find $(FINAL_SRC_DIR) -name "*.c")
FINAL_OBJS := $(patsubst %.c,$(FINAL_BUILD_DIR)/%.o,$(FINAL_SRCS))

FINAL_TARGET := final

TARGETS += $(FINAL_TARGET)
.PHONY += $(FINAL_TARGET)

$(FINAL_TARGET):$(FINAL_OBJS)
	mkdir -p $(FINAL_RESULT_DIR)
	$(FINAL_CC) $(FINAL_LDFLAGS) $^ -o $(FINAL_RESULT_DIR)/$(FINAL_REALNAME) 

$(FINAL_BUILD_DIR)/%.o:%.c
	mkdir -p $(dir $@)
	$(FINAL_CC) $(FINAL_CPPFLAGS) $(FINAL_CFLAGS) $< -o $@

FINAL_CLEAN_TARGET := clean_final

TARGETS += $(FINAL_CLEAN_TARGET)
.PHONY += $(FINAL_CLEAN_TARGET)
$(FINAL_CLEAN_TARGET):
	-rm -rf $(FINAL_BUILD_DIR)
	-rm -rf $(FINAL_RESULT_DIR)

# =============================================================================
# 								Section: Test
# =============================================================================
TEST_CC := gcc

# option `-D_TEST_=1` is used for the test/buffer setting. see sdfs.h for more.
TEST_CPPFALGS = -D_TEST_=1 ${addprefix -I ,$(TEST_INC_DIR)} 
TEST_CFLAGS = -c -Wall -g

TEST_INC_DIR := src
TEST_BUILD_DIR := test_build

TEST_DRIVER = test/test_main.c
TEST_SRCS := $(shell find $(TEST_INC_DIR) -name "*.c") $(TEST_DRIVER)
TEST_OBJS := ${TEST_SRCS:%.c=$(TEST_BUILD_DIR)/%.o}
TEST_DIR := test

TEST_HOSTNAME = 127.0.0.1
TEST_SERVICE = 6828
TEST_TARGET := test
TEST_CLEAN_TARGET := clean_test
TEST_REBUILD_TARGET := rebuild_test
TEST_MODE := auto

TARGETS+=$(TEST_TARGET)
TARGETS+=$(TEST_CLEAN_TARGET)
TARGETS+=$(TEST_REBUILD_TARGET)

.PHONY += $(TEST_TARGET)
$(TEST_TARGET):$(TEST_OBJS)
	@$(TEST_CC) $^ -o $(TEST_DIR)/$@  
	@$(TEST_DIR)/$@ $(TEST_HOSTNAME) $(TEST_SERVICE) $(TEST_MODE)

$(TEST_BUILD_DIR)/%.o:%.c
	@mkdir -p $(dir $@)
	@$(TEST_CC) $(TEST_CPPFALGS) $(TEST_CFLAGS) $< -o $@

$(TEST_REBUILD_TARGET): $(TEST_CLEAN_TARGET) $(TEST_TARGET)

.PHONY += $(TEST_CLEAN_TARGET)
$(TEST_CLEAN_TARGET):
	@rm -f -r $(TEST_BUILD_DIR) $(TEST_DIR)/buffer
