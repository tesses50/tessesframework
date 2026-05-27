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