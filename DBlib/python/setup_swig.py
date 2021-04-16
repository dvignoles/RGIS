#!/usr/bin/env python

"""
setup.py file for SWIG example
"""

from distutils.core import setup, Extension


example_module = Extension('_pyDBlib',
                           sources=['/home/dan/Dev/RGIS/DBlib/python/python/DBlib/dblibPYTHON_wrap.cxx','/home/dan/Dev/RGIS/DBlib/src/DBImpExpNetCDF.C'],
                           include_dirs = ['/usr/local/ghaas/include',
                           '/home/dan/Dev/RGIS/CMlib/include',
                           '/usr/include'],
                    libraries = ['udunits2','libudunits2-0'],
                    library_dirs = ['/usr/local/lib','usr/lib'],
                           )

setup (name = 'pyDBlib',
       version = '0.1',
       author      = "SWIG Docs",
       description = """Simple swig example from docs""",
       ext_modules = [example_module],
       py_modules = ["pyDBlib"],
       )