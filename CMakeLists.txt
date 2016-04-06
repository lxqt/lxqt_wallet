cmake_minimum_required( VERSION 2.6 )
cmake_policy(SET CMP0017 OLD)
set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON)

project(lxqt_wallet)

find_package(PkgConfig)

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY )

add_custom_target( uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )

if( NOT BUILD_SHARED )
	set( BUILD_SHARED "true" )
endif()

if( LIB_SUFFIX )
	if("${LIB_SUFFIX}" STREQUAL "default")

		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib64")
		else()
			set(LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib")
		endif()
	else()
		set(LIB_PATH "${CMAKE_INSTALL_PREFIX}/${LIB_SUFFIX}")
	endif()
else()
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib64")
	else()
		set(LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib")
	endif()
endif()

add_subdirectory(backend)
add_subdirectory(frontend)
