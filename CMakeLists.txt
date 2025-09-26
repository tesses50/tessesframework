cmake_minimum_required(VERSION 3.16)

project(TessesFramework VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

list(APPEND TESSESFRAMEWORK_SOURCE 
src/Date/Date.cpp
src/Http/FileServer.cpp
src/Http/MountableServer.cpp
src/Http/CallbackServer.cpp
src/Http/HttpServer.cpp
src/Http/HttpUtils.cpp
src/Http/HttpClient.cpp
src/Http/HttpStream.cpp
src/Http/ContentDisposition.cpp
src/Http/WebSocket.cpp
src/Mail/Smtp.cpp
src/Serialization/Json.cpp
src/Serialization/SQLite.cpp
src/Serialization/BitConverter.cpp
src/Platform/Environment.cpp
src/Platform/Process.cpp
src/Streams/FileStream.cpp
src/Streams/MemoryStream.cpp
src/Streams/NetworkStream.cpp
src/Streams/Stream.cpp
src/Streams/BufferedStream.cpp
src/Streams/ByteReader.cpp
src/Streams/ByteWriter.cpp
src/Streams/PtyStream.cpp
src/Text/StringConverter.cpp
src/Text/HeaderGenerator.cpp
src/TextStreams/StreamReader.cpp
src/TextStreams/StreamWriter.cpp
src/TextStreams/TextReader.cpp
src/TextStreams/TextWriter.cpp
src/TextStreams/StdIOReader.cpp
src/TextStreams/StdIOWriter.cpp
src/TextStreams/StringReader.cpp
src/TextStreams/StringWriter.cpp
src/Threading/Thread.cpp
src/Threading/Mutex.cpp
src/Threading/ThreadPool.cpp
src/Filesystem/VFS.cpp
src/Filesystem/LocalFS.cpp
src/Filesystem/MemoryFilesystem.cpp
src/Filesystem/SubdirFilesystem.cpp
src/Filesystem/NullFilesystem.cpp
src/Filesystem/MountableFilesystem.cpp
src/Crypto/ClientTLSStream.cpp
src/Crypto/MbedHelpers.cpp
src/TF_Init.cpp
src/wrapper.cpp
src/HiddenField.cpp
)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


option(TESSESFRAMEWORK_EMBED_CERT_BUNDLE "Embed the certificate chain bundle" ON)
option(TESSESFRAMEWORK_ENABLE_MBED "Enable Tesses Framework mbedtls" ON)
option(TESSESFRAMEWORK_ENABLE_NETWORKING "Enable Networking" ON)
option(TESSESFRAMEWORK_ENABLE_THREADING "Enable Threading" ON)
option(TESSESFRAMEWORK_ENABLE_SQLITE "Enable sqlite (embedded in source)" ON)
option(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS "Enable sago platformfolders (embedded in source)" ON)
option(TESSESFRAMEWORK_ENABLE_PROCESS "Enable process api" ON)

if(TESSESFRAMEWORK_ENABLE_SQLITE)
list(APPEND TESSESFRAMEWORK_SOURCE 
    src/Serialization/sqlite/sqlite3.c
    src/Serialization/sqlite/vfs.c
)
endif()
if(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS)
list(APPEND TESSESFRAMEWORK_SOURCE
    src/Platform/sago/platform_folders.cpp
)
endif()
option(TESSESFRAMEWORK_ENABLE_EXAMPLES "Enable Tesses Framework examples" ON)
option(TESSESFRAMEWORK_ENABLE_APPS "Enable Tesses Framework cli apps" ON)
option(TESSESFRAMEWORK_INSTALL_DEVELOPMENT "Enable Installing Tesses Framework Development Packages" ON)
option(TESSESFRAMEWORK_ENABLE_STATIC "Enable Tesses Framework Static Libraries" ON)
option(TESSESFRAMEWORK_ENABLE_SHARED "Enable Tesses Framework Shared Libraries" ON)
option(TESSESFRAMEWORK_ENABLE_SETDATE "Enable setting date to file" ON)
option(TESSESFRAMEWORK_LOGTOFILE "TessesFramework Log to file" OFF)
option(TESSESFRAMEWORK_FETCHCONTENT "TessesFramework fetchcontent" OFF)
option(TESSESFRAMEWORK_VENDERCERTCHAIN "Use the ca-certificates.crt in project rather than system" ON)
if(TESSESFRAMEWORK_FETCHCONTENT)
set(TESSESFRAMEWORK_CERT_BUNDLE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ca-certificates.crt" CACHE FILEPATH "Path to ca-chain")
include(FetchContent)
else()
if(TESSESFRAMEWORK_VENDERCERTCHAIN)
set(TESSESFRAMEWORK_CERT_BUNDLE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ca-certificates.crt" CACHE FILEPATH "Path to ca-chain")
else()
set(TESSESFRAMEWORK_CERT_BUNDLE_FILE "/etc/ssl/certs/ca-certificates.crt" CACHE FILEPATH "Path to ca-chain")
endif()
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)

include(cmake/bin2h.cmake)



# Making sure libraries don't generate symlinks

if(TESSESFRAMEWORK_ENABLE_MBED)
if(TESSESFRAMEWORK_EMBED_CERT_BUNDLE)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/include/TessesFramework/CertificateChain.h" "#pragma once\n")
#target_compile_definitions(TessesFramework PUBLIC TESSESFRAMEWORK_EMBED_CERT_BUNDLE)
bin2h(SOURCE_FILE "${TESSESFRAMEWORK_CERT_BUNDLE_FILE}" HEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/include/TessesFramework/CertificateChain.h" VARIABLE_NAME CertificateChain APPEND NULL_TERMINATE)
file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/include/TessesFramework/CertificateChain.h" "\n")
else()
target_compile_definitions(TessesFramework PUBLIC TESSESFRAMEWORK_CERT_BUNDLE_FILE=${TESSESFRAMEWORK_CERT_BUNDLE_FILE})

endif()
if(TESSESFRAMEWORK_FETCHCONTENT)
set(MBEDTLS_FATAL_WARNINGS OFF CACHE INTERNAL "For TessesFramework" FORCE)
set(ENABLE_TESTING OFF CACHE INTERNAL "For TessesFramework" FORCE)
set(ENABLE_PROGRAMS OFF CACHE INTERNAL "For TessesFramework" FORCE)
if(NOT TESSESFRAMEWORK_ENABLE_STATIC)

set(USE_STATIC_MBEDTLS_LIBRARY OFF CACHE INTERNAL "For TessesFramework" FORCE)
endif()
if(TESSESFRAMEWORK_ENABLE_SHARED)
set(USE_SHARED_MBEDTLS_LIBRARY ON CACHE INTERNAL "For TessesFramework" FORCE)
endif()
FetchContent_Declare(
    mbedtls
    URL https://downloads.tesses.net/cache/libraries/source/mbedtls-3.6.4.tar.bz2
    
)

FetchContent_MakeAvailable(mbedtls)
else()
set(MBEDTLS_DIR "" CACHE PATH "Mbed tls directory")
endif()

endif()
function(TESSESFRAMEWORK_LINKDEPS TessesFramework_TARGET)
if(TESSESFRAMEWORK_ENABLE_SQLITE)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_ENABLE_SQLITE)
endif()
if(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS)
endif()
if(TESSESFRAMEWORK_ENABLE_PROCESS)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_ENABLE_PROCESS)

endif()
target_include_directories(${TessesFramework_TARGET}
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_include_directories(${TessesFramework_TARGET}
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
if(WIN32)
target_link_libraries(${TessesFramework_TARGET} PUBLIC iphlpapi)
endif()
if(TESSESFRAMEWORK_LOGTOFILE)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_LOGTOFILE)
endif()
if(TESSESFRAMEWORK_ENABLE_THREADING)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_ENABLE_THREADING)
endif()
if(TESSESFRAMEWORK_ENABLE_NETWORKING)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_ENABLE_NETWORKING)
endif()
if(TESSESFRAMEWORK_FETCHCONTENT)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_FETCHCONTENT)
endif()
if(TESSESFRAMEWORK_ENABLE_MBED)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_ENABLE_MBED)
if(TESSESFRAMEWORK_EMBED_CERT_BUNDLE)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_EMBED_CERT_BUNDLE)
else()
target_compile_definitions(TessesFramework PUBLIC TESSESFRAMEWORK_CERT_BUNDLE_FILE=${TESSESFRAMEWORK_CERT_BUNDLE_FILE})
endif()
if(NOT TESSESFRAMEWORK_FETCHCONTENT)
if(MBEDTLS_DIR STREQUAL "")
else()
target_include_directories(${TessesFramework_TARGET} PUBLIC ${MBEDTLS_DIR}/include)
target_link_directories(${TessesFramework_TARGET} PUBLIC ${MBEDTLS_DIR}/lib)
endif()
target_link_libraries(${TessesFramework_TARGET} PUBLIC mbedtls mbedx509 mbedcrypto)
endif()
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoWii" OR "${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoGameCube")
target_link_libraries(${TessesFramework_TARGET} PUBLIC fat)

endif()
if("${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoSwitch" AND TESSESFRAMEWORK_ENABLE_THREADING)
target_link_libraries(${TessesFramework_TARGET} PUBLIC -lpthread)
endif()
if("${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoWii")
target_link_libraries(${TessesFramework_TARGET} PUBLIC wiisocket)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_USE_WII_SOCKET)
endif()
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")

target_link_libraries(${TessesFramework_TARGET} PUBLIC ws2_32)

endif()

if(NOT("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows"  OR PLATFORM_PS2 OR "${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoWii" OR "${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoGameCube"))

if(TESSESFRAMEWORK_ENABLE_THREADING)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${TessesFramework_TARGET} PRIVATE Threads::Threads)
endif()
endif()
if(TESSESFRAMEWORK_ENABLE_SETDATE)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_ENABLE_SETDATE)
endif()
target_compile_definitions(${TessesFramework_TARGET} PUBLIC _FILE_OFFSET_BITS=64)

if(PLATFORM_PS2)
target_link_libraries(${TessesFramework_TARGET} PUBLIC -ldebug)
endif()
endfunction()

include(GNUInstallDirs)

if(TESSESFRAMEWORK_ENABLE_STATIC)

add_library(tessesframework STATIC ${TESSESFRAMEWORK_SOURCE})
TESSESFRAMEWORK_LINKDEPS(tessesframework)

if(TESSESFRAMEWORK_FETCHCONTENT AND TESSESFRAMEWORK_ENABLE_MBED)
if(TESSESFRAMEWORK_ENABLE_SHARED)
target_link_libraries(tessesframework PUBLIC mbedtls_static mbedx509_static mbedcrypto_static)
else()

target_link_libraries(tessesframework PUBLIC mbedtls mbedx509 mbedcrypto everest p256m)
endif()
endif()
list(APPEND TessesFrameworkLibs tessesframework)
endif()


if(TESSESFRAMEWORK_ENABLE_SHARED)
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

if (APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../${CMAKE_INSTALL_LIBDIR}")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif()
add_library(tessesframework_shared SHARED ${TESSESFRAMEWORK_SOURCE})
TESSESFRAMEWORK_LINKDEPS(tessesframework_shared)

if(TESSESFRAMEWORK_ENABLE_MBED)
if(TESSESFRAMEWORK_FETCHCONTENT)
target_link_libraries(tessesframework_shared PUBLIC mbedtls mbedx509 mbedcrypto everest p256m)

else()
target_link_libraries(tessesframework_shared PUBLIC mbedtls mbedx509 mbedcrypto)

endif()
endif()
list(APPEND TessesFrameworkLibs tessesframework_shared)




endif()

if((TESSESFRAMEWORK_ENABLE_STATIC OR TESSESFRAMEWORK_ENABLE_SHARED) AND TESSESFRAMEWORK_FETCHCONTENT AND TESSESFRAMEWORK_ENABLE_MBED)
list(APPEND TessesFrameworkLibs mbedtls mbedx509 mbedcrypto everest p256m)
if(TESSESFRAMEWORK_ENABLE_STATIC AND TESSESFRAMEWORK_ENABLE_SHARED)
list(APPEND TessesFrameworkLibs mbedtls_static mbedx509_static mbedcrypto_static)
endif()
endif()


if(TESSESFRAMEWORK_INSTALL_DEVELOPMENT)
install(TARGETS ${TessesFrameworkLibs} 
    EXPORT TessesFrameworkTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/TessesFramework DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/TessesFramework DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT TessesFrameworkTargets
    FILE TessesFrameworkTargets.cmake
    NAMESPACE TessesFramework::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TessesFramework
)


include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/TessesFrameworkConfig.cmake"
INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TessesFramework)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/TessesFrameworkFeatures.h.in "${CMAKE_CURRENT_BINARY_DIR}/include/TessesFramework/TessesFrameworkFeatures.h"
INSTALL_DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/TessesFramework/TessesFrameworkFeatures.h)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/TessesFrameworkConfig.cmake"
DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TessesFramework)
endif()

if(TESSESFRAMEWORK_ENABLE_EXAMPLES)

        add_executable(copyfile examples/copyfile.cpp)
        target_link_libraries(copyfile PUBLIC tessesframework)

        add_executable(webserverex examples/webserverex.cpp)
        target_link_libraries(webserverex PUBLIC tessesframework)

        add_executable(safesubpath examples/safesubpath.cpp)
        target_link_libraries(safesubpath PUBLIC tessesframework)

        add_executable(pathtest examples/pathtest.cpp)
        target_link_libraries(pathtest PUBLIC tessesframework)

        add_executable(mountabletest examples/mountabletest.cpp)
        target_link_libraries(mountabletest PUBLIC tessesframework)

        add_executable(printjsondecodedemoji examples/printjsondecodedemoji.cpp)
        target_link_libraries(printjsondecodedemoji PUBLIC tessesframework)

         add_executable(wsecho examples/wsecho.cpp)
        target_link_libraries(wsecho PUBLIC tessesframework)

         add_executable(runevent examples/runevent.cpp)
        target_link_libraries(runevent PUBLIC tessesframework)
endif()

if(TESSESFRAMEWORK_ENABLE_APPS)
add_executable(tbin2h apps/tbin2h.cpp)
target_link_libraries(tbin2h PUBLIC tessesframework)
install(TARGETS tbin2h DESTINATION "${CMAKE_INSTALL_BINDIR}")
add_executable(tanonydrop apps/tanonydrop.cpp)
target_link_libraries(tanonydrop PUBLIC tessesframework)
install(TARGETS tanonydrop DESTINATION "${CMAKE_INSTALL_BINDIR}")
add_executable(tfetch apps/tfetch.cpp)
target_link_libraries(tfetch PUBLIC tessesframework)
install(TARGETS tfetch DESTINATION "${CMAKE_INSTALL_BINDIR}")
add_executable(tfileserver apps/tfileserver.cpp)
target_link_libraries(tfileserver PUBLIC tessesframework)
install(TARGETS tfileserver DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(tjsonpretty apps/tjsonpretty.cpp)
target_link_libraries(tjsonpretty PUBLIC tessesframework)
install(TARGETS tjsonpretty DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(tjsonunpretty apps/tjsonunpretty.cpp)
target_link_libraries(tjsonunpretty PUBLIC tessesframework)
install(TARGETS tjsonunpretty DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(ttime apps/ttime.cpp)
target_link_libraries(ttime PUBLIC tessesframework)
install(TARGETS ttime DESTINATION "${CMAKE_INSTALL_BINDIR}")
add_executable(tshell apps/tshell.cpp)
target_link_libraries(tshell PUBLIC tessesframework)
install(TARGETS tshell DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_CONTACT "Mike Nolan <tesses@tesses.net>")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
set(CPACK_PACKAGE_VERSION_MAJOR "${TessesFramework_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${TessesFramework_VERSION_MINOR}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libmbedtls-dev (>= 2.28.8)")

include(CPack)
