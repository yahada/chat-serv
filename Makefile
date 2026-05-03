CXX := g++
CXXFLAGS := -std=c++17 -I/opt/homebrew/include -Wall -Wextra
Q := @
BUILD_DIR := lib
SRC_DIR := src

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

TARGET := $(BUILD_DIR)/main

main: $(TARGET)

$(TARGET): $(OBJS)
	$(Q)printf "[CXX] %s\n" "$<"
	$(Q)mkdir -p $(BUILD_DIR)
	$(Q)$(CXX) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(Q)printf "[CXX] %s\n" "$<"
	$(Q)mkdir -p $(BUILD_DIR)
	$(Q)$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

run: $(TARGET)
	$(Q)printf "[RUNNING] %s\n" "$<"
	$(Q)./$(TARGET)

clean:
	$(Q)printf "[CLEAN] %s\n" "$<"
	$(Q)rm -rf $(BUILD_DIR)

-include $(DEPS)
