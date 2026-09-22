if(TESSESFRAMEWORK_ENABLE_MBED)
if(TESSESFRAMEWORK_FETCHCONTENT)
set(MBEDTLS_FATAL_WARNINGS OFF CACHE INTERNAL "For TessesFramework" FORCE)
set(ENABLE_TESTING OFF CACHE INTERNAL "For TessesFramework" FORCE)
set(ENABLE_PROGRAMS OFF CACHE INTERNAL "For TessesFramework" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

if(NOT TESSESFRAMEWORK_STATIC)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Build with PIC" FORCE)
endif()

FetchContent_Declare(
    mbedtls
    URL https://downloads.tesses.net/cache/libraries/source/mbedtls-3.6.4.tar.bz2

)
if(NOT mbedtls_POPULATED)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static" FORCE)
    if(NOT TESSESFRAMEWORK_STATIC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Build PIC" FORCE)
    endif()


    FetchContent_Populate(mbedtls)
   
    
    
    if(NOT TESSESFRAMEWORK_STATIC)
    add_subdirectory(${mbedtls_SOURCE_DIR} ${mbedtls_BINARY_DIR} EXCLUDE_FROM_ALL)
    else()
    add_subdirectory(${mbedtls_SOURCE_DIR} ${mbedtls_BINARY_DIR})
    endif()
    
endif()
else()
if(TESSESFRAMEWORK_MBEDTLS_PORTLIB STREQUAL "")
find_package(MbedTLS REQUIRED)
endif()
endif()
endif()