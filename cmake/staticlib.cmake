add_library(tessesframework STATIC ${TESSESFRAMEWORK_SOURCE})

include(${CMAKE_CURRENT_LIST_DIR}/helpers.cmake)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoWii" OR "${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoGameCube")
target_link_libraries(tessesframework PUBLIC fat)

endif()
if("${CMAKE_SYSTEM_NAME}" STREQUAL "NintendoWii")
target_link_libraries(tessesframework PUBLIC wiisocket)
target_compile_definitions(tessesframework PUBLIC TESSESFRAMEWORK_USE_WII_SOCKET)
endif()

list(APPEND TessesFrameworkLibs tessesframework)