cmake_minimum_required(VERSION 3.16)

#file(TO_CMAKE_PATH "$ENV{HOME}/.local/libwebsockets/cmake" LWS_CMAKE_DIR)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" ${CMAKE_MODULE_PATH})

# set the project name
project(tclwebsockets VERSION 0.0.1)

find_package(TCL)
find_package(Tclsh)
find_package(TclStub)
find_package(OpenSSL)
find_package(LibWebSockets)

#
# Common defines for all project targets
#
include_directories(${TCL_INCLUDE_PATH}
                    ${LIBWEBSOCKETS_INCLUDE_DIR}
                    ${OPENSSL_INCLUDE_DIR}
)
set(TCLWEBSOCKETS_DEFINES -DUSE_TCL_STUBS=1
                          -DPACKAGE_NAME="websockets"
                          -DPACKAGE_VERSION="${CMAKE_PROJECT_VERSION}")

IF (MSVC)
	list(APPEND TCLGRAPHS_DEFINES -D_CRT_SECURE_NO_WARNINGS=1)
ENDIF()

#
# graphs Tcl extension
#
set(WEBSOCKETS_SOURCES generic/websockets.c generic/lws.c)
set(WEBSOCKETS_HEADERS generic/websockets.h)

add_library(websockets SHARED ${WEBSOCKETS_SOURCES} ${WEBSOCKETS_HEADERS}  pkgIndex.tcl.in)
target_compile_definitions(websockets PRIVATE ${TCLWEBSOCKETS_DEFINES} -DBUILD_websockets=1 -DMODULE_SCOPE=extern)
target_link_libraries(websockets ${TCL_STUB_LIBRARY} ${LIBWEBSOCKETS_STATIC_LIBRARIES} ${OPENSSL_LIBRARIES} ${LIBWEBSOCKETS_DEP_LIBS})
if (MSVC)
    target_link_options(websockets PRIVATE /NODEFAULTLIB:msvcrtd)
endif()
set_target_properties(websockets PROPERTIES PREFIX "")

configure_file(pkgIndex.tcl.in pkgIndex.tcl)

#
# install section for packaging
#

set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local/tcl/lib")
file(TO_CMAKE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)
message(STATUS "install prefix: ${CMAKE_INSTALL_PREFIX}")
install(
    TARGETS websockets
    RUNTIME DESTINATION "websockets${CMAKE_PROJECT_VERSION}"
    LIBRARY DESTINATION "websockets${CMAKE_PROJECT_VERSION}"
)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/pkgIndex.tcl"
    DESTINATION "graphs${CMAKE_PROJECT_VERSION}"
)

#
# packaging section
#
IF (WIN32)
    set(CPACK_GENERATOR "ZIP")
ELSE()
    set(CPACK_GENERATOR "TGZ")
ENDIF()

set(CPACK_PACKAGE_NAME "websockets")
set(CPACK_PACKAGE_VENDOR "EL")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
#set(CPACK_PACKAGE_VERSION_MAJOR 0)
#set(CPACK_PACKAGE_VERSION_MINOR 0)
#set(CPACK_PACKAGE_VERSION_PATCH 1)
include(CPack)

