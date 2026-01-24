# RPG Seed - Makefile
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -O2
DEBUG_FLAGS = -g -DDEBUG

# SDL2 configuration
SDL2_CFLAGS := $(shell sdl2-config --cflags)
SDL2_LDFLAGS := $(shell sdl2-config --libs) -lSDL2_image

# Google Test configuration
GTEST_DIR = /opt/homebrew/opt/googletest
GTEST_CFLAGS = -I$(GTEST_DIR)/include
GTEST_LDFLAGS = -L$(GTEST_DIR)/lib -lgtest -lgtest_main -pthread

# Directories
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp) \
       $(wildcard $(SRC_DIR)/game/*.cpp) \
       $(wildcard $(SRC_DIR)/field/*.cpp) \
       $(wildcard $(SRC_DIR)/system/*.cpp) \
       $(wildcard $(SRC_DIR)/entity/*.cpp) \
       $(wildcard $(SRC_DIR)/ui/*.cpp) \
       $(wildcard $(SRC_DIR)/inventory/*.cpp) \
       $(wildcard $(SRC_DIR)/save/*.cpp) \
       $(wildcard $(SRC_DIR)/battle/*.cpp) \
       $(wildcard $(SRC_DIR)/collection/*.cpp)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Test source files
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/test/%.o,$(TEST_SRCS))

# Library objects (exclude main.o for tests)
LIB_OBJS = $(filter-out $(BUILD_DIR)/main.o,$(OBJS))

# Target
TARGET = rpg_seed
TEST_TARGET = run_tests

.PHONY: all clean test debug dirs

all: dirs $(TARGET)

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(SDL2_CFLAGS) -o $@ $^ $(SDL2_LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(SDL2_CFLAGS) -I$(SRC_DIR) -c -o $@ $<

# Test build
test: dirs $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(LIB_OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(SDL2_CFLAGS) $(GTEST_CFLAGS) -o $@ $^ $(SDL2_LDFLAGS) $(GTEST_LDFLAGS)

$(BUILD_DIR)/test/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(SDL2_CFLAGS) $(GTEST_CFLAGS) -I$(SRC_DIR) -c -o $@ $<

dirs:
	@mkdir -p $(BUILD_DIR)/game $(BUILD_DIR)/field $(BUILD_DIR)/system $(BUILD_DIR)/entity $(BUILD_DIR)/ui $(BUILD_DIR)/inventory $(BUILD_DIR)/save $(BUILD_DIR)/battle $(BUILD_DIR)/collection $(BUILD_DIR)/test

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)

# Dependencies
-include $(OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)
