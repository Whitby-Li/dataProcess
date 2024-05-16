cmake_minimum_required(VERSION 3.16)
project(data_process)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(catkin REQUIRED COMPONENTS
        rosbag
        roscpp
        sensor_msgs
        cv_bridge
        geometry_msgs)

if (OpenCV_FOUND)
    message("OpenCV version: ${OpenCV_VERSION}")
endif ()

include_directories(
        ${catkin_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
link_libraries(${OpenCV_LIBRARIES} ${catkin_LIBRARIES})

# NTU_VIRAL
add_executable(parse_ntu_bag ntu_viral/parseNTUBag.cpp)

# TUM
add_executable(parse_tum_bag tum/parseTUMBag.cpp)

# EUROC
add_executable(parse_euroc_bag euroc/parseEurocBag.cpp)
add_executable(process_euroc_truth euroc/processEurocTruth.cpp)