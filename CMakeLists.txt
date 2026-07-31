cmake_minimum_required(VERSION 3.16)

include(cmake/version.cmake)
project(TessesFramework VERSION ${TESSESFRAMEWORK_MAJOR_VERSION}.${TESSESFRAMEWORK_MINOR_VERSION}.${TESSESFRAMEWORK_PATCH_VERSION})

set(CMAKE_CXX_STANDARD 17)
include(GNUInstallDirs)
include(CheckLibraryExists)
include(FetchContent)
include(cmake/sources.cmake)
include(cmake/options.cmake)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(TESSESFRAMEWORK_ENABLE_SQLITE)
list(APPEND TESSESFRAMEWORK_SOURCE
    src/Serialization/sqlite/sqlite3-mod.c
    src/Serialization/sqlite/vfs.c
)
endif()
if(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS)
list(APPEND TESSESFRAMEWORK_SOURCE
    src/Platform/sago/platform_folders.cpp
)
endif()

include(cmake/findmbed.cmake)

set(TESSESFRAMEWORK_CA_CERT_URL "https://curl.se/ca/cacert.pem")
set(TESSESFRAMEWORK_CA_CERT_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ca-certificates.crt")



if(TESSESFRAMEWORK_VENDERCERTCHAIN AND TESSESFRAMEWORK_EMBED_CERT_BUNDLE)
if(TESSESFRAMEWORK_FETCHCONTENT)
# Define paths

# Download the file
# EXPECTED_HASH is optional but recommended for reproducibility.
# To get the current hash, run: curl -sSL https://curl.se/ca/cacert.pem | sha256sum
file(DOWNLOAD
    ${TESSESFRAMEWORK_CA_CERT_URL}
    ${TESSESFRAMEWORK_CA_CERT_OUTPUT}
    SHOW_PROGRESS
    TLS_VERIFY ON
    # EXPECTED_HASH SHA256=<insert_sha256_hash_here>
)

# Optional: Inform the user
message(STATUS "CA certificates bundle downloaded to ${TESSESFRAMEWORK_CA_CERT_OUTPUT}")
else()
set(TESSESFRAMEWORK_CERT_BUNDLE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ca-certificates.crt" CACHE FILEPATH "Path to ca-chain")
endif()
else()
set(TESSESFRAMEWORK_CERT_BUNDLE_FILE "/etc/ssl/certs/ca-certificates.crt" CACHE FILEPATH "Path to ca-chain")
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)

include(cmake/bin2h.cmake)

if(TESSESFRAMEWORK_ENABLE_MBED)
if(TESSESFRAMEWORK_EMBED_CERT_BUNDLE)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/include/TessesFramework/CertificateChain.h" "#pragma once\n")
if(TESSESFRAMEWORK_FETCHCONTENT)

bin2h(SOURCE_FILE "${TESSESFRAMEWORK_CA_CERT_OUTPUT}" HEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/include/TessesFramework/CertificateChain.h" VARIABLE_NAME CertificateChain APPEND NULL_TERMINATE)

else()
bin2h(SOURCE_FILE "${TESSESFRAMEWORK_CERT_BUNDLE_FILE}" HEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/include/TessesFramework/CertificateChain.h" VARIABLE_NAME CertificateChain APPEND NULL_TERMINATE)

endif()
file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/include/TessesFramework/CertificateChain.h" "\n")
endif()
endif()

include(cmake/sources.cmake)

if(TESSESFRAMEWORK_STATIC)
include(cmake/staticlib.cmake)
else()
include(cmake/sharedlib.cmake)
endif()

if(TESSESFRAMEWORK_MBEDTLS_PORTLIB)
target_include_directories(tessesframework PUBLIC "${TESSESFRAMEWORK_MBEDTLS_PORTLIB}/include")
target_link_directories(tessesframework PUBLIC "${TESSESFRAMEWORK_MBEDTLS_PORTLIB}/lib")
endif()
include(cmake/install-dev.cmake)

include(cmake/examples.cmake)
include(cmake/apps.cmake)

if(TESSESFRAMEWORK_INSTALL_DEVELOPMENT)
add_subdirectory(pkgconfig)
endif()

if(TESSESFRAMEWORK_NFPM_ARCH)
configure_file(nfpm.yaml.in nfpm.yaml @ONLY)

endif()

configure_file(ppc-pkgbuild.in ppc-pkgbuild @ONLY)