# ----------------------------------------------------------------------------
#  Root CMake file for the bladeRF recieve example
# ----------------------------------------------------------------------------
#
# To perform an out of source build do the following on the command line:
# 1. navigate to the root project directory
# 2. mkdir build
# 3. cd build
# 4. depending on your system (windows/linux) type the following
# Example CMake build line for windows & VS2015
# windows: cmake -G "Visual Studio 14 2015 Win64" -DUSE_AVX_INSTRUCTIONS=ON ..
# linux: cmake -DUSE_AVX_INSTRUCTIONS=ON ..
#
# 5. To compile:
# windows: cmake --build . --config Release
# linux: cmake --build . --config Release -- -j4
#

cmake_minimum_required(VERSION 3.18)

if(WIN32)
         
    set(CMAKE_TOOLCHAIN_FILE "d:/Projects/vcpkg/scripts/buildsystems/vcpkg.cmake")  #CACHE STRING "")

endif()

PROJECT(arma_test)
set(exe_name "at")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PLATFORM "$ENV{PLATFORM}" CACHE INTERNAL "Copied from environment variable")
if("${PLATFORM}" STREQUAL "")
    set(PLATFORM "NOT_SET")
    message("The PLATFORM Environment variable is not set on this system.  Setting it to:")
endif()
message("Platform: " ${PLATFORM})

# find the root of the main project
string(FIND ${PROJECT_SOURCE_DIR} "/" INDEX REVERSE)
string(SUBSTRING ${PROJECT_SOURCE_DIR} 0 ${INDEX} PR_ROOT)

include("${PR_ROOT}/common/platform_paths.cmake")

if(WIN32)
        
    set(ARMADILLO_INCLUDE_DIRS "D:/Projects/armadillo-14.6.2/include")
    set(BLAS_LIBRARIES "D:/Projects/vcpkg/installed/x64-windows/debug/lib/openblas.lib")
    set(BLAS_INCLUDE_DIR "D:/Projects/vcpkg/installed/x64-windows/include/openblas")
    
    find_package(OpenBLAS CONFIG REQUIRED)
    find_package(lapack CONFIG REQUIRED)
else()
    set(ARMADILLO_INCLUDE_DIRS "../../armadillo-14.6.2/include")
    find_package(BLAS REQUIRED)
    if (BLAS_FOUND)
        message(STATUS "Found BLAS (OpenBLAS): ${BLAS_LIBRARIES}")
        message(STATUS "Found BLAS (OpenBLAS): ${BLAS_INCLUDE_DIRS}")

    else()
        message(FATAL_ERROR "BLAS not found. Please install OpenBLAS or ensure it's in a searchable path.")
    endif()    
endif()

# look for any *.c and *.cpp files
file(GLOB SOURCES "src/*.c*"
    "${COMMON_ROOT}/src/*.cpp"
    )
    
#find_package(OpenBLAS CONFIG REQUIRED)

set(project_includes 
    "include" 
    "${PR_ROOT}/common/include"
    "${COMMON_ROOT}/include"
    )

message(" ")


get_directory_property(cache_vars CACHE_VARIABLES)
message(STATUS "List of CACHE variables: ${cache_vars}")

#if(ARMADILLO_FOUND)
    set(project_includes ${project_includes} ${ARMADILLO_INCLUDE_DIRS} ${BLAS_INCLUDE_DIRS})
    set(project_libs ${project_libs} ${BLAS_LIBRARIES} lapack ) #OpenBLAS::OpenBLAS)
#endif()

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

include_directories(${project_includes})
add_executable(${exe_name} ${SOURCES})
target_link_libraries(${exe_name} ${project_libs})
#target_link_libraries(main PRIVATE OpenBLAS::OpenBLAS)

if(WIN32)
    message("MSVC Version: " ${MSVC_VERSION})
    # uncomment for MSVC and cmake greater than 3.6.0
    set(default_build_type "Release")
    if(MSVC AND (${CMAKE_VERSION} VERSION_GREATER "3.6.0"))
        target_compile_options(${exe_name} PRIVATE "/MP")
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${exe_name})
    endif()
else()

endif()

# ----------------------------------------------------------------------------
message(" ")
message(STATUS "Build Complete!")
message(STATUS "cmake --build . --config Release -- -j4")
message(" ")
