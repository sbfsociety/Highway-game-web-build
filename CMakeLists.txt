cmake_minimum_required(VERSION 3.28)
project(Troll LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/Zombieschannel/SFML.git
        GIT_TAG SFML-3.x.x-EMCC # This points to the web-patched branch!
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)
FetchContent_MakeAvailable(SFML)

add_executable(Troll
        second.cpp
)
target_compile_features(Troll PRIVATE cxx_std_17)
target_link_libraries(Troll PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
)
if(EMSCRIPTEN)
    set_target_properties(Troll PROPERTIES SUFFIX ".html")
    target_link_options(Troll PRIVATE
    	"-sASYNCIFY=1"
    	"-sUSE_VORBIS=1"
    	"-sUSE_OGG=1"
    	"-sUSE_FREETYPE=1"
        "-sEXPORTED_RUNTIME_METHODS=['HEAPF32']"
        "-sMAX_WEBGL_VERSION=2" 
    	"-sMIN_WEBGL_VERSION=1"
    	"-sFULL_ES2=1"
        "-sALLOW_MEMORY_GROWTH=1"
    	"--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
    )
    set_target_properties(Troll PROPERTIES SUFFIX ".html")
    
endif()
