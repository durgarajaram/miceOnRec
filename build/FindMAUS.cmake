# - Try to find MAUS
# Once done this will define
#
#  MAUS_FOUND - system has MAUS
#  MONITOR_FOUND - system has MDMonitor
#  MAUS_INCLUDE_DIR - the MAUS include directory
#  MAUS_LIBRARIES - The libraries needed to use MAUS
#  MAUS_DEFINITIONS - Compiler switches required for using MAUS
#

MESSAGE("\n Looking for MAUS...")

if(NOT EXISTS $ENV{MAUS_ROOT_DIR})

  message(FATAL_ERROR "MAUS NOT FOUND!  $MAUS_ROOT_DIR environmental variable is not set properly.
Did you try running: source env.sh? ")

endif(NOT EXISTS $ENV{MAUS_ROOT_DIR})


FIND_PATH(MAUS_LIBRARY_DIR NAMES    libMausCpp.so          PATHS
                                                           $ENV{MAUS_ROOT_DIR}/build/
                                                           NO_DEFAULT_PATH)

if (MAUS_LIBRARY_DIR)

  set(MAUS_FOUND TRUE)
  set(MAUS_LIBRARY_DIR  ${MAUS_LIBRARY_DIR}
                        $ENV{MAUS_THIRD_PARTY}/third_party/install/lib
                        $ENV{MAUS_THIRD_PARTY}/third_party/install/lib64
                       )


  set(MAUS_INCLUDE_DIR  $ENV{MAUS_THIRD_PARTY}
                        $ENV{MAUS_THIRD_PARTY}/third_party/install/include
                        $ENV{MAUS_THIRD_PARTY}/third_party/install/include/python2.7
                        $ENV{MAUS_ROOT_DIR}/src/common_cpp
                        $ENV{MAUS_ROOT_DIR}/src/legacy
                        $ENV{MAUS_ROOT_DIR}/src/map
                       )

  set(MAUS_LIBRARIES    MausCpp
                        InputCppDAQOfflineData
                        MapCppEMRPlaneHits
                        MapCppEMRSpacePoints
                        MapCppEMRRecon
                        MapCppTOFDigits
                        MapCppTOFSlabHits
                        MapCppTOFSpacePoints
                        MapCppTrackerDigits
                        MapCppTrackerRecon
                        MapCppKLDigits
                        MapCppKLCellHits
                        MapCppCkovDigits
                        ReduceCppTofCalib
                        ReduceCppTOFPlot
                        ReduceCppEMRPlot
                        ReduceCppKLPlot
                        ReduceCppCkovPlot
                        ReduceCppSciFiPlot
                        )

  message(STATUS "found in  $ENV{MAUS_ROOT_DIR}")

  if (DEFINED ENV{MAUS_WEB_MEDIA})
    message(STATUS "Web Media: $ENV{MAUS_WEB_MEDIA}")
    if(NOT EXISTS $ENV{MAUS_WEB_MEDIA}/reco)
     execute_process(COMMAND mkdir $ENV{MAUS_WEB_MEDIA}/reco)
    endif()

  endif ()

else (MAUS_LIBRARY_DIR)

  message(FATAL_ERROR "Could NOT find MAUS!\n")

endif (MAUS_LIBRARY_DIR)


