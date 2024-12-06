cmake_minimum_required(VERSION 3.20)

project(TessesFramework VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

list(APPEND TESSESFRAMEWORK_SOURCE 
src/Http/FileServer.cpp
src/Http/HttpServer.cpp
src/Http/HttpUtils.cpp
src/Http/HttpClient.cpp
src/Http/HttpStream.cpp
src/Http/ContentDisposition.cpp
src/Streams/FileStream.cpp
src/Streams/MemoryStream.cpp
src/Streams/NetworkStream.cpp
src/Streams/Stream.cpp
src/Streams/BufferedStream.cpp
src/TextStreams/StreamReader.cpp
src/TextStreams/StreamWriter.cpp
src/TextStreams/TextReader.cpp
src/TextStreams/TextWriter.cpp
src/Threading/Thread.cpp
src/Threading/Mutex.cpp
src/Filesystem/VFS.cpp
src/Filesystem/LocalFS.cpp
src/Filesystem/SubdirFilesystem.cpp
src/Filesystem/NullFilesystem.cpp
src/Filesystem/MountableFilesystem.cpp
src/Crypto/ClientTLSStream.cpp
src/TF_Init.cpp
)

set(TESSESFRAMEWORK_CERT_BUNDLE_FILE "/etc/ssl/certs/ca-certificates.crt")
option(TESSESFRAMEWORK_EMBED_CERT_BUNDLE "Embed the certificate chain bundle" ON)
option(TESSESFRAMEWORK_ENABLE_MBED "Enable Tesses Framework mbedtls" ON)
option(TESSESFRAMEWORK_ENABLE_EXAMPLES "Enable Tesses Framework examples" ON)
option(TESSESFRAMEWORK_ENABLE_APPS "Enable Tesses Framework cli apps" ON)
option(TESSESFRAMEWORK_INSTALL_DEVELOPMENT "Enable Installing Tesses Framework Development Packages" ON)
option(TESSESFRAMEWORK_ENABLE_STATIC "Enable Tesses Framework Static Libraries" ON)
option(TESSESFRAMEWORK_ENABLE_SHARED "Enable Tesses Framework Shared Libraries" ON)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)

if(TESSESFRAMEWORK_ENABLE_MBED)
if(TESSESFRAMEWORK_EMBED_CERT_BUNDLE)
include(cmake/bin2h.cmake)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/include/CertificateChain.h "#pragma once\n")
#target_compile_definitions(TessesFramework PUBLIC TESSESFRAMEWORK_EMBED_CERT_BUNDLE)
bin2h(SOURCE_FILE ${TESSESFRAMEWORK_CERT_BUNDLE_FILE} HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/CertificateChain.h VARIABLE_NAME CertificateChain APPEND NULL_TERMINATE)
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/include/CertificateChain.h "\n")
#else()
#target_compile_definitions(TessesFramework PUBLIC TESSESFRAMEWORK_CERT_BUNDLE_FILE=${TESSESFRAMEWORK_CERT_BUNDLE_FILE})

endif()
endif()
set(MBEDTLS_DIR "")

function(link_deps TessesFramework_TARGET)
if(TESSESFRAMEWORK_ENABLE_MBED)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_ENABLE_MBED)
if(TESSESFRAMEWORK_EMBED_CERT_BUNDLE)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_EMBED_CERT_BUNDLE)
else()
target_compile_definitions(TessesFramework PUBLIC TESSESFRAMEWORK_CERT_BUNDLE_FILE=${TESSESFRAMEWORK_CERT_BUNDLE_FILE})
endif()
if(MBEDTLS_DIR STREQUAL "")
else()
target_include_directories(${TessesFramework_TARGET} PUBLIC ${MBEDTLS_DIR}/include)
target_link_directories(${TessesFramework_TARGET} PUBLIC ${MBEDTLS_DIR}/lib)
endif()
target_link_libraries(${TessesFramework_TARGET} PUBLIC mbedtls mbedx509 mbedcrypto)
target_include_directories(${TessesFramework_TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)
endif()
target_include_directories(${TessesFramework_TARGET}
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoWii" OR "${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoGameCube")
target_link_libraries(${TessesFramework_TARGET} PUBLIC fat)
endif()
if("${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoWii")
target_link_libraries(${TessesFramework_TARGET} PUBLIC wiisocket)
target_compile_definitions(${TessesFramework_TARGET} PUBLIC TESSESFRAMEWORK_USE_WII_SOCKET)
endif()
endfunction()

include(GNUInstallDirs)

if(TESSESFRAMEWORK_ENABLE_STATIC)

add_library(tessesframework STATIC ${TESSESFRAMEWORK_SOURCE})
link_deps(tessesframework)
list(APPEND TessesFrameworkLibs tessesframework)
endif()


if(TESSESFRAMEWORK_ENABLE_SHARED)

add_library(tessesframework_shared SHARED ${TESSESFRAMEWORK_SOURCE})
link_deps(tessesframework_shared)
list(APPEND TessesFrameworkLibs tessesframework_shared)
endif()




if(TESSESFRAMEWORK_INSTALL_DEVELOPMENT)
install(TARGETS ${TessesFrameworkLibs} 
    EXPORT TessesFrameworkTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/TessesFramework DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT TessesFrameworkTargets
    FILE TessesFrameworkTargets.cmake
    NAMESPACE TessesFramework::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TessesFramework
)


include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/TessesFrameworkConfig.cmake"
INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TessesFramework)

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

        add_executable(mountabletest examples/mountabletest.cpp)
        target_link_libraries(mountabletest PUBLIC tessesframework)

        
endif()

if(TESSESFRAMEWORK_ENABLE_APPS)
add_executable(tfetch apps/tfetch.cpp)
target_link_libraries(tfetch PUBLIC tessesframework)
install(TARGETS tfetch DESTINATION bin)
add_executable(tfileserver apps/tfileserver.cpp)
target_link_libraries(tfileserver PUBLIC tessesframework)
install(TARGETS tfileserver DESTINATION bin)

endif()