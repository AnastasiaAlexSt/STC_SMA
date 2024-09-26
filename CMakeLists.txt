cmake_minimum_required(VERSION 3.5)

project(MovingAverage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/out)
set(SOURCES main.cpp
            src/timer.cpp
            src/data_writer_xlsx.cpp
            src/algorithm_executor.cpp)

set(HEADER_FILES include/timer.h
                 include/data_writer_xlsx.h
                 include/algorithm_executor.h
                 include/algorithm_collection.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

set(LIBXLSXWRITER_DIR ${CMAKE_CURRENT_LIST_DIR}/build_libs/libxlsxwriter)
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBXLSXWRITER_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBXLSXWRITER_DIR}/include/xlsxwriter)

target_link_directories(${PROJECT_NAME} INTERFACE ${LIBXLSXWRITER_DIR}/lib)
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBXLSXWRITER_DIR}/lib/libxlsxwriter.a)

set(LIBZLIB_DIR ${CMAKE_CURRENT_LIST_DIR}/build_libs/zlib)
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBZLIB_DIR}/include)

target_link_directories(${PROJECT_NAME} INTERFACE ${LIBZLIB_DIR}/lib)
if(WIN32)
    set(ZLIB_NAME libzlibstatic.a)
else()
    set(ZLIB_NAME libz.a)
endif (WIN32)
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBZLIB_DIR}/lib/${ZLIB_NAME})

