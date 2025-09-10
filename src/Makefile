# ##############################################################################
# FILE:         Makefile
# DESCRIPTION:  CROW CRC Module Makefile.
# AUTHOR:       Peter Hillerström, <prohstream@gmail.com>
# COPYRIGHT:    prohstream@gmail.com
# LICENSE:      MIT
# DATE:         4 jul 2025
# ###############################################################################
BUILD_DIR   := build
INCL_DIRS   := . ..
TEST_DIR    := test

C_SRCS      := $(wildcard *.c)
CPP_SRCS    := $(wildcard *.cpp)
TEST_SRCS   := $(wildcard $(TEST_DIR)/test_*.c)

C_OBJS      := $(addprefix $(BUILD_DIR)/, $(C_SRCS:.c=.o))
W_OBJS      := $(addprefix $(BUILD_DIR)/, $(C_SRCS:.c=.obj))
CPP_OBJS    := $(addprefix $(BUILD_DIR)/, $(CPP_SRCS:.cpp=.o))
WPP_OBJS    := $(addprefix $(BUILD_DIR)/, $(CPP_SRCS:.cpp=.obj))
LIBS        := $(addprefix lib, $(C_SRCS:.c=.so))
DLLS        := $(addprefix lib, $(C_SRCS:.c=.dll))
TEST_BINS   := $(addprefix $(BUILD_DIR)/, $(notdir $(TEST_SRCS:.c=)))

CC          := gcc
CXX         := g++
WCC         := x86_64-w64-mingw32-gcc # i686-w64-mingw32-gcc for 32 bit
WXX         := x86_64-w64-mingw32-g++ # i686-w64-mingw32-g++ for 32 bit
CFLAGS      := -Wall -Wextra -O2 -fPIC $(addprefix -I, $(INCL_DIRS))
WCFLAGS     := -Wall -Wextra -O2 $(addprefix -I, $(INCL_DIRS))
CXXFLAGS    := -Wall -Wextra -O2 -fPIC $(addprefix -I, $(INCL_DIRS))
WXXFLAGS    := -Wall -Wextra -O2 $(addprefix -I, $(INCL_DIRS))
LDFLAGS     :=

.PHONY: all win libs dlls objs wobjs tests clean

# ===== GNU Targets =====

all: libs objs

libs: $(LIBS)

objs: $(C_OBJS) $(CPP_OBJS)

# ===== MinGW Targets =====

win: dlls wobjs

dlls: $(DLLS)

wobjs: $(W_OBJS) $(WPP_OBJS)

# ===== Tests =====

tests: $(TEST_BINS)
	@echo "--- Running C/C++ Unittests ------------------------------------------"
	@for bin in $(TEST_BINS); do echo "Running $$bin"; ./$$bin; done
	@echo "--- Running Python Unittests -----------------------------------------"
	@python3 -m unittest discover -s $(TEST_DIR) -p "test_*.py"

# ===== Clean =====

clean:
	rm -rf $(BUILD_DIR) *.so *.dll $(TEST_BINS)
	find . -type d -name __pycache__ -exec rm -rf {} +

# ===== Rules =====

lib%.so: %.c
	$(CC) $(CFLAGS) -DBUILD_LIB -shared -o $@ $<

lib%.dll: %.c
	$(WCC) $(WCFLAGS) -DBUILD_LIB -shared -o $@ $<

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.obj: %.c | $(BUILD_DIR)
	$(WCC) $(WCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.obj: %.cpp | $(BUILD_DIR)
	$(WXX) $(WXXFLAGS) -c $< -o $@

$(BUILD_DIR)/test_%: $(TEST_DIR)/test_%.c $(BUILD_DIR)/%.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Create build dir if missing
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

