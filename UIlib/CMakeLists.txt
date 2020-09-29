project(UIlib)
FILE(GLOB sources src/*.C)
add_library(UI30 ${sources})
target_include_directories(UI30 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                       ${CMAKE_CURRENT_SOURCE_DIR}/../DBlib/include
                                       ${CMAKE_CURRENT_SOURCE_DIR}/../CMlib/include)
install(TARGETS UI30 DESTINATION ghaas/lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/UI.H DESTINATION ghaas/include)
