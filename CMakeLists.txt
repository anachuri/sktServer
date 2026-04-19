cmake_minimum_required(VERSION 3.16)

project(sktServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sktServer main.cpp)
add_executable(client client.cpp)
add_executable(basicClient basicClient.cpp)

include(GNUInstallDirs)
install(TARGETS sktServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
