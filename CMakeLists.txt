cmake_minimum_required(VERSION 3.16)

#file(TO_CMAKE_PATH "$ENV{HOME}/.local/libwebsockets/cmake" LWS_CMAKE_DIR)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" ${CMAKE_MODULE_PATH})

# set the project name
project(tclwebsocket VERSION 0.0.1)

find_package(TCL)
find_package(Tclsh)
find_package(TclStub)
find_package(OpenSSL)
find_package(LibWebSockets)

set(CMAKE_CXX_STANDARD 20)

#
# Common defines for all project targets
#
include_directories(${TCL_INCLUDE_PATH}
                    ${LIBWEBSOCKETS_INCLUDE_DIR}
                    ${OPENSSL_INCLUDE_DIR}
)
set(TCLWEBSOCKET_DEFINES -DUSE_TCL_STUBS=1
                          -DPACKAGE_NAME="${CMAKE_PROJECT_NAME}"
                          -DPACKAGE_VERSION="${CMAKE_PROJECT_VERSION}")

#
# graphs Tcl extension
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

#
# packaging section
#
IF (WIN32)
    set(CPACK_GENERATOR "ZIP")
ELSE()
    set(CPACK_GENERATOR "TGZ")
ENDIF()

set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "EL")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)

