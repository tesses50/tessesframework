if(TESSESFRAMEWORK_ENABLE_SQLITE)
target_compile_definitions(tessesframework PRIVATE TESSESFRAMEWORK_ENABLE_SQLITE)
endif()
if(TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS)
target_compile_definitions(tessesframework PRIVATE TESSESFRAMEWORK_ENABLE_PLATFORMFOLDERS)
endif()
if(TESSESFRAMEWORK_ENABLE_PROCESS)
target_compile_definitions(tessesframework PRIVATE TESSESFRAMEWORK_ENABLE_PROCESS)

endif()
target_include_directories(tessesframework
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_include_directories(tessesframework
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
if(WIN32)
target_link_libraries(tessesframework PRIVATE iphlpapi)
target_link_libraries(tessesframework PRIVATE ws2_32)

endif()

if(TESSESFRAMEWORK_LOGTOFILE)
target_compile_definitions(tessesframework PRIVATE TESSESFRAMEWORK_LOGTOFILE)
endif()
if(TESSESFRAMEWORK_ENABLE_THREADING)
target_compile_definitions(tessesframework PRIVATE TESSESFRAMEWORK_ENABLE_THREADING)
endif()
if(TESSESFRAMEWORK_ENABLE_NETWORKING)
target_compile_definitions(tessesframework PRIVATE TESSESFRAMEWORK_ENABLE_NETWORKING)
endif()
if(TESSESFRAMEWORK_FETCHCONTENT)
target_compile_definitions(tessesframework PRIVATE TESSESFRAMEWORK_FETCHCONTENT)
endif()
if(TESSESFRAMEWORK_ENABLE_MBED)
target_compile_definitions(tessesframework PRIVATE TESSESFRAMEWORK_ENABLE_MBED)
if(TESSESFRAMEWORK_EMBED_CERT_BUNDLE)
target_compile_definitions(tessesframework PRIVATE TESSESFRAMEWORK_EMBED_CERT_BUNDLE)
else()
target_compile_definitions(tessesframework PRIVATE TESSESFRAMEWORK_CERT_BUNDLE_FILE=${TESSESFRAMEWORK_CERT_BUNDLE_FILE})
endif()

if(TESSESFRAMEWORK_FETCHCONTENT)
target_link_libraries(tessesframework PRIVATE mbedtls mbedx509 mbedcrypto everest p256m)
list(APPEND TessesFrameworkLibs mbedtls mbedx509 mbedcrypto everest p256m)
else()
target_link_libraries(tessesframework PRIVATE mbedtls mbedx509 mbedcrypto)

endif()
endif()


if(NOT(WIN32 OR PLATFORM_PS2))

if(TESSESFRAMEWORK_ENABLE_THREADING)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(tessesframework PRIVATE Threads::Threads)
endif()
endif()

if(NOT(WIN32 OR PLATFORM_PS2 OR "${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoWii" OR "${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoGameCube") OR "${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoSwitch")
check_library_exists(util forkpty "" HAVE_LIBUTIL)
if(HAVE_LIBUTIL)
    target_link_libraries(tessesframework PRIVATE util)
endif()
endif()

if(NOT(WIN32 OR PLATFORM_PS2 OR "${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoWii" OR "${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoGameCube") OR "${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoSwitch")
check_library_exists(dl dlopen "" HAVE_LIBDL)
if(HAVE_LIBDL)
    target_link_libraries(tessesframework PRIVATE dl)
endif()
endif()