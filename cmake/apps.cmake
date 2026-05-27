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

add_executable(tdoc2json apps/tdoc2json.cpp)
target_link_libraries(tdoc2json PUBLIC tessesframework)
install(TARGETS tdoc2json DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(tjson2doc apps/tjson2doc.cpp)
target_link_libraries(tjson2doc PUBLIC tessesframework)
install(TARGETS tjson2doc DESTINATION "${CMAKE_INSTALL_BINDIR}")

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

add_executable(tprinttorrent apps/printtorrent.cpp)

target_link_libraries(tprinttorrent PUBLIC tessesframework)
install(TARGETS tprinttorrent DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(ttorrentcreate apps/ttorrentcreate.cpp)

target_link_libraries(ttorrentcreate PUBLIC tessesframework)
install(TARGETS ttorrentcreate DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(tclearwebseed apps/tclearwebseed.cpp)

target_link_libraries(tclearwebseed PUBLIC tessesframework)
install(TARGETS tclearwebseed DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(trng apps/trng.cpp)

target_link_libraries(trng PUBLIC tessesframework)
install(TARGETS trng DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(twatch apps/twatch.cpp)

target_link_libraries(twatch PUBLIC tessesframework)
install(TARGETS twatch DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(tuuid apps/tuuid.cpp)

target_link_libraries(tuuid PUBLIC tessesframework)
install(TARGETS tuuid DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()