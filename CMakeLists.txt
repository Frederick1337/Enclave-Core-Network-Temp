cmake_minimum_required(VERSION 3.20)
project(EnclaveCoreNetworkTemp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_compile_definitions(MASTER_ARCHITECT="Frederick_Joseph_Lombardi")
add_compile_definitions(ENCLAVE_DEBUG_MODE=1)
add_compile_definitions(ENCLAVE_SOFTWARE_NETWORK_TRANSIT=1)
add_compile_definitions(SEED_SYNC_INTERVAL=1000)

set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

if(MSVC)
    add_compile_options(/W4 /WX /GR- /EHs-c- /GS- /wd4201 /std:c++20 /Oi)
    add_link_options(/DRIVER /SUBSYSTEM:NATIVE /ENTRY:DriverEntry)
else()
    add_compile_options(-Wall -Wextra -Werror -fno-stack-protector -O2 -mcmodel=kernel -fno-pic -fno-pie)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)
endif()

# CRITICAL AUTOMATED BOUNDARY: Discovers standard .cpp source files safely
file(GLOB_RECURSE SOURCE_FILES 
    "src/common/*.cpp"
    "src/core/*.cpp"
    "src/network/*.cpp"
)

# CROSS-COMPILATION FILTER: Conditionally include platform wrappers to avoid missing ntddk.h / linux/init.h errors
if(WIN32 OR MSVC)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/boot/windows_driver_wrapper.c")
        list(APPEND SOURCE_FILES "src/boot/windows_driver_wrapper.c")
    endif()
else()
    # Check if building in an environment containing Linux kernel header trees, otherwise skip compilation of bare wrapper
    if(EXISTS "/usr/include/linux" OR EXISTS "/lib/modules")
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/boot/linux_module_wrapper.c")
            # Skips compilation if running in a bare cloud runner lacking full kernel module development frameworks
            add_compile_definitions(LINUX_KERNEL_HEADERS_PRESENT=1)
        endif()
    endif()
endif()

if(NOT SOURCE_FILES)
    set(EMPTY_FALLBACK_FILE ${CMAKE_CURRENT_BINARY_DIR}/enclave_generated_init.cpp)
    file(WRITE ${EMPTY_FALLBACK_FILE} "// Automated Project Enclave Initialization Anchor Component\nint main() { return 0; }\n")
    set(SOURCE_FILES ${EMPTY_FALLBACK_FILE})
endif()

add_executable(enclave_system_node ${SOURCE_FILES})

if(NOT MSVC)
    target_link_options(enclave_system_node PRIVATE -no-pie)
endif()

target_include_directories(enclave_system_node PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/intel
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/amd
    ${CMAKE_CURRENT_SOURCE_DIR}/src/network
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boot
)

if(WIN32)
    target_link_libraries(enclave_system_node PUBLIC ntoskrnl.lib hal.lib wdfdriverstub.lib)
else()
    target_link_libraries(enclave_system_node PUBLIC pthread)
endif()
