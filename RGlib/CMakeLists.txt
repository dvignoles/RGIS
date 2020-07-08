FILE(GLOB sources src/*.C)
add_library(RG30 ${sources})
target_include_directories(RG30 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                       ${CMAKE_CURRENT_SOURCE_DIR}/../DBlib/include
                                       ${CMAKE_CURRENT_SOURCE_DIR}/../CMlib/include
                                       ${CMAKE_CURRENT_SOURCE_DIR}/../MFlib/include)
install(TARGETS RG30 DESTINATION ghaas/lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/RG.H DESTINATION ghaas/include)