# CMakeList.txt: billing 的 CMake 项目，包括源和定义
# 此处特定于项目的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 添加依赖
include_directories("deps/json" "deps/asio/asio/include")
#mysql
IF (WIN32)
	MESSAGE(STATUS "Now is windows")
	include_directories("deps/mysql/mysql-connector-c-6.1.11-win32/include")
	link_directories("deps/mysql/mysql-connector-c-6.1.11-win32/lib/vs14")
	link_libraries("mysqlclient")
ELSEIF (APPLE)
	MESSAGE(STATUS "Now is Apple systens.")
ELSEIF (UNIX)
	MESSAGE(STATUS "Now is UNIX-like OS's. Including aPPLE os x  and CygWin")
	include_directories("deps/mysql/mysql-connector-c-6.1.11-linux-i686/include")
	link_directories("deps/mysql/mysql-connector-c-6.1.11-linux-i686/lib")
	link_libraries("libmysqlclient")
ENDIF ()
# 将源添加到此项目的可执行文件。
add_executable (billing "main.cpp" "inc/common.hpp" "inc/billing_server.hpp" "billing_server.cpp" 
"inc/server_config.hpp" "server_config.cpp" "inc/logger.hpp" "logger.cpp" 
"inc/client_connection.hpp" "client_connection.cpp"
"inc/hex_tool.hpp" "inc/hex_tool.cpp"
"inc/billing_data.hpp" "inc/billing_data.cpp" "inc/request_handler.hpp" "inc/handler/connect_handler.hpp")

# TODO: 如有需要，请添加测试并安装目标。