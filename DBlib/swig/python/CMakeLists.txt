# Include python
find_package(PythonLibs 3 REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})

set(CMAKE_SWIG_FLAGS "")
set_source_files_properties(../DB30.i PROPERTIES CPLUSPLUS ON)
include_directories(../../include)

# Add swig module
swig_add_library(DB30py LANGUAGE python SOURCES ../DB30.i)
swig_link_libraries(DB30py DB30 ${PYTHON_LIBRARIES})

###TESTING###
# swig_link_libraries(DB30py DB30 ${PYTHON_LIBRARIES} ${LIBCM30} ${LIBUDUNITS2} ${LIBNETCDF})

# target_include_directories(DB30py
#   PRIVATE
#   ../../include
#   ../../../CMlib/include
#   /usr/include
#   ${PYTHON_INCLUDE_DIRS}
#   )

# target_link_libraries(DB30py ${LIBCM30} ${LIBUDUNITS2} ${LIBNETCDF})
###############

# Files to install with Python
set(PYTHON_INSTALL_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/DB30.py
        ${CMAKE_CURRENT_BINARY_DIR}/_DB30py.so)

# Configure setup.py and copy to output directory
set(SETUP_PY_IN ${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in)
set(SETUP_PY_OUT ${CMAKE_CURRENT_BINARY_DIR}/setup.py)
configure_file(${SETUP_PY_IN} ${SETUP_PY_OUT})

# Install target to call setup.py
add_custom_target(install-python
        DEPENDS DB30py
        COMMAND python3 ${SETUP_PY_OUT} install)
