FILE(GLOB sources src/*.c)
add_executable(WBMstable ${sources})
if(${CMAKE_HOST_APPLE})
    target_link_libraries(WBMstable MF10 CM10 m)
else(${CMAKE_HOST_APPLE})
    target_link_libraries(WBMstable MF10 CM10 m -pthread)
endif(${CMAKE_HOST_APPLE})
target_include_directories(WBMstable PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../CMlib/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../MFlib/include)
install (TARGETS WBMstable RUNTIME DESTINATION ghaas/bin)
