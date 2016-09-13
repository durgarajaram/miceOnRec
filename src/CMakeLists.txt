message(STATUS "src ...")

if    (${MONITORING_FOUND})

  SET(M_UTILS_SOURCE_FILES ${M_UTILS_SOURCE_FILES}
                           "$ENV{MAUS_ROOT_DIR}/src/input/InputCppDAQOnlineData/InputCppDAQOnlineData.cc")

endif (${MONITORING_FOUND})

############### build the library #####################
add_library (miceOnRec   SHARED     MAUSDataStateMachine.cpp
                                    MAUSProcessors.cpp
                                    MAUSWorkers.cpp
                                    MAUSConveyers.cpp
                                    ReduceCppScalerPlot.cc
                                    ReduceCppOnrecMonitor.cc)

target_link_libraries(miceOnRec     mic11api MDUnpack
                                    ${MONITORING_LIBRARIES}
                                    ${MAUS_LIBRARIES}
                                    ${ROOT_LIBRARIES})

add_executable (bexample       BaseExample.cpp)
target_link_libraries(bexample miceOnRec json)

add_executable (stexample SingleThreadingExample.cpp)
target_link_libraries(stexample miceOnRec json)

add_executable (mtexample MultiThreadingExample.cpp)
target_link_libraries(mtexample miceOnRec json)

if (MONITORING_FOUND)

  ADD_EXECUTABLE(onrecost               OnlineReconST.cpp)
  TARGET_LINK_LIBRARIES(onrecost        miceOnRec json)

  ADD_EXECUTABLE(onrecomt               OnlineReconMT.cpp)
  TARGET_LINK_LIBRARIES(onrecomt        miceOnRec json)

endif  (MONITORING_FOUND)
