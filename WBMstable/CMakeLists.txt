project(WBMstable)

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(${GHAASDIR})
    include_directories(${GHAASDIR}/include)
    link_directories(${GHAASDIR}/lib)
else(${GHAASDIR})
    include_directories(/usr/local/share/ghaas/include)
    link_directories(/usr/local/share/ghaas/lib)
endif(${GHAASDIR})

FILE(GLOB sources src/*.c)
add_executable(WBMstable ${sources})

if(${CMAKE_HOST_APPLE})
    target_link_libraries(WBMstable MF30 CM30 m)
else(${CMAKE_HOST_APPLE})
    target_link_libraries(WBMstable MF30 CM30 m -pthread)
endif(${CMAKE_HOST_APPLE})

target_include_directories(WBMstable PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
install (TARGETS WBMstable RUNTIME DESTINATION ghaas/bin)
