include_directories(
    ../thirdParty/mySql/include
)

file(COPY 
    ${CMAKE_SOURCE_DIR}/thirdParty/mySql/lib/libssl-1_1-x64.dll
    ${CMAKE_SOURCE_DIR}/thirdParty/mySql/lib/libcrypto-1_1-x64.dll
    DESTINATION ${CMAKE_BINARY_DIR}/serverApp
)

add_executable(Server
    main.cpp
    ../src/server/server.h
    ../src/server/server.cpp
    ../src/database/database.h
    ../src/database/database.cpp
)

add_compile_definitions(STATIC_CONCPP)

target_link_libraries(Server PUBLIC 
    ${CMAKE_SOURCE_DIR}/thirdParty/mySql/lib/mysqlcppconn-static.lib
)

