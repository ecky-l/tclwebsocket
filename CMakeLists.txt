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
                          -DPACKAGE_NAME="${CMAKE_PROJECT_NAME}"
                          -DPACKAGE_VERSION="${CMAKE_PROJECT_VERSION}")

#IF (MSVC)
#	list(APPEND TCLGRAPHS_DEFINES -D_CRT_SECURE_NO_WARNINGS=1)
#ENDIF()

#
# graphs Tcl extension
#
set(WEBSOCKETS_SOURCES generic/tclwebsockets.c generic/lws.c)
set(WEBSOCKETS_HEADERS generic/tclwebsockets.h)

add_library(${CMAKE_PROJECT_NAME} SHARED ${WEBSOCKETS_SOURCES} ${WEBSOCKETS_HEADERS}  pkgIndex.tcl.in)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE ${TCLWEBSOCKETS_DEFINES} -DBUILD_tclwebsockets=1 -DMODULE_SCOPE=extern)
target_link_libraries(${CMAKE_PROJECT_NAME} ${TCL_STUB_LIBRARY} ${LIBWEBSOCKETS_LIBRARIES} ${LIBWEBSOCKETS_DEP_LIBS})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES PREFIX "")
configure_file(pkgIndex.tcl.in pkgIndex.tcl)

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
#set(CPACK_PACKAGE_VERSION_MAJOR 0)
#set(CPACK_PACKAGE_VERSION_MINOR 0)
#set(CPACK_PACKAGE_VERSION_PATCH 1)
include(CPack)

