cmake_minimum_required(VERSION 3.29)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserv
		src/main.cpp
		src/utils/Log.cpp
		src/utils/Log.h
#		src/server/ConnectionHandler.cpp
#		src/server/ConnectionHandler.h
		src/server/TcpServer.cpp
		src/server/TcpServer.h
		src/config/Config.cpp
		src/config/Config.h
		src/utils/String.cpp
		src/utils/String.h)

target_compile_options(webserv PRIVATE
		-Wall -Wextra -Wpedantic -Wnon-virtual-dtor -Wconversion -Wsign-conversion -Werror)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DWEBSERV_DEBUG")