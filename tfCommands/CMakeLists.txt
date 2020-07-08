include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
		            ${CMAKE_CURRENT_SOURCE_DIR}/../CMlib/include)

foreach(loop_var FAccumulate FCut FSort FStat)
	add_executable(${loop_var} src/${loop_var}.c)
	if(${CMAKE_HOST_APPLE})
		target_link_libraries(${loop_var} LINK_PUBLIC CM30 DB30 F30 -lm)
	else(${CMAKE_HOST_APPLE})
		target_link_libraries(${loop_var} LINK_PUBLIC CM30 DB30 F30 -lm -pthread)
	endif(${CMAKE_HOST_APPLE})
	install (TARGETS ${loop_var} RUNTIME DESTINATION ghaas/bin)
endforeach(loop_var)
