cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)

add_definitions(-Wno-unused-result)

find_file(GCRYPT_INCLUDE_FILE gcrypt.h)

find_library(GCRYPT_LIBRARY gcrypt)

if(NOT GCRYPT_INCLUDE_FILE)
    MESSAGE(FATAL_ERROR "Could not find gcrypt header file")
else()
    MESSAGE(STATUS "Found gcrypt header file: ${GCRYPT_INCLUDE_FILE}")
endif()

if(NOT GCRYPT_LIBRARY)
    MESSAGE(FATAL_ERROR "Could not find gcrypt library(libgcrypt.so)")
else()
    MESSAGE(STATUS "Found gcrypt library: ${GCRYPT_LIBRARY}")
endif()

add_library(lxqt-wallet-backend STATIC lxqtwallet.c)
if( CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0 )
    set_target_properties(lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "-Wall -s -fPIC -pedantic -Wformat-truncation=0")
else()
    set_target_properties(lxqt-wallet-backend PROPERTIES COMPILE_FLAGS "--Wall -s -fPIC -pedantic")
endif()
set_target_properties(lxqt-wallet-backend PROPERTIES LINK_FLAGS "-pie")

target_link_libraries(lxqt-wallet-backend "${GCRYPT_LIBRARY}")

install(FILES lxqt-wallet.h DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lxqt")

add_executable(lxqt_wallet-cli lxqt_wallet-cli.c lxqtwallet.c)
if( CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0 )
    set_target_properties(lxqt_wallet-cli PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIE -pthread  -pedantic -Wformat-truncation=0")
else()
    set_target_properties(lxqt_wallet-cli PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIE -pthread  -pedantic")
endif()
set_target_properties(lxqt_wallet-cli PROPERTIES LINK_FLAGS "-pie")
TARGET_LINK_LIBRARIES(lxqt_wallet-cli "${GCRYPT_LIBRARY}")

install(TARGETS lxqt_wallet-cli RUNTIME DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
