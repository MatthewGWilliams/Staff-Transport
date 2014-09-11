from distutils.core import setup, Extension
from Cython.Build import cythonize

setup(
	name = "routeFinderApp",
	ext_modules = [Extension('PyProb', libraries = ['stdc++'], sources = ['PyProb.cpp', 'Location.cpp', 'Problem.cpp', 'Requestsetup.cpp', 'Route.cpp', 'Solution.cpp', 'Times.cpp', 'Vehdet.cpp'], language = 'c++', extra_compile_args=['-std=c++11', '-O2'], extra_link_args=['-lstdc++', '-include cmath'])],
)
