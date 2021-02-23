cmake_minimum_required(VERSION 3.16.3)

project(IngaSdl)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find sdl
include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)
pkg_search_module(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
link_directories(${SDL2_LIBRARY_DIRS})

# find threads
find_package(Threads)

if (APPLE)
    message(STATUS "mac os x target system detected")
    set(SDL2_INCLUDE_DIR "${SDL2_INCLUDE_DIRS}/..")
elseif (UNIX AND NOT APPLE)
    message(STATUS "linux target system detected")
    set(SDL2_INCLUDE_DIR "${SDL2_INCLUDE_DIRS}")
else ()
    message(WARNING "unknown target system detected")
    set(SDL2_INCLUDE_DIR "${SDL2_INCLUDE_DIRS}")
endif ()

message(STATUS "SDL2_INCLUDE_DIR=${SDL2_INCLUDE_DIR}")
message(STATUS "SDL2_LIBRARY_DIRS=${SDL2_LIBRARY_DIRS}")

set(INGA_SDL_SRC
        sources/cJSON.c
        sources/Cursor.c
        sources/Dialog.c
        sources/Element.c
        sources/Fader.c
        sources/Font.c
        sources/Game.c
        sources/Menu.c
        sources/GameConfig.c
        sources/GameState.c
        sources/Global.c
        sources/Image.c
        sources/ImageSet.c
        sources/InventoryBar.c
        sources/Location.c
        sources/NavigationMap.c
        sources/Script.c
        sources/Sequence.c
        sources/SlotList.c
        sources/SoundManager.c
        sources/Thread.c
        sources/Utils.c
        sources/
        sources/main.c
        )
add_executable(${PROJECT_NAME}
        ${INGA_SDL_SRC}
        )
target_include_directories(${PROJECT_NAME}
        PRIVATE sources
        PRIVATE ${SDL2_INCLUDE_DIR}
        )
target_link_libraries(${PROJECT_NAME}
        ${SDL2_LIBRARIES}
        ${SDL2IMAGE_LIBRARIES}
        ${SDL2MIXER_LIBRARIES}
        ${SDL2TTF_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        m
        )
