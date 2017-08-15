cmake_minimum_required(VERSION 3.6)

project(KittehBot++)
set(CMAKE_CXX_STANDARD 14)
set(TEST "${TEST}")
# -ggdb -g3
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

SET(VERSION "2.0.0 ${GIT_BRANCH}:${GIT_COMMIT_HASH}")


set(SOURCE_FILES main.cc)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_subdirectory(lib/redisclient)
add_subdirectory(lib/libnamedkitten)
add_subdirectory(lib/hexicord)

set_target_properties(benchmark parsertest redisvaluetest async_set_get2 async_pubsub async_pubsub2 async_set_get sync_set_get PROPERTIES EXCLUDE_FROM_ALL ON EXCLUDE_FROM_DEFAULT_BUILD ON)

set(Boost_USE_STATIC_LIBS   ON)

find_package(Boost REQUIRED system timer date_time)

message(STATUS "${PROJECT_NAME} version: ${VERSION_FULL}")
configure_file(${CMAKE_SOURCE_DIR}/cmake/sysconf.h.cmake ${CMAKE_BINARY_DIR}/sysconf.h)


INCLUDE_DIRECTORIES(
        ${Boost_INCLUDE_DIRS}
        ${BOOST_BEAST_INCLUDES}

        lib/redisclient/src
        ${CMAKE_BINARY_DIR}
	      ${CMAKE_SOURCE_DIR}/include
        ${json_SOURCE_DIR}
        lib/libnamedkitten/include
        lib/hexicord/src
        lib/hexicord/lib/beast/include
        build/
)

TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        ${Boost_LIBRARIES}
        redisclient
        namedkitten
        hexicord
)
