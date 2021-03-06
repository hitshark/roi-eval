CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)

# compiler related
PROJECT(roi-eval)
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -g -ggdb -Wall -std=c++11")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -Wall -std=c++11")

# path related
SET(LIB_PATH ${CMAKE_SOURCE_DIR}/lib)
SET(DEMO_PATH ${CMAKE_SOURCE_DIR}/build)

SET(ROI_SRC_PATH ${CMAKE_SOURCE_DIR}/src)
SET(ROI_INC_PATH ${CMAKE_SOURCE_DIR}/include)
SET(TEST_PATH ${CMAKE_SOURCE_DIR}/test)

# include dir and lib dir
LINK_DIRECTORIES(${LIB_PATH})

# debug mesages
MESSAGE(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
MESSAGE(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
MESSAGE(STATUS "PROJECT_NAME: ${PROJECT_NAME}")
MESSAGE(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
MESSAGE(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
MESSAGE(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
MESSAGE(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
MESSAGE(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

# build all targets
ADD_SUBDIRECTORY(${ROI_SRC_PATH})
ADD_SUBDIRECTORY(${TEST_PATH})

