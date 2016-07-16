
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

INCLUDE(FindPkgConfig)

include_directories(${PROJECT_BINARY_DIR}/frontend)
include_directories(${PROJECT_BINARY_DIR}/backend)
include_directories(${PROJECT_BINARY_DIR})

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS}})

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Core_DEFINITIONS})

if(NOSECRETSUPPORT)
    SET(SECRET_SUPPORT "false")
    file(WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 0\n")
else()
    pkg_check_modules(LIBSECRET libsecret-1)
    if(LIBSECRET_FOUND)
        add_subdirectory(libsecret)
        SET(SECRET_SUPPORT "true")
        file(WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 1\n")
    else()
        SET(SECRET_SUPPORT "false")
        file(WRITE  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 0\n")
    endif()
endif()

add_definitions(-std=c++11)

if(NOKDESUPPORT)
    SET(KDE_SUPPORT "false")
    SET(KWALLET_SUPPORT "false")
    file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
else()
    find_package(KF5Wallet QUIET)
    find_package(KF5Notifications QUIET)

    if(KF5Wallet_FOUND AND KF5Notifications_FOUND )
        file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 1\n")
        SET(KDE_SUPPORT "true")
        SET(KWALLET_SUPPORT "true")
    else()
        file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
        SET(KDE_SUPPORT "false")
        SET(KWALLET_SUPPORT "false")
    endif()
endif()

if(KWALLET_SUPPORT)
    message(STATUS "\n--------------------------------------------------------------------------")
    message(STATUS "kwallet support found,will build kwallet functionality")
    message(STATUS "-----------------------------------------------------------------------")
else()
    message(STATUS "\n--------------------------------------------------------------------------")
    message(STATUS "kwallet support NOT found,will not build kwallet functionality")
    message(STATUS "-----------------------------------------------------------------------")
endif()

if(SECRET_SUPPORT)
    message(STATUS "\n--------------------------------------------------------------------------")
    message(STATUS "libsecret support found,will build libsecret functionality")
    message(STATUS "-----------------------------------------------------------------------")
else()
    message(STATUS "\n--------------------------------------------------------------------------")
    message(STATUS "libsecret support NOT found,will not build libsecret functionality")
    message(STATUS "-----------------------------------------------------------------------")
endif()

set(UI_FILES 	changepassworddialog.ui password_dialog.ui)

set(MOC_FILES 	changepassworddialog.h password_dialog.h task.h)
set(MOC_FILES_1 changepassworddialog.h password_dialog.h lxqt_kwallet.h task.h)
set(MOC_FILES_2 changepassworddialog.h password_dialog.h lxqt_kwallet.h task.h)
set(MOC_FILES_3 changepassworddialog.h password_dialog.h )

set(SRC 	changepassworddialog.cpp password_dialog.cpp lxqt_internal_wallet.cpp lxqt_wallet.cpp)
set(SRC_1 	changepassworddialog.cpp password_dialog.cpp lxqt_internal_wallet.cpp lxqt_wallet.cpp lxqt_kwallet.cpp)
set(SRC_2 	changepassworddialog.cpp password_dialog.cpp lxqt_internal_wallet.cpp lxqt_wallet.cpp lxqt_kwallet.cpp lxqt_libsecret.cpp)
set(SRC_3 	changepassworddialog.cpp password_dialog.cpp lxqt_internal_wallet.cpp lxqt_wallet.cpp lxqt_libsecret.cpp)

Qt5_WRAP_UI(UI ${UI_FILES})

if(KDE_SUPPORT)
    if(KWALLET_SUPPORT)
        if(SECRET_SUPPORT)
            Qt5_WRAP_CPP(MOC ${MOC_FILES_2})
            add_library(lxqtwallet SHARED ${UI} ${SRC_2} ${MOC})
            target_link_libraries(lxqtwallet secretlib)
            target_link_libraries(lxqtwallet "${Qt5Widgets_LIBRARIES}")
            target_link_libraries(lxqtwallet "${Qt5Core_LIBRARIES}")
            target_link_libraries(lxqtwallet KF5::Wallet KF5::Notifications)
        else()
            Qt5_WRAP_CPP(MOC ${MOC_FILES_1})
            add_library(lxqtwallet SHARED ${UI} ${SRC_1} ${MOC})
            target_link_libraries(lxqtwallet "${Qt5Widgets_LIBRARIES}")
            target_link_libraries(lxqtwallet "${Qt5Core_LIBRARIES}")
            target_link_libraries(lxqtwallet KF5::Wallet KF5::Notifications)
        endif()
    else()
        if(SECRET_SUPPORT)
            Qt5_WRAP_CPP(MOC ${MOC_FILES_3})
            add_library(lxqtwallet SHARED ${UI} ${SRC_3} ${MOC})
            target_link_libraries(lxqtwallet secretlib)
            target_link_libraries(lxqtwallet "${Qt5Widgets_LIBRARIES}")
            target_link_libraries(lxqtwallet "${QtCore_LIBRARIES}")
        else()
            Qt5_WRAP_CPP(MOC ${MOC_FILES})
            add_library(lxqtwallet SHARED ${UI} ${SRC} ${MOC})
            target_link_libraries(lxqtwallet "${Qt5Widgets_LIBRARIES}")
            target_link_libraries(lxqtwallet "${QtCore_LIBRARIES}")
            target_link_libraries(lxqtwallet "${LIBKDEUI}")
            target_link_libraries(lxqtwallet "${LIBKDECORE}")
        endif()
    endif()
else()
    if(SECRET_SUPPORT)
        Qt5_WRAP_CPP(MOC ${MOC_FILES_3})
        add_library(lxqtwallet SHARED ${UI} ${SRC_3} ${MOC})
        target_link_libraries(lxqtwallet secretlib)
        target_link_libraries(lxqtwallet "${Qt5Widgets_LIBRARIES}")
        target_link_libraries(lxqtwallet "${QtCore_LIBRARIES}")
    else()
        Qt5_WRAP_CPP(MOC ${MOC_FILES})
        add_library(lxqtwallet SHARED ${UI} ${SRC} ${MOC})
        target_link_libraries(lxqtwallet "${Qt5Widgets_LIBRARIES}")
        target_link_libraries(lxqtwallet "${QtCore_LIBRARIES}")
    endif()
endif()

set_target_properties(lxqtwallet PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic ")
set_target_properties(lxqtwallet PROPERTIES SOVERSION "${LIBRARY_VERSION}")

TARGET_LINK_LIBRARIES(lxqtwallet "${link_libraries}" lxqtwallet-backend ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES} )

install(FILES ../translations.qm/german_DE.qm   DESTINATION share/lxqt_wallet/translations.qm/)
install(FILES ../translations.qm/greek_GR.qm    DESTINATION share/lxqt_wallet/translations.qm/)
install(FILES ../translations.qm/dutch_NL.qm    DESTINATION share/lxqt_wallet/translations.qm/)
install(FILES ../translations.qm/french_FR.qm   DESTINATION share/lxqt_wallet/translations.qm/)
install(FILES ../translations.qm/spanish_ES.qm   DESTINATION share/lxqt_wallet/translations.qm/)

install(TARGETS lxqtwallet RUNTIME LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(FILES lxqt_wallet.h DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lxqt")

file( WRITE ${PROJECT_BINARY_DIR}/lxqtwallet.pc
"prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
includedir=${CMAKE_INSTALL_PREFIX}/include/lxqt

Name: lxqt_wallet
Description: lxqt secure storage system
Version: ${LIBRARY_VERSION}
Requires: Qt5Core Qt5Widget
Libs: -L${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR} -llxqtwallet
Cflags: -I${CMAKE_INSTALL_PREFIX}/include/lxqt
\n")

install(FILES ${PROJECT_BINARY_DIR}/lxqtwallet.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig/ PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
