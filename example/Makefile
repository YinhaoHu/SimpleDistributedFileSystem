# =======================================================================================
#								Makefile of Examples
#	
#	Make the specified target of the supported targets.
#	Type `make help` or just `make` to see what the suppoted targets are.				
# =======================================================================================


TARGETS := help
.PHONY : help

help:
	@echo "Supported targets:"
	@echo $(foreach target,$(TARGETS),"\t->$(target)\n")

# =======================================================================================
#								Section: Generic Variables
# =======================================================================================
CC := gcc
CFLAGS := -c -Wall -g
CPPFLAGS = $(addprefix -I ,$(INC_DIR))
LDFLAGS = -Wl,-rpath,'$$ORIGIN/lib' -L$(LIB_DIR) -lsdfs 

INC_DIR := include
LIB_DIR := lib

# =======================================================================================
#								Section: example1
#	 See the header in example1.c for more information.
# =======================================================================================

EXAMPLE1_SRC := example1.c
EXAMPLE1_OBJ := $(subst .c,.o,$(EXAMPLE1_SRC))

EXAMPLE1_TARGET := example1

TARGETS += $(EXAMPLE1_TARGET)
.PHONY += $(EXAMPLE1_TARGET)

$(EXAMPLE1_TARGET): $(EXAMPLE1_OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	rm -rf $^

%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ 

# =======================================================================================
#								Section: example2
#	 See the header in example2.c for more information.
# =======================================================================================


EXAMPLE2_SRC := example2.c
EXAMPLE2_OBJ := $(subst .c,.o,$(EXAMPLE2_SRC))

EXAMPLE2_TARGET := example2

TARGETS += $(EXAMPLE2_TARGET)
.PHONY += $(EXAMPLE2_TARGET)

$(EXAMPLE2_TARGET): $(EXAMPLE2_OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	rm -rf $^

%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ 
