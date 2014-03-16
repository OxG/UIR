cmake_minimum_required(VERSION 2.8)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/share/llvm-3.4/cmake")
include(LLVMConfig OPTIONAL)
include(LLVM-Config OPTIONAL)
set(LLVM_ROOT "/usr/" CACHE PATH "Root of LLVM install.")
# Define add_llvm_* macro's.
include(AddLLVM)
set(LLVM_ON_UNIX = true)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_ROOT}/include/llvm-3.4)
include_directories(${LLVM_ROOT}/include/llvm-c-3.4)
link_directories(${LLVM_ROOT}/lib/llvm-3.4)

add_subdirectory(Pass)
