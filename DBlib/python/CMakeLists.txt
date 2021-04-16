cmake_minimum_required(VERSION 3.20)
project(pyDBlib)

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

set_property(SOURCE dblib.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE dblib.i PROPERTY SWIG_MODULE_NAME pyDBlib)
swig_add_library(pyDBlib
  TYPE SHARED
  LANGUAGE python
  OUTPUT_DIR ${CMAKE_BINARY_DIR}/pyDBlib
  SOURCES dblib.i)

find_package(PythonLibs REQUIRED)
set_property(TARGET pyDBlib PROPERTY SWIG_USE_TARGET_INCLUDE_DIRECTORIES ON)

target_include_directories(pyDBlib
  PRIVATE
  ../include
  ../../CMlib/include
  /usr/include
  ${PYTHON_INCLUDE_DIRS}
  )

find_library(LIBDB30 NAMES DB30 PATHS /usr/local/ghaas/lib/ REQUIRED)
find_library(LIBCM30 NAMES CM30 PATHS /usr/local/ghaas/lib/ REQUIRED)

find_library(LIBUDUNITS2 udunits2 /usr/lib REQUIRED)
find_library(LIBNETCDF netcdf /usr/lib REQUIRED)

target_link_libraries(pyDBlib ${PYTHON_LIBRARIES} ${LIBDB30} ${LIBCM30} ${LIBUDUNITS2} ${LIBNETCDF})