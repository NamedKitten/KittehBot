cmake_minimum_required(VERSION 3.6)

project(KittehBot++)
set(CMAKE_CXX_STANDARD 14)
set(TEST "${TEST}")
# -ggdb -g3 -Os
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -ggdb -g3")
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
option(USE_CLANG "build bot with clang" OFF)
if(USE_CLANG)
SET (CMAKE_C_COMPILER             "/usr/bin/clang-6.0")
SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++-6.0")
SET (CMAKE_AR      "/usr/bin/llvm-ar-6.0")
SET (CMAKE_LINKER  "/usr/bin/llvm-link-6.0")
SET (CMAKE_NM      "/usr/bin/llvm-nm-6.0")
SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump-6.0")
SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib-6.0")
endif(USE_CLANG)
#find_library(CRYPTO_LIB libcrypto.a REQUIRED)
#find_library(PTHREAD_LIB libpthread.a REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")
set(BUILD_SHARED_LIBS OFF)

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

SET(CMAKE_FIND_LIBRARY_SUFFIXES .so)
set(SOURCE_FILES main.cc)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})


set(Boost_USE_STATIC_LIBS   ON )
set(Boost_USE_STATIC_RUNTIME   ON )

add_subdirectory(lib/redisclient)
add_subdirectory(lib/libnamedkitten)
add_subdirectory(lib/hexicord)

set_target_properties(benchmark parsertest redisvaluetest async_set_get2 async_pubsub async_pubsub2 async_set_get sync_set_get PROPERTIES EXCLUDE_FROM_ALL ON EXCLUDE_FROM_DEFAULT_BUILD ON)


find_package(Boost REQUIRED date_time thread)

message(STATUS "${PROJECT_NAME} version: ${VERSION}")
configure_file(${CMAKE_SOURCE_DIR}/cmake/sysconf.h.cmake ${CMAKE_BINARY_DIR}/sysconf.h)

add_custom_command(
    TARGET KittehBot++
    POST_BUILD
        COMMENT "Stripping..."
        #COMMAND wc -c KittehBot++
        #COMMAND strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag -s -g --strip-all -x -X KittehBot++
        #COMMAND wc -c KittehBot++
        #COMMAND sstrip -z KittehBot++
        #COMMAND wc -c KittehBot++

)



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
        -Og

)


find_program(APPIMAGE appimagetool)


if (APPIMAGE)
add_custom_target(appimage
COMMENT "Making appimage..."
COMMAND ${CMAKE_COMMAND} -E make_directory KittehBot++.AppDir/
COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/appimage/KittehBot++.desktop KittehBot++.AppDir
COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/appimage/icon.svg KittehBot++.AppDir
COMMAND ${CMAKE_COMMAND} -E copy KittehBot++ KittehBot++.AppDir/AppRun
COMMAND appimagetool --comp xz -n KittehBot++.AppDir
WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
endif (APPIMAGE)
