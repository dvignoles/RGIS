project(DBlib)
FILE(GLOB sources src/*.cpp)
add_library(DB30 ${sources})
target_include_directories(DB30 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                       ${CMAKE_CURRENT_SOURCE_DIR}/../CMlib/include)
install(TARGETS DB30 DESTINATION ghaas/lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/DB.hpp   DESTINATION ghaas/include)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/DBif.hpp DESTINATION ghaas/include)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/DBtf.h   DESTINATION ghaas/include)
