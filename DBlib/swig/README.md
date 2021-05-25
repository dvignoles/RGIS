## Python SWIG Interface

> WORK IN PROGRESS

The intention of this code is to generate of python wrapper around a small subset of the DBlib library 
by integrating the swig build process directly into CMake. 

Heavily inspired by: 

https://github.com/Mizux/cmake-swig

https://github.com/danielunderwood/swig-example

## Installation

Default make target will generate wrapper python as part of installation.

`sudo make install.`

There is also a custom target for installing the wrapper library to your environment. 

`sudo make install-python`