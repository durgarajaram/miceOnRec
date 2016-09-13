# Check if cmake has the required version
cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

# Set name of our project
project(mic11api)

set(ONREC_DIR    "${CMAKE_SOURCE_DIR}/")
# message(STATUS ${ONREC_DIR})

set(STOP_SIGNAL "${CMAKE_BINARY_DIR}/stop")
# message(STATUS ${STOP_SIGNAL})

# Load some basic macros which are needed later on
include(${CMAKE_BINARY_DIR}/FindROOT.cmake)
include(${CMAKE_BINARY_DIR}/FindMAUS.cmake)
include(${CMAKE_BINARY_DIR}/FindAPI.cmake)

include(${CMAKE_BINARY_DIR}/FindUNPACKING.cmake)
include(${CMAKE_BINARY_DIR}/FindMONITORING.cmake)

#include(${CMAKE_BINARY_DIR}/FindBOOST.cmake)
include(${CMAKE_BINARY_DIR}/FindCppUnit.cmake)

# Add API
include_directories(${API_INCLUDE_DIR})
link_directories   (${API_LIBRARY_DIR})

# Add ROOT
include_directories(${ROOT_INCLUDE_DIR})
link_directories   (${ROOT_LIBRARY_DIR})

# Add Maus
include_directories(${MAUS_INCLUDE_DIR})
link_directories   (${MAUS_LIBRARY_DIR})

configure_file(  ${CMAKE_BINARY_DIR}/maus_env.sh.cmake
                   ${CMAKE_SOURCE_DIR}/maus_env.sh)


configure_file(  ${CMAKE_BINARY_DIR}/config.h.cmake
                 ${CMAKE_SOURCE_DIR}/src/Config.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall -O3")

set(LIBRARY_OUTPUT_PATH    "${CMAKE_SOURCE_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

include_directories(  ${CMAKE_SOURCE_DIR}/.)
include_directories(  ${CMAKE_SOURCE_DIR}/src)
link_directories(     ${MIC11_DIR}/lib)

message(STATUS "C flags      : " ${CMAKE_C_FLAGS})
message(STATUS "CXX flags    : " ${CMAKE_CXX_FLAGS})
message(STATUS "Linker flags : " ${CMAKE_EXE_LINKER_FLAGS}\n)


add_subdirectory(${CMAKE_SOURCE_DIR}/src)

# if    (CPPUNIT_FOUND)
#   add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
# endif (CPPUNIT_FOUND)

