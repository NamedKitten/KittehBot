cmake_minimum_required(VERSION 3.6)

project(KittehBot++)

set(CMAKE_CXX_STANDARD 17)
SET(VERSION "1.2.0")
set(TEST "${TEST}")
set(CMAKE_EXE_LINKER_FLAGS    "-Wl,--as-needed ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed ${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_CXX17_EXTENSION_COMPILE_OPTION "-std=c++1z")
set(SOURCE_FILES main.cc)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_subdirectory(lib/discordpp)
add_subdirectory(lib/rest-curlpp)
add_subdirectory(lib/websocket-websocketpp)
add_subdirectory(lib/json)

add_subdirectory(lib/redisclient)
#set(Boost_STATIC ON)
#set(Boost_USE_STATIC_LIBS   ON)


find_package(Boost REQUIRED system filesystem timer)
include_directories( include ${json_SOURCE_DIR} )


message(STATUS "${PROJECT_NAME} version: ${VERSION_FULL}")
configure_file(${CMAKE_SOURCE_DIR}/cmake/sysconf.h.cmake ${CMAKE_BINARY_DIR}/sysconf.h)
include_directories(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIR} )


# -ggdb -g3
set(CFLAGS "${CFLAGS}")
set(CMAKE_C_FLAGS "${CFLAGS} ")

set(CMAKE_CXX_FLAGS "${CFLAGS} ")



INCLUDE_DIRECTORIES(
        ${Boost_INCLUDE_DIRS}
        ${discordpp_SOURCE_DIR}
        ${discordpp-rest-curlpp_SOURCE_DIR}
        ${discordpp-websocket-websocketpp_SOURCE_DIR}
        ${nlohmann_json_include_dir}
        lib/redisclient/src

)

TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        ${Boost_LIBRARIES}
        discordpp
        discordpp-rest-curlpp
        discordpp-websocket-websocketpp
        redisclient
)

foreach(DATFILE time.py) #login.dat

    if(EXISTS ${DATFILE})
        configure_file(${DATFILE} ${DATFILE} COPYONLY)
    else()
        file(REMOVE ${CMAKE_BINARY_DIR}/${DATFILE})
    endif()
endforeach()
