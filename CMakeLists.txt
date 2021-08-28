cmake_minimum_required(VERSION 3.16)

#file(TO_CMAKE_PATH "$ENV{HOME}/.local/libwebsockets/cmake" LWS_CMAKE_DIR)
#set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" ${CMAKE_MODULE_PATH})

# set the project name
project(tclwebsocket VERSION 0.1.0)

find_package(TCL)
find_package(Tclsh)
find_package(TclStub)

if (APPLE)
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif()
find_package(OpenSSL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE true)

if (WIN32)
    # sometimes, expecially on the github windows runners, there
    # is a different tclsh on the path than we want. This is a
    # problem with the tests
    get_filename_component(TCL_HOME "${TCL_INCLUDE_PATH}" PATH)
    set(TCL_TCLSH "${TCL_HOME}/bin/tclsh.exe")
elseif(APPLE)
    # Default tcl is too old on github runners. Use the brew installation
    set(TCL_TCLSH "/usr/local/opt/tcl-tk/bin/tclsh")
endif()

#
# Build libwebsockets first from git submodule.
#
# Linking against libwebsockets dynamic lib has some problems:
# 
# * on windows, there is not yet an easy way for users to install a libwebsockets dll in
#   any version. Neither there is a chocolatey package nor a clickable MSI. So, windows
#   users would have to compile it from source, which might be painful or impossible.
# * on linux and macos, there are libwebsockets packages in package managers. But the
#   version and the configuration does highly depend on the distribution and on how the
#   package was configured at built time. If the package manager delivers an older version
#   of libwebsockets, there might be features missing that we need or, worse, work differently.
#   Likewise if the package was configured in a way that does not work for us - this is a
#   particular problem with the lws event loop plugins.
#
# To get around these problems, we build and link libwebsockets statically from a git
# submodule with fixed commit hash. The commit is currently set at version v4.2.1
#
option(LWS_OPENSSL_LIBRARIES "LWS openssl libraries" OFF)
option(LWS_OPENSSL_INCLUDE_DIRS "LWS openssl include dirs" OFF)
option(LWS_WITH_STATIC OFF)
option(LWS_WITH_SHARED OFF)
set(LWS_OPENSSL_LIBRARIES "${OPENSSL_LIBRARIES}")
set(LWS_OPENSSL_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR})
set(LWS_WITH_STATIC true)
set(LWS_WITH_SHARED false)
set(LIBWEBSOCKETS_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/libwebsockets/include")
include_directories(${LIBWEBSOCKETS_INCLUDE_DIR})
add_subdirectory(libwebsockets EXCLUDE_FROM_ALL)

set(LIBWEBSOCKETS_LIBRARIES websockets)

#
# Common defines for all project targets
#
include_directories(${TCL_INCLUDE_PATH}
                    ${OPENSSL_INCLUDE_DIR}
)
set(TCLWEBSOCKET_DEFINES -DUSE_TCL_STUBS=1
                          -DPACKAGE_NAME="${CMAKE_PROJECT_NAME}"
                          -DPACKAGE_VERSION="${CMAKE_PROJECT_VERSION}")

#
# websocket Tcl extension
#
set(TCLWEBSOCKET_SOURCES generic/globals.cpp
                       generic/tclWebsocket.cpp
                       generic/wsChannel.cpp
                       generic/lws.cpp
                       generic/websocketCmd.cpp
)
set(TCLWEBSOCKET_HEADERS generic/tclWebsocket.hpp
                       generic/wsChannel.hpp
                       generic/lws.hpp
)

add_library(${CMAKE_PROJECT_NAME} SHARED ${TCLWEBSOCKET_SOURCES} ${TCLWEBSOCKET_HEADERS}  pkgIndex.tcl.in)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE ${TCLWEBSOCKET_DEFINES} -DBUILD_tclwebsocket=1 -DMODULE_SCOPE=extern)
target_link_libraries(${CMAKE_PROJECT_NAME} ${TCL_STUB_LIBRARY} ${LIBWEBSOCKETS_LIBRARIES} ${LIBWEBSOCKETS_DEP_LIBS})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES PREFIX "")
configure_file(pkgIndex.tcl.in pkgIndex.tcl)

#
# The tests
#
enable_testing()
add_test(
    NAME "client-connect"
    COMMAND ${TCL_TCLSH} tests/run.tcl -load $<TARGET_FILE:tclwebsocket> -dir ${CMAKE_CURRENT_BINARY_DIR} -files client-connect
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME "client-io"
    COMMAND ${TCL_TCLSH} tests/run.tcl -load $<TARGET_FILE:tclwebsocket> -dir ${CMAKE_CURRENT_BINARY_DIR} -files client-io
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME "options"
    COMMAND ${TCL_TCLSH} tests/run.tcl -load $<TARGET_FILE:tclwebsocket> -dir ${CMAKE_CURRENT_BINARY_DIR} -files options
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

#
# install section for packaging
#

set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local/tcl/lib")
file(TO_CMAKE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)
message(STATUS "install prefix: ${CMAKE_INSTALL_PREFIX}")
install(
    TARGETS ${CMAKE_PROJECT_NAME}
    RUNTIME DESTINATION "${CMAKE_PROJECT_NAME}${CMAKE_PROJECT_VERSION}"
    LIBRARY DESTINATION "${CMAKE_PROJECT_NAME}${CMAKE_PROJECT_VERSION}"
)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/pkgIndex.tcl"
    DESTINATION "${CMAKE_PROJECT_NAME}${CMAKE_PROJECT_VERSION}"
)
install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    DESTINATION "${CMAKE_PROJECT_NAME}${CMAKE_PROJECT_VERSION}"
)
install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/libwebsockets/LICENSE"
    DESTINATION "${CMAKE_PROJECT_NAME}${CMAKE_PROJECT_VERSION}"
    RENAME LICENSE.libwebsockets
)

#
# packaging section
#
IF (WIN32)
    set(CPACK_GENERATOR "ZIP")
ELSE()
    set(CPACK_GENERATOR "TGZ")
ENDIF()

set(CPACK_OUTPUT_FILE_PREFIX packages)

set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "EL")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)

