## Python SWIG Interface

> WORK IN PROGRESS

The intention of this code is to generate of python wrapper around a small subset of the DBlib library 
by integrating the swig build process directly into CMake. 

Heavily inspired by: https://github.com/Mizux/cmake-swig

To try it out:

1. Build RGIS from root directory as normal. The CMake config here relies on DBlib and CMlib already being installed. 

2. Run Cmake/make in this directory

```
$ ..../RGIS/DBlib/python/cmake .
$ make
```

3. A folder called `pyDBlib` should appear
```
$ cd pyDBlib
$ python3
>>> import pyDBlib
```

## Future

The CMake build process could be further improved to generate an actual python package as done in the cmake-swig project. 