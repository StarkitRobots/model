cmake_minimum_required(VERSION 2.8)
project(ExperimentCartWalkGradientOptim)

#Rhoban dependencies repositories paths for headers inclusion
set(RHOBAN_MATH_DIRECTORY
    "/home/rhoban/Math/" 
    CACHE STRING "RhobanMath directory")
set(RHOBAN_COMMON_DIRECTORY
    "/home/rhoban/Common/" 
    CACHE STRING "RhobanCommon directory")
set(RHOBAN_UTILS_DIRECTORY
    "/home/rhoban/Utils/" 
    CACHE STRING "RhobanUtils directory")

#Rhoban C++ SDK directory path
set(RHOBAN_SDK_DIRECTORY 
    "/home/rhoban/Edu/sdk/cpp" 
    CACHE STRING "RhobanSdk cpp directory")

#Path to a SDK build directory (with SDK, Utils, Common and library binaries)
set(SDK_BUILD_DIRECTORY 
    "/home/rhoban/Edu/build" 
    CACHE STRING "RhobanSdkBuild directory")

#Thesis Code directory
set(LEPH_CODE_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    CACHE STRING "LephCode directory")

#Eigen linear algebra library directory
set(EIGEN_DIRECTORY
    ${RHOBAN_UTILS_DIRECTORY}/eigen-3.2
    CACHE STRING "Eigen library directory")

#ZMQ communication library directory
set(ZMQ_DIRECTORY
    ${RHOBAN_UTILS_DIRECTORY}/zmq3.2
    CACHE STRING "ZMQ library directory")

#Enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   
# Header includes
include_directories(
    ${RHOBAN_MATH_DIRECTORY}
    ${RHOBAN_COMMON_DIRECTORY}
    ${RHOBAN_UTILS_DIRECTORY}
    ${RHOBAN_SDK_DIRECTORY}
    ${RHOBAN_SDK_DIRECTORY}/rhoban
    ${SDK_BUILD_DIRECTORY}/common/
    ${LEPH_CODE_DIRECTORY}
    ${LEPH_CODE_DIRECTORY}/Experiments/Utils
    ${EIGEN_DIRECTORY}
    ${ZMQ_DIRECTORY}/include
)

link_directories(
    ${SDK_BUILD_DIRECTORY}
    ${SDK_BUILD_DIRECTORY}/common
    ${SDK_BUILD_DIRECTORY}/utils
    ${SDK_BUILD_DIRECTORY}/utils/yaml-cpp
    ${SDK_BUILD_DIRECTORY}/utils/xml/TinyXml
    ${SDK_BUILD_DIRECTORY}/utils/zmq3.2
)

#Build Leph Code
add_subdirectory(${LEPH_CODE_DIRECTORY} ${CMAKE_CURRENT_BINARY_DIR}/LephCode)

#Lists of linked library
set(LINKED_LIBRARIES
    LephCode
    RhobanSDKMain 
    RhobanCommon 
    RhobanUtils
    pthread
    yaml-cpp
    TinyXml
    ZeroMQ
    ncurses
)

#Build Experiments
add_executable(ExperimentCartWalkGradientOptim 
    CartWalkGradientOptim/main.cpp
    Utils/MotionCapture.cpp
)
target_link_libraries(ExperimentCartWalkGradientOptim ${LINKED_LIBRARIES})
add_executable(CartWalkTunner
    CartWalkTunner/main.cpp
)
target_link_libraries(CartWalkTunner ${LINKED_LIBRARIES})
