MESSAGE("\n Looking for UNPACKING...")

FIND_PATH(UNPACKING_LIB_DIR NAMES "libMDUnpack.so"  PATHS
                                                    $ENV{MAUS_THIRD_PARTY}/third_party/install/lib/
                                                    NO_DEFAULT_PATH)

FIND_PATH(UNPACKING_INCLUDE_DIR NAMES  "event.h"    PATHS
                                                    $ENV{MAUS_THIRD_PARTY}/third_party/install/include/unpacking/
                                                    NO_DEFAULT_PATH)

if (UNPACKING_LIB_DIR AND UNPACKING_INCLUDE_DIR)

  set(UNPACKING_FOUND         TRUE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dlong32=int -DdatePointer=long")

  MESSAGE(STATUS "Unpacking found : ${UNPACKING_LIB_DIR}\n")

else (UNPACKING_LIB_DIR AND UNPACKING_INCLUDE_DIR)

  message(FATAL_ERROR "Could NOT find Unpacking! \n")

endif (UNPACKING_LIB_DIR AND UNPACKING_INCLUDE_DIR)
