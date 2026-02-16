# 1. 声明 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 2. 声明项目名称，并明确指定语言为 C (注意这里的 C)
project(HANDMADE-HTTP-SERVER LANGUAGES C)

if(WIN32)
    # 强制编译器将执行时的字符集转为 GBK（适配 Windows 控制台）
    add_compile_options("$<$<C_COMPILER_ID:GNU>:-fexec-charset=GBK>")
    add_compile_options("$<$<C_COMPILER_ID:GNU>:-finput-charset=UTF-8>")
endif()


# 3. 设置 C 语言标准（例如 C11 或 C99）
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 自动搜寻 src 目录下的所有 .c 文件
file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS "src/*.c")

# 告诉 CMake 头文件在 include 文件夹里
include_directories(include)

# 编译
add_executable(tiny-server ${SRC_LIST})