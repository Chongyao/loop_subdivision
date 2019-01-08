project(Hierarchical-Z-Buffer)
cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_CXX_FLAGS "-fopenmp -std=c++11 -o3 -fpermissive")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/;${CMAKE_MODULE_PATH}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/external)

# BOOST
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS filesystem system program_options REQUIRED)
if(Boost_FOUND)
  message("-- Boost @ ${Boost_INCLUDE_DIRS}")
  include_directories(${Boost_INCLUDE_DIRS})
endif(Boost_FOUND)

# EIGEN3
find_package(Eigen3 REQUIRED)
if(EIGEN3_FOUND)
  message("-- Eigen @ ${EIGEN3_INCLUDE_DIR}")
  message("-- Eigen version: ${EIGEN3_VERSION}")
  include_directories(${EIGEN3_INCLUDE_DIR})
endif(EIGEN3_FOUND)

add_subdirectory(src)
add_subdirectory(examples)
