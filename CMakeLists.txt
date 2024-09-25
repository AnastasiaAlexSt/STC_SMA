cmake_minimum_required(VERSION 3.5)

project(MovingAverage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES main.cpp
            src/timer.cpp
            src/data_writer_xlsx.cpp
            src/algorithm_executor.cpp
            src/algorithm_collection.cpp)

set(HEADER_FILES include/timer.h
                 include/data_writer_xlsx.h
                 include/algorithm_executor.h
                 include/algorithm_collection.h)

add_executable(MovingAverage ${SOURCES} ${HEADER_FILES})

target_include_directories(MovingAverage PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(MovingAverage PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

set(LIBXLSXWRITER_DIR ${CMAKE_CURRENT_LIST_DIR}/libs/libxlsxwriter)
target_include_directories(MovingAverage PUBLIC ${LIBXLSXWRITER_DIR}/include)
target_include_directories(MovingAverage PUBLIC ${LIBXLSXWRITER_DIR}/include/xlsxwriter)

target_link_directories(MovingAverage INTERFACE ${LIBXLSXWRITER_DIR}/build)
target_link_libraries(MovingAverage PUBLIC ${LIBXLSXWRITER_DIR}/build/libxlsxwriter.a)

find_package(ZLIB REQUIRED)
target_link_libraries(MovingAverage PUBLIC ZLIB::ZLIB)


