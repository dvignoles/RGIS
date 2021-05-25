cmake_minimum_required(VERSION 3.20)

project(DBlib)

# -fPIC gcc flag
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB sources src/*.C)
add_library(DB30 SHARED ${sources})

find_library(LIBCM30 NAMES CM30 PATHS /usr/local/ghaas/lib/ REQUIRED)
find_library(LIBUDUNITS2 udunits2 /usr/lib REQUIRED)
find_library(LIBNETCDF netcdf /usr/lib REQUIRED)

target_link_libraries(DB30 ${LIBCM30} ${LIBUDUNITS2} ${LIBNETCDF})

target_include_directories(DB30 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                       ${CMAKE_CURRENT_SOURCE_DIR}/../CMlib/include)
install(TARGETS DB30 DESTINATION ghaas/lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/DB.H   DESTINATION ghaas/include)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/DBif.H DESTINATION ghaas/include)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/DBtf.h DESTINATION ghaas/include)

# swig wrapper
add_subdirectory(swig)