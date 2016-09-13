# - Try to find Mic11api
# Once done this will define
#
#  API_FOUND - system has API
#  API_INCLUDE_DIR - the API include directory
#  API_LIBRARY_DIR - The libraries needed to use API
#

MESSAGE("\n Looking for API...")

FIND_PATH(API_LIBRARY_DIR NAMES       libmic11api.so       PATHS
                                                           ../mic11api/lib/
                                                           NO_DEFAULT_PATH)

FIND_PATH(API_INCLUDE_DIR NAMES       Fifo.h               PATHS
                                                           ../mic11api/src/
                                                           NO_DEFAULT_PATH)

if (API_LIBRARY_DIR AND API_INCLUDE_DIR)

  set(API_FOUND TRUE)

  message(STATUS "found in:  ${API_LIBRARY_DIR}")

else (API_LIBRARY_DIR AND API_INCLUDE_DIR)

  message(FATAL_ERROR "Could NOT find API!\n")

endif (API_LIBRARY_DIR AND API_INCLUDE_DIR)
