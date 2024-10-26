# Compiler
###########################
CXX				:= c++
CXXFLAGS		:= -std=c++98 -I$(SRC_DIR)
DEBUG_FLAGS		:= -g3 -O0 -DWEBSERV_DEBUG \
				   -Wall -Wextra -Wpedantic -Wnon-virtual-dtor -Wconversion -Wsign-conversion -Werror
RELEASE_FLAGS	:= -O3 -DNDEBUG

# Directories/files
###########################
SRC_DIR			:= src
BUILD_DIR		:= build
DEBUG_DIR		:= $(BUILD_DIR)/debug
RELEASE_DIR		:= $(BUILD_DIR)/release

SOURCES			:= $(SRC_DIR)/main.cpp $(SRC_DIR)/utils/Log.cpp $(SRC_DIR)/server/TcpServer.cpp
DEBUG_OBJS		:= $(patsubst $(SRC_DIR)/%.cpp, $(DEBUG_DIR)/%.o, $(SOURCES))
RELEASE_OBJS	:= $(patsubst $(SRC_DIR)/%.cpp, $(RELEASE_DIR)/%.o, $(SOURCES))

DEBUG_EXEC		:= $(DEBUG_DIR)/webserv
RELEASE_EXEC	:= $(RELEASE_DIR)/webserv

# Targets
###########################
.PHONY:	all debug release

all: debug

# debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_EXEC)
	@if [ -n "$?" ]; then \
		echo "Executable is located at: $(DEBUG_EXEC)"; \
	fi
	@cp $(DEBUG_EXEC) .

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DEBUG_EXEC): $(DEBUG_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@if [ -n "$?" ]; then \
		echo -n "Debug build completed. "; \
	fi

# release
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(RELEASE_EXEC)
	@if [ -n "$?" ]; then \
		echo "Executable is located at: $(RELEASE_EXEC)"; \
	fi

$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(RELEASE_EXEC): $(RELEASE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@if [ -n "$?" ]; then \
		echo -n "Release build completed. "; \
	fi

# clean
.PHONY: clean fclean re

clean:
	@rm -rf $(DEBUG_OBJS)
	@rm -rf $(RELEASE_OBJS)
	@echo "Object files cleaned."

fclean:
	@rm -rf $(BUILD_DIR)
	@rm -f webserv
	@echo "Build outputs cleaned."

re:	fclean all
