# CMakeList.txt: billing 的 CMake 项目，包括源和定义
# 此处特定于项目的逻辑。
#
cmake_minimum_required (VERSION 2.8)

# 添加依赖
add_definitions("-DASIO_STANDALONE" "-DNDEBUG")
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
	set(CMAKE_BUILD_TYPE "MinSizeRel")
	set(CMAKE_CXX_FLAGS "-std=c++11 -m32 -Os -Wall ${CMAKE_CXX_FLAGS}")
	set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
	MESSAGE(STATUS "Now is UNIX-like OS's. Including aPPLE os x  and CygWin")
	include_directories("deps/mysql/mysql-connector-c-6.1.11-linux-i686/include")
	link_directories("${CMAKE_CURRENT_SOURCE_DIR}/deps/mysql/mysql-connector-c-6.1.11-linux-i686/lib")
	link_libraries("libmysqlclient.a" "pthread" "dl")
ENDIF ()
# 将源添加到此项目的可执行文件。
aux_source_directory(. BASE_SRCS)
aux_source_directory(./inc INC_SRCS)
add_executable (billing "${BASE_SRCS};${INC_SRCS}")

# TODO: 如有需要，请添加测试并安装目标。