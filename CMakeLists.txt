cmake_minimum_required(VERSION 3.16.3)

project(IngaSdl)

if(UNIX)
  add_compile_options(-Wall -Wextra -pedantic -Wno-unused-parameter)
endif()


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SDL2)

# find sdl
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

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
	PRIVATE ${SDL2_INCLUDE_DIR}/../
	)
target_link_libraries(${PROJECT_NAME}
        SDL2::Main
        SDL2::Mixer
        SDL2::TTF
        )
if (UNIX)
    target_link_libraries(${PROJECT_NAME}
            m
            )
endif ()
