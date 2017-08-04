cmake_minimum_required(VERSION 3.6)

project(KittehBot++)
set(CMAKE_CXX_STANDARD 14)
SET(VERSION "1.3.2")
set(TEST "${TEST}")
set(SOURCE_FILES main.cc)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(cotire)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_subdirectory(lib/discordpp)
add_subdirectory(lib/rest-curlpp)
add_subdirectory(lib/websocket-websocketpp)
#add_subdirectory(lib/docopt)
add_subdirectory(lib/redisclient)
add_subdirectory(lib/libnamedkitten)

set_target_properties(benchmark parsertest redisvaluetest async_set_get2 async_pubsub async_pubsub2 async_set_get sync_set_get PROPERTIES EXCLUDE_FROM_ALL ON EXCLUDE_FROM_DEFAULT_BUILD ON)

find_package(Boost REQUIRED system timer date_time)

message(STATUS "${PROJECT_NAME} version: ${VERSION_FULL}")
configure_file(${CMAKE_SOURCE_DIR}/cmake/sysconf.h.cmake ${CMAKE_BINARY_DIR}/sysconf.h)

cotire(KittehBot++)


# -ggdb -g3
set(CFLAGS "${CFLAGS}")
set(CXX_FLAGS "${CFLAGS}")

INCLUDE_DIRECTORIES(
        ${Boost_INCLUDE_DIRS}
        ${discordpp_SOURCE_DIR}
        ${discordpp-rest-curlpp_SOURCE_DIR}
        ${discordpp-websocket-websocketpp_SOURCE_DIR}
        lib/redisclient/src
        ${CMAKE_BINARY_DIR}
	      ${CMAKE_SOURCE_DIR}/include
	      ${OPENSSL_INCLUDE_DIR}
        ${json_SOURCE_DIR}
        #${DOCOPT_INCLUDE_DIRS}
        lib/libnamedkitten/include
)

TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        ${Boost_LIBRARIES}
        discordpp
        discordpp-rest-curlpp
        discordpp-websocket-websocketpp
        redisclient
        #docopt
        namedkitten
)
