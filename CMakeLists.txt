CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( "cef-gl-example" )

OPTION( AUTO_DOWNLOAD_EXT "When needed external library is not found, Automatically download it." OFF )
OPTION( SUBMODULE_UPDATE "Try to update submodules" ON )

SET( ERIKA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )
SET( ERIKA_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext )

IF( SUBMODULES_UPDATE )
	MESSAGE( "Git Submodule update..." )
	EXECUTE_PROCESS( COMMAND git submodule update --init
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.. )
	MESSAGE( "Git Submodule update complete" )
ENDIF(  )

MACRO( APPEND_LINK_LIBS )
	LIST( APPEND ERIKA_LINK_LIBS ${ARGN} )
ENDMACRO( APPEND_LINK_LIBS )

#########################################################
# Add glew
ADD_SUBDIRECTORY ( ${ERIKA_EXT_DIR}/glew )
INCLUDE_DIRECTORIES( ${ERIKA_EXT_DIR}/glew/include )
APPEND_LINK_LIBS( libglew_shared )

# Add glfw
SET( GLFW_BUILD_EXAMPLES OFF )
SET( GLFW_BUILD_TESTS OFF  )
IF( APPLE )
	SET(_GLFW_COCOA ON)
	SET(_GLFW_NSGL ON)
ENDIF(  )
ADD_SUBDIRECTORY ( ${ERIKA_EXT_DIR}/glfw )
INCLUDE_DIRECTORIES( ${ERIKA_EXT_DIR}/glfw/include )
APPEND_LINK_LIBS( glfw )

#########################################################

FILE( GLOB ERIKA_SRC_FILES "${ERIKA_SRC_DIR}/*.*" )

#########################################################

ADD_EXECUTABLE( erika ${ERIKA_SRC_FILES} )

TARGET_LINK_LIBRARIES( erika ${ERIKA_LINK_LIBS} )