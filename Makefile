TARGET_EXEC = a-star

CPU ?= $(shell uname -m)

BIN_DIR   = bin/$(CPU)
BUILD_DIR = build/$(CPU)
SRC_DIRS  = src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS = -Iinclude

CPPFLAGS = $(INC_DIRS) -MMD -MP --std=c++17 -g
LDFLAGS  = -lpthread   \
           -lX11       \
           -lGL        \
           -lpng       \
           -lstdc++fs

all: $(BIN_DIR)/$(TARGET_EXEC)

$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	@$(MKDIR_P) $(dir $@)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r bin
	$(RM) -r build

-include $(DEPS)

MKDIR_P ?= mkdir -p
