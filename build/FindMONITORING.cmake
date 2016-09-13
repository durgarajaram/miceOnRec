MESSAGE(" Looking for MONITORINGING...")

FIND_PATH(MONITORING_LIB_DIR NAMES "libMDMonitor.a" PATHS
                                                    ${MIC11_DIR}/../monitor/
                                                    $ENV{MAUS_THIRD_PARTY}/third_party/install/lib/
                                                    NO_DEFAULT_PATH)

FIND_PATH(MONITORING_INCLUDE_DIR NAMES  "monitor.h" PATHS
                                                    ${MIC11_DIR}/../monitor/
                                                    $ENV{MAUS_THIRD_PARTY}/third_party/install/include/daq/
                                                    NO_DEFAULT_PATH)

if (MONITORING_LIB_DIR AND MONITORING_INCLUDE_DIR)

  set(MONITORING_FOUND         TRUE)

  # Add MONITORING
  INCLUDE_DIRECTORIES(  ${MONITORING_INCLUDE_DIR})
  LINK_DIRECTORIES(     ${MONITORING_LIB_DIR})
  SET(MONITORING_LIBRARIES  "MDMonitor")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_STEPIV_DATA=1 -lrt")
  MESSAGE(STATUS "Monitoring found : ${MONITORING_LIB_DIR}\n")

  set (MAUS_LIBRARIES ${MAUS_LIBRARIES}
                      InputCppDAQOnlineData)

else (MONITORING_LIB_DIR AND MONITORING_INCLUDE_DIR)

  message(STATUS "Could NOT find Monitoring! \n")

endif (MONITORING_LIB_DIR AND MONITORING_INCLUDE_DIR)

