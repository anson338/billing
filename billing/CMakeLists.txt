# CMakeList.txt: billing 的 CMake 项目，包括源和定义
# 此处特定于项目的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 将源添加到此项目的可执行文件。
add_executable (billing "main.cpp" "inc/common.hpp" "inc/billing_server.hpp" "billing_server.cpp" 
"inc/server_config.hpp" "server_config.cpp" "inc/logger.hpp" "logger.cpp" 
"inc/client_connection.hpp" "client_connection.cpp")

# TODO: 如有需要，请添加测试并安装目标。
# 添加依赖
include_directories("deps/json" "deps/asio/asio/include")