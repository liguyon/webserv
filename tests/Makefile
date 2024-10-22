# Compiler
###########################
CXX				:= c++
CXXFLAGS		:= -std=c++98 -I$(SRC_DIR)
DEBUG_FLAGS		:= -g -O0 -DWEBSERV_DEBUG \
				   -Wall -Wextra -Wpedantic -Wnon-virtual-dtor -Wconversion -Wsign-conversion -Werror

# Directories/files
###########################
SRC_DIR			:= ../src

# Targets
###########################
.PHONY: all test_logger

all: test_logger

test_logger:
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) test_logger.cpp $(SRC_DIR)/utils/Log.cpp -I$(SRC_DIR)/utils -o test_logger

# clean
.PHONY: clean

clean:
	@rm -f test_logger
	@echo "Build outputs cleaned."

