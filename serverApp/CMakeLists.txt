include_directories(
    ../thirdParty/mySql/include
)

add_executable(Server
    main.cpp
    ../src/server/server.h
    ../src/server/server.cpp
    ../src/database/database.h
    ../src/database/database.cpp
)

add_compile_definitions(STATIC_CONCPP)

target_link_libraries(Server PUBLIC ${CMAKE_SOURCE_DIR}/thirdParty/mySql/lib/mysqlcppconn-static.lib)
