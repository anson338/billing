# CMakeList.txt: billing 的 CMake 项目，包括源和定义
# 此处特定于项目的逻辑。
#
cmake_minimum_required (VERSION 2.8)

# 添加依赖
add_definitions("-DASIO_STANDALONE")
include_directories("deps/json" "deps/asio/asio/include")
#mysql
IF (WIN32)
	add_definitions("-D_WIN32_WINNT=0x0501")
	MESSAGE(STATUS "Now is windows")
	include_directories("deps/mysql/mysql-connector-c-6.1.11-win32/include")
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/deps/mysql/mysql-connector-c-6.1.11-win32/lib/vs14")
	link_libraries("mysqlclient")
ELSEIF (APPLE)
	MESSAGE(STATUS "Now is Apple systens.")
ELSEIF (UNIX)
#64位环境编译32位时需要安装的
#apt-get install gcc-multilib g++-multilib
#指定特定版本
#apt-get install gcc-4.9-multilib g++-4.9-multilib
	set(CMAKE_BUILD_TYPE "Release")
	set(CMAKE_CXX_FLAGS "-std=c++11 -m32 ${CMAKE_CXX_FLAGS}")
	set(CMAKE_C_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
	MESSAGE(STATUS "Now is UNIX-like OS's. Including aPPLE os x  and CygWin")
	include_directories("deps/mysql/mysql-connector-c-6.1.11-linux-i686/include")
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/deps/mysql/mysql-connector-c-6.1.11-linux-i686/lib")
	link_libraries("libmysqlclient.a" "pthread" "dl")
ENDIF ()
# 将源添加到此项目的可执行文件。
add_executable (billing "main.cpp" "inc/common.hpp" "inc/billing_server.hpp" "billing_server.cpp" 
"inc/server_config.hpp" "server_config.cpp" "inc/logger.hpp" "logger.cpp" 
"inc/client_connection.hpp" "client_connection.cpp"
"inc/hex_tool.hpp" "inc/hex_tool.cpp"
"inc/billing_data.hpp" "inc/billing_data.cpp" "inc/request_handler.hpp" 
"inc/account_model.hpp" "inc/account_model.cpp")

# TODO: 如有需要，请添加测试并安装目标。