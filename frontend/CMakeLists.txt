
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)

INCLUDE(FindPkgConfig)

include_directories(${PROJECT_BINARY_DIR}/frontend)
include_directories(${PROJECT_BINARY_DIR}/backend)
include_directories(${PROJECT_BINARY_DIR})

include_directories(${CMAKE_BINARY_DIR})

message(STATUS "Checking for modules 'Qt5Widgets' and 'Qt5Core'")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

message(STATUS "  Found Qt5Widgets, version ${Qt5Widgets_VERSION}")
message(STATUS "  Found Qt5Core, version ${Qt5Core_VERSION}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS}})

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Core_DEFINITIONS})

if(NOSECRETSUPPORT)
    SET(SECRET_SUPPORT "false")
    file(WRITE ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 0\n")
else()
    pkg_check_modules(LIBSECRET libsecret-1)
    if(LIBSECRET_FOUND)
        add_subdirectory(libsecret)
        SET(SECRET_SUPPORT "true")
        file(WRITE ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 1\n")
    else()
        SET(SECRET_SUPPORT "false")
        file(WRITE ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 0\n")
    endif()
endif()

add_definitions(-std=c++11)

if(NOKDESUPPORT)
    SET(KWALLET_SUPPORT "false")
    file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
else()
    message(STATUS "Checking for modules 'KF5Wallet' and 'KF5Notifications'")
    find_package(KF5Wallet)
    find_package(KF5Notifications)
    if(KF5Wallet_FOUND AND KF5Notifications_FOUND )
        message(STATUS "  Found KF5Wallet, version ${KF5Wallet_VERSION}")
        message(STATUS "  Found KF5Notifications, version ${KF5Notifications_VERSION}")
        file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 1\n")
        SET(KWALLET_SUPPORT "true")
    else()
        file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
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

set(COMMON_UI_FILES  changepassworddialog.ui password_dialog.ui)
set(COMMON_MOC_FILES task.h changepassworddialog.h password_dialog.h)
set(COMMON_SRC_FILES changepassworddialog.cpp password_dialog.cpp lxqt_internal_wallet.cpp lxqt_wallet.cpp)

Qt5_WRAP_UI(UI ${COMMON_UI_FILES})
Qt5_WRAP_CPP(MOC ${COMMON_MOC_FILES})

if(KWALLET_SUPPORT)
    Qt5_WRAP_CPP(MOC_KWALLET lxqt_kwallet.h)
    set(KWALLET_COMMON lxqt_kwallet.cpp)
    if(SECRET_SUPPORT)
        add_library(lxqtwallet SHARED ${KWALLET_COMMON} lxqt_libsecret.cpp ${UI} ${COMMON_SRC_FILES} ${MOC} ${MOC_KWALLET})
        target_link_libraries(lxqtwallet secretlib)
    else()
        add_library(lxqtwallet SHARED ${KWALLET_COMMON} ${UI} ${COMMON_SRC_FILES} ${MOC} ${MOC_KWALLET})
    endif()
    target_link_libraries(lxqtwallet KF5::Wallet)
    target_link_libraries(lxqtwallet KF5::Notifications)
else()
    if(SECRET_SUPPORT)
        add_library(lxqtwallet SHARED lxqt_libsecret.cpp ${UI} ${COMMON_SRC_FILES} ${MOC})
        target_link_libraries(lxqtwallet secretlib)
    else()
        add_library(lxqtwallet SHARED ${UI} ${COMMON_SRC_FILES} ${MOC} ${MOC_KWALLET})
    endif()
endif()

target_link_libraries(lxqtwallet lxqtwallet-backend)

target_link_libraries(lxqtwallet ${Qt5Widgets_LIBRARIES})
target_link_libraries(lxqtwallet ${Qt5Core_LIBRARIES})

set_target_properties(lxqtwallet PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic ")
set_target_properties(lxqtwallet PROPERTIES SOVERSION "${LIBRARY_VERSION}")

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
Requires: Qt5Core Qt5Widgets
Libs: -L${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR} -llxqtwallet
Cflags: -I${CMAKE_INSTALL_PREFIX}/include/lxqt
\n")

install(FILES ${PROJECT_BINARY_DIR}/lxqtwallet.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig/ PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
