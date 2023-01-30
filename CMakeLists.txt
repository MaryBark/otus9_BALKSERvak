#cmake_minimum_required(VERSION 3.5)

#project(otus9_BALKSERvak LANGUAGES CXX)

#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_executable(otus9_BALKSERvak main.cpp)

cmake_minimum_required(VERSION 3.5)

# аааааа Огромное задание.... Не знаю, что там надо было брать из 6 задания, но я сделала что-то в лоб)))))
project(otus9_BALKSERvak VERSION 0.0 $ENV{TRAVIS_BUILD_NUMBER})

configure_file(version.h.in version.h)

set(DEBUG_CONS "0")

if (DEBUG_CONS)
        message("DEBUG_CONS is activated!")
        add_definitions(-DDEBUG_CONS)
endif()

add_executable(otus9_BALKSERvak AllProzessors.h AllProzessors.cpp
    Writer.h Writer.cpp
    TreadProzessors.h TreadProzessors.cpp
    OutPuts2Console.h OutPuts2Console.cpp
    server.h server.cpp
    Sessia.h Sessia.cpp)

add_executable(otus9_BALKCLient main.cpp )

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

set_target_properties(otus9_BALKSERvak PROPERTIES
        COMPILE_OPTIONS -Wpedantic -Wall -Wextra
        CMAKE_CXX_STANDARD 17
        CMAKE_CXX_STANDARD_REQUIRED ON
)

set(CMAKE_CXX_FLAGS "-std=c++1z")

target_link_libraries(otus9_BALKSERvak
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries(otus9_BALKCLient
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS otus9_BALKSERvak DESTINATION bin)
install(TARGETS otus9_BALKCLient DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT Alttaira@yandex.ru)

include(CPack)
