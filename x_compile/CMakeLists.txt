# ----------------------------------------------------------------------------
#  Root CMake file
# ----------------------------------------------------------------------------
#
# To perform an out of source build do the following on the command line:
# 1. navigate to the root project directory
# 2. mkdir build
# 3. cd build
# 4. depending on your system (windows/linux) type the following
# Example CMake build line for windows & VS2015
# windows: cmake -G "Visual Studio 14 2015 Win64" ..
# linux: cmake -DCROSS_COMPILE=ON ..
#
# 5. To compile:
# windows: cmake --build . --config Release
# linux: cmake --build . --config Release -- -j4
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

project(test_lib VERSION 1.0 DESCRIPTION "Shared library generation code")
set(LIB_NAME "test_gen")
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

# ----------------------------------------------------------------------------
if(WIN32)
    message(" ")
    message(STATUS "Windows")
    message(" ")

else()
    set(HOME "$ENV{HOME}" CACHE INTERNAL "Copied from environment variable")

    message(" ")
    message(STATUS "Linux")
    message(" ")

    # checking for cross compile.  Only setup for the Linux side
    if(CROSS_COMPILE)
    
        message(" ")
        message(STATUS "Cross Compile option selected")
        message(" ")

        # use pluto-0.35
        # wget https://github.com/analogdevicesinc/plutosdr-fw/releases/download/v0.35/sysroot-v0.35.tar.gz -P ~/Projects/
        SET(CMAKE_SYSROOT ${HOME}/Projects/pluto-0.35.sysroot/)
            
    endif()
    
    set(CMAKE_CXX_FLAGS "-O3")
    
endif()

#------------------------------------------------------------------------------
# look for any *.c and *.cpp source files
file(GLOB SOURCES "src/*.c*"
    )

set(project_includes ${project_includes}
    "include" 
    "${PR_ROOT}/common/include"
    )
    
set(project_libs ${project_libs}
    )

#------------------------------------------------------------------------------
message(" ")
message("--------------------------------------------------------------------------------")
message("Source Files: ")
foreach(S ${SOURCES})
    message("  " ${S})
endforeach()
message("--------------------------------------------------------------------------------")

message(" ")

message("--------------------------------------------------------------------------------")
message("Project Includes: ")
foreach(S ${project_includes})
    message("  " ${S})
endforeach()
message("--------------------------------------------------------------------------------")

message(" ")

message("--------------------------------------------------------------------------------")
message("Project Libraries: ")
foreach(S ${project_libs})
    message("  " ${S})
endforeach()
message("--------------------------------------------------------------------------------")

message(" ")

#------------------------------------------------------------------------------
message("Building Library...")


include_directories(${project_includes})

if(WIN32)

    add_definitions(-DDLL_EXPORTS)

    add_library(${LIB_NAME} SHARED ${SOURCES})
    set_target_properties(${LIB_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
    target_link_libraries(${LIB_NAME} ${project_libs})

    if(MSVC AND (${CMAKE_VERSION} VERSION_GREATER "3.6.0"))
        message("MSVC Version: " ${MSVC_VERSION})
        target_compile_options(${LIB_NAME} PRIVATE "/MP")
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${LIB_NAME})
    endif() 

elseif(UNIX)

    # Change these to point to the right location for a particular machine
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    add_library(${LIB_NAME} SHARED ${SOURCES})
    set_target_properties(${LIB_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
    target_link_libraries(${LIB_NAME} ${project_libs})
endif()

# ----------------------------------------------------------------------------
message(" ")
message(STATUS "Build Complete!")
message(STATUS "cmake --build . --config Release -- -j4")
message(" ")

