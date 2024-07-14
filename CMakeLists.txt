cmake_minimum_required(VERSION 3.12)

project(qt-databinder)

if (WIN32)
  #编译使用utf8文件
  add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

add_subdirectory(test)