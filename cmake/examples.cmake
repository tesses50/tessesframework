if(TESSESFRAMEWORK_ENABLE_EXAMPLES)
        add_executable(start-my-website examples/start-my-website.cpp)
        target_link_libraries(start-my-website PUBLIC tessesframework)

        add_executable(echo-my-name examples/echo-my-name.cpp)
        target_link_libraries(echo-my-name PUBLIC tessesframework)

        add_executable(echo-platform examples/echo-platform.cpp)
        target_link_libraries(echo-platform PUBLIC tessesframework)

        add_executable(console-list examples/console-list.cpp)
        target_link_libraries(console-list PUBLIC tessesframework)

        add_executable(console-test examples/console-test.cpp)
        target_link_libraries(console-test PUBLIC tessesframework)
        add_executable(console-raw examples/console-raw.cpp)
        target_link_libraries(console-raw PUBLIC tessesframework)

        add_executable(timer examples/timer.cpp)
        target_link_libraries(timer PUBLIC tessesframework)

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


         add_executable(tests examples/tests.cpp)
        target_link_libraries(tests PUBLIC tessesframework)
endif()