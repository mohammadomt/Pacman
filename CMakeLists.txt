cmake_minimum_required(VERSION 3.6.0)
SET(PROJECT_NAME Pacman)
project(${PROJECT_NAME})

set(CMAKE_C_STANDARD 99)

set(SDL2_GFX_IncludeDir "sdl/sdl2-gfx-include")
set(SDL2_GFX_LibDir "E:/Programming/C/Pacman2/sdl/sdl2-gfx-lib")
set(SDL2_Flags "-mwindows -Wl ,--no -undefined -static -libgcc")
set(SDL2_LibDir "E:/Programming/C/Pacman2/sdl/sdl2-lib")
set(SDL2_IncludeDir "sdl/sdl2-include")
set(SDL2_TTF_LibDir "E:/Programming/C/Pacman2/sdl/sdl2-ttf")
set(SDL2_TTF_IncludeDir "sdl/sdl2-ttf-include")

add_library(SDL2 STATIC IMPORTED)
add_library(SDL2main STATIC IMPORTED)
add_library(SDL2_GFX STATIC IMPORTED)
#add_library(SDL2_TTF STATIC IMPORTED)

find_package(SDL2_TTF REQUIRED)

set_property(TARGET SDL2 PROPERTY IMPORTED_LOCATION "${SDL2_LibDir}/libSDL2.a")
set_property(TARGET SDL2main PROPERTY IMPORTED_LOCATION "${SDL2_LibDir}/libSDL2main.a")
set_property(TARGET SDL2_GFX PROPERTY IMPORTED_LOCATION "${SDL2_GFX_LibDir}/libsdl-gfx.a")
#set_property(TARGET SDL2_TTF PROPERTY IMPORTED_LOCATION "${SDL2_TTF_LibDir}/libSDL_ttf.a")

set(SDL2_Libs SDL2 SDL2main SDL2_GFX m dinput8 dxguid dxerr8 user32 gdi32 winmm imm32 ole32 oleaut32 shell32 version uuid)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${SDL2_Flags}")


set(SOURCE src/main.c src/game.c src/physics.c src/map.c src/Values.c src/Point.c src/Rect.c src/SDLHelper.h src/SDLHelper.c src/Graphics.h src/Graphics.c)

add_executable(${PROJECT_NAME} ${SOURCE})

include_directories(src ${SDL2_GFX_IncludeDir} ${SDL2_IncludeDir} ${SDL2_TTF_IncludeDir})

#add_library(lib STATIC IMPORTED)

#set_property(TARGET lib PROPERTY IMPORTED_LOCATION "E:/Programming/C/Pacman2/liblib.a")

#target_link_libraries(${PROJECT_NAME} lib)
target_link_libraries(${PROJECT_NAME} ${SDL2_Libs}  ${SDL2_TTF_LIBRARY})