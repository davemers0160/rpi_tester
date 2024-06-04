# ----------------------------------------------------------------------------
#  Root CMake file for ML Project
# ----------------------------------------------------------------------------
#
# Example CMake build line for windows & VS2015
# windows w/cuda: cmake -G "Visual Studio 14 2015 Win64" -DUSE_AVX_INSTRUCTIONS=ON ..
# linux w/o cuda: cmake -DUSE_AVX_INSTRUCTIONS=ON -DDLIB_USE_CUDA=OFF ..
#
#
# To compile:
# windows: cmake --build . --config Release
# linux: cmake --build . --config Release -- -j10
#

cmake_minimum_required(VERSION 3.9)

#------------------------------------------------------------------------------
# Custom Build Options
option(CROSS_COMPILE "Switch that tells CMake that we want to cross compile for an ARM based processor" OFF)
option(BUILD_ON_PI "Switch that tells CMake to cross compile on a Raspberry PI or Linux based PC" OFF)

#------------------------------------------------------------------------------
# checking for cross compile.  Only setup for the Linux side
if(CROSS_COMPILE)
    # the name of the target operating system
    SET(CMAKE_SYSTEM_NAME Linux)
    SET(CMAKE_SYSTEM_VERSION 1)
    SET(CMAKE_SYSTEM_PROCESSOR arm)

    set(CMAKE_C_COMPILE_OPTIONS_SYSROOT "--sysroot=") 
    set(CMAKE_CXX_COMPILE_OPTIONS_SYSROOT "--sysroot=")

    # setting which compilers to use for C and C++ cross compile
    # use gcc-arm-9.2-2019.12-arch64arm
    # RPI: wget https://developer.arm.com/-/media/Files/downloads/gnu-a/9.2-2019.12/binrel/gcc-arm-9.2-2019.12-aarch64-arm-none-linux-gnueabihf.tar.xz -P /opt/
    # PC: wget https://developer.arm.com/-/media/Files/downloads/gnu-a/9.2-2019.12/binrel/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf.tar.xz -P /opt/ 
    if(BUILD_ON_PI)
        set(CMAKE_C_COMPILER   /opt/gcc-arm-9.2-2019.12-aarch64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-gcc)
        set(CMAKE_CXX_COMPILER /opt/gcc-arm-9.2-2019.12-aarch64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-g++)
    else()
        set(CMAKE_C_COMPILER   /opt/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-gcc)
        set(CMAKE_CXX_COMPILER /opt/gcc-arm-9.2-2019.12-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-g++)
    endif()
    
    # adjust the default behavior of the FIND_XXX() commands:
    # search programs in the host environment
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

    # search headers and libraries in the target environment
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
    
endif()

#------------------------------------------------------------------------------
PROJECT(burst_test_project)
set(exe_name "burst_test")
set(default_build_type "Release")

set(CMAKE_CXX_STANDARD 11)

set(PLATFORM "$ENV{PLATFORM}" CACHE INTERNAL "Copied from environment variable")
if("${PLATFORM}" STREQUAL "")
    set(PLATFORM "NOT_SET")
    message("The PLATFORM Environment variable is not set on this system.  Setting it to:")
endif()

message("Platform: " ${PLATFORM})

# find the root of the main project
string(FIND ${PROJECT_SOURCE_DIR} "/" INDEX REVERSE)
string(SUBSTRING ${PROJECT_SOURCE_DIR} 0 ${INDEX} PR_ROOT)

#------------------------------------------------------------------------------
if(WIN32)
    if(${PLATFORM} STREQUAL "Laptop")
        set(COMMON_ROOT "D:/Common")
    elseif(${PLATFORM} STREQUAL "LaptopN")
        set(COMMON_ROOT "C:/Projects/Common")   
    elseif(${PLATFORM} STREQUAL "BEAST")
        set(COMMON_ROOT "D:/Common")
    else()
        set(COMMON_ROOT "D:/Common")
    endif()
    
    set(TEST_GEN_DIRS "${PR_ROOT}/x_compile/include")
    set(TEST_GEN_LIBS "${PR_ROOT}/x_compile/build/Debug/test_gen.lib")

else()

    set(HOME "$ENV{HOME}" CACHE INTERNAL "Copied from environment variable")

    if(${PLATFORM} STREQUAL "HPC")
        set(COMMON_ROOT "~/Projects/Common")
    elseif(${PLATFORM} STREQUAL "MainGear")
        set(COMMON_ROOT "~/Projects/Common")
    else()
        set(COMMON_ROOT "~/Projects/Common")
    endif()

    set(TEST_GEN_DIRS "${PR_ROOT}/x_compile/include")
    set(TEST_GEN_LIBS "${PR_ROOT}/x_compile/build/libtest_gen.so")
    
    # checking for cross compile.  Only setup for the Linux side
    if(CROSS_COMPILE)
    
        message(" ")
        message(STATUS "Cross Compile option selected")
        message(" ")

        # use pluto-0.35
        # wget https://github.com/analogdevicesinc/plutosdr-fw/releases/download/v0.35/sysroot-v0.35.tar.gz -P ~/Projects/
        SET(CMAKE_SYSROOT ${HOME}/Projects/pluto-0.35.sysroot/)
            
    endif()
    
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    
endif()


# grab all of the *.cpp files in the src directory
file(GLOB SOURCES "src/*.cpp" 
    )

set(project_includes "include" 
    "${PR_ROOT}/common/include"
    "${TEST_GEN_DIRS}"
    )
    
set(project_libs ${project_libs}
    "${TEST_GEN_LIBS}"
    )

#------------------------------------------------------------------------------
message(" ")
message("--------------------------------------------------------")
message("Source Files: ")
foreach(S ${SOURCES})
    message("  " ${S})
endforeach()
message("--------------------------------------------------------")

message(" ")

message("--------------------------------------------------------")
message("Project Includes: ")
foreach(S ${project_includes})
    message("  " ${S})
endforeach()
message("--------------------------------------------------------")

message(" ")

message("--------------------------------------------------------------------------------")
message("Project Libraries: ")
foreach(S ${project_libs})
    message("  " ${S})
endforeach()
message("--------------------------------------------------------------------------------")

message(" ")

#------------------------------------------------------------------------------
include_directories(${project_includes})
add_executable(${exe_name} ${SOURCES})
target_link_libraries(${exe_name} ${project_libs})


if(WIN32)
    #target_link_libraries(${exe_name} dlib::dlib ${OpenCV_LIBS} ${OBJ_DET_LIBS})
    message("MSVC Version: " ${MSVC_VERSION})
    
    # uncomment for MSVC and cmake greater than 3.6.0
    if(MSVC AND (${CMAKE_VERSION} VERSION_GREATER "3.6.0"))
        target_compile_options(${exe_name} PRIVATE "/MP")
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${exe_name})
    endif()
    
else()
    #target_link_libraries(${exe_name} ${OpenCV_LIBS} ${OBJ_DET_LIBS})
endif()

# ----------------------------------------------------------------------------
message(" ")
message(STATUS "Build Complete!")
message(STATUS "cmake --build . --config Release -- -j4")
message(" ")

