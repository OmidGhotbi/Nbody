Nbody code in different language in order to compare performance. C, C++, Cython, Taichi lang, Python 


## How to compile
# nbody C windows
mkdir build
cd build
cmake -G "Visual Studio 17 2022" ..

# nbody linux
nbody C windows
mkdir build
cmake ..
make

# python
python NBody.py

# Cython
python setup.py build_ext --inplace

# Taichi lang
python NBody.py

# Taichi lang AOT Windows
python NBody.py
md build
cd build
cmake -G "Visual Studio 17 2022" ..
then build the solution .sln

# Taichi lang AOT Windows
python NBody.py
md build
cd build
cmake ..
make
