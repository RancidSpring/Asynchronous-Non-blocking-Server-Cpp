cmake_minimum_required(VERSION 2.8)

project(server)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
SET(PROTO_FILE ${CMAKE_SOURCE_DIR}/proto/measurements.proto)
SET(PROTO_GENERATED_PATH ${CMAKE_SOURCE_DIR}/proto)

INCLUDE(FindProtobuf)

# generate proto files
FIND_PACKAGE(Protobuf REQUIRED)
INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER ${PROTO_FILE})

ADD_LIBRARY(proto ${PROTO_HEADER} ${PROTO_SRC})
INCLUDE_DIRECTORIES(${PROTO_GENERATED_PATH})

find_package(PkgConfig REQUIRED)
find_package(ZLIB)
FIND_PACKAGE(Boost 1.42.0 REQUIRED COMPONENTS system thread regex iostreams)

IF(Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)


find_package(TBB REQUIRED)

SET(USED_LIBS proto ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_IOSTREAMS_LIBRARY} ${TBB_IMPORTED_TARGETS})


add_executable(${PROJECT_NAME} "main.cpp" Server.cpp Server.h Acceptor.cpp Acceptor.h Service.cpp Service.h config.h decompress.h)
target_link_libraries(${PROJECT_NAME} ${JSONCPP_LIBRARIES} ${USED_LIBS} ${Protobuf_LIBRARIES} ZLIB::ZLIB)

# move proto files from build to the separate directory
get_filename_component(PROTO_HEADER_NAME ${PROTO_HEADER} NAME)
get_filename_component(PROTO_SRC_NAME ${PROTO_SRC} NAME)
add_custom_target(move-protos ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROTO_GENERATED_PATH}
        COMMAND ${CMAKE_COMMAND} -E rename ${PROTO_HEADER} ${PROTO_GENERATED_PATH}/${PROTO_HEADER_NAME}
        COMMAND ${CMAKE_COMMAND} -E rename ${PROTO_SRC} ${PROTO_GENERATED_PATH}/${PROTO_SRC_NAME}
        )


add_dependencies(move-protos proto)
add_dependencies(server move-protos)
