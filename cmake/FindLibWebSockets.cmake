# This module tries to find libWebsockets library and include files
#
# LIBWEBSOCKETS_INCLUDE_DIR, path where to find libwebsockets.h
# LIBWEBSOCKETS_LIBRARY_DIR, path where to find libwebsockets.so
# LIBWEBSOCKETS_LIBRARIES, the library to link against
# LIBWEBSOCKETS_FOUND, If false, do not try to use libWebSockets
#
# This currently works probably only for Linux

file(TO_CMAKE_PATH "$ENV{HOME}/.local" LOCAL_PATH)

include(FindPackageHandleStandardArgs)
SET ( LIBWEBSOCKETS_FOUND FALSE )

FIND_PATH ( LIBWEBSOCKETS_INCLUDE_DIR NAMES libwebsockets.h
    HINTS /usr/local/include
    	  /usr/include
	  ${LOCAL_PATH}/include
	  ${LOCAL_PATH}/libwebsockets/include
)

FIND_LIBRARY ( LIBWEBSOCKETS_LIBRARIES NAMES websockets
    HINTS /usr/local/lib
    	  /usr/lib
          ${LOCAL_PATH}/lib
          ${LOCAL_PATH}/libwebsockets/lib
)

FIND_LIBRARY ( LIBWEBSOCKETS_STATIC_LIBRARIES NAMES websockets_static
    HINTS /usr/local/lib
    	  /usr/lib
          ${LOCAL_PATH}/lib
          ${LOCAL_PATH}/libwebsockets/lib
)


GET_FILENAME_COMPONENT( LIBWEBSOCKETS_LIBRARY_DIR ${LIBWEBSOCKETS_LIBRARIES} PATH )

IF ( LIBWEBSOCKETS_INCLUDE_DIR )
    IF ( LIBWEBSOCKETS_LIBRARIES )
        SET ( LIBWEBSOCKETS_FOUND TRUE )
    ENDIF ( LIBWEBSOCKETS_LIBRARIES )
ENDIF ( LIBWEBSOCKETS_INCLUDE_DIR )

if(WIN32)
    # These are additional libs that lws wants your app to also link to
    foreach(item "ws2_32.lib;userenv.lib;psapi.lib;iphlpapi.lib;crypt32.lib")
        list(APPEND LIBWEBSOCKETS_DEP_LIBS ${item})
    endforeach()
endif()

IF ( LIBWEBSOCKETS_FOUND )
    MARK_AS_ADVANCED(
        LIBWEBSOCKETS_LIBRARY_DIR
        LIBWEBSOCKETS_INCLUDE_DIR
        LIBWEBSOCKETS_LIBRARIES
	LIBWEBSOCKETS_STATIC_LIBRARIES
	LIBWEBSOCKETS_DEP_LIBS
    )
ENDIF ( )

find_package_handle_standard_args(LibWebSockets
	DEFAULT_MSG
	LIBWEBSOCKETS_INCLUDE_DIR
	LIBWEBSOCKETS_LIBRARIES)
