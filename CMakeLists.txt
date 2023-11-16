# CMakeList.txt : CMake project for QuadTreeLib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("QuadTreeLib")
# Source files
file(GLOB SOURCES "src/*.cpp")

# Header files
file(GLOB HEADERS "include/*.hpp" "include/*.tpp")

# Add source to this project's executable.
add_executable (QuadTreeLib "QuadTreeTEST.cpp" ${HEADERS} ${SOURCES})

target_link_libraries(QuadTreeLib sfml-graphics sfml-audio)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET QuadTreeLib PROPERTY CXX_STANDARD 20)
endif()




# SFML
set(SFML_DIR "D:/libs/SFML-2.6.0-windows-vc17-64-bit/SFML-2.6.0/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS graphics audio REQUIRED)


include_directories(${SFML_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)




