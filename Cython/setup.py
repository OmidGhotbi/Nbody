from setuptools import setup
from Cython.Build import cythonize
import numpy

setup(
    ext_modules = cythonize("nbody_cython.pyx"),
    include_dirs=[numpy.get_include()]
)