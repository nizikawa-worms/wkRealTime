cmake_minimum_required(VERSION 3.17)
project(wkRealTime
        VERSION "0.0.6.1"
        DESCRIPTION "Real-time network multiplayer for racing schemes"
        )

set(CMAKE_CXX_STANDARD 23)

add_library(hacklib STATIC
        lib/hacklib/ExeFile.cpp lib/hacklib/ExeFile_WIN32.cpp
        lib/hacklib/PatternScanner.cpp lib/hacklib/Memory_WIN32.cpp)

link_directories(lib)
include_directories(include lib/hacklib)

set(PRE_CONFIGURE_FILE "cmake/git.cc.in")
set(POST_CONFIGURE_FILE "${CMAKE_CURRENT_BINARY_DIR}/gen/git.cc")
include(cmake/git_watcher.cmake)

add_library(git STATIC ${POST_CONFIGURE_FILE})
target_include_directories(git PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} cmake)
add_dependencies(git check_git)
configure_file(cmake/git.h ${CMAKE_CURRENT_BINARY_DIR}/gen/git.h)

configure_file(resource.rc.in resource.rc)

configure_file(cmake/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/gen/version.h)
add_definitions(-DLIBRARY_EXPORTS)
add_library(wkRealTime SHARED
        src/dllmain.cpp
        src/Config.cpp src/Config.h
        src/WaLibc.cpp src/WaLibc.h
        src/DevConsole.cpp src/DevConsole.h
        src/Hooks.cpp src/Hooks.h
        src/LobbyPackets.cpp src/LobbyPackets.h
        src/LobbyChat.cpp src/LobbyChat.h
        src/LobbyProtocol.cpp src/LobbyProtocol.h
        src/Frontend.cpp src/Frontend.h
        src/W2App.cpp src/W2App.h
        src/Utils.cpp src/Utils.h
        src/Debugf.h
        src/Constants.h
        src/entities/CTask.h
        src/entities/CGameTask.h
        src/entities/CTaskTeam.cpp src/entities/CTaskTeam.h
        src/entities/CTaskTurnGame.cpp src/entities/CTaskTurnGame.h
        src/RealTime.cpp src/RealTime.h
        src/TaskMessageFifo.cpp src/TaskMessageFifo.h
        src/Chat.cpp src/Chat.h
        src/entities/CTaskWorm.cpp src/entities/CTaskWorm.h
        src/GameNet.cpp src/GameNet.h
        src/Camera.cpp src/Camera.h
        src/Drawing.cpp src/Drawing.h
        ${CMAKE_CURRENT_BINARY_DIR}/resource.rc
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gen)
set_property(TARGET wkRealTime hacklib git PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_link_libraries(wkRealTime hacklib capstone Polyhook_2 version libz-static git)
