add_library(schifra STATIC)
set_target_properties(schifra PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(schifra)
target_include_directories(schifra PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)