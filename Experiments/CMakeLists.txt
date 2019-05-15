cmake_minimum_required(VERSION 2.8)
project(ExperimentCartWalkGradientOptim)

#Starkit dependencies repositories paths for headers inclusion
set(STARKIT_MATH_DIRECTORY
    "/home/starkit/Math/" 
    CACHE STRING "StarkitMath directory")
set(STARKIT_COMMON_DIRECTORY
    "/home/starkit/Common/" 
    CACHE STRING "StarkitCommon directory")
set(STARKIT_UTILS_DIRECTORY
    "/home/starkit/Utils/" 
    CACHE STRING "StarkitUtils directory")

#Starkit C++ SDK directory path
set(STARKIT_SDK_DIRECTORY 
    "/home/starkit/Edu/sdk/cpp" 
    CACHE STRING "StarkitSdk cpp directory")

#Path to a SDK build directory (with SDK, Utils, Common and library binaries)
set(SDK_BUILD_DIRECTORY 
    "/home/starkit/Edu/build" 
    CACHE STRING "StarkitSdkBuild directory")

#Thesis Code directory
set(LEPH_CODE_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    CACHE STRING "LephCode directory")

#Eigen linear algebra library directory
set(EIGEN_DIRECTORY
    "/home/starkit/Eigen"
    CACHE STRING "Eigen library directory")

#RBDL library directory
set(RBDL_DIRECTORY
    "/home/starkit/RBDL"
    CACHE STRING "RBDL library directory")

#ZMQ communication library directory
set(ZMQ_DIRECTORY
    ${STARKIT_UTILS_DIRECTORY}/zmq3.2
    CACHE STRING "ZMQ library directory")

#Enable C++17
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
   
# Header includes
include_directories(
    ${STARKIT_MATH_DIRECTORY}
    ${STARKIT_COMMON_DIRECTORY}
    ${STARKIT_UTILS_DIRECTORY}
    ${STARKIT_SDK_DIRECTORY}
    ${STARKIT_SDK_DIRECTORY}/starkit
    ${SDK_BUILD_DIRECTORY}/common/
    ${LEPH_CODE_DIRECTORY}
    ${LEPH_CODE_DIRECTORY}/Experiments/Utils
    ${EIGEN_DIRECTORY}
    ${ZMQ_DIRECTORY}/include
    ${RBDL_DIRECTORY}/include
    ${RBDL_DIRECTORY}/build/include
    ${RBDL_DIRECTORY}/addons
)

link_directories(
    ${SDK_BUILD_DIRECTORY}
    ${SDK_BUILD_DIRECTORY}/common
    ${SDK_BUILD_DIRECTORY}/utils
    ${SDK_BUILD_DIRECTORY}/utils/yaml-cpp
    ${SDK_BUILD_DIRECTORY}/utils/xml/TinyXml
    ${SDK_BUILD_DIRECTORY}/utils/zmq3.2
    ${RBDL_DIRECTORY}/build
    ${RBDL_DIRECTORY}/build/addons/urdfreader
)

#Build Leph Code
add_subdirectory(${LEPH_CODE_DIRECTORY} ${CMAKE_CURRENT_BINARY_DIR}/LephCode)

#
# TinyXml in StarkitUtils.a have to be
# compiled with -DTIXML_USE_STL to be compatible with
# libtinyxml in the system used by RBDL urdf parse (urdfdom)
#

#Lists of linked library
set(LINKED_LIBRARIES
    LephCode
    StarkitSDKMain 
    StarkitCommon 
    StarkitUtils
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
    Utils/SDKConnection.cpp
)
target_link_libraries(ExperimentCartWalkGradientOptim ${LINKED_LIBRARIES})
add_executable(CartWalkTunner
    CartWalkTunner/main.cpp
    Utils/SDKConnection.cpp
)
target_link_libraries(CartWalkTunner ${LINKED_LIBRARIES})
add_executable(StaticWalkTest
    StaticWalkTest/main.cpp
    Utils/SDKConnection.cpp
)
target_link_libraries(StaticWalkTest ${LINKED_LIBRARIES})
add_executable(SplinesPlayer
    SplinesPlayer/main.cpp
    Utils/SDKConnection.cpp
)
target_link_libraries(SplinesPlayer ${LINKED_LIBRARIES})

