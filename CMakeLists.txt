# CMakeList.txt : CMake project for Metin2-Clientless-Bot, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Metin2-Clientless-Bot")

# https://github.com/fpagliughi/sockpp
find_package(sockpp CONFIG REQUIRED)

# https://github.com/jarro2783/cxxopts
find_package(cxxopts CONFIG REQUIRED)

# Add source to this project's executable.
add_executable (${PROJECT_NAME}
    "src/main.cpp"
)

# sockpp
target_include_directories(${PROJECT_NAME} PRIVATE ${SOCKPP_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SOCKPP_LIBRARIES})

# cxxopts
target_link_libraries(${PROJECT_NAME} PRIVATE cxxopts::cxxopts)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE Ws2_32)
endif()
