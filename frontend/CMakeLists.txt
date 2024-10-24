cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)

INCLUDE(FindPkgConfig)

include_directories(${PROJECT_BINARY_DIR}/frontend)
include_directories(${PROJECT_BINARY_DIR}/backend)
include_directories(${PROJECT_BINARY_DIR})

include_directories(${CMAKE_BINARY_DIR})

if(APPLE)
    file( WRITE ${PROJECT_BINARY_DIR}/osx_keychain.h "#define OSX_KEYCHAIN 1" )
else()
    file( WRITE ${PROJECT_BINARY_DIR}/osx_keychain.h "#define OSX_KEYCHAIN 0" )
endif()

message(STATUS "Checking for modules 'Qt6Widgets','Qt6Core' and 'Qt6LinguistTools'")

find_package(Qt6Widgets REQUIRED)
find_package(Qt6Core REQUIRED)
find_package(Qt6DBus REQUIRED)
find_package(Qt6LinguistTools REQUIRED)

message(STATUS "  Found Qt6Widgets, version ${Qt6Widgets_VERSION}")
message(STATUS "  Found Qt6Core, version ${Qt6Core_VERSION}")
message(STATUS "  Found Qt6DBus, version ${Qt6DBus_VERSION}")
message(STATUS "  Found Qt6LinguistTools, version ${Qt6LinguistTools_VERSION}")

include_directories(${Qt6Widgets_INCLUDE_DIRS})
include_directories(${Qt6Core_INCLUDE_DIRS})
include_directories(${Qt6DBus_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOSECRETSUPPORT)
    SET(SECRET_SUPPORT "false")
    file(WRITE ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 0\n")
else()
    pkg_check_modules(LIBSECRET libsecret-1 QUIET)
    if(LIBSECRET_FOUND)
        add_subdirectory(libsecret)
        SET(SECRET_SUPPORT "true")
        file(WRITE ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 1\n")
    else()
        SET(SECRET_SUPPORT "false")
        file(WRITE ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_SECRET_SUPPORT 0\n")
    endif()
endif()

if(NOKDESUPPORT)
    SET(KWALLET_SUPPORT "false")
    file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
else()
    message(STATUS "Checking for modules 'KF6Wallet'")
    find_package(KF6Wallet QUIET)
    if(KF6Wallet_FOUND )
        message(STATUS "  Found KF6Wallet, version ${KF6Wallet_VERSION}")
        file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 1\n")
        SET(KWALLET_SUPPORT "true")
    else()
        file(APPEND  ${PROJECT_BINARY_DIR}/storage_manager.h "\n#define HAS_KWALLET_SUPPORT 0\n")
        SET(KWALLET_SUPPORT "false")
    endif()
endif()

if(KWALLET_SUPPORT)
    message(STATUS "\n--------------------------------------------------------------------------")
    message(STATUS "kwallet support found, will build kwallet functionality")
    message(STATUS "-----------------------------------------------------------------------")
else()
    message(STATUS "\n--------------------------------------------------------------------------")
    message(STATUS "kwallet support NOT found, support will be discovered at runtime through dbus")
    message(STATUS "-----------------------------------------------------------------------")
endif()

if(SECRET_SUPPORT)
    message(STATUS "\n--------------------------------------------------------------------------")
    message(STATUS "libsecret support found, will build libsecret functionality")
    message(STATUS "-----------------------------------------------------------------------")
else()
    message(STATUS "\n--------------------------------------------------------------------------")
    message(STATUS "libsecret support NOT found, will not build libsecret functionality")
    message(STATUS "-----------------------------------------------------------------------")
endif()

set(COMMON_UI_FILES  changepassworddialog.ui password_dialog.ui)
set(COMMON_MOC_FILES task.h changepassworddialog.h password_dialog.h)
set(COMMON_SRC_FILES changepassworddialog.cpp password_dialog.cpp lxqt_windows_dpapi.cpp lxqt_internal_wallet.cpp lxqt_wallet.cpp lxqt_osx_keychain.cpp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(KWALLET_SUPPORT)
    set(MOC_KWALLET lxqt_kwallet.h)
    set(KWALLET_COMMON lxqt_kwallet.cpp)
    if(SECRET_SUPPORT)
	add_library(lxqt-wallet SHARED ${KWALLET_COMMON} lxqt_libsecret.cpp ${COMMON_UI_FILES} ${COMMON_SRC_FILES} ${COMMON_MOC_FILES} ${MOC_KWALLET})
	target_link_libraries(lxqt-wallet secretlib)
    else()
	add_library(lxqt-wallet SHARED ${KWALLET_COMMON} ${COMMON_UI_FILES} ${COMMON_SRC_FILES} ${COMMON_MOC_FILES} ${MOC_KWALLET})
    endif()
    target_link_libraries(lxqt-wallet KF6::Wallet)
else()
    set(MOC_KWALLET_DBUS lxqt_kwallet-dbus.h)
    set(KWALLET_DBUS lxqt_kwallet-dbus.cpp)
    if(SECRET_SUPPORT)
	add_library(lxqt-wallet SHARED lxqt_libsecret.cpp ${KWALLET_DBUS} ${COMMON_UI_FILES} ${COMMON_SRC_FILES} ${COMMON_MOC_FILES} ${MOC_KWALLET_DBUS})
	target_link_libraries(lxqt-wallet secretlib)
    else()
	add_library(lxqt-wallet SHARED ${COMMON_UI_FILES} ${COMMON_SRC_FILES} ${KWALLET_DBUS} ${COMMON_MOC_FILES} ${MOC_KWALLET_DBUS})
    endif()
endif()

target_link_libraries(lxqt-wallet lxqt-wallet-backend)

target_link_libraries(lxqt-wallet ${Qt6Widgets_LIBRARIES})
target_link_libraries(lxqt-wallet ${Qt6Core_LIBRARIES})

if(WIN32)
    target_link_libraries(lxqt-wallet -lcrypt32 -lbcrypt)
else()
    target_link_libraries(lxqt-wallet ${Qt6DBus_LIBRARIES})
endif()

if(APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    find_library(SECURITY_LIBRARY Security REQUIRED)
    target_link_libraries(lxqt-wallet ${COREFOUNDATION_LIBRARY} ${SECURITY_LIBRARY})
endif()

set_target_properties(lxqt-wallet PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIC -pedantic ")
set_target_properties(lxqt-wallet PROPERTIES SOVERSION "${LIBRARY_VERSION}")

set_target_properties(lxqt-wallet PROPERTIES OUTPUT_NAME lxqt-wallet)

file(WRITE ${PROJECT_BINARY_DIR}/translations_path.h
"
#define TRANSLATIONS_PATH \"${CMAKE_INSTALL_PREFIX}/share/lxqt/translations/lxqt-wallet/\"
")

SET(TRANSLATIONS
   ../translations/lxqt-wallet_bg.ts
   ../translations/lxqt-wallet_cs.ts
   ../translations/lxqt-wallet_de.ts
   ../translations/lxqt-wallet_el.ts
   ../translations/lxqt-wallet_eo.ts
   ../translations/lxqt-wallet_es.ts
   ../translations/lxqt-wallet_fr.ts
   ../translations/lxqt-wallet_he.ts
   ../translations/lxqt-wallet_hr.ts
   ../translations/lxqt-wallet_hu.ts
   ../translations/lxqt-wallet_it.ts
   ../translations/lxqt-wallet_ja.ts
   ../translations/lxqt-wallet_nl.ts
   ../translations/lxqt-wallet_pl.ts
   ../translations/lxqt-wallet_pt.ts
   ../translations/lxqt-wallet_ru.ts
   ../translations/lxqt-wallet_tr.ts
   )

QT6_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS})
SET(QM_FILES ${QM_FILES} PARENT_SCOPE)
ADD_CUSTOM_TARGET(translations ALL DEPENDS ${QM_FILES})

install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_bg.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_cs.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_de.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_el.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_eo.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_es.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_fr.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_he.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_hr.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_hu.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_it.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_ja.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_nl.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_pl.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_pt.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_ru.qm DESTINATION share/lxqt/translations/lxqt-wallet/)
install(FILES ${CMAKE_BINARY_DIR}/frontend/lxqt-wallet_tr.qm DESTINATION share/lxqt/translations/lxqt-wallet/)


install(TARGETS lxqt-wallet RUNTIME LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(FILES lxqt_wallet.h DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lxqt")

file( WRITE ${PROJECT_BINARY_DIR}/lxqt-wallet.pc
"prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${CMAKE_INSTALL_PREFIX}
libdir=${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
includedir=${CMAKE_INSTALL_PREFIX}/include/lxqt

Name: lxqt_wallet
Description: lxqt secure storage system
Version: ${LIBRARY_VERSION}
Requires: Qt6Core Qt6Widgets
Libs: -L${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR} -llxqt-wallet
Cflags: -I${CMAKE_INSTALL_PREFIX}/include/lxqt
\n")

install(FILES ${PROJECT_BINARY_DIR}/lxqt-wallet.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig/ PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
