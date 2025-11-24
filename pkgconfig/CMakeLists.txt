set(PKGCONFIG_INCLUDEDIR "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
set(PKGCONFIG_LIBDIR "\${prefix}/${CMAKE_INSTALL_LIBDIR}")

set(PKGCONFIG_PROJECT_DESCRIPTION "A cross platform wrapper library")
set(PKGCONFIG_PROJECT_HOMEPAGE_URL "https://onedev.site.tesses.net/tesses-framework")

if(TESSESFRAMEWORK_ENABLE_MBED)
set(PKGCONFIG_DEPS "Requires: mbedtls")
else()
set(PKGCONFIG_DEPS "")
endif()

configure_file(tessesframework_static.pc.in tessesframework_static.pc @ONLY)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/tessesframework_static.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

configure_file(tessesframework.pc.in tessesframework.pc @ONLY)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/tessesframework.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)