cmake_minimum_required(VERSION 3.23)
project(weather_forecast)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenSSL REQUIRED)

set(OpenSSL_LIBS
        OpenSSL::Crypto OpenSSL::SSL
        )

include_directories(${OPENSSL_INCLUDE_DIRS})

# 查找当前目录下的所有源文件，并将名称保存到 DIR_SRCS 变量
aux_source_directory(./src DIR_SRCS)
aux_source_directory(./include/gumbo-parser/ DIR_INCLUDES)

add_executable(weather_forecast main.cpp ${DIR_INCLUDES} ${DIR_SRCS})
target_link_libraries(weather_forecast ${OpenSSL_LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output)
