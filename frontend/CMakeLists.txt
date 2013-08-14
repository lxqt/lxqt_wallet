
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

INCLUDE(FindPkgConfig)

SET( QT_USE_QTCORE TRUE )
SET( QT_USE_QTGUI TRUE )

find_package( Qt4 COMPONENTS REQUIRED )

set( UI_FILES 	changepassworddialog.ui
		password_dialog.ui )

set( MOC_FILES 	changepassworddialog.h 
		lxqt_internal_wallet.h 
		lxqt_kwallet.h 
		lxqt_wallet_interface.h
		open_wallet_thread.h
		password_dialog.h )

set( SRC 	changepassworddialog.cpp
		lxqt_kwallet.cpp
		password_dialog.cpp
		lxqt_internal_wallet.cpp
		lxqt_wallet_interface.cpp
		open_wallet_thread.cpp )


find_package( KDE4 )
include (KDE4Defaults)
include_directories(${KDE4_INCLUDES})
add_definitions( -I${KDE4_INCLUDE_DIR} )

add_definitions( -I${Qt4_INCLUDE_DIR} )
add_definitions( -I${QT_HEADERS_DIR} )

INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

INCLUDE( ${QT_USE_FILE} )

QT4_WRAP_UI( UI ${UI_FILES} )

QT4_WRAP_CPP( MOC ${MOC_FILES} )

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

#
# TODO creating a dynamic library fails to link,look it up why
#
add_library( lxqtwallet STATIC ${UI} ${MOC} ${SRC} )

if( NOKDESUPPORT ) 
	SET( KDE_SUPPORT "false" )
	SET( KWALLET_SUPPORT "false" )
	file( WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n" )
else()
	SET( KDE_SUPPORT "true" )
	
	if( NOKWALLETSUPPORT )
		SET( KWALLET_SUPPORT "false" )	
		file( WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n" )
	else()	
		find_library( LIBKWALLETBACKEND libkwalletbackend.so )
		if( NOT LIBKWALLETBACKEND )
			# some distributions do not have libkwalletbackend.so for reasons i currently do not know
			# look for libkwalletbackend.sp.4 when the above happen
			find_library( LIBKWALLETBACKEND libkwalletbackend.so.4 )
			file( WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n" )
			SET( KWALLET_SUPPORT "false" )	
		else()
			SET( KWALLET_SUPPORT "true" )
			file( WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 1\n" )
		endif()

		if( NOT LIBKWALLETBACKEND )
			SET( KWALLET_SUPPORT "false" )	
			file( WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n" )
			#message( FATAL_ERROR "could not find libkwalletbackend.so or libkwalletbackend.so.4,please make sure you have kwallet development libraries installed and try again" )
		else()
			SET( KWALLET_SUPPORT "true" )	
			file( WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 1\n" )
		endif()
	endif()
	
	find_library( LIBKDEUI libkdeui.so )
	if( NOT LIBKDEUI )
		# adding this one just in case the same case as above happen
		find_library( LIBKDEUI libkdeui.so.5 )
	endif()
	
	if( NOT LIBKDEUI )
		SET( KDE_SUPPORT "false" )
		message( STATUS "could not find libkdeui.so or libkdeui.so.5,please make sure you have kde development libraries installed and try again" )
	endif()

	find_library( LIBKDECORE libkdecore.so )
	if( NOT LIBKDECORE )
		# adding this one just in case the same case as above happen	
		find_library( LIBKDECORE libkdecore.so.5 )
	endif()

	if( NOT LIBKDECORE )
		SET( KDE_SUPPORT "false" )
		message( STATUS "could not find libkdecore.so or libkdecore.so.5,please make sure you have kde development libraries installed and try again" )
	endif()
endif()

if( KWALLET_SUPPORT )
	message( STATUS "\n--------------------------------------------------------------------------" )
	message( STATUS "kwallet support found,will build kwallet functionality" )
	message( STATUS "-----------------------------------------------------------------------" )
else()
	message( STATUS "\n--------------------------------------------------------------------------" )
	message( STATUS "kwallet support NOT found,will not build kwallet functionality" )
	message( STATUS "-----------------------------------------------------------------------" )
endif()

if( KDE_SUPPORT )
	#message( STATUS "\n--------------------------------------------------------------------------" )
	#message( STATUS "kde support found,will build a version that uses kde status notifier" )
	#message( STATUS "-----------------------------------------------------------------------" )
else()
	#message( STATUS "\n--------------------------------------------------------------------------" )
	#message( STATUS "kde support NOT found,will build a version with no kde support" )
	#message( STATUS "-----------------------------------------------------------------------" )
endif()

if( KDE_SUPPORT )
	if( KWALLET_SUPPORT )
		TARGET_LINK_LIBRARIES( lxqtwallet -L${KDE4_LIB_DIR} -L${QT_LIBRARY_DIR} ${LIBKDEUI} ${LIBKDECORE} ${LIBKWALLETBACKEND} -lQtCore -lQtGui lxqtwallet-backend ) 
	else()
		TARGET_LINK_LIBRARIES( lxqtwallet -L${KDE4_LIB_DIR} -L${QT_LIBRARY_DIR} ${LIBKDEUI} ${LIBKDECORE} -lQtCore -lQtGui lxqtwallet-backend ) 
	endif()	
else()
	TARGET_LINK_LIBRARIES( lxqtwallet lxqtwallet-backend )
endif()	



